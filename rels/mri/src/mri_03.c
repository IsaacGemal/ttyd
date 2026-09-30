#include "mri_03.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "party/party_motion.h"
#include "eff/eff_mahojin.h"
#include "rel/mri/mri.h"
#include <dolphin/gx.h>

#pragma pool_data off

#pragma force_active on
extern s32 evt_mri_get16bitdata[];
extern s32 evt_puni_get_lsw_index[];
extern s32 evt_puni_init_puni[];
extern s32 evt_puni_move_position[];
extern s32 evt_puni_pera_onoff[];
extern s32 evt_puni_puninum_onoff[];
extern s32 evt_puni_set_lsw_index[];
extern s32 evt_puni_set_position[];
extern s32 evt_puni_set_trap_index[];
extern s32 mri_guide_goryu[];
extern s32 mri_guide_hagure_check[];
extern s32 mri_puni_walk_se[];
static u8* _ap;
s32 guide_sister[775];

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

// Tev callback used by the alpha fade effect. Called before the scripts, so its
// .rodata zero constant lands first, matching the original's section order.
static void alpha_tev(TevCallbackParam* param) {
    s32 stage = param->field_C;
    u8* ap = _ap;

    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColor(GX_TEVREG0, (GXColor){0, 0, 0, ap[0]});
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    param->field_C = param->field_C + 1;
}

static USER_FUNC(alpha_init_func) {
    _ap = _mapAlloc(1);
    _ap[0] = 0xFF;
    return EVT_RETURN_DONE;
}

static USER_FUNC(alpha_set_func) {
    _ap[0] = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(jail_alpha_mario_check)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(2), -81)
            SET_VALUE(GW(2), 80)
        ELSE
            SET_VALUE(GW(2), 255)
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(jail_alpha)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_l_saku"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_l_doa"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_l_01"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_r_saku"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_r_doa"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_r_01"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("mario"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("party"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("プニ族長老"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ガイド妹"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ガイド"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("プニ族"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("プニ族３"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ガイドsp"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ガイド妹sp"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ちびトゲsp"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ライバルsp"), 2048)
    IF_INT_EQUAL(GSWF(2832), 0)
        CALL(evt_map_blend_set_mobj_flag, 1, STRING("mobj_lock00"), 2048)
    END_IF
    IF_INT_EQUAL(GSWF(2834), 0)
        CALL(evt_map_blend_set_mobj_flag, 1, STRING("mobj_lock01"), 2048)
    END_IF
    IF_INT_EQUAL(GSWF(5613), 0)
        CALL(evt_map_blend_set_mobj_flag, 1, STRING("kururin1"), 2048)
    END_IF
    SPAWN_EVENT(jail_alpha_mario_check)
    SET_VALUE(LW(10), 255)
    SET_VALUE(GW(2), 255)
    SET_VALUE(GW(3), 255)
    SET_VALUE(GF(2), 0)
    LOOP(0)
        IF_INT_EQUAL(GF(2), 0)
            IF_INT_LESS(GW(2), LW(10))
                ADD(LW(10), -5)
                IF_INT_GREATER(GW(2), LW(10))
                    SET_VALUE(LW(10), GW(2))
                END_IF
            END_IF
            IF_INT_GREATER(GW(2), LW(10))
                ADD(LW(10), 5)
                IF_INT_LESS(GW(2), LW(10))
                    SET_VALUE(LW(10), GW(2))
                END_IF
            END_IF
            CALL(evt_map_set_blend, 0, 255, 255, 255, LW(10))
        ELSE
            IF_INT_LESS(GW(3), LW(10))
                ADD(LW(10), -5)
                IF_INT_GREATER(GW(3), LW(10))
                    SET_VALUE(LW(10), GW(3))
                END_IF
            END_IF
            IF_INT_GREATER(GW(3), LW(10))
                ADD(LW(10), 5)
                IF_INT_LESS(GW(3), LW(10))
                    SET_VALUE(LW(10), GW(3))
                END_IF
            END_IF
            CALL(evt_map_set_blend, 0, 255, 255, 255, LW(10))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(after_2hours)
    CALL(evt_map_blend_off, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("mario"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("party"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("プニ族長老"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("ガイド妹"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("ガイド"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("プニ族"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("プニ族３"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("ガイドsp"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("ガイド妹sp"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("ちびトゲsp"), 2048)
    CALL(evt_map_blend_set_flag, 0, STRING("ライバルsp"), 2048)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 255)
    RETURN
EVT_END()

static Vec spline_dt[8] = {
    { 132.0f, 0.0f, -153.0f },
    { 95.26720428466797f, 0.0f, -108.09037017822266f },
    { 81.30668640136719f, 0.0f, -100.69795227050781f },
    { 79.7278060913086f, 0.0f, -73.32685852050781f },
    { 117.33457946777344f, 0.0f, -59.177791595458984f },
    { 162.0447998046875f, 0.0f, -53.07642364501953f },
    { 228.6510772705078f, 0.0f, 36.3253173828125f },
    { 245.45486450195312f, 0.0f, 22.319448471069336f },
};

Vec spline_dt1[5] = {
    { 98.0f, 0.0f, -156.0f },
    { 65.11044311523438f, 0.0f, -80.23627471923828f },
    { 56.63582992553711f, 0.0f, -57.38899612426758f },
    { -4.335236072540283f, 0.0f, -40.861289978027344f },
    { -145.0f, 0.0f, -60.0f },
};

EVT_BEGIN(elder_rescue)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_cam3d_evt_set, 105, 82, 92, 105, 30, -203, 1000, 11)
    CALL(evt_npc_flag_onoff, 1, STRING("プニ族長老"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 1536)
    CALL_CHILD(mri_guide_goryu)
    CALL(evt_puni_set_mode, 1, 30)
    CALL(evt_puni_get_position, 1, LW(0), LW(1), LW(2))
    CALL(evt_puni_set_position, 1, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    SET_VALUE(GW(3), 80)
    SET_VALUE(GF(2), 1)
    SET_VALUE(LW(0), STRING("ガイド"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイド"), 98, -156, 0, FLOAT(120.0f), 0)
    DELETE_EVENT(LW(9))
    CALL(evt_npc_set_ry, STRING("ガイド"), 90)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_00"), 0, STRING("ガイド"))
    CALL(evt_npc_set_ry, STRING("プニ族長老"), 270)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_mario_mov_pos2, 69, -144, FLOAT(100.0f))
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    CALL(evt_party_move_pos2, 0, 63, -170, FLOAT(150.0f))
    CALL(evt_party_set_dir, 0, 90, -1)
    WAIT_MS(300)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set, FLOAT(105.0f), FLOAT(73.6f), FLOAT(44.3f), FLOAT(105.0f), FLOAT(30.0f), FLOAT(-203.0f), 1000, 8)
        WAIT_MS(900)
        CALL(evt_cam3d_evt_set, 105, 82, 92, 105, 30, -203, 400, 11)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("プニ族長老"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_CHOUROU_BIG1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 8, FLOAT(1.0f), FLOAT(2.5f), 700)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_npc_set_scale, STRING("プニ族長老"), LW(0), LW(0), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_N_7"))
        WAIT_MS(1000)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(0), 2)
        CALL(evt_party_jump_pos, 0, LW(0), LW(1), LW(2), 400, 1, -1000)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_STG2_PUNI1"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_01"), 0, STRING("プニ族長老"))
    WAIT_WHILE(GF(0))
    CALL(evt_sub_intpl_msec_init, 5, FLOAT(2.5f), FLOAT(1.0f), 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_npc_set_scale, STRING("プニ族長老"), LW(0), LW(0), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_continue)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_02"), 0, STRING("ガイド"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_03"), 0, STRING("プニ族長老"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_04"), 0, STRING("ガイド"))
    CALL(evt_npc_get_position, STRING("プニ族長老"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("プニ族長老"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("プニ族長老"), LW(0), LW(1), LW(2), 200, 18)
    CALL(evt_npc_get_position, STRING("プニ族長老"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("プニ族長老"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("プニ族長老"), LW(0), LW(1), LW(2), 200, 18)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_05"), 0, STRING("プニ族長老"))
    CALL(evt_snd_bgmoff, 26625)
    CALL(evt_fade_out, 1000)
    WAIT_MS(100)
    CALL(evt_fade_entry, 12, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_06"), 0, 0)
    BEGIN_INLINE_EVENT_GET_ID(GW(1))
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SLEEP1"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(3000)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mario_sleep_on)
    CALL(evt_party_sleep_on, 0)
    WAIT_MS(1000)
    CALL(evt_fade_entry, 11, 0, 0, 0, 0)
    CALL(evt_snd_bgmon, 161, 0)
    CALL(evt_fade_in, 1000)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_07"), 0, STRING("プニ族長老"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_08"), 0, STRING("ガイド"))
    CALL(evt_npc_get_position, STRING("プニ族長老"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("プニ族長老"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("プニ族長老"), LW(0), LW(1), LW(2), 200, 18)
    CALL(evt_npc_get_position, STRING("プニ族長老"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("プニ族長老"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("プニ族長老"), LW(0), LW(1), LW(2), 200, 18)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_09"), 0, STRING("プニ族長老"))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_10"), 0, STRING("ガイド"))
    CALL(evt_npc_get_position, STRING("プニ族長老"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("プニ族長老"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("プニ族長老"), LW(0), LW(1), LW(2), 200, 18)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_11"), 0, STRING("プニ族長老"))
    CALL(evt_snd_bgmoff_f, 513, 1000)
    CALL(evt_cam3d_evt_off, 1000, 11)
    CALL(evt_npc_status_onoff, 1, STRING("プニ族長老"), 2)
    CALL(evt_sub_spline_init, 0, 0, PTR(spline_dt), 8, 30, 0)
    LABEL(0)
    CALL(evt_sub_spline_get_value, LW(3), LW(4), LW(5), LW(6))
    SET_VALUE(LW(0), STRING("プニ族長老"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("プニ族長老"), LW(3), LW(5), 160, 0, 0)
    DELETE_EVENT(LW(9))
    IF_INT_EQUAL(LW(6), 1)
        GOTO(0)
    END_IF
    CALL(evt_npc_set_position, STRING("プニ族長老"), 0, -1000, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_12"), 0, STRING("ガイド"))
    WAIT_MS(300)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_cam3d_evt_set, -155, 85, 204, -155, 50, 7, 1500, 11)
    END_INLINE_EVENT
    CALL(evt_npc_status_onoff, 1, STRING("ガイド"), 2)
    CALL(evt_sub_spline_init, 0, 0, PTR(spline_dt1), 5, 15, 0)
    LABEL(1)
    CALL(evt_sub_spline_get_value, LW(3), LW(4), LW(5), LW(6))
    SET_VALUE(LW(0), STRING("ガイド"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイド"), LW(3), LW(5), 160, 0, 0)
    DELETE_EVENT(LW(9))
    IF_INT_EQUAL(LW(6), 1)
        GOTO(1)
    END_IF
    WAIT_MS(100)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_12_01"), 0, STRING("ガイド"))
    DELETE_EVENT(GW(1))
    CALL(evt_mario_sleep_off)
    CALL(evt_party_sleep_off, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_pos, -10, 0, -52)
        CALL(evt_mario_mov_pos2, -110, -52, FLOAT(120.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_pos, 0, 32, 0, -52)
        CALL(evt_party_move_pos2, 0, -68, -52, FLOAT(120.0f))
    END_INLINE_EVENT
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_12_02"), 0, STRING("ガイド妹"))
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_npc_get_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("ガイド"), 0, -1000, 0)
    CALL(evt_puni_set_position, 1, LW(0), LW(1), LW(2))
    CALL(evt_puni_set_mode, 1, 1)
    SET_VALUE(GF(2), 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 94)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(elder_init)
    SWITCH(GSW(0))
        CASE_LESS(94)
        CALL(evt_npc_set_position, STRING("me"), 132, 0, -153)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(elder_talk)
    IF_INT_EQUAL(GSWF(2832), 0)
        CALL(evt_msg_print, 0, STRING("stg2_mri_e14_13"), 0, STRING("me"))
    ELSE
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(sister_init)
    SWITCH(GSW(0))
        CASE_LESS(101)
        CALL(evt_npc_set_position, STRING("me"), -192, 0, -142)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(sister_regl)
    IF_INT_EQUAL(GSW(0), 91)
        RETURN
    END_IF
    IF_INT_EQUAL(GSW(0), 98)
        RETURN
    END_IF
    LOOP(0)
        CALL(evt_set_dir_to_target, STRING("me"), STRING("ガイドsp"))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(sister_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e14_14"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(guide_init)
    IF_INT_EQUAL(GSWF(2836), 1)
        CALL(evt_npc_set_position, STRING("me"), -145, 0, -60)
        CALL(evt_npc_set_ry, STRING("me"), 270)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(guide_talk)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e19_04_02"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(puni_3_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(118)
        IF_INT_EQUAL(GSWF(2884), 1)
            CALL(evt_npc_set_position, STRING("me"), 50, 0, 30)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(puni_3_talk)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg2_mri_re_018"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg2_mri_re_018_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {
        "プニ族長老", 0x600, 0, elder_init, NULL, elder_talk, NULL, NULL,
        NULL, NULL, NULL, NPC_TERRITORY_TYPE_NOTHING, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f },
        0.0f, 0.0f, 0.0f, 0.0f, 0,
    },
    {
        "ガイド妹", 0x600, 0, sister_init, sister_regl, sister_talk, NULL, NULL,
        NULL, NULL, NULL, NPC_TERRITORY_TYPE_NOTHING, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f },
        0.0f, 0.0f, 0.0f, 0.0f, 0,
    },
    {
        "ガイド", 0x600, 0, guide_init, NULL, guide_talk, NULL, NULL,
        NULL, NULL, NULL, NPC_TERRITORY_TYPE_NOTHING, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f },
        0.0f, 0.0f, 0.0f, 0.0f, 0,
    },
    {
        "プニ族３", 0x600, 0, puni_3_init, NULL, puni_3_talk, NULL, NULL,
        NULL, NULL, NULL, NPC_TERRITORY_TYPE_NOTHING, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f },
        0.0f, 0.0f, 0.0f, 0.0f, 0,
    },
    { NULL },
};

EVT_BEGIN(bero_custom_00)
    SET_VALUE(GF(0), 1)
    IF_INT_EQUAL(GSW(0), 91)
        CALL(evt_bero_exec_onoff, 0, 4)
        BEGIN_INLINE_EVENT
            WAIT_MS(2000)
            CALL(evt_bero_exec_onoff, 0, 2)
            WAIT_MS(1000)
            SET_VALUE(GF(0), 0)
            CALL(evt_bero_exec_onoff, 1, 2)
            CALL(evt_bero_exec_onoff, 1, 4)
        END_INLINE_EVENT
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"dokan", 2, 0, 0x09, 100000, 0, 0, -1, bero_custom_00, 6, NULL, "mri_20", "dokan1", 1, 1},
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "mri_04", "e_bero", 1, 1},
    {NULL},
};

// s16 data; evt_mri_get16bitdata reads it with lhax
static s16 puni_rand_position[178] = {
    -68, -143, -146, -199, -101, -286, -126, -253, -144, -149, -229, -226, -183, -191, -40, -200,
    -166, -202, -218, -153, -78, -160, -197, -271, -244, -212, -145, -179, -59, -233, -64, -214,
    -177, -240, -44, -180, -230, -247, -147, -298, -115, -152, -260, -201, -211, -216, -110, -268,
    -140, -239, -175, -282, -98, -162, -217, -174, -40, -240, -78, -199, -161, -252, -165, -144,
    -176, -219, -131, -164, -249, -253, -87, -246, -50, -282, -212, -237, -235, -291, -165, -182,
    -73, -180, -141, -218, -93, -182, -96, -210, -223, -275, -197, -156, -267, -263, -107, -248,
    -89, -144, -226, -202, -80, -227, -208, -288, -90, -267, -244, -273, -144, -263, -123, -209,
    -179, -167, -128, -189, -236, -179, -248, -233, -237, -158, -48, -148, -120, -294, -67, -271,
    -59, -195, -44, -220, -121, -230, -129, -276, -48, -258, -179, -262, -256, -151, -155, -280,
    -58, -166, -256, -181, -129, -136, -194, -250, -114, -175, -193, -209, -213, -258, -159, -163,
    -67, -251, -109, -194, -208, -195, -107, -134, -159, -231, -194, -229, -100, -229, -81, -286,
    -197, -176,
};

EVT_BEGIN(jail_init_00)
    SET_VALUE(LW(10), 14)
    SET_VALUE(LW(11), 0)
    LOOP(0)
        CALL(evt_mri_get16bitdata, PTR(puni_rand_position), LW(11), LW(0))
        ADD(LW(11), 1)
        CALL(evt_mri_get16bitdata, PTR(puni_rand_position), LW(11), LW(2))
        ADD(LW(11), 1)
        CALL(evt_puni_init_puni, LW(10), 14, LW(0), 0, LW(2), 0)
        CALL(evt_puni_flag_onoff, LW(10), 0, 8388608)
        CALL(evt_puni_pera_onoff, LW(10), 1)
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(red_jail_open)
    SET_VALUE(GSWF(2833), 1)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(1000)
    CALL(evt_mapobj_get_position, STRING("S_hako_r_doa"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_DOOR_GRID_OPEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 85, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_hako_r_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_hitobj_onoff, STRING("A_hako_r1"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_hako_r_01"), 1, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_opendoor00)
    CALL(evt_mapobj_rotate, 1, STRING("S_hako_r_doa"), 0, 85, 0)
    CALL(evt_hitobj_onoff, STRING("A_hako_r1"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_hako_r_01"), 1, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock00)
    CALL_CHILD(red_jail_open)
    SPAWN_EVENT(elder_rescue)
    RETURN
EVT_END()

EVT_BEGIN(evt_unlock00_01)
    SPAWN_EVENT(red_jail_open)
    RETURN
EVT_END()

s32 key_table00[] = {
    0x00000010, 0xFFFFFFFF,
};

EVT_BEGIN(evt_check00)
    CALL(evt_pouch_check_item, 16, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(key_table00), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(blue_jail_open)
    SET_VALUE(GSWF(2835), 1)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(1000)
    CALL(evt_mapobj_get_position, STRING("S_hako_l_doa"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_DOOR_GRID_OPEN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 85, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_rotate, 1, STRING("S_hako_l_doa"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_hitobj_onoff, STRING("A_hako_l_01"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("S_hako_l1"), 1, 0)
    CALL(evt_mario_key_onoff, 1)
    SPAWN_EVENT(guide_sister)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_opendoor01)
    CALL(evt_mapobj_rotate, 1, STRING("S_hako_l_doa"), 0, 85, 0)
    CALL(evt_hitobj_onoff, STRING("A_hako_l_01"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("S_hako_l1"), 1, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_unlock01)
    CALL(evt_pouch_remove_item, 17, LW(0))
    SPAWN_EVENT(blue_jail_open)
    RETURN
EVT_END()

s32 key_table01[] = {
    0x00000010, 0x00000011, 0xFFFFFFFF,
};

EVT_BEGIN(evt_check01)
    CALL(evt_win_item_maketable, PTR(key_table01), LW(10))
    IF_INT_NOT_EQUAL(LW(10), 0)
        CALL(evt_win_item_select, 0, LW(10), LW(0), 0)
        SWITCH(LW(0))
            CASE_EQUAL(17)
            CASE_EQUAL(16)
            CALL(evt_mobj_exec_cancel, STRING("me"))
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_msg_toge, 1, 0, 0, 0)
            CALL(evt_msg_print, 0, STRING("stg2_mri_e14_15"), 0, 0)
            CALL(evt_puni_get_lsw_index, LSW(0), 0, LW(0), LW(1), LW(2), LW(3))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_puni_flag_onoff, 1, 0, 262144)
                CALL(evt_npc_flag_onoff, 1, STRING("ガイドsp"), 1536)
                CALL(evt_msg_print, 0, STRING("stg2_mri_e14_16"), 0, STRING("ガイドsp"))
                CALL(evt_puni_flag_onoff, 1, 1, 262144)
                CALL(evt_msg_print, 0, STRING("stg2_mri_e14_17"), 0, STRING("ガイド妹"))
            END_IF
            CALL(evt_mario_key_onoff, 1)
            CASE_DEFAULT
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_SWITCH
    END_IF
    CALL(evt_win_item_desttable, LW(10), 2)
EVT_END()

EVT_BEGIN(bluejail_alpha)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_l_saku"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_l_doa"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_l_01"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_r_saku"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_r_doa"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_r_01"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("mario"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("party"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("プニ族長老"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ガイド妹"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ガイド"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("プニ族"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ガイドsp"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ガイド妹sp"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ちびトゲsp"), 2048)
    CALL(evt_map_blend_set_flag, 1, STRING("ライバルsp"), 2048)
    CALL(evt_map_set_blend, 0, 255, 255, 255, 255)
    IF_INT_EQUAL(GSWF(2832), 0)
        CALL(evt_map_blend_set_mobj_flag, 1, STRING("mobj_lock00"), 2048)
    END_IF
    IF_INT_EQUAL(GSWF(2834), 0)
        CALL(evt_map_blend_set_mobj_flag, 1, STRING("mobj_lock01"), 2048)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(bluejail_event)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_puni_puninum_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    DELETE_EVENT(GW(0))
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envoff, 512)
    CALL_CHILD(bluejail_alpha)
    CALL(evt_npc_set_position, STRING("ガイド妹"), -192, 0, -120)
    CALL(evt_npc_set_ry_lr, STRING("ガイド妹"), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド妹"), 1073741856)
    CALL(evt_puni_set_position, 14, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("プニ族"), -114, 0, -120)
    CALL(evt_npc_set_ry_lr, STRING("プニ族"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("プニ族"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("プニ族"), 1536)
    CALL(evt_cam3d_evt_set, -160, 160, 540, -160, 60, 0, 0, 11)
    CALL(evt_seq_wait, 2)
    BEGIN_INLINE_EVENT
        WAIT_MS(1500)
        CALL(evt_sub_intpl_msec_init, 11, 255, 80, 1500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(evt_map_set_blend, 0, 255, 255, 255, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -160, 90, 147, -160, 60, 0, 3000, 11)
    WAIT_MS(3000)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e19_00"), 0, STRING("プニ族"))
    CALL(evt_npc_get_position, STRING("ガイド妹"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNIKO_KINOKO1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_item_entry, STRING("item00"), 152, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    ADD(LW(0), 20)
    CALL(evt_item_move_3d_position, STRING("item00"), LW(0), LW(1), LW(2), 300, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e19_01"), 0, STRING("ガイド妹"))
    CALL(evt_cam3d_evt_set, -159, 197, 270, -159, 81, -32, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e19_02"), 0, STRING("プニ族"))
    CALL(evt_npc_get_position, STRING("ガイド妹"), LW(0), LW(1), LW(2))
    CALL(evt_item_move_3d_position, STRING("item00"), LW(0), LW(1), LW(2), 300, 0)
    CALL(evt_item_set_position, STRING("item00"), 0, -1000, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e19_03"), 0, STRING("ガイド妹"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e19_04"), 0, STRING("プニ族"))
    WAIT_MS(1500)
    SET_VALUE(GSW(0), 99)
    CALL(evt_fade_set_mapchange_type, 0, 2, 500, 1, 500)
    CALL(evt_bero_mapchange, STRING("mri_08"), STRING("e_bero"))
    RETURN
EVT_END()

static EVT_BEGIN(mario_kirimomi_event)
    WAIT_MS(300)
    CALL(evt_mario_dispflag_onoff, 1, 256)
    CALL(evt_mario_get_dispdir, LW(8))
    SET_VALUE(LW(9), 1980)
    ADD(LW(9), LW(8))
    CALL(evt_sub_intpl_msec_init, 11, LW(8), LW(9), 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mario_set_dispdir, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_dispflag_onoff, 0, 256)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

EVT_BEGIN(party_kirimomi_event)
    WAIT_MS(500)
    CALL(evt_party_stop, 0)
    CALL(evt_party_dispflg_onoff, 0, 16, 1)
    CALL(evt_party_get_dispdir, 0, LW(8))
    SET_VALUE(LW(9), 1980)
    ADD(LW(9), LW(8))
    CALL(evt_sub_intpl_msec_init, 11, LW(8), LW(9), 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_party_set_dispdir, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_party_dispflg_onoff, 0, 16, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

s32 JailAwayTable_New[15] = {
    0xFFFFFFAD, 0xFFFFFF28, 0xFFFFFF67, 0xFFFFFF3D,
    0xFFFFFF33, 0xFFFFFF66, 0xFFFFFF0F, 0xFFFFFFA1,
    0xFFFFFF24, 0xFFFFFFE7, 0xFFFFFF42, 0x00000000,
    0xFFFFFF60, 0x00000028, 0x00002710,
};

void puniModeEventparent(PuniData* punidat) {
    s32* table = JailAwayTable_New;
    u8* p = (u8*)punidat;
    Vec v;
    Vec target;
    f32 dist;

    switch (*(u16*)(p + 0x114)) {
        case 0: {
            f32 best = 1000000.0f;
            s32 i;
            s32 bestIdx = -1;

            for (i = 0; i < 4; i++) {
                target.x = (f32)table[i * 2];
                target.y = 0.0f;
                target.z = (f32)table[i * 2 + 1];
                dist = PSVECSquareDistance(&target, (Vec*)(p + 0x12C));
                if (best > dist) {
                    best = dist;
                    bestIdx = i;
                }
            }
            *(s32*)(p + 0x1E0) = bestIdx;
            *(u16*)(p + 0x11E) = 2;
            *(u16*)(p + 0x114) = 1;
            // fallthrough
        }
        case 1: {
            target.x = (f32)table[*(s32*)(p + 0x1E0) * 2];
            target.y = 0.0f;
            target.z = (f32)table[*(s32*)(p + 0x1E0) * 2 + 1];
            dist = PSVECSquareDistance(&target, (Vec*)(p + 0x12C));
            if (dist > 100.0f) {
                PSVECSubtract(&target, (Vec*)(p + 0x12C), &v);
                PSVECNormalize(&v, &v);
                v.y = 0.0f;
                PSVECScale(&v, &v, 1.5f);
                PSVECAdd((Vec*)(p + 0x15C), &v, (Vec*)(p + 0x15C));
            } else if (*(s32*)(p + 0x1E0) == *(s32*)(p + 0x1E4) + 5) {
                *(Vec*)(p + 0x15C) = (Vec){ 0.0f, 0.0f, 0.0f };
                *(u16*)(p + 0x114) = 2;
            } else if (table[*(s32*)(p + 0x1E0) * 2 + 2] != 0x2710) {
                *(s32*)(p + 0x1E0) = *(s32*)(p + 0x1E0) + 1;
            } else {
                *(Vec*)(p + 0x15C) = (Vec){ 0.0f, 0.0f, 0.0f };
                *(u16*)(p + 0x114) = 2;
            }
            break;
        }
        case 2:
            *(u16*)(p + 0x11E) = 0;
            *(u16*)(p + 0x114) = 3;
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 12:
            break;
        case 10: {
            Vec pos;

            target = (Vec){ -160.0f, 0.0f, 40.0f };
            if (PSVECSquareDistance(&target, (Vec*)(p + 0x12C)) < 4900.0f) {
                PSVECSubtract((Vec*)(p + 0x12C), &target, &v);
                PSVECNormalize(&v, &v);
                v.y = 0.0f;
                PSVECScale(&v, &v, 0.7f);
                PSVECAdd((Vec*)(p + 0x15C), &v, (Vec*)(p + 0x15C));
                *(f32*)(p + 0x174) = 2.1f;
                {
                    f32 mag = PSVECMag((Vec*)(p + 0x15C));

                    pos = *(Vec*)(p + 0x12C);
                    pos.y += 1.0f;
                    if (mag != 0.0f) {
                        PSVECNormalize((Vec*)(p + 0x15C), &v);
                    } else {
                        v = (Vec){ 0.0f, 0.0f, 0.0f };
                    }
                    puniMoveAtari(punidat, pos, v, 0, mag);
                }
            } else {
                *(u16*)(p + 0x114) = 11;
            }
            break;
        }
        case 11:
            *(Vec*)(p + 0x15C) = (Vec){ 0.0f, 0.0f, 0.0f };
            if (*(f32*)(p + 0x12C) < -160.0f) {
                *(f32*)(p + 0x180) = 90.0f;
            } else {
                *(f32*)(p + 0x180) = 270.0f;
            }
            *(u16*)(p + 0x114) = 12;
            break;
    }
}

USER_FUNC(puni_parent) {
    PuniData* punidat = &puniGetPunidat()[evtGetValue(event, event->args[0])];
    u8* p = (u8*)punidat;
    u16 dir;

    if (*(f32*)(p + 0x12C) > -180.0f) {
        *(s32*)(p + 0x1E4) = event->lwData[11] % 2;
        event->lwData[11]++;
        dir = 1;
    } else {
        *(s32*)(p + 0x1E4) = event->lwData[12] % 2;
        event->lwData[12]++;
        dir = 2;
    }
    *(s32*)(p + 0x4) |= 0x100000;
    *(u16*)(p + 0x118) = dir;
    *(u16*)(p + 0xC) = 0x1D;
    *(u16*)(p + 0x114) = 0;
    *(u16*)(p + 0x11E) = 1000;
    *(void**)(p + 0x56C) = (void*)puniModeEventparent;
    *(void**)(p + 0x570) = (void*)puniMoveDecel;
    return EVT_RETURN_DONE;
}

USER_FUNC(puni_away) {
    PuniData* punidat = &puniGetPunidat()[evtGetValue(event, event->args[0])];
    u8* p = (u8*)punidat;

    *(u16*)(p + 0x114) = 10;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(kinoko_small)
    CALL(evt_item_get_position, LW(10), LW(0), LW(1), LW(2))
    SET_VALUE(LW(10), STRING("item00"))
    SET_FLOAT_VALUE(LW(11), FLOAT(0.7f))
    CALL(evt_item_set_scale, LW(10), LW(11))
    WAIT_MS(750)
    SET_FLOAT_VALUE(LW(11), FLOAT(0.35f))
    CALL(evt_item_set_scale, LW(10), LW(11))
    WAIT_MS(750)
    CALL(evt_item_set_position, STRING("item00"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(guide_sister)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LW(10), 14)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    LOOP(0)
        CALL(puni_parent, LW(10))
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_puni_set_mode, 4, 14)
    CALL(evt_puni_set_anim, 4, 0)
    CALL(evt_puni_flag_onoff, 4, 0, 1048576)
    CALL(evt_puni_set_mode, 3, 14)
    CALL(evt_puni_set_anim, 3, 0)
    CALL(evt_puni_flag_onoff, 3, 0, 1048576)
    SET_VALUE(LW(10), 5)
    LOOP(9)
        CALL(evt_puni_set_mode, LW(10), 14)
        CALL(evt_puni_set_anim, LW(10), 0)
        CALL(evt_puni_flag_onoff, LW(10), 0, 1048576)
        ADD(LW(10), 1)
    END_LOOP
    SPAWN_EVENT(mario_kirimomi_event)
    SPAWN_EVENT(party_kirimomi_event)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_NOKO_SAIKAI1"))
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド妹"), 1073741856)
    WAIT_MS(2000)
    CALL(evt_npc_get_reglid, STRING("ガイド妹"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_set_ry, STRING("ガイド妹"), 270)
    SET_VALUE(LW(0), STRING("ガイド妹"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイド妹"), -245, -108, 0, FLOAT(180.0f), 0)
    DELETE_EVENT(LW(9))
    CALL(evt_npc_set_ry, STRING("ガイド妹"), 90)
    SET_VALUE(LW(0), STRING("ガイド妹"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイド妹"), -180, 40, 0, FLOAT(180.0f), 0)
    DELETE_EVENT(LW(9))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_00"), 0, STRING("ガイド妹"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -230, -45, FLOAT(120.0f))
        WAIT_FRAMES(1)
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -250, -40, FLOAT(120.0f))
        WAIT_FRAMES(1)
        CALL(evt_party_set_dir, 0, 90, -1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ガイド"), 0, 0, 0, 0, 0, 0, 0, 60)
        WAIT_MS(1000)
        SET_VALUE(LW(0), STRING("ガイド"))
        SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
        CALL(evt_npc_move_position, STRING("ガイド"), -140, 40, 0, FLOAT(180.0f), 0)
        DELETE_EVENT(LW(9))
        CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    END_INLINE_EVENT
    WAIT_MS(1000)
    SET_VALUE(LW(10), 14)
    LOOP(0)
        CALL(puni_away, LW(10))
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_cam3d_evt_set, -160, 90, 352, -160, 30, 7, 500, 11)
    WAIT_MS(1500)
    SET_VALUE(LW(10), STRING("ガイド"))
    SET_VALUE(LW(11), STRING("ガイド妹"))
    CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, LW(10), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, LW(10), LW(0), LW(1), LW(2), 200, 18)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, LW(10), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, LW(10), LW(0), LW(1), LW(2), 200, 18)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_01"), 0, LW(10))
    CALL(evt_npc_get_position, LW(11), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, LW(11), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, LW(11), LW(0), LW(1), LW(2), 200, 18)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, LW(11), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, LW(11), LW(0), LW(1), LW(2), 200, 18)
    CALL(evt_msg_togelr, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_02"), 0, LW(11))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_03"), 0, LW(10))
    CALL(evt_msg_togelr, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_04"), 0, LW(11))
    CALL(evt_npc_set_ry, LW(11), 270)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_05"), 0, LW(11))
    CALL(evt_npc_set_ry, LW(11), 90)
    WAIT_MS(200)
    CALL(evt_npc_get_position, LW(11), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNIKO_KINOKO1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_item_entry, STRING("item00"), 152, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    ADD(LW(0), 20)
    CALL(evt_item_move_3d_position, STRING("item00"), LW(0), LW(1), LW(2), 500, 0)
    CALL(evt_msg_togelr, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_06"), 0, LW(11))
    CALL(evt_cam3d_evt_set, -150, 73, 278, -150, 25, 7, 500, 11)
    SET_VALUE(LW(0), LW(10))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, LW(10), -150, 40, 0, FLOAT(60.0f), 0)
    DELETE_EVENT(LW(9))
    CALL(evt_npc_set_autotalkpose, LW(10), STRING("T_1"), STRING("T_1"))
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT_GET_ID(LW(9))
        LOOP(0)
            CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_EAT1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(30)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, LW(10), STRING("T_1"))
    WAIT_MS(2000)
    SPAWN_EVENT(kinoko_small)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_07"), 0, LW(10))
    WAIT_FRAMES(1)
    DELETE_EVENT(LW(9))
    CALL(evt_npc_set_anim, LW(10), STRING("S_1"))
    CALL(evt_npc_set_autotalkpose, LW(10), STRING("S_1"), STRING("T_1"))
    CALL(evt_msg_togelr, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_08"), 0, LW(11))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_09"), 0, LW(10))
    CALL(evt_eff_fukidashi, 1, STRING(""), LW(11), 1, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_msg_togelr, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_10"), 0, LW(11))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_11"), 0, LW(10))
    CALL(evt_npc_get_position, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(0), -20)
    CALL(evt_cam3d_evt_set, -175, 73, 278, -175, 25, 7, 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, LW(10), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_PUNI_JUMP1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, LW(10), LW(0), LW(1), LW(2), 200, 18)
    CALL(evt_msg_togelr, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_12"), 0, LW(11))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_13"), 0, LW(10))
    WAIT_MS(2000)
    CALL(evt_msg_togelr, 0)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e20_14"), 0, LW(11))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSWF(2836), 0)
    SET_VALUE(GSWF(2877), 1)
    SET_VALUE(GSW(651), 98)
    CALL(evt_npc_get_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("ガイド"), 0, -1000, 0)
    CALL(evt_puni_init_puni, 1, 1, LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_get_position, STRING("ガイド妹"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("ガイド妹"), 0, -1000, 0)
    CALL(evt_puni_init_puni, 2, 1, LW(0), LW(1), LW(2), 0)
    SET_VALUE(LW(10), 1)
    LOOP(0)
        CALL(evt_puni_set_mode, LW(10), 1)
        CALL(evt_puni_flag_onoff, LW(10), 1, 8388608)
        ADD(LW(10), 1)
        IF_INT_EQUAL(LW(10), 103)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_GET_PARTY1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    CALL(evt_msg_toge, 1, 0, 0, 0)
    IF_INT_EQUAL(GSWF(2878), 0)
        CALL(evt_msg_print, 0, STRING("stg2_mri_e20_16"), 0, 0)
    ELSE
        CALL(evt_msg_print, 0, STRING("stg2_mri_e20_16_1"), 0, 0)
    END_IF
    SET_VALUE(GSW(0), 101)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(elder_shaberi)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_06"), 0, LW(10))
    LOOP(0)
        CALL(evt_msg_repeat)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(toraware_elder)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_puni_wait_systemmode, 2)
    CALL(evt_bero_exec_wait, 65536)
    WAIT_WHILE(GF(0))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL_CHILD(mri_guide_hagure_check)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_puni_set_lsw_index, LSW(0), 0, 0, 0, 0, 0)
        CALL(evt_puni_set_trap_index, 1, 0)
        CALL(evt_puni_init_puni, 1, 30, 0, -1000, 0, 0)
        CALL(evt_puni_flag_onoff, 1, 1, 65536)
        CALL(evt_puni_set_position, 1, 240, 0, 0)
        CALL(evt_puni_move_position, 1, 240, 40, FLOAT(120.0f))
        CALL(evt_puni_set_mode, 1, 1)
    END_IF
    CALL(evt_puni_flag_onoff, 1, 0, 262144)
    CALL(evt_puni_set_anim, 1, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイドsp"), 1073743392)
    CALL(evt_npc_set_ry, STRING("ガイドsp"), 270)
    SET_VALUE(LW(0), STRING("ガイドsp"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイドsp"), 195, 40, 0, FLOAT(120.0f), 0)
    DELETE_EVENT(LW(9))
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ガイドsp"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_01"), 0, STRING("ガイドsp"))
    CALL(evt_cam3d_evt_set, 135, 85, 204, 135, 50, 7, 500, 11)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), STRING("ガイドsp"))
        SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
        CALL(evt_npc_move_position, STRING("ガイドsp"), 145, -60, 0, FLOAT(120.0f), 0)
        DELETE_EVENT(LW(9))
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    SET_VALUE(LW(10), STRING("プニ族長老"))
    CALL(evt_npc_flag_onoff, 1, LW(10), 1073741856)
    CALL(evt_npc_set_ry, LW(10), 90)
    SET_VALUE(LW(0), LW(10))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, LW(10), 115, -100, 0, FLOAT(60.0f), 0)
    DELETE_EVENT(LW(9))
    WAIT_WHILE(GF(0))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_02"), 0, LW(10))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 175, -30, FLOAT(120.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        CALL(evt_party_move_pos2, 0, 210, -30, FLOAT(120.0f))
    END_INLINE_EVENT
    WAIT_MS(333)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_03"), 0, STRING("ガイドsp"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_04"), 0, LW(10))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_05"), 0, STRING("ガイドsp"))
    SPAWN_EVENT_GET_ID(PTR(elder_shaberi), LW(15))
    SET_VALUE(LW(10), STRING("ガイド妹"))
    CALL(evt_npc_set_ry, LW(10), 90)
    CALL(evt_npc_set_position, LW(10), -165, 0, -100)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_07"), 0, LW(10))
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ガイドsp"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -155, 85, 240, -155, 50, 7, 3000, 11)
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        DELETE_EVENT(LW(15))
        CALL(evt_msg_close, STRING("stg2_mri_e12_26_06"))
    END_INLINE_EVENT
    SET_VALUE(LW(0), STRING("ガイドsp"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイドsp"), -140, -40, 0, FLOAT(120.0f), 0)
    DELETE_EVENT(LW(9))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_08"), 0, STRING("ガイドsp"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -110, -30, FLOAT(120.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -75, -30, FLOAT(120.0f))
    END_INLINE_EVENT
    WAIT_MS(250)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_09"), 0, LW(10))
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_10"), 0, STRING("ガイドsp"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_11"), 0, LW(10))
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("ガイドsp"), 90)
    WAIT_MS(833)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e12_26_12"), 0, STRING("ガイドsp"))
    SET_VALUE(LW(0), STRING("ガイドsp"))
    SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
    CALL(evt_npc_move_position, STRING("ガイドsp"), -125, -10, 0, FLOAT(120.0f), 0)
    DELETE_EVENT(LW(9))
    CALL(evt_puni_flag_onoff, 1, 1, 262144)
    CALL(evt_npc_get_position, STRING("ガイドsp"), LW(0), LW(1), LW(2))
    CALL(evt_puni_set_position, 1, LW(0), LW(1), LW(2))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 92)
    CALL(evt_npc_restart_regular_event, STRING("ガイド妹"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(guide_sister_before)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_puni_wait_systemmode, 2)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 270, 0)
    CALL(evt_bero_exec_wait, 65536)
    WAIT_MS(500)
    CALL_CHILD(mri_guide_hagure_check)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_puni_set_position, 1, 240, 0, 0)
        CALL(evt_puni_move_position, 1, 240, 40, FLOAT(120.0f))
    END_IF
    CALL(evt_puni_get_position, 1, LW(0), LW(1), LW(2))
    CALL(evt_puni_set_position, 1, 0, -1000, 0)
    CALL(evt_puni_set_mode, 1, 30)
    CALL(evt_puni_flag_onoff, 1, 0, 8388608)
    SET_VALUE(GSWF(2836), 1)
    CALL(evt_npc_set_position, STRING("ガイド"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("ガイド"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド"), 1073743392)
    CALL(evt_msg_print, 0, STRING("stg2_mri_e19_04_01"), 0, STRING("ガイド"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_change_interrupt, STRING("ガイド"), 6, 0)
        SET_VALUE(LW(0), STRING("ガイド"))
        SPAWN_EVENT_GET_ID(PTR(mri_puni_walk_se), LW(9))
        CALL(evt_npc_move_position, STRING("ガイド"), -145, -60, 0, FLOAT(120.0f), 0)
        DELETE_EVENT(LW(9))
        CALL(evt_npc_flag_onoff, 0, STRING("ガイド"), 1073741856)
        CALL(evt_npc_change_interrupt, STRING("ガイド"), 6, PTR(guide_talk))
    END_INLINE_EVENT
    SET_VALUE(GSW(0), 100)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static KusaAccessEntry ueki_access_data[] = {
    { 1, "A_ueki01", "S_ueki01", NULL },
    { 1, "A_ueki03", "S_ueki03", NULL },
    { 1, "A_ueki04", "S_ueki04", NULL },
    { 1, "A_ueki05", "S_ueki05", NULL },
    { 4, NULL, NULL, NULL },
};

EVT_BEGIN(mri_03_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 3, 0, -310, -1000, -1000, 0, 0)
    CALL(evt_puni_set_hamari, 0, 0, 0, 2, -2, -1000, -1000)
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
    SET_VALUE(LW(0), PTR(ueki_access_data))
    CALL_CHILD(evt_sub_kusa_access_entry)
    SWITCH(GSW(0))
        CASE_BETWEEN(109, 110)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI9"))
        CASE_EQUAL(98)
        CASE_GREATER_OR_EQUAL(101)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG2_PUNI2"))
        CALL(evt_snd_envoff, 512)
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI4"))
    END_SWITCH
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hako_r_saku"), 2)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), -150, 0, -200, STRING("S_yuka_oku"), 125, GSWF(5613))
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_map_playanim, STRING("mizu_01"), 1, 0)
    CALL(evt_map_playanim, STRING("mizu_02"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_01"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_02"), 1, 0)
    CALL(evt_map_playanim, STRING("hikari_01"), 1, 0)
    SWITCH(GSW(0))
        CASE_BETWEEN(90, 100)
        SPAWN_EVENT(jail_init_00)
    END_SWITCH
    CALL(evt_mobj_lock, STRING("mobj_lock00"), 16, 65, 10, -70, 0, PTR(evt_check00), PTR(evt_unlock00), GSWF(2832))
    IF_INT_EQUAL(GSWF(2832), 1)
        IF_INT_EQUAL(GSWF(2833), 1)
            SPAWN_EVENT(evt_opendoor00)
        ELSE
            SPAWN_EVENT(evt_unlock00_01)
        END_IF
    END_IF
    CALL(evt_mobj_lock, STRING("mobj_lock01"), PTR(key_table01), -245, 10, -70, 0, PTR(evt_check01), PTR(evt_unlock01), GSWF(2834))
    IF_INT_EQUAL(GSWF(2834), 1)
        IF_INT_EQUAL(GSWF(2835), 1)
            SPAWN_EVENT(evt_opendoor01)
        ELSE
            SPAWN_EVENT(evt_unlock01)
        END_IF
    END_IF
    SPAWN_EVENT_GET_ID(PTR(jail_alpha), GW(0))
    IF_INT_EQUAL(GSW(0), 98)
        SPAWN_EVENT(bluejail_event)
    END_IF
    IF_INT_EQUAL(GSW(0), 91)
        SPAWN_EVENT(toraware_elder)
    END_IF
    IF_INT_EQUAL(GSW(0), 99)
        SPAWN_EVENT(guide_sister_before)
    END_IF
    RETURN
EVT_END()
// clang-format on

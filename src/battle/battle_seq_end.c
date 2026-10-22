#include "battle/battle_seq_end.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "battle/battle_audience.h"
#include "battle/battle_stage.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>
#include "battle/battle_sub.h"
#include "battle/battle_actrecord.h"
#include "battle/battle_break_slot.h"
#include "battle/battle_seq.h"
#include "driver/mapdrv.h"
#include "driver/fadedrv.h"
#include "driver/seqdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_recovery.h"
#include "eff/eff_pointget.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "party.h"
#include "pmario_sound.h"
#include "eff/n64/eff_pokopi_pcharge_n64.h"
#include <stdio.h>
#include "evt/evt_cmd.h"
#include "evt/evt_batstage.h"
#include "evt/evt_fade.h"
#include "evt/evt_sub.h"
#include "evt/evt_snd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_audience.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_camera.h"
#include "battle/battle_message.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_803ACB10[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803ACB20[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_803ACB30[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803ACB40[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };
static s32 _check_guard_koura(EventEntry* event, BOOL isFirstCall);  // local to this file in the original

// Prototypes for functions that are not declared in a shared header.



// .sdata2 constants this file loads by name.
extern const f32 float_3_80420c6c;
extern const f32 float_4_80420c78;
extern const f32 float_0_80420c7c;
extern const f32 float_255_80420c88;
extern const f32 float_0p5_80420c8c;
extern const f32 float_neg20_80420c90;
extern const f32 float_50_80420c94;
extern const f32 float_1p1_80420c98;
extern const f32 float_25_80420c9c;
extern const f32 float_90_80420ca0;
extern const f32 float_2p5_80420ca4;
extern const f32 float_65_80420ca8;
extern const f32 float_180_80420cac;
extern const f32 float_7p5_80420cb0;
extern const f32 float_270_80420cb4;
extern const f32 float_4p5_80420cb8;
extern const f32 float_neg25_80420cbc;
extern const f32 float_128_80420cc0;
// sdata2 constants used by the display helpers below.
extern const f32 float_20_80420c48;
extern const f32 float_neg145_80420cf8;
extern const f32 float_10_80420cd8;
extern const f32 float_neg148_80420cfc;
// sdata2/rodata constants used by _lvup_select_object_disp.
extern const f32 float_deg2rad_80420c40;
extern const f32 float_16_80420c44;
extern const f32 float_2_80420c4c;
extern const f32 float_8_80420c50;
extern const f32 float_6_80420c54;
extern const f32 float_96_80420c58;
extern const f32 float_5_80420c5c;
extern const f32 float_0p8_80420c60;
extern const f32 float_1_80420c64;
extern const f32 float_1p2_80420c68;
extern const f32 float_neg0p6_80420c70;
extern const f32 float_0p6_80420c74;
extern const f64 double_to_int_802fe050;

// Remaining pool objects; definitions retain retail address order.
extern const f32 float_1p5_80420c80;
extern const f32 float_2160_80420c84;
extern const f32 float_neg50_80420cc4;
extern const f32 float_neg76p9_80420cc8;
extern const f32 float_21_80420ccc;
extern const f32 float_165p8_80420cd0;
extern const f32 float_24p2_80420cd4;
extern const f32 float_24_80420cdc;
extern const f32 float_18_80420ce0;
extern const f32 float_36_80420ce4;
extern const f32 float_0p9_80420ce8;
extern const f32 float_0p45_80420cec;
extern const f32 float_86p9_80420cf0;
extern const f32 float_16p2_80420cf4;
extern const f32 float_neg30_80420d00;
extern const f32 float_6p25_80420d04;
extern const f32 float_30_80420d08;
extern const f32 float_12p5_80420d0c;
extern const f32 float_32_80420d10;
extern const f32 float_neg70_80420d14;
extern const f32 float_neg10_80420d18;
extern const f32 float_130_80420d1c;
extern const f32 float_60_80420d20;
extern const f32 float_27_80420d24;
extern const f32 float_neg13_80420d28;
extern const f32 float_neg38_80420d2c;
extern const f32 float_neg40_80420d30;
extern const f32 float_200_80420d34;
extern const f32 float_neg200_80420d38;
extern const char str_fmt_2d[4];
extern const f32 float_neg160_80420d40;
extern const f32 float_225_80420d44;
extern const f32 float_neg163_80420d48;
extern const f32 float_900_80420d4c;
extern const f32 float_40_80420d50;
extern const char str_M_V_1[8];

//.data
RankUpData _rank_up_data[] = {
    {1, 0, "msg_menu_mario_name_0", "butai_01"},
    {10, 1, "msg_menu_mario_name_1", "butai_01"},
    {20, 2, "msg_menu_mario_name_2", "butai_02"},
    {30, 3, "msg_menu_mario_name_3", "butai_03"},
    {0xFFFF, 0, NULL, NULL},
};



// .sdata2 objects defined at the end of the file that the event scripts reference.
extern const f32 float_0_80420c1c;
extern const char str_M_V_2[8];
extern s32 _confetti_control_event[];

const char str_btl_msg_mj_last[] = "btl_msg_mj_last";
const Vec vec3_802fde84 = {0.0f, 0.0f, 0.0f};

// clang-format off
EVT_BEGIN(_curtain_close_event)
    BEGIN_SIBLING_EVENT
        CALL(evt_batstage_set_stg_dark, 80, 30, 0)
        CALL(evt_batstage_set_stg_dark_base, 80, 30, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_fade_entry, 24, 1000, 0, 0, 0)
        CALL(evt_fade_set_anim_ofs_pos, 0, 130)
        CALL(evt_fade_set_anim_virtual_pos, 0, 0, 80)
        CALL(evt_fade_end_wait)
    END_SIBLING_EVENT
    CALL(btlevtcmd_check_battleflag, LW(0), 256)
    IF_INT_NOT_EQUAL(LW(0), 0)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_AnimeChangePoseType, -3, 1, 42)
            CALL(btlevtcmd_GetPos, -3, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_MovePosition, -3, -300, LW(1), LW(2), 120, -1, 0)
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetUnitId, -4, LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
                CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 42)
                CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_MovePosition, LW(3), -330, LW(1), LW(2), 120, -1, 0)
            END_SIBLING_EVENT
        END_IF
    END_IF
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_intpl_init_float, 0, 0, FLOAT(-40.0f), 29)
        LOOP(30)
            CALL(evt_sub_intpl_get_float, LW(0), EVTDAT_POINTER_MAX)
            CALL(_nozzle_set_offset, STRING((char*)"nozzle_l"), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_intpl_init_float, 0, 0, FLOAT(-40.0f), 29)
        LOOP(30)
            CALL(evt_sub_intpl_get_float, LW(0), EVTDAT_POINTER_MAX)
            CALL(_nozzle_set_offset, STRING((char*)"nozzle_c"), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_intpl_init_float, 0, 0, FLOAT(-40.0f), 29)
        LOOP(30)
            CALL(evt_sub_intpl_get_float, LW(0), EVTDAT_POINTER_MAX)
            CALL(_nozzle_set_offset, STRING((char*)"nozzle_r"), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_onoff_battleflag, 0, 256)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_MS(1500)
    BEGIN_SIBLING_EVENT
        LOOP(6)
            CALL(evt_eff, STRING((char*)&float_0_80420c1c), STRING((char*)"confetti"), 13, 0, 300, 95, 180, 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(20)
        END_LOOP
    END_SIBLING_EVENT
    SPAWN_EVENT_GET_ID(PTR(_confetti_control_event), LW(0))
    CALL(_set_confetti_control_evtid, LW(0))
    CALL(evt_eff, STRING((char*)&float_0_80420c1c), STRING((char*)"levelup"), 0, 0, 50, 100, 60, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(48)
    CALL(_lvup_ap_recover_full)
    WAIT_MS(1000)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_FRAMES(2)
        CALL(_lvup_spot_on, 0)
        CALL(_lvup_spot_move, 0)
    END_SIBLING_EVENT
    CALL(_disable_enemy_unit_disp)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetUnitId, -4, LW(3))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_SetPos, LW(3), -230, 0, 90)
            CALL(btlevtcmd_ClearAllStatus, LW(3))
            CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
            CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 40)
            SET_VALUE(LW(0), -155)
            SET_VALUE(LW(1), 0)
            SET_VALUE(LW(2), 90)
            SET_VALUE(LW(6), 28)
            CALL(btlevtcmd_GetUnitKind, LW(3), LW(5))
            SWITCH(LW(5))
                CASE_OR_EQUAL(224)
                CASE_OR_EQUAL(225)
                CALL(btlevtcmd_SetMoveSoundDataWork, LW(3), STRING((char*)"SFX_PARTY_BATTLE_SUBMOVE1L"), STRING((char*)"SFX_PARTY_BATTLE_SUBMOVE1R"), 6, 12, 16, 0, 0)
                CALL(btlevtcmd_MovePosition, LW(3), LW(0), LW(1), LW(2), LW(6), -2, 1)
                CASE_END
                CASE_EQUAL(226)
                CALL(btlevtcmd_SetMoveSoundDataWork, LW(3), STRING((char*)"SFX_PARTY_BATTLE_SUBMOVE1L"), STRING((char*)"SFX_PARTY_BATTLE_SUBMOVE1R"), 0, 6, 6, 0, 0)
                CALL(btlevtcmd_MovePosition, LW(3), LW(0), LW(1), LW(2), LW(6), -2, 1)
                CASE_EQUAL(227)
                CALL(btlevtcmd_SetMoveSoundDataWork, LW(3), STRING((char*)"SFX_PARTY_BATTLE_SUBHANG1"), STRING((char*)"SFX_PARTY_BATTLE_SUBHANG1"), 6, 12, 16, 0, 0)
                CALL(btlevtcmd_MovePosition, LW(3), LW(0), LW(1), LW(2), LW(6), -2, 1)
                CASE_EQUAL(228)
                CALL(btlevtcmd_SetMoveSoundDataWork, LW(3), STRING((char*)"SFX_PARTY_BATTLE_SUBHANG1"), STRING((char*)"SFX_PARTY_BATTLE_SUBHANG1"), 0, 15, 13, 0, 0)
                CALL(btlevtcmd_MovePosition, LW(3), LW(0), LW(1), LW(2), LW(6), -2, 1)
                CASE_EQUAL(229)
                CALL(btlevtcmd_SetMoveSoundDataWork, LW(3), STRING((char*)"SFX_PARTY_BATTLE_SUBMOVE1L"), STRING((char*)"SFX_PARTY_BATTLE_SUBMOVE1R"), 0, 18, 18, 0, 0)
                CALL(btlevtcmd_MovePosition, LW(3), LW(0), LW(1), LW(2), LW(6), -2, 1)
                CASE_EQUAL(230)
                CALL(btlevtcmd_SetMoveSoundDataWork, LW(3), STRING((char*)"SFX_PARTY_BATTLE_SUBMOVE1L"), STRING((char*)"SFX_PARTY_BATTLE_SUBMOVE1R"), 0, 15, 13, 0, 0)
                CALL(btlevtcmd_MovePosition, LW(3), LW(0), LW(1), LW(2), LW(6), -2, 1)
            END_SWITCH
            CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 43)
        END_IF
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetPos, -3, -200, 0, 90)
    CALL(btlevtcmd_ClearAllStatus, -3)
    CALL(btlevtcmd_AnimeChangePoseType, -3, 1, 40)
    CALL(btlevtcmd_MovePosition, -3, 0, 0, 90, 75, -1, 0)
    CALL(btlevtcmd_snd_se, -3, STRING((char*)"SFX_VOICE_MARIO_PAUSE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING((char*)str_M_V_2))
    CALL(evt_audience_sound_cheer, 180, 40)
    WAIT_MS(1000)
    CALL(_lvup_select_object_init)
    BEGIN_SIBLING_EVENT
        CALL(_lvup_spot_off, 30)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_batstage_set_stg_dark, 0, 30, 0)
        CALL(evt_batstage_set_stg_dark_base, 0, 30, 0)
        CALL(btlevtcmd_BtlStageObjFlagOnOff, STRING((char*)"spotlight"), 1, 0)
        CALL(_lvup_obj_set_color, 0, 100, 128, 128, 255)
        CALL(_lvup_obj_set_color, 1, 100, 128, 128, 255)
        CALL(_lvup_obj_set_color, 2, 100, 128, 128, 255)
        CALL(evt_batstage_set_aud_dark, 0, 60, 0)
        CALL(evt_fade_entry, 25, 1000, 0, 0, 0)
        CALL(evt_fade_set_anim_ofs_pos, 0, 130)
        CALL(evt_fade_set_anim_virtual_pos, 0, 0, 80)
        CALL(evt_fade_end_wait)
    END_SIBLING_EVENT
    WAIT_MS(1000)
    BEGIN_SIBLING_EVENT_GET_ID(LW(0))
        CALL(_lvup_obj_set_color, 1, 255, 255, 255, 255)
        CALL(_lvup_disp_flag_onoff, 1, 32)
        CALL(_lvup_select_obj_disp_on)
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(0))
    CALL(_lvup_select_obj_lvup_param_disp)
    RETURN
EVT_END()

EVT_BEGIN(_confetti_control_event)
    SET_VALUE(LW(0), 1)
    SET_VALUE(LW(5), 5)
    LOOP(LW(5))
        SUBTRACT(LW(0), 1)
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            SET_VALUE(LW(0), 20)
            CALL(evt_eff, STRING((char*)&float_0_80420c1c), STRING((char*)"confetti"), 3, 0, 300, 5, 180, 0, 0, 0, 0, 0, 0, 0)
        END_IF
        WAIT_FRAMES(1)
        SET_VALUE(LW(5), 5)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(_jump_event)
    BEGIN_SIBLING_EVENT
        CALL(_lvup_object_noselect_delete)
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon_3d, STRING((char*)"SFX_BTL_LVUP_BLOCK_MOVE1"), 0, 125, 0, 0)
    CALL(_lvup_object_select_move)
    CALL(btlevtcmd_GetPos, -3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePoseType, -3, 1, 44)
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(10)
        CALL(evt_snd_sfxon_3d, STRING((char*)"SFX_BTL_LVUP_BLOCK_HIT1"), 0, 125, 0, 0)
        CALL(evt_btl_camera_shake_h, 0, 6, 0, 20, 13)
        CALL(_lvup_object_decide_bound)
    END_INLINE_EVENT
    CALL(btlevtcmd_SetFallAccel, -3, FLOAT(0.4f))
    CALL(btlevtcmd_JumpPosition, -3, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_snd_se, -3, STRING((char*)"SFX_VOICE_MARIO_PAUSE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING((char*)str_M_V_2))
    CALL(evt_audience_sound_cheer, 180, 40)
    RETURN
EVT_END()

EVT_BEGIN(_move_to_heart_event)
    CALL(evt_snd_sfxon_3d, STRING((char*)"SFX_BTL_LVUP_CURSOR1"), 0, 125, 0, 0)
    CALL(_lvup_obj_set_color, 0, 100, 128, 128, 255)
    CALL(_lvup_obj_set_color, 1, 100, 128, 128, 255)
    CALL(_lvup_obj_set_color, 2, 100, 128, 128, 255)
    CALL(_lvup_disp_flag_onoff, 0, 32)
    CALL(evt_sub_intpl_init, 5, 0, -30, 10)
    LOOP(11)
        CALL(evt_sub_intpl_get_value)
        CALL(btlevtcmd_BtlStageObjSetRotate, STRING((char*)"spotlight"), 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_BtlStageObjSetRotate, STRING((char*)"spotlight"), 0, 0, -30)
    CALL(_lvup_obj_set_color, 0, 255, 255, 255, 255)
    CALL(_lvup_disp_flag_onoff, 1, 32)
    RETURN
EVT_END()

EVT_BEGIN(_move_to_flower_event)
    CALL(evt_snd_sfxon_3d, STRING((char*)"SFX_BTL_LVUP_CURSOR1"), 0, 125, 0, 0)
    CALL(_lvup_obj_set_color, 0, 100, 128, 128, 255)
    CALL(_lvup_obj_set_color, 1, 100, 128, 128, 255)
    CALL(_lvup_obj_set_color, 2, 100, 128, 128, 255)
    CALL(_lvup_disp_flag_onoff, 0, 32)
    CALL(btlevtcmd_BtlStageObjGetRotate, STRING((char*)"spotlight"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(5))
    CALL(evt_sub_intpl_init, 5, LW(5), 0, 10)
    LOOP(11)
        CALL(evt_sub_intpl_get_value)
        CALL(btlevtcmd_BtlStageObjSetRotate, STRING((char*)"spotlight"), 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_BtlStageObjSetRotate, STRING((char*)"spotlight"), 0, 0, 0)
    CALL(_lvup_obj_set_color, 1, 255, 255, 255, 255)
    CALL(_lvup_disp_flag_onoff, 1, 32)
    RETURN
EVT_END()

EVT_BEGIN(_move_to_badge_point_event)
    CALL(evt_snd_sfxon_3d, STRING((char*)"SFX_BTL_LVUP_CURSOR1"), 0, 125, 0, 0)
    CALL(_lvup_obj_set_color, 0, 100, 128, 128, 255)
    CALL(_lvup_obj_set_color, 1, 100, 128, 128, 255)
    CALL(_lvup_obj_set_color, 2, 100, 128, 128, 255)
    CALL(_lvup_disp_flag_onoff, 0, 32)
    CALL(evt_sub_intpl_init, 5, 0, 30, 10)
    LOOP(11)
        CALL(evt_sub_intpl_get_value)
        CALL(btlevtcmd_BtlStageObjSetRotate, STRING((char*)"spotlight"), 0, 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_BtlStageObjSetRotate, STRING((char*)"spotlight"), 0, 0, 30)
    CALL(_lvup_obj_set_color, 2, 255, 255, 255, 255)
    CALL(_lvup_disp_flag_onoff, 1, 32)
    RETURN
EVT_END()

// clang-format on

s32 _koura_delete_event[] = {
    0x0003005B, (s32)_check_guard_koura, 0xFFFFFFFD, 0xFE363C8A,
    0x00020019, 0xFE363C8A, 0xFFFFFFFF, 0x0003005B,
    (s32)btlevtcmd_RunDataEventChild, 0xFE363C8A, 0x0000003E,
    0x00000021, 0x00000002, 0x00000001,
};

// clang-format off
__declspec(section ".sdata") EVT_BEGIN(_event_MajinaiEnd)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -36, STRING((char*)str_btl_msg_mj_last), 60)
    WAIT_FRAMES(60)
    RETURN
EVT_END()

__declspec(section ".sdata") EVT_BEGIN(_rank_up_event)
    CALL(_get_rank_up_msg, LW(0))
    CALL(evt_msg_print, 0, LW(0), 0, 0)
    RETURN
EVT_END()
// clang-format on


extern const char str_msg_menu_mario_name_0[];
extern s32 marioAttackEvent_MajinaiExpUp[];
extern BattleDispPose pose_table_nokotarou_stay[];

RankUpData* _get_rank_data(s32 level) {
    RankUpData* rank;

    for (rank = _rank_up_data; level != rank->startLevel; rank++) {
        if (!rank->rankUpMessage) {
            return NULL;
        }
    }
    return rank;
}

const char* BattleGetRankNameLabel(s32 level) {
    RankUpData* rank = _rank_up_data;
    RankUpData* found = NULL;

    while (level >= rank->startLevel) {
        if (rank->startLevel < 0) {
            break;
        }
        found = rank;
        rank++;
    }
    if (found == NULL) {
        return NULL;
    }
    return found->rankUpName;
}

// ---- btlseqEnd ------------------------------------------------------------
// Data tables in this unit's .data, reached as offsets from its first object,
// and the string bank in .rodata reached from its first string.

#pragma opt_lifetimes off // the target keeps unit/party/setup in the registers the default lifetime pass moves; also drops the two `mr rN, r0` moves
void btlseqEnd(BattleWork* wp) {
    BattleSeqEndWork* work;
    FieldBattleInfo* info;
    PouchData* pouch;
    BattleSeqEndWork* newWork;
    BattleUnit* unit;
    BattleUnit* party;
    EventEntry* event;
    s32 flag;
    s8 i;
    NpcBattleInfo* setup;

    work = wp->seqEndWork;
    info = wp->fbatInfo;
    pouch = pouchGetPtr();

    _audience_Whistle_control();

    switch (BattleGetSeq(wp, SEQ_END)) {
    case SEQ_END_INIT:
        btlsubResetMoveColorLvAll(wp);
        unit = BattleGetMarioPtr(wp);
        {
            s32 marioId = unit->mAlliance;
            s32 belong = BtlUnit_GetEnemyBelong(unit);
            u32 flags = wp->flags;

            if (flags & 0x40) {
                BattleInformationSetResult(info, 4);
            } else if (flags & 0x20) {
                BattleInformationSetResult(info, 5);
            } else if ((s32)wp->alliances[marioId].clearConditionMet != 0) {
                BattleInformationSetResult(info, 3);
            } else if ((s32)wp->alliances[belong].clearConditionMet != 0) {
                BattleInformationSetResult(info, 1);
            } else if (flags & BATTLE_ESCAPED) {
                BattleInformationSetResult(info, 4);
            }
        }
        switch (BattleInformationGetResult(info)) {
        case 1:
            if (fbatGetPointer()->endEvt != 0) {
                BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_EVT);
                return;
            }
            _check_audience_battle_end_exec();
            BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_CLEAR_STATUS);
            _BattleMarioKouraDelete();
            break;
        case 3:
            BattleSetSeq(wp, SEQ_END, SEQ_END_LOSE_FADE);
            break;
        case 4:
        case 5:
        default:
            _ExecAllUnitBattleEndEvent();
            BattleSetSeq(wp, SEQ_END, SEQ_END_FADE_OUT);
            BattleAudienceSoundSetVolAll(0, 0x1E);
            break;
        }
        newWork = BattleAlloc(0x2AC);
        wp->seqEndWork = newWork;
        btlseqEnd_DispInit(wp);
        newWork->unk0 = 0;
        newWork->effect = NULL;
        newWork->whistleTimer = irand(0x78) + 0x3C;
        newWork->confettiEvtId = 0;
        break;

    case SEQ_END_WIN_EVT:
        {
            FieldBattleData* fbat = fbatGetPointer();

            event = evtEntry(fbat->endEvt, 0xA, 0);
            fbat->endEvtId = event->eventId;
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_EVT_WAIT);
        return;

    case SEQ_END_WIN_EVT_WAIT:
        {
            FieldBattleData* fbat = fbatGetPointer();

            if (evtCheckID(fbat->endEvtId)) {
                break;
            }
            if (!BattleWaitAllActiveEvtEnd(wp)) {
                break;
            }
            if (fbat->field_44 != 0) {
                BattleSetSeq(wp, SEQ_TURN, 0x03000001);
                BattleSetSeq(wp, SEQ_UNKNOWN, 2);
                return;
            }
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_CLEAR_STATUS);
        _BattleMarioKouraDelete();
        newWork = BattleAlloc(0x2AC);
        wp->seqEndWork = newWork;
        btlseqEnd_DispInit(wp);
        newWork->unk0 = 0;
        return;

    case SEQ_END_LOSE_FADE:
        _ExecAllUnitBattleEndEvent();
        btl_camera_set_moveSpeedLv(0, 2);
        btl_camera_set_mode(0, 0x13);
        btl_camera_set_posoffset(0, float_0_80420c7c, float_0_80420c7c, float_0_80420c7c);
        fadeEntry(0xA, 0xC8, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
        psndBGMOff(0x201);
        BattleSetSeq(wp, SEQ_END, SEQ_END_LOSE_FADE_WAIT);
        break;

    case SEQ_END_LOSE_FADE_WAIT:
        if (!fadeIsFinish()) {
            break;
        }
        if (!(wp->fbatInfo->setup->flags & NPC_BATTLE_LOSS_RETURNS_TO_FIELD)) {
            seqSetSeq(SEQ_GAME_OVER, (char*)1, NULL);
        } else {
            BattleSetSeq(wp, SEQ_END, SEQ_END_RETURN);
        }
        break;

    case SEQ_END_WIN_CLEAR_STATUS:
        unit = BattleGetMarioPtr(wp);
        party = BattleGetPartyPtr(wp);
        if (party != NULL && BtlUnit_CheckStatusFlag(party, kStatusFlipped)) {
            party->flippedTurns = 0;
            BtlUnit_OffStatusFlag(party, kStatusFlipped);
            BtlUnit_GetPartsPtr(party, BtlUnit_GetBodyPartsId(party))->poseTable = pose_table_nokotarou_stay;
        }
        BattleConsumeReserveItem();
        for (i = 0; i < 0x1C; i++) {
            if (i >= 0x1C || i < 0x19) {
                if (party != NULL) {
                    BtlUnit_SetStatus(party, i, 0, 0);
                }
                BtlUnit_SetStatus(unit, i, 0, 0);
            }
        }
        if (unit->mAttributeFlags & kVeiled) {
            party = BattleGetPartyPtr(wp);
            if (party == NULL) {
                break;
            }
            event = evtEntry(BtlUnit_GetData(party, 3), 0xA, 0);
            event->unitId = party->unitId;
            party->attackEventId = event->eventId;
            break;
        } else {
            BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_START);
        }

    case SEQ_END_WIN_START:
        if (!BattleWaitAllActiveEvtEnd(wp)) {
            break;
        }
        BattleAudienceSoundClap(-1, 0);
        _ExecAllUnitBattleEndEvent();
        BattleInformationSetDropMaterial(info);
        pouch = pouchGetPtr();
        unit = BattleGetMarioPtr(wp);
        if (pouch->mMerleeNextCurseType == 3) {
            wp->field_18FE1 = 3;
            BattleMajinaiDone();
            BattleMajinaiEndCheck();
            event = evtEntry(marioAttackEvent_MajinaiExpUp, 0xA, 0);
            event->unitId = unit->unitId;
            unit->attackEventId = event->eventId;
            BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_MAJINAI_WAIT);
        } else {
            BattleSetSeq(wp, SEQ_END, SEQ_END_EXP_INIT);
        }
        break;

    case SEQ_END_WIN_MAJINAI_WAIT:
        if (BattleWaitAllActiveEvtEnd(wp)) {
            BattleSetSeq(wp, SEQ_END, SEQ_END_EXP_INIT);
        }
        break;

    case SEQ_END_EXP_INIT:
        BattleFogForceStop();
        work->lvupPending = 0;
        work->stockExp = BattleGetStockExp(wp);
        if (wp->field_18FE1 == 3) {
            work->stockExp *= 2;
        }
        if (pouch->level >= 0x63) {
            work->stockExp = 0;
        }
        if (work->stockExp > 0x64) {
            work->stockExp = 0x64;
        }
        work->expRemaining = work->stockExp;
        wp->dispFlags &= ~2;
        if (work->stockExp > 0) {
            _GetExpIcon_Init(work);
        }
        work->timer = 0x78;
        btl_camera_set_moveSpeedLv(0, 2);
        if (!(wp->flags & 0x100)) {
            btl_camera_set_mode(0, 0x11);
        } else {
            btl_camera_set_mode(0, 0x12);
        }
        btl_camera_set_posoffset(0, float_0_80420c7c, float_0_80420c7c, float_0_80420c7c);
        BattleSetSeq(wp, SEQ_END, SEQ_END_EXP_DELAY);
        BattleAudienceSoundCheer(0x3C, 0x3C);
        BattleAudienceSoundClap(-1, 0);
        BattleAudienceSoundWhistle();
        flag = 0;
        party = BattleGetPartyPtr(wp);
        if (party != NULL && BtlUnit_CheckStatus(party, STATUS_INSTANT_KILL)) {
            flag = 1;
        }
        if (flag == 0) {
            psndBGMOff(0x400);
            psndBGMOn(1, "BGM_BATTLE_WIN1");
        } else {
            psndBGMOff(0x400);
            psndBGMOn(1, "BGM_BATTLE_WIN2");
        }
        /* fallthrough */
    case SEQ_END_EXP_DELAY:
        work->timer--;
        if (work->timer > 0) {
            break;
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_EXP_START);
        break;

    case SEQ_END_EXP_START:
        if (work->stockExp > 0) {
            BattleUnit* u = BattleGetMarioPtr(wp);
            BattleUnit* p = BattleGetPartyPtr(wp);
            event = evtEntry(BtlUnit_GetData(u, 0x40), 0xA, 0);
            if (event != NULL) {
                event->unitId = u->unitId;
                u->attackEventId = event->eventId;
            }
            if (p != NULL) {
                event = evtEntry(BtlUnit_GetData(p, 0x40), 0xA, 0);
                if (event != NULL) {
                    event->unitId = p->unitId;
                    p->attackEventId = event->eventId;
                }
            }
            if (work->effect == NULL) {
                work->effect = effPointGetEntry(0, float_0_80420c7c, float_90_80420ca0, float_0_80420c7c,
                                                work->stockExp, 0);
            }
            BattleSetSeq(wp, SEQ_END, SEQ_END_EXP_COUNT);
            break;
        }
        if (!(wp->flags & BATTLE_MAJINAI_END)) {
            BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_POSE);
            break;
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_MAJINAI_END);
        break;

    case SEQ_END_EXP_COUNT:
        if (work->expRemaining > 0) {
            work->timer++;
            if (BattlePadCheckNow(0x300) && work->timer < 0x3E8) {
                work->timer += 0x3E8;
            }
            if ((work->timer & 3) == 0 || work->timer >= 0x3E8) {
                work->expRemaining--;
                pouch->mStarPoints++;
                if ((work->timer & 7) == 0) {
                    psndSFXOn("SFX_STAR_POINT_GET1");
                }
            }
            if (pouch->mStarPoints >= 0x64) {
                pouch->mStarPoints = 0;
                work->lvupPending = 1;
                if (pouch->level + 1 >= 0x63) {
                    work->expRemaining = 0;
                }
            }
            break;
        }
        if (work->timer >= 0x3E8) {
            work->timer = 0xA;
        } else if (work->timer < 0x78) {
            work->timer = 0x8C - work->timer;
        } else {
            work->timer = 0x14;
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_EXP_COUNT_END);
        break;

    case SEQ_END_EXP_COUNT_END:
        work->timer--;
        if (work->timer > 0) {
            break;
        }
        work->timer = 0x3C;
        _GetExpIcon_End(work);
        BattleSetSeq(wp, SEQ_END, SEQ_END_EXP_END_WAIT);
        effSoftDelete(work->effect);
        if (work->lvupPending != 0) {
            work->timer = 0x3C;
            break;
        }
        work->timer = 5;
        break;

    case SEQ_END_EXP_END_WAIT:
        work->timer--;
        if (work->timer > 0) {
            break;
        }
        work->effect = NULL;
        BattleSetSeq(wp, SEQ_END, SEQ_END_MAJINAI_END);
        break;

    case SEQ_END_MAJINAI_END:
        if (wp->flags & BATTLE_MAJINAI_END) {
            unit = BattleGetMarioPtr(wp);
            event = evtEntry(_event_MajinaiEnd, 0xA, 0);
            event->unitId = unit->unitId;
            unit->attackEventId = event->eventId;
            BattleSetSeq(wp, SEQ_END, SEQ_END_MAJINAI_END_WAIT);
            break;
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_CHECK);
        break;

    case SEQ_END_MAJINAI_END_WAIT:
        if (!BattleWaitAllActiveEvtEnd(wp)) {
            break;
        }
        if (work->stockExp > 0) {
            BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_CHECK);
            break;
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_POSE);
        break;

    case SEQ_END_LVUP_CHECK:
        work->dispFlags &= ~1;
        if (work->lvupPending != 0) {
            work->lvupSelect = 1;
            work->unk14 = 0;
            work->lvupCount = 3;
            work->timer = 0;
            BattleAudience_WinSetActive(0);
            BattleBreakSlot_HideReel();
            BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_CURTAIN);
            break;
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_POSE);
        break;

    case SEQ_END_LVUP_CURTAIN:
        if (work->timer == 0) {
            event = evtEntry(_curtain_close_event, 0xA, 0);
            work->waitEvtId = event->eventId;
            BattleStatusWindowSystemOff();
            psndBGMOff(0x400);
            psndBGMOn(1, "BGM_BATTLE_WIN3");
        }
        work->timer++;
        if (evtCheckID(work->waitEvtId)) {
            break;
        }
        work->waitEvtId = 0;
        BattleStatusWindowSystemOn();
        BattleStatusWindowEventOn();
        BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_SELECT);
        break;

    case SEQ_END_LVUP_SELECT:
        flag = work->lvupSelect;
        if (work->waitEvtId != 0) {
            if (evtCheckID(work->waitEvtId)) {
                break;
            }
            work->waitEvtId = 0;
        }
        if (BattlePadCheckTrigger(0x80000)) {
            if (work->lvupSelect < work->lvupCount - 1) {
                work->lvupSelect++;
            }
        }
        if (BattlePadCheckTrigger(0x40000)) {
            if (work->lvupSelect > 0) {
                work->lvupSelect--;
            }
        }
        if (work->lvupSelect != flag) {
            event = NULL;
            switch (work->lvupSelect) {
            case 0: event = evtEntry(_move_to_heart_event, 0xA, 0); break;
            case 1: event = evtEntry(_move_to_flower_event, 0xA, 0); break;
            case 2: event = evtEntry(_move_to_badge_point_event, 0xA, 0); break;
            }
            if (event != NULL) {
                work->waitEvtId = event->eventId;
                break;
            }
        }
        if (BattlePadCheckTrigger(0x100)) {
            s32 f2 = 0;
            switch (work->lvupSelect) {
            case 0: if (pouch->baseMaxHP < 0xC8) f2 = 1; break;
            case 1: if (pouch->baseMaxFP < 0xC8) f2 = 1; break;
            case 2: if (pouch->maximumBP < 0x63) f2 = 1; break;
            }
            if (f2 != 0) {
                work->confirmSel = 0;
                work->unk28 = 0;
                work->confirmCount = 2;
                work->dispFlags &= ~SEQEND_DISP_LVUP_HELP;
                BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_JUMP_WAIT);
                event = evtEntry(_jump_event, 0xA, 0);
                work->waitEvtId = event->eventId;
            }
        }
        break;

    case SEQ_END_LVUP_CONFIRM:
        if (BattlePadCheckTrigger(0x20000)) {
            work->confirmSel++;
            if (work->confirmSel >= work->confirmCount) {
                work->confirmSel = 0;
            }
        }
        if (BattlePadCheckTrigger(0x10000)) {
            work->confirmSel--;
            if (work->confirmSel < 0) {
                work->confirmSel = work->confirmCount - 1;
            }
        }
        if (BattlePadCheckTrigger(0x200)) {
            work->dispFlags &= ~SEQEND_DISP_LVUP_CONFIRM;
            BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_SELECT);
        }
        if (BattlePadCheckTrigger(0x100)) {
            if (work->confirmSel == 0) {
                work->dispFlags &= ~(SEQEND_DISP_LVUP_CONFIRM | SEQEND_DISP_LVUP_OBJECTS);
                BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_APPLY);
                break;
            }
            work->dispFlags &= ~SEQEND_DISP_LVUP_CONFIRM;
            BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_SELECT);
        }
        break;

    case SEQ_END_LVUP_JUMP_WAIT:
        if (!evtCheckID(work->waitEvtId)) {
            work->waitEvtId = 0;
            BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_APPLY);
        }
        break;

    case SEQ_END_LVUP_APPLY:
        pouch->level++;
        switch (work->lvupSelect) {
        case 0: pouch->baseMaxHP += 5; break;
        case 1: pouch->baseMaxFP += 5; break;
        case 2: pouch->maximumBP += 3; break;
        }
        pouchReviseMarioParam();
        pouch->currentHP = pouch->maximumHP;
        pouch->currentFP = pouch->maximumFP;
        unit = BattleGetMarioPtr(wp);
        BtlUnit_SetParamFromPouch(unit);
        party = BattleGetPartyPtr(wp);
        if (party != NULL) {
            s32 id = BattleTransPartyId(party->currentKind);

            if (id != 0 && id < 8) {
                pouch->partyData[id].currentHP = pouch->partyData[id].maximumHP;
                party->currentHp = party->maxHp;
            }
        }
        {
            NpcBattleInfo* su;
            BattleUnit* uu;

            flag = 1;
            su = wp->fbatInfo->setup;
            uu = BattleGetMarioPtr(wp);
            party = BattleGetPartyPtr(wp);
            if ((uu->mStatusFlags & kStatusDanger) || (uu->mStatusFlags & kStatusPeril)) {
                flag = 0;
            } else if (party != NULL) {
                if ((uu->mStatusFlags & kStatusDanger) || (uu->mStatusFlags & kStatusPeril) ||
                    BtlUnit_CheckStatus(party, STATUS_INSTANT_KILL)) {
                    flag = 0;
                }
            }
            if (flag != 0) {
                if (su->flags & 0x20000000) {
                    BtlUnit_snd_se(uu, "SFX_VOICE_MARIO_PAUSE3", 0xF1194D80, 0);
                } else {
                    BtlUnit_snd_se(uu, "SFX_VOICE_MARIO_PAUSE1", 0xF1194D80, 0);
                }
            }
        }
        BtlUnit_SetBodyAnim(BattleGetMarioPtr(wp), str_M_V_2);
        BattleAudienceSoundCheer(0xB4, 0x28);
        work->timer = 0xB4;
        BattleSetSeq(wp, SEQ_END, SEQ_END_LVUP_CHEER);
        unit = BattleGetMarioPtr(wp);
        party = BattleGetPartyPtr(wp);
        if (unit != NULL) {
            effRecoveryEntry(4, 0, unit->mPosition.x, float_40_80420d50 + unit->mPosition.y, unit->mPosition.z);
        }
        if (party != NULL) {
            effRecoveryEntry(4, 0, party->mPosition.x, float_40_80420d50 + party->mPosition.y, party->mPosition.z);
        }
        break;

    case SEQ_END_LVUP_CHEER:
        work->timer--;
        if (work->timer == 0x78) {
            unit = BattleGetMarioPtr(wp);
            if (unit != NULL) {
                effRecoveryEntry(5, 0, unit->mPosition.x, float_40_80420d50 + unit->mPosition.y, unit->mPosition.z);
            }
        }
        if (work->timer > 0) {
            break;
        }
        if (work->confettiEvtId != 0) {
            evtDeleteID(work->confettiEvtId);
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_RANK_UP);
        break;

    case SEQ_END_RANK_UP:
        if (!_get_rank_data(pouch->level)) {
            BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_DONE);
            break;
        }
        pouch->rank = _get_rank_data(pouch->level)->rankId;
        event = evtEntry(_rank_up_event, 0xA, 0);
        work->rankUpEvtId = event->eventId;
        BattleSetSeq(wp, SEQ_END, SEQ_END_RANK_UP_WAIT);
        /* fallthrough */
    case SEQ_END_RANK_UP_WAIT:
        if (!evtCheckID(work->rankUpEvtId)) {
            BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_DONE);
        }
        break;

    case SEQ_END_WIN_POSE:
        {
            NpcBattleInfo* su;
            BattleUnit* uu;

            flag = 1;
            su = wp->fbatInfo->setup;
            uu = BattleGetMarioPtr(wp);
            party = BattleGetPartyPtr(wp);
            if ((uu->mStatusFlags & kStatusDanger) || (uu->mStatusFlags & kStatusPeril)) {
                flag = 0;
            } else if (party != NULL) {
                if ((uu->mStatusFlags & kStatusDanger) || (uu->mStatusFlags & kStatusPeril) ||
                    BtlUnit_CheckStatus(party, STATUS_INSTANT_KILL)) {
                    flag = 0;
                }
            }
            if (flag == 0) {
                if (su->flags & 0x20000000) {
                    BtlUnit_snd_se(uu, "SFX_VOICE_MARIO_RELIEF1", 0xF1194D80, 0);
                } else {
                    BtlUnit_snd_se(uu, "SFX_VOICE_MARIO_RELIEF2", 0xF1194D80, 0);
                }
            }
        }
        BtlUnit_SetBodyAnim(BattleGetMarioPtr(wp), str_M_V_1);
        work->timer = 0x3C;
        BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_POSE_WAIT);
        break;

    case SEQ_END_WIN_POSE_WAIT:
        if (--work->timer > 0) {
            break;
        }
        BtlUnit_SetBodyAnimType(BattleGetMarioPtr(wp), 0x2A);
        if (BattleGetPartyPtr(wp) != NULL) {
            party = BattleGetPartyPtr(wp);
            if (!(party->currentKind == UNIT_KOOPS && party->flippedTurns >= 1)) {
                BtlUnit_SetBodyAnimType(BattleGetPartyPtr(wp), 0x2A);
            }
        }
        BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_POSE_DONE);
        break;

    case SEQ_END_WIN_POSE_DONE:
        BattleSetSeq(wp, SEQ_END, SEQ_END_WIN_DONE);
        break;

    case SEQ_END_WIN_DONE:
        BattleSetSeq(wp, SEQ_END, SEQ_END_FADE_OUT);
        BattleAudienceSoundSetVolAll(0, 0x1E);
        {
            NpcEntry* target = fbatGetPointer()->target;

            if (target != NULL) {
                target->interruptFlags |= 0x100;
            }
        }
        break;

    case SEQ_END_FADE_OUT:
        fadeEntry(0xA, 0xC8, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
        BattleSetSeq(wp, SEQ_END, SEQ_END_FADE_WAIT);
        psndBGMOff(0x201);
        /* fallthrough */
    case SEQ_END_FADE_WAIT:
        if (fadeIsFinish()) {
            BattleSetSeq(wp, SEQ_END, SEQ_END_RETURN);
        }
        break;

    case SEQ_END_RETURN:
        {
            s32 id;
            NpcBattleInfo* su;
            BattleUnit* uu;

            party = BattleGetPartyPtr(wp);
            flag = (party != NULL) ? party->currentKind : 0;
            id = BattleTransPartyId((BattleUnitType)flag);
            su = wp->fbatInfo->setup;
            uu = BattleGetMarioPtr(wp);
            if ((su->flags & NPC_BATTLE_LOSS_RETURNS_TO_FIELD) && uu->currentHp <= 0) {
                uu->currentHp = 1;
            }
            BattleSetMarioParamToFieldBattle(wp);
            BattleFree(wp->seqEndWork);
            wp->seqEndWork = NULL;
            BattleSetSeq(wp, SEQ_UNKNOWN, 4);
            marioGetPtr()->dispFlags &= ~1;
            if (flag != 0) {
                marioPartyEntry(id);
            } else {
                marioPartyKill();
            }
        }
        {
            PartyEntry* p = partyGetPtr(marioGetPartyId());

            if (p != NULL) {
                p->flags2 &= ~1;
            }
        }
        BtlActRec_JudgeRuleKeep();
        BattleAudienceSoundStop(1);
        break;
    }

    btlseqEnd_DispMain(8, wp);
}
#pragma opt_lifetimes reset

void _ExecAllUnitBattleEndEvent(void) {
    BattleWork* wp;
    s32 i;

    wp = _battleWorkPointer;
    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL) {
            void* data = BtlUnit_GetData(unit, 0x3F);

            if (data != NULL) {
                EventEntry* event = evtEntry(data, 0xA, 0);
                event->unitId = unit->unitId;
            }
        }
    }
}

void btlseqEnd_DispInit(BattleWork* wp) {
    wp->seqEndWork->dispFlags = 0;
}

// The parameter copy keeps its own web only with propagation off; it then colours ahead of `work` (r31/r30), as in
// retail (battleSortPhaseMoveTable in battle_seq.c has the same shape).
#pragma opt_propagation off
void btlseqEnd_DispMain(CameraId camId, BattleWork* wp) {
    BattleWork* param = wp;
    BattleSeqEndWork* work = param->seqEndWork;

    if (work == NULL) {
        return;
    }
    if (work->dispFlags & SEQEND_DISP_EXP_ICONS) {
        _GetExpIcon_Main(param);
        dispEntry(camId, float_900_80420d4c, 1, _GetExpIcon_Disp, param);
    }
    if (work->dispFlags & SEQEND_DISP_MARIO_EXP) {
        dispEntry(camId, float_900_80420d4c, 1, _MarioExpDisp, param);
    }
    if (work->dispFlags & SEQEND_DISP_LVUP_OBJECTS) {
        Vec pos = vec3_802fde84;
        dispEntry(4, dispCalcZ(&pos), 2, _lvup_select_object_disp, param);
    }
    if (work->dispFlags & SEQEND_DISP_LVUP_CONFIRM) {
        dispEntry(camId, float_900_80420d4c, 1, _LvupParamConfirmDisp, param);
    }
    if (work->dispFlags & SEQEND_DISP_LVUP_HELP) {
        dispEntry(camId, float_900_80420d4c, 1, L__LvupParamHelpMsgDisp, param);
    }
}
#pragma opt_propagation reset

void _MarioExpDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    BattleSeqEndWork* work = wp->seqEndWork;
    char buf[0x10];
    Vec pos;
    u32 len;

    pouchGetPtr();
    pos.x = float_200_80420d34;
    pos.y = float_neg200_80420d38;
    pos.z = float_0_80420c7c;
    if (work != NULL) {
        sprintf(buf, str_fmt_2d, work->expRemaining);
        len = strlen(buf);
        windowDispGX_Waku_col(float_180_80420cac, float_neg160_80420d40, float_12p5_80420d0c * len + float_65_80420ca8,
                              float_32_80420d10, float_10_80420cd8, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        iconDispGx(float_1_80420c64, &pos, 0x10, 0x18C);
        FontDrawStart();
        FontDrawString(float_225_80420d44, float_neg163_80420d48, buf);
    }
}

void _LvupParamConfirmDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    const s32* selection = &wp->seqEndWork->confirmSel;
    Vec pos;
    char buf[0x14];
    u32 len;
    f32 halfWidth;

    pouchGetPtr();
    sprintf(buf, msgSearch("btl_msg_lvup_ok"));
    len = strlen(buf);
    windowDispGX_Waku_col(float_neg30_80420d00 - (halfWidth = float_6p25_80420d04 * len), float_30_80420d08,
                          float_12p5_80420d0c * len + float_65_80420ca8, float_32_80420d10, float_10_80420cd8, 0,
                          (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    windowDispGX_Waku_col(float_neg70_80420d14, float_neg10_80420d18, float_130_80420d1c, float_60_80420d20,
                          float_10_80420cd8, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    FontDrawStart();
    FontDrawString(float_0_80420c7c - halfWidth, float_27_80420d24, buf);
    FontDrawString(float_neg20_80420c90, float_neg13_80420d28, msgSearch("btl_msg_lvup_yes"));
    FontDrawString(float_neg20_80420c90, float_neg38_80420d2c, msgSearch("btl_msg_lvup_no"));
    pos.x = float_neg40_80420d30;
    pos.y = (f32)(-50 - *selection * 25);
    pos.z = float_0_80420c7c;
    iconDispGx(float_1_80420c64, &pos, 0x10, 0x18C);
}

// The window y/height reuse the truncated lines + 1; width keeps clrlwi plus srawi/addze.
void L__LvupParamHelpMsgDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    BattleSeqEndWork* work = wp->seqEndWork;
    PouchData* pouch;
    const char* msg;
    u16 lines;
    s32 width;
    f32 zero;

    pouch = pouchGetPtr();
    switch (work->lvupSelect) {
    case 0:
        msg = msgSearch("help_btl_levelup_hp");
        if (pouch->baseMaxHP >= 0xC8) {
            msg = msgSearch("helperr_btl_levelup_hp");
        }
        break;
    case 1:
        msg = msgSearch("help_btl_levelup_fp");
        if (pouch->baseMaxFP >= 0xC8) {
            msg = msgSearch("helperr_btl_levelup_fp");
        }
        break;
    case 2:
        msg = msgSearch("help_btl_levelup_bp");
        if (pouch->maximumBP >= 0x63) {
            msg = msgSearch("helperr_btl_levelup_bp");
        }
        break;
    }
    width = FontGetMessageWidthLine(msg, &lines);
    lines++;
    zero = float_0_80420c7c;
    windowDispGX_Waku_col(zero - (f32)(width / 2) - float_20_80420c48,
                          float_neg145_80420cf8 + (f32)((lines - 1) * 29), (f32)(width + 40),
                          (f32)(lines * 29 + 1), float_10_80420cd8, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    FontDrawStart_alpha(0xFF);
    FontDrawMessage((s32)(zero - (f32)(width / 2)), (s32)float_neg148_80420cfc + (lines - 1) * 29, msg);
}

void _GetExpIcon_Init(BattleSeqEndWork* arg) {
    BattleSeqEndWork* work = arg;
    s32 i;
    char name[0x10];

    for (i = 0; i < 10; i++) {
        sprintf(name, "_Bgsp%d", i);
        iconEntry2D(name, 0x18C);
        work->iconPtr[i] = iconNameToPtr(name);
        iconSetScale(name, float_0p9_80420ce8);
        iconFlagOn(name, 2);
        work->iconFrames[i] = 0;
        work->iconPos[i].x =
            float_36_80420ce4 * (f32)i + (float_24_80420cdc - float_18_80420ce0 * (f32)(work->expRemaining / 10));
        work->iconPos[i].y = float_25_80420c9c;
        work->iconPos[i].z = float_0_80420c7c;
        iconSetPos(name, work->iconPos[i].x,
                   work->iconPos[i].y, work->iconPos[i].z);
    }
    for (i = 10; i < 0x13; i++) {
        sprintf(name, "_Bgsp%d", i);
        iconEntry2D(name, 0x18C);
        work->iconPtr[i] = iconNameToPtr(name);
        iconSetScale(name, float_0p45_80420cec);
        iconFlagOn(name, 2);
        work->iconFrames[i] = 0;
        work->iconPos[i].x = float_86p9_80420cf0 - float_16p2_80420cf4 * (f32)(i - 9);
        work->iconPos[i].y = float_0_80420c7c;
        work->iconPos[i].z = float_0_80420c7c;
        iconSetPos(name, work->iconPos[i].x,
                   work->iconPos[i].y, work->iconPos[i].z);
    }
    work->dispFlags |= SEQEND_DISP_EXP_ICONS;
}

void _GetExpIcon_Main(BattleWork* wp) {
    BattleSeqEndWork* work = wp->seqEndWork;
    s32 exp = work->expRemaining;
    s32 j;

    for (j = 0; j < exp / 10; j++) {
        iconFlagOff(work->iconPtr[j]->name, 2);
        if (work->iconAlpha[j] <= 0) {
            work->iconAlpha[j] = 0x40;
        } else {
            work->iconAlpha[j] += 0x40;
            if (work->iconAlpha[j] > 0xFF) {
                work->iconAlpha[j] = 0xFF;
            }
        }
        if (work->iconFrames[j] < 0x10) {
            work->iconFrames[j] += 1;
        }
        work->iconPos[j].x =
            float_36_80420ce4 * (f32)j + (float_24_80420cdc - float_18_80420ce0 * (f32)(work->expRemaining / 10));
    }
    for (; j < 10; j++) {
        iconFlagOn(work->iconPtr[j]->name, 2);
        work->iconAlpha[j] = 0;
        work->iconFrames[j] = 0;
    }
    {
        for (j = 10; j - 10 < exp % 10; j++) {
            iconFlagOff(work->iconPtr[j]->name, 2);
            if (work->iconAlpha[j] <= 0) {
                work->iconAlpha[j] = 0x40;
            } else {
                work->iconAlpha[j] += 0x40;
                if (work->iconAlpha[j] > 0xFF) {
                    work->iconAlpha[j] = 0xFF;
                }
            }
        }
        for (; j < 0x13; j++) {
            iconFlagOn(work->iconPtr[j]->name, 2);
            work->iconAlpha[j] = 0;
        }
    }
    for (j = 0; j < 0x13; j++) {
        iconSetAlpha(work->iconPtr[j]->name,
                     (u8)work->iconAlpha[j]);
        iconSetPos(work->iconPtr[j]->name,
                   work->iconPos[j].x, work->iconPos[j].y + float_0_80420c7c,
                   work->iconPos[j].z);
    }
}

void _GetExpIcon_Disp(CameraId camId, void* param) {
    GXColor color = {0xFF, 0xFF, 0xFF, 0x80};

    windowDispGX_Waku_col(float_neg76p9_80420cc8, float_21_80420ccc, float_165p8_80420cd0, float_24p2_80420cd4,
                          float_10_80420cd8, 0, color);
}

// Keep named objects between the compiler colour templates, in retail order.
__declspec(section ".sdata2") const f32 float_0_80420c1c = 0.0f;
__declspec(section ".sdata2") const char str_M_V_2[8] = "M_V_2";

void _GetExpIcon_End(BattleSeqEndWork* work) {
    s32 i;

    work->dispFlags &= ~SEQEND_DISP_EXP_ICONS;
    for (i = 0; i < 0x13; i++) {
        iconDelete(work->iconPtr[i]->name);
    }
}

s32 _nozzle_set_offset(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    f32 offset = evtGetFloat(event, args[1]);
    BattleStageObject* obj = BattleSearchObjectPtr((const char*)id);

    if (obj != NULL) {
        obj->mPositionOffset1.y = offset;
    }
    return 2;
}

s32 _lvup_ap_recover_full(void) {
    BattleAudience* audience = BattleAudienceBaseGetPtr();

    audience->excited = TRUE;
    pouchSetAP(pouchGetMaxAP());
    return 2;
}

s32 _disable_enemy_unit_disp(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        BattleUnitType type;

        if (unit == NULL) {
            continue;
        }
        if ((u32)unit->flags & 0x80000000) {
            continue;
        }
        type = unit->currentKind;
        if (type == UNIT_SYSTEM) {
            continue;
        }
        if (type == UNIT_MARIO) {
            continue;
        }
        if (type >= TYPE_PARTNER_MIN && type < TYPE_PARTNER_MAX && unit->mAlliance == 0 && type != UNIT_BOBBERY) {
            continue;
        }
        {
            void* data = BtlUnit_GetData(unit, 0x42);

            if (data != NULL) {
                EventEntry* e = evtEntry(data, 0xA, 0);
                e->unitId = unit->unitId;
            } else if (unit->currentKind != UNIT_BOBBERY) {
                unit->mAttributeFlags |= 0x21000000;
            }
        }
    }
    {
        BattleStageObject* obj = BattleSearchObjectPtr("if_body");

        if (obj != NULL) {
            wp->field_180C0.stageFlags |= 0x20;
            obj->mFlags |= BSO_FLAG_HIDDEN;
            mapGrpFlagOn("if_body", 1);
            mapGrpFlagOn("if_wire", 1);
        }
    }
    return 2;
}

s32 _set_confetti_control_evtid(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 val = *args++;

    work->confettiEvtId = evtGetValue(event, val);
    return 2;
}

s32 _lvup_select_obj_disp_on(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;

    work->dispFlags |= SEQEND_DISP_LVUP_OBJECTS;
    return 2;
}

s32 _lvup_select_obj_lvup_param_disp(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 i;

    for (i = 0; i < 3; i++) {
        work->objs[i].state = 1;
    }
    return 2;
}

s32 _lvup_obj_set_color(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    LvupSelectObj* obj = &work->objs[evtGetValue(event, args[0])];

    obj->color.r = evtGetValue(event, args[1]);
    obj->color.g = evtGetValue(event, args[2]);
    obj->color.b = evtGetValue(event, args[3]);
    obj->color.a = evtGetValue(event, args[4]);
    return 2;
}

s32 _lvup_disp_flag_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 flags = evtGetValue(event, args[0]);
    u32 mask = args[1];

    if (flags != 0) {
        work->dispFlags = work->dispFlags | mask;
    } else {
        work->dispFlags = work->dispFlags & ~mask;
    }
    return 2;
}

// Keep named objects between the compiler colour templates, in retail order.
__declspec(section ".sdata2") const u32 dat_80420c28 = 0xFFFFFFFF;

s32 _lvup_spot_on(EventEntry* event, BOOL isFirstCall) {
    BattleSpotWork* spot = BatSpotGetPtr();
    BattleSpotEntry* entry;
    s32 code;

    s32* args = event->args;
    spot->skipRender = TRUE;
    code = evtGetValue(event, args[0]);
    entry = &spot->entries[code];

    switch (code) {
    case 0:
        entry->pos = (Vec){float_25_80420c9c * (f32)code + float_neg50_80420cc4, 25.0f, -50.0f};
        break;
    case 1:
        entry->pos = (Vec){float_25_80420c9c * (f32)code + float_neg50_80420cc4, -10.0f, -50.0f};
        break;
    case 2:
        entry->pos = (Vec){float_25_80420c9c * (f32)code + float_neg50_80420cc4, 25.0f, -50.0f};
        break;
    case 3:
        entry->pos = (Vec){float_25_80420c9c * (f32)code + float_neg50_80420cc4, -10.0f, -50.0f};
        break;
    case 4:
        entry->pos = (Vec){float_25_80420c9c * (f32)code + float_neg50_80420cc4, 25.0f, -50.0f};
        break;
    }
#ifdef MUST_MATCH
    // MUST_MATCH: the volatile Vec stores stop MWCC hoisting the dat_80420c28 load across them, and the
    // volatile read pins it to the end in r0; the non-volatile form schedules it one slot early (98.39).
    *(volatile Vec*)&entry->rot = (Vec){0.0f, 0.0f, 0.0f};
    *(volatile Vec*)&entry->scale = (Vec){15.0f, 15.0f, 150.0f};
    entry->color = *(volatile const GXColor*)&dat_80420c28;
#else
    entry->rot = (Vec){0.0f, 0.0f, 0.0f};
    entry->scale = (Vec){15.0f, 15.0f, 150.0f};
    entry->color = *(const GXColor*)&dat_80420c28;
#endif
    entry->flags |= 2;
    return 2;
}

s32 _lvup_spot_move(EventEntry* event, BOOL isFirstCall) {
    BattleSpotWork* spot = BatSpotGetPtr();
    BattleSpotEntry* entry;
    s32 code;

    code = evtGetValue(event, event->args[0]);
    entry = &spot->entries[code];

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
        event->userdata[2] = (s32)entry->pos.x;
        event->field_84 = (s32)entry->pos.y;
        event->userdataEx[0] = 0;
    }

    event->userdata[0]++;
    switch (event->userdata[1]) {
    case 0:
        event->userdata[0] = 1;
        event->userdata[1]++;
    case 1:
        switch (code) {
        case 0:
            entry->pos.x = float_0p5_80420c8c * BattleGetMarioPtr(_battleWorkPointer)->mPosition.x;
            entry->pos.y = float_neg20_80420c90;
            break;
        case 1:
            entry->pos.x = float_50_80420c94 * sinfd(reviseAngle(float_1p1_80420c98 * (float_3_80420c6c * (f32)event->userdata[0]))) + (f32)event->userdata[2];
            entry->pos.y = (f32)((s32)event->field_84 + 25) - float_25_80420c9c * sinfd(reviseAngle(float_1p1_80420c98 * (float_2p5_80420ca4 * (f32)event->userdata[0]) + float_90_80420ca0));
            break;
        case 2:
            entry->pos.x = float_65_80420ca8 * sinfd(reviseAngle(float_1p1_80420c98 * (float_2p5_80420ca4 * (f32)event->userdata[0]) + float_180_80420cac)) + (f32)event->userdata[2];
            entry->pos.y = (f32)((s32)event->field_84 - 25) + float_25_80420c9c * sinfd(reviseAngle(float_1p1_80420c98 * (float_7p5_80420cb0 * (f32)event->userdata[0]) + float_90_80420ca0));
            break;
        case 3:
            entry->pos.x = (f32)event->userdata[2] - float_50_80420c94 * sinfd(reviseAngle(float_1p1_80420c98 * (float_4_80420c78 * (f32)event->userdata[0])));
            entry->pos.y = (f32)((s32)event->field_84 + 25) - float_25_80420c9c * sinfd(reviseAngle(float_1p1_80420c98 * (float_2p5_80420ca4 * (f32)event->userdata[0]) + float_90_80420ca0));
            break;
        case 4:
            entry->pos.x = (f32)(event->userdata[2] - 50) - float_50_80420c94 * sinfd(reviseAngle(float_1p1_80420c98 * (float_3_80420c6c * (f32)event->userdata[0]) + float_270_80420cb4));
            entry->pos.y = (f32)((s32)event->field_84 - 25) + float_25_80420c9c * sinfd(reviseAngle(float_1p1_80420c98 * (float_4p5_80420cb8 * (f32)event->userdata[0]) + float_90_80420ca0));
            break;
        }
        break;
    case 2:
        event->userdataEx[0] = 20;
        event->userdata[2] = (s32)entry->pos.x;
        event->field_84 = (s32)entry->pos.y;
        event->userdata[1]++;
    case 3:
        --event->userdataEx[0];
        entry->pos.x = intplGetValue(2, event->userdataEx[0], 21, float_0_80420c7c, (f32)event->userdata[2]);
        entry->pos.y = intplGetValue(2, event->userdataEx[0], 21, float_neg25_80420cbc, (f32)(s32)event->field_84);
        {
            GXColor color = {0xFF, 0xFF, 0xFF, 0x00};

            color.a = (s32)intplGetValue(0, event->userdataEx[0], 21, float_255_80420c88, float_128_80420cc0);
            entry->color = color;
        }
        if (event->userdataEx[0] <= 0) {
            event->userdata[1]++;
        }
        break;
    }
    return (event->userdata[1] < 4) ? 0 : 2;
}

// The declaration order below is deliberate: it drives MWCC's callee-saved allocation to the
// original's (spot r31, i r30, event r29, entry r26, byte offset r28).
s32 _lvup_spot_off(EventEntry* event, BOOL isFirstCall) {
    BattleSpotWork* spot = BatSpotGetPtr();
    s32* args = event->args;
    BattleSpotEntry* entry;
    s32 i;
    GXColor color;

    if (isFirstCall) {
        event->userdata[0] = evtGetValue(event, args[0]);
        event->userdata[1] = event->userdata[0];
        event->userdata[2] = spot->entries->color.a;
    }
    event->userdata[1]--;
    for (i = 0; i < 5; i++) {
        entry = &spot->entries[i];
        color = (GXColor){0xFF, 0xFF, 0xFF, 0x00};

        color.a = (s32)intplGetValue(2, event->userdata[1], event->userdata[0], float_0_80420c7c, (f32)event->userdata[2]);
        entry->color = color;
        if (entry->color.a == 0) {
            entry->flags &= ~2;
        }
    }
    return (event->userdata[1] > 0) ? 0 : 2;
}

s32 _lvup_object_noselect_delete(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 selected = work->lvupSelect;
    s32 i;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[2] = 0x1E;
        for (i = 0; i < work->lvupCount; i++) {
            if (i != selected) {
                work->objs[i].state = 3;
            }
        }
    }
    event->userdata[0]++;
    if (event->userdata[0] < event->userdata[2]) {
        for (i = 0; i < work->lvupCount; i++) {
            if (i != selected) {
                work->objs[i].color.a = (u8)(s32)intplGetValue(5, event->userdata[0], event->userdata[2], float_255_80420c88, float_0_80420c7c);
            }
        }
        return 0;
    }
    for (i = 0; i < work->lvupCount; i++) {
        if (i != selected) {
            work->objs[i].color.a = 0;
        }
    }
    return 2;
}

s32 _lvup_object_select_move(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    BattleSeqEndWork* work = wp->seqEndWork;
    s32 selected = work->lvupSelect;
    BattleUnit* mario = BattleGetMarioPtr(wp);
    LvupSelectObj* entry = &work->objs[selected];

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0x3C;
        event->userdata[2] = (s32)entry->pos.x;
        event->field_84 = (s32)entry->pos.z;
        event->userdataEx[0] = (s32)mario->mPosition.x;
        event->userdataEx[1] = (s32)mario->mPosition.z;
    }
    event->userdata[0]++;
    if (event->userdata[0] < event->userdata[1]) {
        entry->pos.x = intplGetValue(5, event->userdata[0], event->userdata[1], (f32)(s32)event->userdata[2], (f32)event->userdataEx[0]);
        entry->pos.z = intplGetValue(5, event->userdata[0], event->userdata[1], (f32)(s32)event->field_84, (f32)event->userdataEx[1]);
        entry->rot.y = reviseAngle(intplGetValue(5, event->userdata[0], event->userdata[1], float_0_80420c7c, float_2160_80420c84));
        return 0;
    }
    entry->pos.x = (f32)event->userdataEx[0];
    entry->pos.z = (f32)event->userdataEx[1];
    return 2;
}

s32 _lvup_object_decide_bound(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    BattleSeqEndWork* work = wp->seqEndWork;
    s32 selected = work->lvupSelect;
    LvupSelectObj* entry = &work->objs[selected];
    s32 half;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = (s32)entry->pos.y;
        event->userdata[2] = 0x10;
        event->field_84 = 0x1E;
        entry->state = 2;
    }
    event->userdata[0]++;
    if (event->userdata[0] < event->userdata[2]) {
        half = event->userdata[2] / 2;
        if (event->userdata[0] < half) {
            entry->pos.y = (f32)event->userdata[1] + intplGetValue(5, event->userdata[0], half, float_0_80420c7c, (f32)(s32)event->field_84);
        } else {
            entry->pos.y = (f32)event->userdata[1] + intplGetValue(5, event->userdata[0] - half, half, (f32)(s32)event->field_84, float_0_80420c7c);
        }
        return 0;
    }
    entry->pos.y = (f32)event->userdata[1];
    entry->state = 4;
    effPokopiPchargeN64Entry(entry->pos.x, entry->pos.y, entry->pos.z, float_1p5_80420c80, 3, 0x78);
    return 2;
}

s32 _lvup_select_object_init(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    PouchData* pouch = pouchGetPtr();

    work->objs[0].pos = (Vec){-80.0f, 105.0f, 0.0f};
    work->objs[0].scale = (Vec){0.5f, 0.5f, 1.0f};
    work->objs[1].pos = (Vec){0.0f, 105.0f, 0.0f};
    work->objs[1].scale = (Vec){0.5f, 0.5f, 1.0f};
    work->objs[2].pos = (Vec){80.0f, 105.0f, 0.0f};
    work->objs[2].scale = (Vec){0.5f, 0.5f, 1.0f};

    work->objs[0].state = 0;
    work->objs[0].color = (GXColor){0xC8, 0xFF, 0xFF, 0xFF};
    work->objs[0].value = pouch->baseMaxHP;
    work->objs[0].rot = (Vec){0.0f, 0.0f, 0.0f};
    if (work->objs[0].value >= 0xC8) {
        work->objs[0].max = work->objs[0].value;
    } else {
        work->objs[0].max = work->objs[0].value + 5;
    }

    work->objs[1].state = 0;
    work->objs[1].color = (GXColor){0xC8, 0xFF, 0xFF, 0xFF};
    work->objs[1].value = pouch->baseMaxFP;
    work->objs[1].rot = (Vec){0.0f, 0.0f, 0.0f};
    if (work->objs[1].value >= 0xC8) {
        work->objs[1].max = work->objs[1].value;
    } else {
        work->objs[1].max = work->objs[1].value + 5;
    }

    work->objs[2].state = 0;
    work->objs[2].color = (GXColor){0xC8, 0xFF, 0xFF, 0xFF};
    work->objs[2].value = pouch->maximumBP;
    work->objs[2].rot = (Vec){0.0f, 0.0f, 0.0f};
    if (work->objs[2].value >= 99) {
        work->objs[2].max = work->objs[2].value;
    } else {
        work->objs[2].max = work->objs[2].value + 3;
    }

    work->dispFlags |= SEQEND_DISP_LVUP_OBJECTS;
    return 2;
}

s32 _keta_get(s32 value) {
    s32 digits = 0;

    if (value < 0) {
        value *= -1;
    }
    if (value == 0) {
        return 1;
    }
    while (value > 0) {
        value /= 10;
        digits++;
    }
    return digits;
}

// The three level-up select objects live in the sequence-end work at 0x200, stride 0x34.

// The 0x200 area of the sequence-end work: three select objects.

// The original was built without common-subexpression elimination here: with it, the frontend merges
// PSMTXScale's two 0.8 arguments into one temp whose load the scheduler issues first, and the pos2 block's
// FP registers and order drift. Without it the backend loads 0.8 once and copies it (`lfs f1; fmr f2, f1`)
// late, as in retail.
#pragma opt_common_subs off
void _lvup_select_object_disp(CameraId camId, void* param) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    Mtx mtxMain;
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxRotX;
    Mtx mtxRotY;
    Mtx mtxRotZ;
    Mtx mtxT2;
    Mtx mtxT3;
    Mtx mtxT4;
    Mtx mtxS2;
    Vec pos;
    Vec pos2;
    Vec pos3;
    LvupSelectObj* obj;
    s32 i;

    for (i = 0; i < 3; i++) {
        obj = &work->objs[i];

        switch (obj->state) {
        case 4:
            obj->state = 5;
            obj->blinkTimer = 1;
            /* fallthrough */
        case 5:
            obj->blinkTimer--;
            if (obj->blinkTimer == 0) {
                obj->blinkTimer = 8;
                if (obj->visible != 0) {
                    obj->visible = 0;
                } else {
                    obj->visible = 1;
                }
            }
            break;
        default:
            obj->visible = 1;
            break;
        }
    }

    if (work->objs[0].visible != 0) {
        PSMTXIdentity(mtxMain);
        PSMTXTrans(mtxTrans, work->objs[0].pos.x, work->objs[0].pos.y, work->objs[0].pos.z);
        PSMTXScale(mtxScale, work->objs[0].scale.x, work->objs[0].scale.y, work->objs[0].scale.z);
        PSMTXRotRad(mtxRotX, 0x78, float_deg2rad_80420c40 * work->objs[0].rot.x);
        PSMTXRotRad(mtxRotY, 0x79, float_deg2rad_80420c40 * work->objs[0].rot.y);
        PSMTXRotRad(mtxRotZ, 0x7a, float_deg2rad_80420c40 * work->objs[0].rot.z);
        PSMTXConcat(mtxRotZ, mtxScale, mtxMain);
        PSMTXConcat(mtxRotZ, mtxMain, mtxMain);
        PSMTXConcat(mtxRotX, mtxMain, mtxMain);
        PSMTXConcat(mtxRotY, mtxMain, mtxMain);
        PSMTXConcat(mtxTrans, mtxMain, mtxMain);
        btlDispTexPlane2(mtxMain, 0x24, work->objs[0].color, 0);
        btlDispTexPlane(work->objs[0].pos.x - float_16_80420c44, float_20_80420c48 + work->objs[0].pos.y,
                        float_2_80420c4c + work->objs[0].pos.z, work->objs[0].scale.x, work->objs[0].scale.y, 0x27,
                        work->objs[0].color, 0);
    }

    if (work->objs[1].visible != 0) {
        PSMTXIdentity(mtxMain);
        PSMTXTrans(mtxTrans, work->objs[1].pos.x, work->objs[1].pos.y, work->objs[1].pos.z);
        PSMTXScale(mtxScale, work->objs[1].scale.x, work->objs[1].scale.y, work->objs[1].scale.z);
        PSMTXRotRad(mtxRotX, 0x78, float_deg2rad_80420c40 * work->objs[1].rot.x);
        PSMTXRotRad(mtxRotY, 0x79, float_deg2rad_80420c40 * work->objs[1].rot.y);
        PSMTXRotRad(mtxRotZ, 0x7a, float_deg2rad_80420c40 * work->objs[1].rot.z);
        PSMTXConcat(mtxRotZ, mtxScale, mtxMain);
        PSMTXConcat(mtxRotZ, mtxMain, mtxMain);
        PSMTXConcat(mtxRotX, mtxMain, mtxMain);
        PSMTXConcat(mtxRotY, mtxMain, mtxMain);
        PSMTXConcat(mtxTrans, mtxMain, mtxMain);
        btlDispTexPlane2(mtxMain, 0x25, work->objs[1].color, 0);
        btlDispTexPlane(work->objs[1].pos.x - float_16_80420c44, float_20_80420c48 + work->objs[1].pos.y,
                        float_2_80420c4c + work->objs[1].pos.z, work->objs[1].scale.x, work->objs[1].scale.y, 0x28,
                        work->objs[1].color, 0);
    }

    if (work->objs[2].visible != 0) {
        PSMTXIdentity(mtxMain);
        PSMTXTrans(mtxTrans, work->objs[2].pos.x, work->objs[2].pos.y, work->objs[2].pos.z);
        PSMTXScale(mtxScale, work->objs[2].scale.x, work->objs[2].scale.y, work->objs[2].scale.z);
        PSMTXRotRad(mtxRotX, 0x78, float_deg2rad_80420c40 * work->objs[2].rot.x);
        PSMTXRotRad(mtxRotY, 0x79, float_deg2rad_80420c40 * work->objs[2].rot.y);
        PSMTXRotRad(mtxRotZ, 0x7a, float_deg2rad_80420c40 * work->objs[2].rot.z);
        PSMTXConcat(mtxRotZ, mtxScale, mtxMain);
        PSMTXConcat(mtxRotZ, mtxMain, mtxMain);
        PSMTXConcat(mtxRotX, mtxMain, mtxMain);
        PSMTXConcat(mtxRotY, mtxMain, mtxMain);
        PSMTXConcat(mtxTrans, mtxMain, mtxMain);
        btlDispTexPlane2(mtxMain, 0x26, work->objs[2].color, 0);
        btlDispTexPlane(work->objs[2].pos.x - float_16_80420c44, float_20_80420c48 + work->objs[2].pos.y,
                        float_2_80420c4c + work->objs[2].pos.z, work->objs[2].scale.x, work->objs[2].scale.y, 0x29,
                        work->objs[2].color, 0);
    }

    for (i = 0; i < 3; i++) {
        obj = &work->objs[i];

        if (obj->visible == 0) {
            continue;
        }
        if (obj->state == 1) {
            if (obj->value == obj->max) {
                continue;
            }
            pos2 = obj->pos;
            pos2.x = pos2.x - float_8_80420c50 - float_6_80420c54;
            pos2.y = pos2.y - (float_96_80420c58 * obj->scale.y + float_2_80420c4c);
            pos2.z = pos2.z + float_5_80420c5c;
            PSMTXScale(mtxS2, float_0p8_80420c60, float_0p8_80420c60, float_1_80420c64);
            PSMTXTrans(mtxT2, pos2.x, pos2.y, pos2.z);
            PSMTXConcat(mtxT2, mtxS2, mtxT2);
            iconNumberDispGx3D(mtxT2, obj->value, 0, obj->color);

            pos3 = obj->pos;
            pos3.x = pos3.x + float_1p2_80420c68 * (f32)(_keta_get(obj->max) * 8 + 6);
            pos3.y = pos3.y - (float_96_80420c58 * obj->scale.y + float_3_80420c6c);
            pos3.z = pos3.z + float_5_80420c5c;
            PSMTXTrans(mtxT3, pos3.x, pos3.y, pos3.z);
            PSMTXScale(mtxS2, float_1p2_80420c68, float_1p2_80420c68, float_1_80420c64);
            PSMTXConcat(mtxT3, mtxS2, mtxT3);
            iconNumberDispGx3D(mtxT3, obj->max, 0, obj->color);

            pos = obj->pos;
            pos.y = pos.y - float_96_80420c58 * obj->scale.y;
            pos.z = pos.z + float_5_80420c5c;
            PSMTXTrans(mtxT4, pos.x, pos.y, pos.z);
            PSMTXScale(mtxS2, float_neg0p6_80420c70, float_0p6_80420c74, float_1_80420c64);
            PSMTXConcat(mtxT4, mtxS2, mtxT4);
            iconDispGxCol(mtxT4, 0, 0x163, obj->color);
        } else if (obj->state == 2 || obj->state == 5) {
            pos3 = obj->pos;
            pos3.x = pos3.x + ((f32)(_keta_get(obj->max) * 8 / 2) - float_4_80420c78);
            pos3.y = pos3.y - (float_96_80420c58 * obj->scale.y + float_2_80420c4c);
            pos3.z = pos3.z + float_5_80420c5c;
            PSMTXTrans(mtxT3, pos3.x, pos3.y, pos3.z);
            iconNumberDispGx3D(mtxT3, obj->max, 0, obj->color);
        }
    }
}
#pragma opt_common_subs reset

void _check_audience_battle_end_exec(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleGroupSetup* group = wp->fbatInfo->setup->groupSetup;
    s32 firstLevel = group->enemyData[0].mUnitKindParams->mLevel;
    s32 turn;
    s32 sum;
    s32 i;
    s32 level;
    BattleUnitSetup* entry;
    PouchData* pouch;

    if (firstLevel > (pouch = pouchGetPtr())->level + 1) {
        BattleAudience_Case_GreatVictory();
    }
    turn = wp->turnCount;
    pouch = pouchGetPtr();
    sum = 0;
    level = pouch->level + 1;
    entry = group->enemyData;
    for (i = 0; i < group->numEnemies; i++, entry++) {
        sum += entry->mUnitKindParams->mLevel;
    }
    level = sum * 2 / level / 3;
    if (turn < level) {
        BattleAudience_Case_FastVictory((f32)(level - turn));
    }
}

// A switch over the steps SEQ_END_WIN_MAJINAI_WAIT..SEQ_END_WIN_POSE_DONE: MWCC lowers the run of labels to the
// original's range test (`bge end`, `bge body; b end`), which no if/else form reproduces.
void _audience_Whistle_control(void) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;

    if (work == NULL) {
        return;
    }
    switch (work->unk0) {
    case SEQ_END_WIN_MAJINAI_WAIT: case SEQ_END_EXP_INIT: case SEQ_END_EXP_DELAY: case SEQ_END_EXP_START:
    case SEQ_END_EXP_COUNT: case SEQ_END_EXP_COUNT_END: case SEQ_END_EXP_END_WAIT: case SEQ_END_MAJINAI_END:
    case SEQ_END_MAJINAI_END_WAIT: case SEQ_END_LVUP_CHECK: case SEQ_END_LVUP_CURTAIN: case SEQ_END_LVUP_SELECT:
    case SEQ_END_LVUP_CONFIRM: case SEQ_END_LVUP_JUMP_WAIT: case SEQ_END_LVUP_APPLY: case SEQ_END_LVUP_CHEER:
    case SEQ_END_RANK_UP: case SEQ_END_RANK_UP_WAIT: case SEQ_END_WIN_POSE: case SEQ_END_WIN_POSE_WAIT:
    case SEQ_END_WIN_POSE_DONE:
        if (--work->whistleTimer < 0) {
            work->whistleTimer = irand(0x78) + 0x3C;
            BattleAudienceSoundWhistle();
        }
        break;
    }
}

void _BattleMarioKouraDelete(void) {
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    EventEntry* event = evtEntry(_koura_delete_event, 0xA, 0);

    event->unitId = mario->unitId;
}

static s32 _check_guard_koura(EventEntry* event, BOOL isFirstCall) {
    EventEntry* e = event;
    BattleWork* wp = _battleWorkPointer;
    s32* args = e->args;
    s32 type = evtGetValue(e, args[0]);
    s32 outIdx = args[1];
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(e, type));

    if (!(unit->mAttributeFlags & kShellShielded)) {
        evtSetValue(e, outIdx, -1);
        return 2;
    }
    {
        BattleUnit* koura = BtlUnit_GetGuardKouraPtr(unit);

        if (koura == NULL) {
            evtSetValue(e, outIdx, -1);
            return 2;
        }
        evtSetValue(e, outIdx, koura->unitId);
    }
    return 2;
}

s32 _get_rank_up_msg(EventEntry* event, BOOL isFirstCall) {
    s32 idx = event->args[0];
    PouchData* pouch = pouchGetPtr();
    RankUpData* rank = _get_rank_data(pouch->level);

    evtSetValue(event, idx, (s32)rank->rankUpMessage);
    return 2;
}

// Named .sdata2 suffix follows the compiler colour templates, in retail address order.
// Keep unreferenced tail words: they are part of the original section layout.
#ifdef MUST_MATCH
#pragma force_active on
#endif
__declspec(section ".sdata2") const f32 float_deg2rad_80420c40 = 0.017453292f;
__declspec(section ".sdata2") const f32 float_16_80420c44 = 16.0f;
__declspec(section ".sdata2") const f32 float_20_80420c48 = 20.0f;
__declspec(section ".sdata2") const f32 float_2_80420c4c = 2.0f;
__declspec(section ".sdata2") const f32 float_8_80420c50 = 8.0f;
__declspec(section ".sdata2") const f32 float_6_80420c54 = 6.0f;
__declspec(section ".sdata2") const f32 float_96_80420c58 = 96.0f;
__declspec(section ".sdata2") const f32 float_5_80420c5c = 5.0f;
__declspec(section ".sdata2") const f32 float_0p8_80420c60 = 0.8f;
__declspec(section ".sdata2") const f32 float_1_80420c64 = 1.0f;
__declspec(section ".sdata2") const f32 float_1p2_80420c68 = 1.2f;
__declspec(section ".sdata2") const f32 float_3_80420c6c = 3.0f;
__declspec(section ".sdata2") const f32 float_neg0p6_80420c70 = -0.6f;
__declspec(section ".sdata2") const f32 float_0p6_80420c74 = 0.6f;
__declspec(section ".sdata2") const f32 float_4_80420c78 = 4.0f;
__declspec(section ".sdata2") const f32 float_0_80420c7c = 0.0f;
__declspec(section ".sdata2") const f32 float_1p5_80420c80 = 1.5f;
__declspec(section ".sdata2") const f32 float_2160_80420c84 = 2160.0f;
__declspec(section ".sdata2") const f32 float_255_80420c88 = 255.0f;
__declspec(section ".sdata2") const f32 float_0p5_80420c8c = 0.5f;
__declspec(section ".sdata2") const f32 float_neg20_80420c90 = -20.0f;
__declspec(section ".sdata2") const f32 float_50_80420c94 = 50.0f;
__declspec(section ".sdata2") const f32 float_1p1_80420c98 = 1.1f;
__declspec(section ".sdata2") const f32 float_25_80420c9c = 25.0f;
__declspec(section ".sdata2") const f32 float_90_80420ca0 = 90.0f;
__declspec(section ".sdata2") const f32 float_2p5_80420ca4 = 2.5f;
__declspec(section ".sdata2") const f32 float_65_80420ca8 = 65.0f;
__declspec(section ".sdata2") const f32 float_180_80420cac = 180.0f;
__declspec(section ".sdata2") const f32 float_7p5_80420cb0 = 7.5f;
__declspec(section ".sdata2") const f32 float_270_80420cb4 = 270.0f;
__declspec(section ".sdata2") const f32 float_4p5_80420cb8 = 4.5f;
__declspec(section ".sdata2") const f32 float_neg25_80420cbc = -25.0f;
__declspec(section ".sdata2") const f32 float_128_80420cc0 = 128.0f;
__declspec(section ".sdata2") const f32 float_neg50_80420cc4 = -50.0f;
__declspec(section ".sdata2") const f32 float_neg76p9_80420cc8 = -76.899994f;
__declspec(section ".sdata2") const f32 float_21_80420ccc = 21.0f;
__declspec(section ".sdata2") const f32 float_165p8_80420cd0 = 165.79999f;
__declspec(section ".sdata2") const f32 float_24p2_80420cd4 = 24.199999f;
__declspec(section ".sdata2") const f32 float_10_80420cd8 = 10.0f;
__declspec(section ".sdata2") const f32 float_24_80420cdc = 24.0f;
__declspec(section ".sdata2") const f32 float_18_80420ce0 = 18.0f;
__declspec(section ".sdata2") const f32 float_36_80420ce4 = 36.0f;
__declspec(section ".sdata2") const f32 float_0p9_80420ce8 = 0.9f;
__declspec(section ".sdata2") const f32 float_0p45_80420cec = 0.45f;
__declspec(section ".sdata2") const f32 float_86p9_80420cf0 = 86.899994f;
__declspec(section ".sdata2") const f32 float_16p2_80420cf4 = 16.199999f;
__declspec(section ".sdata2") const f32 float_neg145_80420cf8 = -145.0f;
__declspec(section ".sdata2") const f32 float_neg148_80420cfc = -148.0f;
__declspec(section ".sdata2") const f32 float_neg30_80420d00 = -30.0f;
__declspec(section ".sdata2") const f32 float_6p25_80420d04 = 6.25f;
__declspec(section ".sdata2") const f32 float_30_80420d08 = 30.0f;
__declspec(section ".sdata2") const f32 float_12p5_80420d0c = 12.5f;
__declspec(section ".sdata2") const f32 float_32_80420d10 = 32.0f;
__declspec(section ".sdata2") const f32 float_neg70_80420d14 = -70.0f;
__declspec(section ".sdata2") const f32 float_neg10_80420d18 = -10.0f;
__declspec(section ".sdata2") const f32 float_130_80420d1c = 130.0f;
__declspec(section ".sdata2") const f32 float_60_80420d20 = 60.0f;
__declspec(section ".sdata2") const f32 float_27_80420d24 = 27.0f;
__declspec(section ".sdata2") const f32 float_neg13_80420d28 = -13.0f;
__declspec(section ".sdata2") const f32 float_neg38_80420d2c = -38.0f;
__declspec(section ".sdata2") const f32 float_neg40_80420d30 = -40.0f;
__declspec(section ".sdata2") const f32 float_200_80420d34 = 200.0f;
__declspec(section ".sdata2") const f32 float_neg200_80420d38 = -200.0f;
__declspec(section ".sdata2") const char str_fmt_2d[4] = "%2d";
__declspec(section ".sdata2") const f32 float_neg160_80420d40 = -160.0f;
__declspec(section ".sdata2") const f32 float_225_80420d44 = 225.0f;
__declspec(section ".sdata2") const f32 float_neg163_80420d48 = -163.0f;
__declspec(section ".sdata2") const f32 float_900_80420d4c = 900.0f;
__declspec(section ".sdata2") const f32 float_40_80420d50 = 40.0f;
__declspec(section ".sdata2") const char str_M_V_1[8] = "M_V_1";
__declspec(section ".sdata2") const f32 float_0_80420d5c = 0.0f;
__declspec(section ".sdata2") const u32 dat_80420d60 = 0xFFFFFF00;
__declspec(section ".sdata2") const f32 float_0_80420d64 = 0.0f;
__declspec(section ".sdata2") const f32 float_20_80420d68 = 20.0f;
__declspec(section ".sdata2") const f32 float_neg145_80420d6c = -145.0f;
__declspec(section ".sdata2") const f32 float_10_80420d70 = 10.0f;
__declspec(section ".sdata2") const f32 float_neg148_80420d74 = -148.0f;
__declspec(section ".sdata2") const f32 float_498_80420d78 = 498.0f;
#ifdef MUST_MATCH
#pragma force_active reset
#endif

#include "tou_07.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/itemdrv.h"
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
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "party/party_motion.h"
#include "pmario_sound.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_tou_get_ranking_27_text_1ED4[];
extern s32 evt_tou_match_after_default[];
extern s32 evt_tou_match_make_default[];
extern s32 evt_tou_monitor_init[];
extern s32 unk_evt_tou_00012b54[];
static USER_FUNC(paper_off);
static USER_FUNC(paper_on);

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(n_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(n_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[] = {
    {"n_bero", 1, 4, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tou_04", "s_bero_3", 5, 5,
     n_door_close, n_door_open},
    {"e_bero", 0, 1, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "tou_13", "e_bero_", 0, 0, 0, 0},
    {NULL},
};

static EVT_BEGIN(init_gans)
    RETURN
EVT_END()

static EVT_BEGIN(talk_gans)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"ガンス", 0x40000600, 0, init_gans, NULL, talk_gans},
    {"キノシコワ", 0x40000620, 0, init_gans, NULL, talk_gans},
    {"ガードマン", 0x40000600, 0, init_gans, NULL, talk_gans},
    {NULL},
};

static EVT_BEGIN(evt_sensyu)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_pos, 0, 0, -160)
    CALL(evt_party_set_pos, 0, 0, 0, -190)
    CALL(evt_npc_set_position, STRING("ガンス"), 0, 0, -155)
    CALL(evt_cam3d_evt_set, 0, 145, 534, 0, 50, -8, 0, 11)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(n_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_status_onoff, 1, STRING("ガンス"), 2)
    CALL(evt_npc_move_position, STRING("ガンス"), 1, -50, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_move_position, STRING("ガンス"), -30, -50, 0, FLOAT(80.0f), 1)
    CALL(evt_set_dir_to_target, STRING("ガンス"), STRING("mario"))
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        WAIT_MS(500)
        CALL(evt_party_move_pos2, 0, 0, -70, FLOAT(60.0f))
        CALL(evt_party_stop, 0)
        WAIT_MS(100)
        CALL(evt_party_move_pos2, 0, 50, -70, FLOAT(60.0f))
        CALL(evt_party_set_dir_npc, 0, STRING("ガンス"))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 0, -50, FLOAT(60.0f))
    CALL(evt_mario_mov_pos2, 35, -50, FLOAT(60.0f))
    CALL(evt_mario_set_dir_npc, STRING("ガンス"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(n_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_msg_print, 0, STRING("stg3_tou_49"), 0, STRING("ガンス"))
    WAIT_MS(300)
    CALL(evt_fade_set_mapchange_type, 0, 49, 0, 50, 0)
    CALL(evt_bero_mapchange, STRING("tou_08"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_champion)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_set_pos, 0, 0, -160)
    CALL(evt_party_set_pos, 0, 0, 0, -165)
    CALL(evt_npc_set_position, STRING("キノシコワ"), 0, 0, -155)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_npc_set_ry, STRING("キノシコワ"), 90)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, -180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(n_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 3, -100, 0, FLOAT(80.0f), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_party_stop, 0)
        WAIT_MS(500)
        CALL(evt_party_move_pos2, 0, 3, -110, FLOAT(60.0f))
        CALL(evt_party_stop, 0)
        WAIT_MS(100)
        CALL(evt_party_move_pos2, 0, 70, -110, FLOAT(60.0f))
        CALL(evt_party_set_dir_npc, 0, STRING("キノシコワ"))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 3, -110, FLOAT(60.0f))
    CALL(evt_mario_mov_pos2, 40, -100, FLOAT(60.0f))
    CALL(evt_mario_set_dir_npc, STRING("キノシコワ"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, -180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(n_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_cam3d_evt_set, 20, 90, 189, 20, 60, -9, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_449"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
    WAIT_MS(600)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
    CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -140, 0, FLOAT(80.0f), 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(n_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_position, STRING("キノシコワ"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL_CHILD(n_door_open)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_cam3d_evt_set, 63, 78, 139, 63, 55, -9, 1000, 11)
    CALL(evt_party_move_pos2, 0, 85, -100, FLOAT(60.0f))
    CALL(evt_party_set_dir, 0, 270, 200)
    CALL(evt_mario_set_dir, 90, 200, 0)
    WAIT_MS(1000)
    CALL(evt_party_stop, 0)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_450"))
    WAIT_MS(300)
    WAIT_MS(1000)
    CALL(evt_eff_fukidashi, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(2000)
    CALL(evt_msg_print_party, STRING("stg3_tou_450_01"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_QUESTION1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 30)
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -75, 68, 270, 59, 78, 2, 1000, 11)
    WAIT_MS(1000)
    LOOP(2)
        CALL(evt_msg_toge, 2, 150, 100, -90)
        CALL(evt_msg_print, 0, STRING("stg3_tou_450_02"), 0, 0)
        WAIT_MS(300)
        CALL(evt_msg_toge, 2, 140, 90, -90)
        CALL(evt_msg_print, 0, STRING("stg3_tou_450_03"), 0, 0)
        WAIT_MS(300)
    END_LOOP
    CALL(evt_cam3d_evt_set, 63, 78, 139, 63, 55, -9, 0, 11)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg3_tou_450_04"))
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(1500)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION2"))
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg3_tou_450_05"))
    LOOP(0)
        IS_EVENT_RUNNING(LW(11), LW(12))
        IF_INT_EQUAL(LW(12), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_bikkuri)
    LOOP(0)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        IF_INT_EQUAL(LW(0), 256)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION1"), LW(0), LW(1), LW(2), 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg3_tou_450_06"), 0, 0)
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print_party, STRING("stg3_tou_450_07"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_pouch_receive_mail, 43)
    CALL(evt_pouch_open_mail, 43)
    SET_VALUE(GSW(0), 158)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// 0x6D34
USER_FUNC(unk_tou_00006d34) {
    EffectEntry* eff = effNameToPtr("belt");
    NpcEntry* npc = npcNameToPtr("ベルト");
    *(s32*)((u8*)eff->userdata + 0x38) = 7;
    npc->cameraId = 7;
    return EVT_RETURN_DONE;
}

// 0x6CFC
USER_FUNC(unk_tou_00006cfc) {
    iconFlagOn(itemNameToPtr("belt")->name, 2);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_champion_re)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_set_pos, 65, 0, -95)
    CALL(evt_party_set_pos, 0, 95, 0, -105)
    CALL(evt_npc_set_position, STRING("キノシコワ"), 30, 0, -95)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 270, 0)
    CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("mario"))
    CALL(evt_cam3d_evt_set, 45, 91, 266, 45, 57, -11, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_140"), 0, STRING("キノシコワ"))
    CALL(evt_npc_flag_onoff, 1, STRING("キノシコワ"), 32)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 40, -95, 0, FLOAT(80.0f), 1)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 30, -95, 0, FLOAT(80.0f), 1)
    CALL(evt_set_dir_to_target, STRING("キノシコワ"), STRING("mario"))
    CALL(evt_npc_entry, STRING("ベルト"), STRING("c_bred_h"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 56)
    CALL(evt_npc_set_position, STRING("ベルト"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_anim, STRING("ベルト"), STRING("C_1"))
    ADD(LW(1), 4)
    ADD(LW(2), 15)
    CALL(evt_eff, STRING("belt"), STRING("starstone"), 9, LW(0), LW(1), LW(2), FLOAT(0.75f), 0, 0, 0, 0, 0, 0, 0)
    CALL(unk_tou_00006d34)
    CALL(evt_item_entry, STRING("belt"), 65, 0, -1000, 0, 17, -1, 0)
    CALL(unk_tou_00006cfc)
    CALL(evt_item_get_item, STRING("belt"))
    CALL(evt_item_get_item_end_wait, STRING("belt"))
    CALL(evt_npc_delete, STRING("ベルト"))
    CALL(evt_eff_delete, STRING("belt"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_141"), 0, STRING("キノシコワ"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_142"), 0, STRING("キノシコワ"))
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_msg_print, 0, STRING("stg3_tou_re_143"), 0, STRING("キノシコワ"))
    WAIT_MS(300)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("M_1"))
    WAIT_MS(600)
    CALL(evt_npc_set_anim, STRING("キノシコワ"), STRING("S_1"))
    CALL(evt_npc_status_onoff, 1, STRING("キノシコワ"), 2)
    CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -130, 0, FLOAT(80.0f), 1)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_open_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("キノシコワ"), 0, -200, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_position, STRING("キノシコワ"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(800)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_close_door_play_se)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("door_1"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(2503), 1)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_dakuto)
    CALL(evt_cam_shake, 4, FLOAT(0.05f), FLOAT(0.0f), 500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_DUCT_KOUSHI_HIT1"), 135, 80, -95, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kankikuti"), 1)
    CALL(evt_hitobj_onoff, STRING("win_kabe"), 1, 0)
    IF_INT_LESS(GSW(0), 159)
        SET_VALUE(GSW(0), 159)
    ELSE
        SET_VALUE(GSWF(2502), 1)
    END_IF
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_dakuto2)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AIRDUCT_SHAKE1"), 135, 80, -95, 0)
        WAIT_MS(400)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AIRDUCT_SHAKE1"), 135, 80, -95, 0)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_futa"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_futa"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

// 0x6C7C
static USER_FUNC(paper_on) {
    f32 value = evtGetFloat(event, event->args[0]);
    MarioWork* mario = marioGetPtr();
    if (isFirstCall) {
        marioPaperOn("p_slit");
        marioChgPose("M_S_1");
        marioChgPaper("PM_S_3A");
    }
    mario->dispReqDir = value;
    mario->unk1A8 = value;
    return EVT_RETURN_BLOCK;
}

// 0x6C58
static USER_FUNC(paper_off) {
    marioPaperOff();
    return EVT_RETURN_DONE;
}

// 0x6C28
USER_FUNC(mario_recovery) {
    pouchSetHP(pouchGetMaxHP());
    pouchSetFP(pouchGetMaxFP());
    return EVT_RETURN_DONE;
}

// 0x6C00
USER_FUNC(unk_tou_00006c00) {
    psndSetFlag(0x80);
    return EVT_RETURN_DONE;
}

// 0x6BD8
USER_FUNC(unk_tou_00006bd8) {
    psndClearFlag(0x80);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_tou_bed)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LF(0), 0)
    SET_VALUE(LF(1), 0)
    SET_VALUE(LF(2), 0)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("tou_07"))
        SET_VALUE(LF(0), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_08"))
        SET_VALUE(LF(1), 1)
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_10"))
        SET_VALUE(LF(2), 1)
    END_IF
    CALL(evt_msg_print, 0, STRING("stg3_yado_00"), 0, 0)
    CALL(evt_msg_select, 0, STRING("stg3_yado_01"))
    CALL(evt_msg_continue)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_get_party, LW(12))
        CALL(evt_mario_get_exparty, LW(13))
        CALL(evt_mario_goodbye_party, 0)
        CALL(evt_mario_goodbye_party, 1)
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_cam3d_evt_set, -135, 193, 295, 120, 45, 82, 0, 11)
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            CALL(evt_cam3d_evt_set, -49, 117, 190, 152, 45, -10, 0, 11)
        END_IF
        IF_INT_EQUAL(LF(2), 1)
            CALL(evt_cam3d_evt_set, -75, 160, 266, 155, 65, -10, 0, 11)
        END_IF
        WAIT_FRAMES(1)
        CALL(evt_mario_flag_onoff, 1, 768)
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP5"), LW(0), LW(1), LW(2), 0)
            CALL(evt_mario_jump_pos, 115, 32, 90, FLOAT(400.0f), 0, -1000)
            CALL(evt_mario_set_pos, 115, 32, 90)
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP3"), LW(0), LW(1), LW(2), 0)
            CALL(evt_mario_jump_pos, 205, 16, -115, FLOAT(400.0f), 0, -1000)
            CALL(evt_mario_set_pos, 205, 16, -115)
        END_IF
        IF_INT_EQUAL(LF(2), 1)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_mario_jump_pos, 228, 17, -90, FLOAT(400.0f), 0, -1000)
            CALL(evt_mario_set_pos, 228, 17, -90)
        END_IF
        CALL(evt_mario_adjust_dir)
        WAIT_MS(200)
        BEGIN_INLINE_EVENT_GET_ID(LW(10))
            CALL(paper_on, FLOAT(-45.0f))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_MARIO_SLEEP1"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(200)
            CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_MARIO_SLEEP2"), LW(0), LW(1), LW(2), 0)
        END_INLINE_EVENT
        CALL(unk_tou_00006c00)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_GO_SLEEPING1"))
        CALL(evt_fade_entry, 4, 1000, 0, 0, 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_fade_set_spot_pos, LW(0), LW(1), LW(2))
        CALL(evt_fade_end_wait)
        IF_INT_EQUAL(LF(0), 1)
            CALL(evt_pouch_mario_recovery)
        ELSE
            CALL(mario_recovery)
        END_IF
        CALL(evt_pouch_all_party_recovery)
        WAIT_MS(3000)
        DELETE_EVENT(LW(10))
        CALL(paper_off)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_mario_flag_onoff, 0, 768)
        IF_INT_EQUAL(LF(0), 1)
            IF_INT_NOT_EQUAL(LW(12), 0)
                CALL(evt_mario_set_party_pos, 0, LW(12), 75, 37, 100)
            END_IF
            IF_INT_NOT_EQUAL(LW(13), 0)
                CALL(evt_mario_set_party_pos, 1, LW(13), 75, 37, 105)
            END_IF
            CALL(evt_mario_set_pos, 115, 32, 90)
        END_IF
        IF_INT_EQUAL(LF(1), 1)
            IF_INT_NOT_EQUAL(LW(12), 0)
                CALL(evt_mario_set_party_pos, 0, LW(12), 160, 22, -115)
            END_IF
            IF_INT_NOT_EQUAL(LW(13), 0)
                CALL(evt_mario_set_party_pos, 1, LW(13), 160, 22, -110)
            END_IF
            CALL(evt_mario_set_pos, 205, 16, -115)
        END_IF
        IF_INT_EQUAL(LF(2), 1)
            IF_INT_NOT_EQUAL(LW(12), 0)
                CALL(evt_mario_set_party_pos, 0, LW(12), 183, 22, -90)
            END_IF
            IF_INT_NOT_EQUAL(LW(13), 0)
                CALL(evt_mario_set_party_pos, 1, LW(13), 205, 22, -85)
            END_IF
            CALL(evt_mario_set_pos, 228, 17, -90)
        END_IF
        CALL(evt_mario_set_dir, 270, 0, 0)
        CALL(evt_party_set_dir, 0, 270, 0)
        CALL(evt_party_set_dir, 1, 270, 0)
        CALL(evt_mario_adjust_dir)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_FRAMES(1)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        CALL(unk_tou_00006bd8)
        CALL(evt_fade_entry, 9, 1000, 0, 0, 0)
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(bed_entry)
    CALL(evt_run_case_evt, 9, 1, LW(0), 0, PTR(evt_tou_bed), 0)
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 260, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(tou_07_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_hitobj_name_callback, STRING("a_bed"), PTR(bed_entry))
    CALL(evt_sub_animgroup_async, STRING("p_slit"))
    CALL(evt_npc_setup, PTR(npcEnt))
    IF_INT_EQUAL(GSWF(2389), 0)
        CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            IF_INT_LESS(GSW(0), 158)
                SET_VALUE(GSW(0), 158)
            END_IF
        END_IF
    END_IF
    CALL(evt_tou_get_ranking_27_text_1ED4, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("e_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("e_2"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("e_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("e_2"), 1)
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(159)
        SET_VALUE(LF(0), 0)
        CASE_LESS(172)
        SET_VALUE(LF(0), 1)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2502), 0)
            SET_VALUE(LF(0), 0)
        ELSE
            SET_VALUE(LF(0), 1)
        END_IF
    END_SWITCH
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_run_case_evt, 14, 1, STRING("win_kabe"), 0, PTR(evt_dakuto), 0)
        CALL(evt_run_case_evt, 13, 1, STRING("win_kabe"), 0, PTR(evt_dakuto2), 0)
        CALL(evt_run_case_evt, 9, 1, STRING("win_kabe"), 0, PTR(evt_dakuto2), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kankikuti"), 1)
        CALL(evt_hitobj_onoff, STRING("win_kabe"), 1, 0)
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("M_make"), 0, PTR(evt_tou_match_make_default), 0)
    CALL_CHILD(evt_tou_monitor_init)
    IF_INT_EQUAL(GSWF(2389), 1)
        SET_VALUE(GSWF(2389), 0)
        SWITCH(GSW(0))
            CASE_BETWEEN(127, 137)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU2"))
            CALL(evt_snd_bgmoff, 26624)
            CASE_GREATER_OR_EQUAL(165)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU2"))
            CALL(evt_snd_bgmoff, 26624)
            CASE_DEFAULT
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
        END_SWITCH
        IF_INT_LESS(GSW(0), 158)
            SPAWN_EVENT(evt_champion)
            RETURN
        END_IF
        IF_INT_LESS(GSW(0), 172)
            SPAWN_EVENT(evt_tou_match_after_default)
            RETURN
        END_IF
        IF_INT_EQUAL(GSWF(2503), 0)
            SPAWN_EVENT(evt_champion_re)
        ELSE
            SPAWN_EVENT(evt_tou_match_after_default)
        END_IF
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2375), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG3_GANCE1"))
        CALL(evt_snd_envoff, 512)
        SPAWN_EVENT(evt_sensyu)
        RETURN
    END_IF
    IF_INT_EQUAL(GSW(0), 163)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envoff, 512)
    ELSE
        SWITCH(GSW(0))
            CASE_BETWEEN(127, 138)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU2"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
            CALL(evt_snd_bgmoff, 26624)
            CASE_DEFAULT
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG3_TOU1"))
            CALL(evt_snd_bgmoff, 26624)
            CALL(evt_snd_envon, 272, STRING("ENV_STG3_TOU6"))
        END_SWITCH
    END_IF
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("e_bero"))
        IF_INT_EQUAL(GSWF(2388), 1)
            SET_VALUE(LW(0), STRING("n_bero"))
            CALL_CHILD(bero_case_switch_off)
            SPAWN_EVENT(unk_evt_tou_00012b54)
        END_IF
    END_IF
    RETURN
EVT_END()
// clang-format on

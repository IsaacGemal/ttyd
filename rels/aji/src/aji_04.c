#include "aji_04.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/msgdrv.h"
#include "pmario_sound.h"
#include <string.h>

#pragma pool_data off

#pragma force_active on
extern s32 evt_emergency[];
static void callback(s32 state, smartEntry* smart);

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

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static BeroEntry bero_entry_data[2] = {
    {
        "w_bero", 0x0001, 0x000A, 0x00004E20,
        0x000186A0, 0x00000000, 0x00000000, 0xFFFFFFFF,
        NULL, 0xFFFFFFFF, NULL, "aji_01",
        "e_bero_2", 0x0005, 0x0005, (s32)w_door_close,
        (s32)w_door_open,
    },
};

EVT_BEGIN(sekizou_kaiten)
    CALL(evt_mapobj_get_position, LW(2), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_DSN_ROUND1"), LW(10), LW(11), LW(12), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, LW(0), -50, LW(1))
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, LW(2), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_mapobj_get_position, LW(2), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_QUAKE2"), LW(10), LW(11), LW(12), 0)
    CALL(evt_sub_intpl_msec_init, 11, LW(0), 0, 500)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, LW(2), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_snd_sfxoff, LW(9))
    RETURN
EVT_END()

EVT_BEGIN(sekizou_kaiten2)
    CALL(evt_mapobj_get_position, LW(2), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_DSN_ROUND1"), LW(10), LW(11), LW(12), LW(9))
    CALL(evt_sub_intpl_msec_init, 11, LW(0), 0, LW(1))
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, LW(2), FLOAT(0.0f), LW(0), FLOAT(0.0f))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_mapobj_get_position, LW(2), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_QUAKE2"), LW(10), LW(11), LW(12), 0)
    CALL(evt_snd_sfxoff, LW(9))
    RETURN
EVT_END()

EVT_BEGIN(sekizou_talk)
    RETURN
EVT_END()

EVT_BEGIN(evt_item)
    SET_VALUE(GSW(0), 367)
    RETURN
EVT_END()

static EVT_BEGIN(evt_down_snd)
    WAIT_MS(100)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(666)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(333)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(250)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(83)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(183)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(166)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(83)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(150)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(16)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(16)
    WAIT_MS(133)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(16)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(16)
    WAIT_MS(33)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(16)
    WAIT_MS(50)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(16)
    WAIT_MS(33)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE1"), 0)
    WAIT_MS(83)
    WAIT_MS(50)
    WAIT_MS(16)
    WAIT_MS(33)
    WAIT_MS(33)
    WAIT_MS(16)
    WAIT_MS(16)
    WAIT_MS(16)
    RETURN
EVT_END()

static EVT_BEGIN(evt_up_snd_)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE_MOVE1"), 0)
    WAIT_MS(366)
    CALL(evt_snd_sfxon, STRING("SFX_OFF1_PUZZLE2"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_up_snd__)
    WAIT_MS(16)
    WAIT_MS(16)
    WAIT_MS(16)
    WAIT_MS(16)
    WAIT_MS(16)
    WAIT_MS(16)
    WAIT_MS(16)
    WAIT_MS(50)
    WAIT_MS(83)
    WAIT_MS(16)
    WAIT_MS(16)
    WAIT_MS(16)
    WAIT_MS(50)
    WAIT_MS(16)
    WAIT_MS(50)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(16)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(83)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(50)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(16)
    WAIT_MS(16)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(400)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(183)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(83)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(250)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(333)
    SPAWN_EVENT(evt_up_snd_)
    WAIT_MS(666)
    SPAWN_EVENT(evt_up_snd_)
    RETURN
EVT_END()

EVT_BEGIN(offscreen_to_kuizu)
    CALL(evt_paper_entry, STRING("OFF_d_pazu"))
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_pazu"))
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("Z_1"))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, 0, 1, 0, 0, 608, 480)
    CALL(evt_img_clear_virtual_point, STRING("capture_img"))
    WAIT_FRAMES(1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_aji_04"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kuizu"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_ato2"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_aka"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_ao"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("dennshoku"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dennshoku_2"), 1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, -90, 20, -10)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_set_pos, 0, -105, 20, -20)
    CALL(evt_cam3d_evt_set, -189, 72, 207, 219, 69, -80, 0, 11)
    SPAWN_EVENT(evt_down_snd)
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(offscreen_to_normal)
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_aji_04"), STRING("offscreen"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_kuizu"))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 0, 0, 0, 608, 480)
    SPAWN_EVENT(evt_up_snd__)
    WAIT_MS(500)
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    CALL(evt_img_clear_virtual_point, STRING("capture_img"))
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_2"))
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_aji_04"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_aji_04"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kuizu"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_mae"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_ato"), 1)
    CALL(evt_mario_set_pos, -90, 0, -10)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_set_pos, 0, -105, 0, -20)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_cam3d_evt_set, -189, 72, 207, 219, 69, -80, 0, 11)
    RETURN
EVT_END()

// TODO: type
u32 kuizu_data[28] = {
    (u32)"anm_1mon", (u32)"stg7_aji_06", (u32)"stg7_aji_07", 0x00000002,
    (u32)"anm_2mon", (u32)"stg7_aji_08", (u32)"stg7_aji_09", 0x00000003,
    (u32)"anm_3mon", (u32)"stg7_aji_10", (u32)"stg7_aji_11", 0x00000001,
    (u32)"anm_4mon", (u32)"stg7_aji_12", (u32)"stg7_aji_13", 0x00000002,
    (u32)"anm_5mon", (u32)"stg7_aji_14", (u32)"stg7_aji_15", 0x00000000,
    (u32)"anm_6mon", (u32)"stg7_aji_16", (u32)"stg7_aji_17", 0x00000002,
    (u32)"anm_7mon", (u32)"stg7_aji_18", (u32)"stg7_aji_19", 0x00000003,
};

EVT_BEGIN(UNK_0x00022a90)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 150)
    CALL(evt_eff, 0, STRING("confetti"), 3, LW(0), LW(1), LW(2), 160, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("dennshoku"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dennshoku_2"), 1)
    CALL(evt_map_playanim, STRING("anm_seikai"), 1, 0)
    WAIT_MS(1800)
    RETURN
EVT_END()

EVT_BEGIN(quiz_ok)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("dennshoku"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dennshoku_2"), 1)
    CALL(evt_map_playanim, STRING("anm_seikai"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(quiz_ng)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dennshoku"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("dennshoku_2"), 1)
    CALL(evt_map_playanim, STRING("anm_fuseikai"), 1, 0)
    WAIT_MS(1500)
    RETURN
EVT_END()

EVT_BEGIN(UNK_0x00022be8)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dennshoku"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("dennshoku_2"), 1)
    CALL(evt_map_playanim, STRING("anm_fuseikai"), 0, 0)
    WAIT_MS(1500)
    RETURN
EVT_END()

EVT_BEGIN(UNK_0x00022c3c)
    CALL(evt_sub_intpl_msec_init, 11, 0, 70, 100)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, STRING("kuti"), 0, 0, LW(0))
    CALL(evt_mapobj_rotate, 1, STRING("kuti2"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_sub_intpl_msec_init, 11, 70, 0, 100)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mapobj_rotate, 1, STRING("kuti"), 0, 0, LW(0))
    CALL(evt_mapobj_rotate, 1, STRING("kuti2"), 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    RETURN
EVT_END()

void unk_aji_00000ed0(s32 state, smartEntry* smart) {
    MessageData* data = smart->address;
    EventEntry* event = data->event;

    if ((EventEntry*)event->lwData[8] == NULL || !evtCheckID(((EventEntry*)event->lwData[8])->eventId)) {
        if (state == 5) {
            event->lwData[8] = (s32)evtEntry(UNK_0x00022c3c, 0, 0);
        }
    }
}

static void callback(s32 state, smartEntry* smart) {
    MessageData* data;
    EventEntry* event;

    data = smart->address;
    event = data->event;
    if (strcmp(data->tag, "stg7_aji_06") == 0) {
        if (state == 3) {
            event->lwData[5]++;
            if (event->lwData[5] == 3) {
                psndSFXOn("SFX_STG7_QUIZ_START1");
            }
            else if (event->lwData[5] == 4) {
                psndBGMOff(0x400);
                psndBGMOn(1, "BGM_EVT_QUIZ2");
            }
        }
    }
    else if (state == 3) {
        event->lwData[5]++;
        if (event->lwData[5] == 1) {
            psndBGMOff(0x400);
            psndBGMOn(1, "BGM_EVT_QUIZ2");
        }
    }

    data = smart->address;
    event = data->event;
    if ((EventEntry*)event->lwData[8] == NULL || !evtCheckID(((EventEntry*)event->lwData[8])->eventId)) {
        if (state == 5) {
            event->lwData[8] = (s32)evtEntry(UNK_0x00022c3c, 0, 0);
        }
    }
}

EVT_BEGIN(kuizu_sekizou)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_envoff_f, 512, 2000)
    CALL(evt_snd_sfxon, STRING("SFX_STG7_QUAKE1"), GW(10))
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 2500)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, -189, 72, 207, 219, 69, -80, 1500, 11)
    WAIT_MS(2500)
    CALL(evt_snd_sfxoff, GW(10))
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_DSN_AWAKE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_mae"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_ato"), 1)
    WAIT_MS(500)
    SET_VALUE(LW(8), 0)
    CALL(evt_msg_toge, 2, 95, 65, 0)
    SPAWN_EVENT(sekizou_talk)
    CALL(evt_msg_print, 0, STRING("stg7_aji_01"), PTR(unk_aji_00000ed0), 0)
    CALL(evt_msg_select, 0, STRING("stg7_aji_02"))
    IF_INT_EQUAL(LW(0), 1)
        SET_VALUE(LW(0), 360)
        SET_VALUE(LW(1), 500)
        SET_VALUE(LW(2), STRING("DOSN"))
        CALL_CHILD(sekizou_kaiten)
        SPAWN_EVENT(sekizou_talk)
        CALL(evt_msg_print_add, 0, STRING("stg7_aji_04"))
        WAIT_MS(500)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_mae"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_ato"), 1)
        CALL(evt_cam3d_evt_off, 1000, 11)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    SET_VALUE(LW(0), 360)
    SET_VALUE(LW(1), 500)
    SET_VALUE(LW(2), STRING("DOSN"))
    CALL_CHILD(sekizou_kaiten)
    SPAWN_EVENT(sekizou_talk)
    CALL(evt_msg_print_add, 0, STRING("stg7_aji_05"))
    WAIT_MS(500)
    CALL(evt_map_playanim, STRING("anm_denshoku"), 1, 0)
    CALL(evt_map_playanim, STRING("anm_mamekyuu"), 1, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_QUIZ1"))
    CALL_CHILD(offscreen_to_kuizu)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 0, 83, 591, 0, 100, 91, 1000, 11)
    WAIT_MS(1000)
    SET_VALUE(LW(13), 0)
    SET_VALUE(LW(14), 0)
    SET_VALUE(LW(15), 0)
    SET_INT_READ_ADDR(PTR(kuizu_data))
    CALL(evt_snd_bgmoff, 18432)
    LOOP(0)
        READ_INT4(LW(9), LW(10), LW(11), LW(12))
        CALL(evt_map_playanim, LW(9), 1, 0)
        WAIT_MS(400)
        SET_VALUE(LF(0), 0)
        SET_VALUE(LW(0), 360)
        SET_VALUE(LW(1), 500)
        SET_VALUE(LW(2), STRING("DOSN2"))
        SPAWN_EVENT(sekizou_kaiten)
        WAIT_MS(500)
        IF_INT_NOT_EQUAL(LW(13), 0)
            CALL(evt_snd_sfxon, STRING("SFX_STG7_QUIZ_START1"), 0)
        END_IF
        CALL(evt_msg_toge, 2, 95, 65, 0)
        SPAWN_EVENT(sekizou_talk)
        SET_VALUE(LW(5), 0)
        CALL(evt_msg_print, 0, LW(10), PTR(callback), 0)
        LOOP(0)
            CALL(evt_msg_select, 0, LW(11))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(0), LW(12))
                SET_VALUE(LF(0), 1)
                DEBUG_PUT_MSG(STRING("せいかい"))
            ELSE
                SET_VALUE(LF(0), 0)
                DEBUG_PUT_MSG(STRING("ふせいかい"))
            END_IF
            SUBTRACT(LW(0), 2)
            IF_INT_EQUAL(LW(13), 0)
                SET_VALUE(LW(0), 0)
            END_IF
            IF_INT_EQUAL(LF(1), 1)
                SET_VALUE(LW(0), 0)
                SET_VALUE(LF(1), 0)
            END_IF
            IF_INT_EQUAL(LW(0), 0)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print_add, 0, STRING("stg7_aji_20"))
                CALL(evt_msg_select, 0, STRING("stg7_aji_03"))
                IF_INT_EQUAL(LW(0), 0)
                    SET_VALUE(LF(1), 0)
                    BREAK_LOOP
                ELSE
                    SPAWN_EVENT(sekizou_talk)
                    CALL(evt_msg_print_add, 0, STRING("stg7_aji_21"))
                END_IF
            ELSE
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        CALL(evt_msg_continue)
        CALL(evt_msg_toge, 2, 95, 65, 0)
        ADD(LW(13), 1)
        IF_INT_EQUAL(LF(0), 1)
            IF_INT_EQUAL(LW(14), 4)
                CALL(evt_snd_sfxon, STRING("SFX_STG7_GOOD_SOUND2"), 0)
            ELSE
                CALL(evt_snd_sfxon, STRING("SFX_STG7_GOOD_SOUND1"), 0)
            END_IF
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_ato2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_ao"), 1)
            SPAWN_EVENT(UNK_0x00022a90)
            WAIT_MS(1500)
            CALL(evt_cam3d_evt_set, -189, 72, 207, 219, 69, -80, 0, 11)
            WAIT_FRAMES(1)
            ADD(LW(14), 1)
            DEBUG_PUT_REG(LW(14))
            SWITCH(LW(14))
                CASE_LESS(4)
                SET_VALUE(LW(0), 360)
                SET_VALUE(LW(1), 500)
                SET_VALUE(LW(2), STRING("DOSN2"))
                SPAWN_EVENT(sekizou_kaiten2)
                WAIT_MS(500)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_22"), PTR(unk_aji_00000ed0), 0)
                CASE_EQUAL(4)
                SET_VALUE(LW(0), 360)
                SET_VALUE(LW(1), 500)
                SET_VALUE(LW(2), STRING("DOSN2"))
                SPAWN_EVENT(sekizou_kaiten2)
                WAIT_MS(500)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_23"), PTR(unk_aji_00000ed0), 0)
                SET_VALUE(LF(1), 1)
                CASE_EQUAL(5)
                SET_VALUE(LW(0), 1080)
                SET_VALUE(LW(1), 1000)
                SET_VALUE(LW(2), STRING("DOSN2"))
                SPAWN_EVENT(sekizou_kaiten)
                WAIT_MS(1000)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_24"), PTR(unk_aji_00000ed0), 0)
                CALL_CHILD(offscreen_to_normal)
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI1"))
                CALL(evt_msg_toge, 2, 95, 65, 0)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_25"), PTR(unk_aji_00000ed0), 0)
                BREAK_LOOP
            END_SWITCH
            CALL_CHILD(quiz_ok)
        ELSE
            IF_INT_EQUAL(LW(15), 2)
                CALL(evt_snd_sfxon, STRING("SFX_STG7_BAD_SOUND2"), 0)
            ELSE
                CALL(evt_snd_sfxon, STRING("SFX_STG7_BAD_SOUND1"), 0)
            END_IF
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_ato2"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_aka"), 1)
            SPAWN_EVENT(quiz_ng)
            WAIT_MS(1500)
            CALL(evt_cam3d_evt_set, -189, 72, 207, 219, 69, -80, 0, 11)
            WAIT_FRAMES(1)
            ADD(LW(15), 1)
            DEBUG_PUT_REG(LW(15))
            SWITCH(LW(15))
                CASE_EQUAL(1)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_26"), PTR(unk_aji_00000ed0), 0)
                CASE_EQUAL(2)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_27"), PTR(unk_aji_00000ed0), 0)
                SET_VALUE(LF(1), 1)
                CASE_EQUAL(3)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_28"), PTR(unk_aji_00000ed0), 0)
                CALL(evt_snd_bgmoff_f, 512, 2000)
                CALL_CHILD(offscreen_to_normal)
                SET_VALUE(LW(0), 360)
                SET_VALUE(LW(1), 500)
                SET_VALUE(LW(2), STRING("DOSN"))
                SPAWN_EVENT(sekizou_kaiten2)
                WAIT_MS(250)
                CALL(evt_msg_toge, 2, 95, 65, 0)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_29"), PTR(unk_aji_00000ed0), 0)
                CALL(evt_npc_set_position, STRING("バリアーン１"), 50, 250, -50)
                CALL(evt_npc_set_position, STRING("バリアーン２"), 50, 250, 50)
                BEGIN_INLINE_EVENT
                    CALL(evt_npc_jump_position_nohit, STRING("バリアーン１"), 50, 20, -50, 300, 0)
                    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 200)
                END_INLINE_EVENT
                WAIT_MS(300)
                BEGIN_INLINE_EVENT
                    CALL(evt_npc_jump_position_nohit, STRING("バリアーン２"), 50, 20, 50, 300, 0)
                    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 200)
                END_INLINE_EVENT
                WAIT_MS(300)
                WAIT_MS(500)
                CALL(evt_msg_toge, 2, 95, 65, 0)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_30"), PTR(unk_aji_00000ed0), 0)
                CALL(evt_cam3d_evt_off, 500, 11)
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                BEGIN_INLINE_EVENT
                    CALL(evt_npc_move_position, STRING("バリアーン１"), LW(0), LW(2), 1000, 0, 0)
                END_INLINE_EVENT
                WAIT_MS(100)
                BEGIN_INLINE_EVENT
                    CALL(evt_npc_move_position, STRING("バリアーン２"), LW(0), LW(2), 1000, 0, 0)
                END_INLINE_EVENT
                WAIT_MS(100)
                CALL(evt_mario_dispflag_onoff, 1, 2)
                CALL(evt_mario_set_pose, STRING("M_N_7"))
                WAIT_MS(500)
                CALL(evt_npc_set_link, STRING("バリアーン１"), STRING("バリアーン２"))
                CALL(evt_npc_set_battle_info, STRING("バリアーン１"), 15)
                CALL(evt_npc_battle_start, STRING("バリアーン１"))
                CALL(evt_npc_wait_battle_end)
                CALL(evt_mario_set_pose, STRING("M_S_1"))
                CALL(evt_mario_dispflag_onoff, 0, 2)
                CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG7_DUN2"), 2000)
                CALL(evt_snd_envon_f, 272, STRING("ENV_STG7_AJI1"), 2000)
                CALL(evt_npc_set_position, STRING("バリアーン１"), 0, -1000, 0)
                CALL(evt_npc_set_position, STRING("バリアーン２"), 0, -1000, 0)
                WAIT_MS(1000)
                CALL(evt_cam3d_evt_set, -189, 72, 207, 219, 69, -80, 2300, 11)
                WAIT_MS(300)
                SET_VALUE(LW(0), 1080)
                SET_VALUE(LW(1), 1500)
                SET_VALUE(LW(2), STRING("DOSN"))
                CALL_CHILD(sekizou_kaiten)
                CALL(evt_msg_toge, 2, 95, 65, 0)
                SPAWN_EVENT(sekizou_talk)
                CALL(evt_msg_print, 0, STRING("stg7_aji_31"), PTR(unk_aji_00000ed0), 0)
                BREAK_LOOP
            END_SWITCH
            CALL_CHILD(UNK_0x00022be8)
        END_IF
        CALL(evt_cam3d_evt_set, 0, 83, 591, 0, 100, 91, 500, 11)
        WAIT_MS(500)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_ato2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_ao"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_aka"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("dennshoku"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dennshoku_2"), 1)
    END_LOOP
    CALL(evt_snd_sfxon, STRING("SFX_STG7_QUAKE1"), LW(10))
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 1500)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_item_entry, STRING("dummy"), 28, 50, 100, 0, 13, -1, PTR(evt_item))
    CALL(evt_msg_toge, 2, 95, 65, 0)
    SPAWN_EVENT(sekizou_talk)
    CALL(evt_msg_print, 0, STRING("stg7_aji_32"), PTR(unk_aji_00000ed0), 0)
    SET_VALUE(LW(0), 360)
    SET_VALUE(LW(1), 500)
    SET_VALUE(LW(2), STRING("DOSN"))
    CALL_CHILD(sekizou_kaiten2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_mae"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_ato"), 1)
    SET_VALUE(GSW(0), 366)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(sekizou_init)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_mae"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_ato"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("dennshoku"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dennshoku_2"), 1)
    CALL(evt_mapobj_trans, 1, STRING("S_kuizu"), 0, -1000, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_kuizu"), 1)
    CALL(evt_run_case_evt, 9, 1, STRING("a_sekizou_1_a"), 0, PTR(kuizu_sekizou), 0)
    RETURN
EVT_END()

EVT_BEGIN(aji_04_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI1"))
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    IF_INT_LESS(GSW(0), 366)
        CALL_CHILD(sekizou_init)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("me_mae"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("me_ato"), 1)
    END_IF
    IF_INT_EQUAL(GSW(0), 366)
        CALL(evt_item_entry, STRING("dummy"), 28, 50, 0, 0, 16, -1, PTR(evt_item))
    END_IF
    SPAWN_EVENT(evt_emergency)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

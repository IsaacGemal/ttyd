#include "data/item_data.h"
#include "driver/animdrv.h"
#include "driver/mobjdrv.h"
#include "driver/npcdrv.h"
#include "mario/mario.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "memory.h"
#include "pmario_sound.h"
#include <string.h>
#include "usu_01.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_door.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_lecture.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_shop.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_urouro.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_kagemario_init_29_text_1A0C[];
extern s32 evt_usu_kagemario_party_kill_29_data_104A8[];
extern s32 roll_majin_lecture[];
extern s32 roll_majin_lecture_init[];
static USER_FUNC(bgm_start_wait);
static void majin_callback(s32 type);
static USER_FUNC(npc_process);
static USER_FUNC(booboo_main);
static USER_FUNC(set_fook_evt);
s32 village_chief_change_pig_event[166];
static DoorEntry door_1[1];
s32 evt_monban_buta2[17];
static char* obj_list[12];

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

EVT_BEGIN(villagerF_init)
    CALL(evt_npc_flag_onoff, 1, STRING("村人Ｆ"), 1536)
    RETURN
EVT_END()

EVT_BEGIN(villagerF_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_msg_print, 0, STRING("stg4_usu_82"), 0, STRING("me"))
        CASE_LESS(219)
        CALL(evt_msg_print, 0, STRING("stg4_usu_30"), 0, STRING("me"))
        CALL(evt_msg_select, 0, STRING("stg4_usu_30_yn"))
        CALL(evt_msg_print_add, 0, STRING("stg4_usu_30_00"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("stg4_usu_84"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg4_usu_84_1"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_84_1_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(villagerG_init)
    CALL(evt_npc_flag_onoff, 1, STRING("村人Ｇ"), 1536)
    RETURN
EVT_END()

EVT_BEGIN(villagerG_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_msg_print, 0, STRING("stg4_usu_85"), 0, STRING("me"))
        CASE_LESS(219)
        CALL(evt_msg_print, 0, STRING("stg4_usu_86"), 0, STRING("me"))
        CALL(evt_msg_select, 0, STRING("stg4_usu_86_yn"))
        CALL(evt_msg_print_add, 0, STRING("stg4_usu_86_00"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("stg4_usu_87"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg4_usu_87_1"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_87_1_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(villagerH_init)
    CALL(evt_npc_flag_onoff, 1, STRING("村人Ｈ"), 1536)
    RETURN
EVT_END()

EVT_BEGIN(villagerH_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_msg_print, 0, STRING("stg4_usu_88"), 0, STRING("me"))
        CASE_LESS(219)
        CALL(evt_msg_print, 0, STRING("stg4_usu_89"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("stg4_usu_90"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg4_usu_90_1"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_90_1_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(villagerI_init)
    CALL(evt_npc_flag_onoff, 1, STRING("村人Ｉ"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_npc_set_position, STRING("村人Ｉ"), 0, -2000, 0)
        CASE_DEFAULT
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(villagerI_pig_init)
    CALL(evt_npc_flag_onoff, 1, STRING("ブタ村人Ｉ"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("ブタ村人Ｉ"), 0, -2000, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(villagerI_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_VOICE_PIG1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_msg_print, 0, STRING("stg4_usu_91"), 0, STRING("me"))
        CASE_LESS(219)
        CALL(evt_msg_print, 0, STRING("stg4_usu_92"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("stg4_usu_93"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg4_usu_93_1"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_93_1_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(villagerJ_init)
    CALL(evt_npc_flag_onoff, 1, STRING("村人Ｊ"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_npc_set_position, STRING("村人Ｊ"), 0, -2000, 0)
        CASE_DEFAULT
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(villagerJ_pig_init)
    CALL(evt_npc_flag_onoff, 1, STRING("ブタ村人Ｊ"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("ブタ村人Ｊ"), 0, -2000, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(villagerJ_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_VOICE_PIG1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_msg_print, 0, STRING("stg4_usu_94"), 0, STRING("me"))
        CASE_LESS(219)
        CALL(evt_msg_print, 0, STRING("stg4_usu_95"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("stg4_usu_96"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg4_usu_96_1"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_96_1_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(shop_man_init)
    CALL(evt_npc_flag_onoff, 1, STRING("店の主人"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CASE_LESS(219)
        CALL(evt_npc_set_position, STRING("店の主人"), -125, 0, -475)
        CALL(evt_npc_set_ry, STRING("店の主人"), 90)
        CALL(evt_npc_flag_onoff, 1, STRING("店の主人"), 4194304)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("店の主人"), -130, 0, -480)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(shop_man_talk)
    SWITCH(GSW(0))
        CASE_LESS(219)
        CALL(evt_msg_print, 0, STRING("stg4_usu_61_5"), 0, STRING("店の主人"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg4_usu_61_6"), 0, STRING("店の主人"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_61_7"), 0, STRING("店の主人"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(shop_woman_init)
    CALL(evt_npc_flag_onoff, 1, STRING("お店の奥さん"), 1536)
    RETURN
EVT_END()

USER_FUNC(sleep_voice) {
    MarioWork* mario = marioGetPtr();
    if (isFirstCall) {
        event->userdata[0] = -1;
    }
    if (strcmp(mario->animName, "M_N_5A") == 0) {
        s32 loop = (s32)animPoseGetLoopTimes(mario->paperAnimGroupId[marioAnimeId()]);
        if (loop != event->userdata[0]) {
            event->userdata[0] = loop;
            evtSetValue(event, GW(10), psndSFXOn_3D("SFX_VOICE_MARIO_SLEEP1", &mario->position));
        }
    }
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(shop_woman_talk)
    SWITCH(GSW(0))
        CASE_LESS(184)
        IF_INT_EQUAL(GSWF(1925), 0)
            CALL(evt_msg_print, 0, STRING("stg4_usu_52"), 0, STRING("me"))
            WAIT_MS(200)
            CALL(evt_mario_set_pose, STRING("M_I_2"))
            WAIT_MS(1000)
            CALL(evt_msg_print, 0, STRING("stg4_usu_53"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_I_N"))
            WAIT_MS(1000)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            BEGIN_INLINE_EVENT_GET_ID(LW(10))
                CALL(sleep_voice)
            END_INLINE_EVENT
            SET_VALUE(GF(0), 0)
            CALL(evt_msg_print, 0, STRING("stg4_usu_54"), 0, STRING("me"))
            BEGIN_INLINE_EVENT
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_YAWN1"), LW(0), LW(1), LW(2), 0)
                CALL(evt_mario_set_pose, STRING("M_I_5"))
                WAIT_MS(1000)
                CALL(evt_mario_set_pose, STRING("M_N_5A"))
                WAIT_MS(1000)
                SET_VALUE(GF(0), 1)
            END_INLINE_EVENT
            CALL(evt_msg_continue)
            LOOP(0)
                WAIT_FRAMES(1)
                IF_INT_EQUAL(GF(0), 1)
                    BREAK_LOOP
                END_IF
            END_LOOP
            DELETE_EVENT(LW(10))
            CALL(evt_snd_sfxoff, GW(10))
            WAIT_MS(200)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_AWAKE1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_mario_set_pose, STRING("M_N_5B"))
            WAIT_MS(600)
            CALL(evt_mario_set_pose, STRING("M_N_2"))
            WAIT_MS(600)
            CALL(evt_msg_print, 0, STRING("stg4_usu_55"), 0, STRING("me"))
            SET_VALUE(GSWF(1925), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg4_usu_56"), 0, STRING("me"))
        END_IF
        CASE_LESS(185)
        CALL(evt_msg_print, 0, STRING("stg4_usu_58"), 0, STRING("me"))
        CASE_LESS(200)
        CALL(evt_msg_print, 0, STRING("stg4_usu_58_01"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_59"), 0, STRING("me"))
        CALL(evt_msg_continue)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(kizayarou_init)
    CALL(evt_npc_flag_onoff, 1, STRING("キザ野郎"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_npc_set_position, STRING("キザ野郎"), 0, -2000, 0)
        CASE_LESS(219)
        CALL(evt_npc_set_position, STRING("キザ野郎"), -100, 0, -450)
        CALL(evt_npc_flag_onoff, 1, STRING("キザ野郎"), 4194304)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("キザ野郎"), 0, -2000, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(kizayarou_pig_init)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("ブタキザ野郎"), 0, -2000, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(kizayarou_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_VOICE_PIG1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_msg_print, 0, STRING("stg4_usu_61_00"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_61"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(childABC_talk_01)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg4_usu_63"), 0, STRING("子供Ａ"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_msg_print, 0, STRING("stg4_usu_64"), 0, STRING("子供Ｂ"))
    END_INLINE_EVENT
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("stg4_usu_65"), 0, STRING("子供Ｃ"))
    RETURN
EVT_END()

EVT_BEGIN(childABC_talk_02)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg4_usu_79"), 0, STRING("子供Ａ"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_msg_print, 0, STRING("stg4_usu_80"), 0, STRING("子供Ｂ"))
    END_INLINE_EVENT
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("stg4_usu_81"), 0, STRING("子供Ｃ"))
    RETURN
EVT_END()

EVT_BEGIN(childABC_talk_03)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg4_usu_81_2"), 0, STRING("子供Ａ"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_msg_print, 0, STRING("stg4_usu_81_3"), 0, STRING("子供Ｂ"))
    END_INLINE_EVENT
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("stg4_usu_81_4"), 0, STRING("子供Ｃ"))
    RETURN
EVT_END()

EVT_BEGIN(childABC_talk_04)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg4_usu_81_4_2"), 0, STRING("子供Ａ"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_msg_print, 0, STRING("stg4_usu_81_4_3"), 0, STRING("子供Ｂ"))
    END_INLINE_EVENT
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("stg4_usu_81_4_4"), 0, STRING("子供Ｃ"))
    RETURN
EVT_END()

EVT_BEGIN(aunt_init)
    CALL(evt_npc_flag_onoff, 1, STRING("おばさん"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(200)
        IF_INT_EQUAL(GSWF(1927), 1)
            CALL(evt_npc_set_position, STRING("おばさん"), 0, -2000, 0)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(aunt_pig_init)
    CALL(evt_npc_flag_onoff, 1, STRING("ブタおばさん"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(200)
        IF_INT_EQUAL(GSWF(1927), 0)
            CALL(evt_npc_set_position, STRING("ブタおばさん"), 0, -2000, 0)
        END_IF
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("ブタおばさん"), 0, -2000, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(iri_20_out_house)
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam3d_evt_set_xyz, -420, 1, 0, 0, 150, 11)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(14), 0)
        SPAWN_EVENT(door_light2)
    END_IF
    SPAWN_EVENT(door_up_se)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(8), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
            LABEL(3)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LF(0), 1)
            SPAWN_EVENT(LW(8))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(3)
            END_IF
        END_INLINE_EVENT
    END_IF
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(9), 0)
        BEGIN_INLINE_EVENT
            WAIT_MS(400)
            SPAWN_EVENT(house_close_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            SET_VALUE(LW(6), 0)
            CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 3)
            SET_VALUE(LF(0), 1)
            SPAWN_EVENT(LW(9))
            SET_VALUE(LW(6), 1)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(2)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    IF_INT_EQUAL(LW(14), 0)
        SPAWN_EVENT(door_light)
    END_IF
    SPAWN_EVENT(door_open_se)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 2)
            SET_VALUE(LF(0), 1)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(1)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    CALL(evt_door_param, LW(0), 5, LW(1), LW(2), LW(3))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("おばさん"), LW(1), LW(3), 0, 60, 0)
        CALL(evt_npc_move_position, STRING("おばさん"), -368, 0, 0, 60, 0)
        CALL(evt_npc_set_ry, STRING("おばさん"), 270)
        WAIT_MS(300)
    END_INLINE_EVENT
    CALL(evt_mario_party_door_move, LW(1), LW(3), 60)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -420, 0, 60)
        CALL(evt_mario_set_dir_npc, STRING("おばさん"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -450, -1, 60)
        CALL(evt_party_set_dir_npc, 0, STRING("おばさん"))
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            SPAWN_EVENT(door_close_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 3)
            SET_VALUE(LF(0), 1)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(0)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    CALL(evt_cam3d_evt_xyz_off, 100, 11)
    CALL(door_dark_flag, LW(0), 0)
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        IF_INT_NOT_EQUAL(LW(15), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 4096)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 0)
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2048)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 4096)
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_door_load_mapflag, LW(1))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(iri_20_aunt_to_house)
    CALL(evt_npc_move_position, STRING("おばさん"), -390, -40, 0, 60, 0)
    SET_VALUE(LW(0), PTR(door_1))
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1024)
    CALL(evt_npc_move_position, STRING("me"), -390, -20, 0, 60, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1024)
    SPAWN_EVENT(door_open_se)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 0)
            SET_VALUE(LF(0), 0)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(0)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("おばさん"), -390, -185, 0, 60, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            SPAWN_EVENT(door_close_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 1)
            SET_VALUE(LF(0), 0)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(1)
            END_IF
        END_INLINE_EVENT
    END_IF
    RETURN
EVT_END()

USER_FUNC(get_item_list) {
    s32* args = event->args;
    s32* list = _mapAlloc((pouchGetHaveItemCnt() + 1) * 4);
    s32 j, i;
    for (i = 0, j = 0; i < 20; i++) {
        if (pouchHaveItem(i) != 0) {
            list[j] = pouchHaveItem(i);
            j++;
        }
    }
    list[j] = -1;
    evtSetValue(event, args[0], (s32)list);
    return EVT_RETURN_DONE;
}

USER_FUNC(delete_item_list) {
    _mapFree((void*)evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(check_tabemono) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    ItemData* item = &itemDataTable[idx];
    s32 flag = 0;
    if (item->usable_locations & 4) {
        flag = 1;
    }
    evtSetValue(event, args[1], flag);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(aunt_talk)
    IF_INT_EQUAL(GSW(1420), 20)
        IF_INT_EQUAL(GSWF(5405), 0)
            CALL(evt_msg_print, 0, STRING("irai_20_001"), 0, STRING("me"))
            CALL(evt_npc_flag_onoff, 1, STRING("me"), 32)
            BEGIN_INLINE_EVENT
                CALL(evt_npc_move_position, STRING("me"), -390, -130, 0, 60, 0)
            END_INLINE_EVENT
            CALL(evt_mario_mov_pos2, -390, -100, 60)
            WAIT_MS(500)
            CALL(evt_npc_flag_onoff, 0, STRING("me"), 32)
            SET_VALUE(LW(0), PTR(door_1))
            SET_VALUE(LW(14), 0)
            SET_VALUE(LW(15), 0)
            CALL_CHILD(iri_20_out_house)
            CALL(evt_snd_bgmoff_f, 1024, 2000)
            WAIT_MS(1000)
            BEGIN_INLINE_EVENT
                WAIT_MS(2000)
                CALL(evt_snd_bgmoff, 1024)
                CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_ORGEL1"))
            END_INLINE_EVENT
            CALL(evt_msg_print, 0, STRING("irai_20_002"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_N_1"))
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("irai_20_003"), 0, STRING("me"))
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_snd_bgmoff_f, 513, 3000)
            CALL_CHILD(iri_20_aunt_to_house)
            CALL(evt_snd_bgmoff_f, 513, 2000)
            CALL(evt_snd_bgmon_f, 288, 0, 2000)
            CALL(evt_snd_bgmon, 160, 0)
            CALL(evt_snd_envon, 160, 0)
            CALL(evt_snd_env_lpf, 0, 0)
            SET_VALUE(GSWF(5405), 1)
        ELSE
            IF_INT_EQUAL(GSWF(5406), 0)
                CALL(evt_msg_print, 0, STRING("irai_20_004"), 0, STRING("me"))
            ELSE
                CALL(evt_npc_flag_onoff, 1, STRING("おばさん"), 32)
                CALL(evt_npc_flag_onoff, 1, STRING("子供Ｂ"), 32)
                SET_VALUE(GF(0), 1)
                BEGIN_INLINE_EVENT
                    CALL(evt_party_move_pos2, 0, -443, -140, 60)
                    CALL(evt_party_set_dir_npc, 0, STRING("おばさん"))
                    WAIT_MS(300)
                    SET_VALUE(GF(0), 0)
                END_INLINE_EVENT
                CALL(evt_mario_mov_pos2, -415, -150, 60)
                CALL(evt_mario_set_dir_npc, STRING("おばさん"))
                WAIT_MS(400)
                CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
                WAIT_FRAMES(1)
                CALL(evt_npc_wait_pera, STRING("me"))
                WAIT_WHILE(GF(0))
                CALL(evt_npc_flag_onoff, 0, STRING("おばさん"), 32)
                CALL(evt_npc_flag_onoff, 0, STRING("子供Ｂ"), 32)
                CALL(evt_msg_print, 0, STRING("irai_20_005"), 0, STRING("me"))
                CALL(evt_msg_select, 0, STRING("irai_20_006"))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(evt_msg_print_add, 0, STRING("irai_20_007"))
                ELSE
                    CALL(evt_msg_continue)
                    CALL(evt_mario_set_pose, STRING("M_I_2"))
                    WAIT_MS(1200)
                    CALL(evt_mario_set_pose, STRING("M_S_1"))
                    CALL(evt_msg_print, 0, STRING("irai_20_008"), 0, STRING("me"))
                END_IF
                BEGIN_INLINE_EVENT
                    CALL(evt_msg_print, 0, STRING("irai_20_009"), 0, STRING("子供Ａ"))
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    WAIT_MS(500)
                    CALL(evt_msg_print, 0, STRING("irai_20_010"), 0, STRING("子供Ｂ"))
                END_INLINE_EVENT
                WAIT_MS(1000)
                CALL(evt_msg_print, 0, STRING("irai_20_011"), 0, STRING("子供Ｃ"))
                CALL(evt_msg_print, 0, STRING("irai_20_012"), 0, STRING("me"))
                CALL(evt_pouch_get_haveitemcnt, LW(0))
                CALL(evt_pouch_check_item, 1, LW(1))
                IF_INT_EQUAL(LW(1), 0)
                    SET_VALUE(LW(1), 10)
                ELSE
                    SET_VALUE(LW(1), 20)
                END_IF
                IF_INT_LESS(LW(0), LW(1))
                    CALL(evt_msg_continue)
                    CALL(evt_item_entry, STRING("sd"), 217, 0, -1000, 0, 17, -1, 0)
                    CALL(evt_item_get_item, STRING("sd"))
                    CALL(evt_item_get_item_end_wait, STRING("sd"))
                    SET_VALUE(GSWF(1938), 1)
                ELSE
                    CALL(evt_msg_print_add, 0, STRING("irai_20_012_1"))
                    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
                    CALL(evt_npc_move_position, STRING("me"), -363, -152, 0, 40, 0)
                    WAIT_MS(300)
                    CALL(evt_item_entry, STRING("iri_20"), 217, -345, 20, -132, 16, GSWF(1938), 0)
                    WAIT_MS(300)
                    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, 40, 0)
                    CALL(evt_set_dir_to_target, STRING("me"), STRING("mario"))
                END_IF
                CALL(evt_msg_print, 0, STRING("irai_20_014"), 0, STRING("me"))
                CALL(evt_sub_save_playtime, GSW(1453))
                SET_VALUE(GSW(1420), 0)
                SET_VALUE(GSWF(5394), 1)
                CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
                BEGIN_INLINE_EVENT
                    CALL(evt_msg_print, 0, STRING("irai_20_015"), 0, STRING("子供Ａ"))
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    WAIT_MS(500)
                    CALL(evt_msg_print, 0, STRING("irai_20_016"), 0, STRING("子供Ｂ"))
                END_INLINE_EVENT
                BEGIN_INLINE_EVENT
                    WAIT_MS(1000)
                    CALL(evt_msg_print, 0, STRING("irai_20_017"), 0, STRING("子供Ｃ"))
                END_INLINE_EVENT
            END_IF
        END_IF
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(200)
        IF_INT_EQUAL(GSWF(1927), 0)
            CALL(evt_msg_print, 0, STRING("stg4_usu_62"), 0, STRING("me"))
            CALL_CHILD(childABC_talk_01)
            WAIT_MS(1400)
            CALL(evt_msg_print, 0, STRING("stg4_usu_66"), 0, STRING("me"))
            CALL(evt_msg_select, 0, STRING("stg4_usu_66_yn"))
            CALL(evt_msg_continue)
            WAIT_FRAMES(1)
            CALL(evt_pouch_get_haveitemcnt, LW(1))
            IF_INT_EQUAL(LW(1), 0)
                SET_VALUE(LW(0), 1)
            END_IF
            IF_INT_EQUAL(LW(0), 0)
                CALL(get_item_list, LW(0))
                LOOP(0)
                    CALL(evt_win_item_select, 0, LW(0), LW(1), 0)
                    IF_INT_NOT_EQUAL(LW(1), -1)
                        CALL(check_tabemono, LW(1), LW(2))
                        IF_INT_EQUAL(LW(2), 1)
                            CALL(evt_pouch_remove_item, LW(1), LW(2))
                            BREAK_LOOP
                        ELSE
                            CALL(evt_msg_print, 0, STRING("stg4_usu_67"), 0, 0)
                        END_IF
                    ELSE
                        BREAK_LOOP
                    END_IF
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(delete_item_list, LW(0))
                IF_INT_NOT_EQUAL(LW(1), -1)
                    CALL(evt_msg_print, 0, STRING("stg4_usu_68"), 0, STRING("me"))
                    CALL(evt_item_entry, STRING("orei"), 130, 0, 0, 0, 17, -1, 0)
                    CALL(evt_item_get_item, STRING("orei"))
                    CALL(evt_item_get_item_end_wait, STRING("orei"))
                    CALL(evt_msg_print, 0, STRING("stg4_usu_69"), 0, STRING("me"))
                ELSE
                    CALL(evt_msg_print, 0, STRING("stg4_usu_70"), 0, STRING("me"))
                END_IF
            ELSE
                CALL(evt_msg_print, 0, STRING("stg4_usu_70"), 0, STRING("me"))
            END_IF
            WAIT_MS(500)
            CALL(evt_snd_sfxon, STRING("SFX_STG4_TEMPLE_BELL1"), 0)
            WAIT_MS(2000)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_CHANGE_PIG1"), LW(0), LW(1), LW(2), 0)
            BEGIN_INLINE_EVENT
                ADD(LW(2), 20)
                CALL(evt_eff, 0, STRING("kemuri_test"), 7, LW(0), LW(1), LW(2), FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
            END_INLINE_EVENT
            WAIT_MS(300)
            CALL(evt_npc_set_position, STRING("ブタおばさん"), LW(0), LW(1), LW(2))
            CALL(evt_npc_set_position, STRING("me"), 0, -2000, 0)
            CALL(evt_npc_get_reglid, STRING("ブタおばさん"), LW(0))
            DELETE_EVENT(LW(0))
            CALL(evt_npc_restart_regular_event, STRING("ブタおばさん"))
            BEGIN_INLINE_EVENT
                CALL(evt_msg_print, 0, STRING("stg4_usu_71"), 0, STRING("子供Ａ"))
            END_INLINE_EVENT
            BEGIN_INLINE_EVENT
                WAIT_MS(400)
                CALL(evt_msg_print, 0, STRING("stg4_usu_72"), 0, STRING("子供Ｂ"))
            END_INLINE_EVENT
            BEGIN_INLINE_EVENT
                WAIT_MS(600)
                CALL(evt_msg_print, 0, STRING("stg4_usu_73"), 0, STRING("子供Ｃ"))
            END_INLINE_EVENT
            SET_VALUE(GSWF(1927), 1)
        ELSE
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_VOICE_PIG1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_msg_print, 0, STRING("stg4_usu_73_01"), 0, STRING("me"))
        END_IF
        CASE_LESS(219)
        CALL(evt_msg_print, 0, STRING("stg4_usu_77"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("stg4_usu_78"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg4_usu_81_1"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_81_4_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(childA_init)
    CALL(evt_npc_flag_onoff, 1, STRING("子供Ａ"), 1536)
    RETURN
EVT_END()

EVT_BEGIN(childA_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        IF_INT_EQUAL(GSWF(1927), 1)
            CALL(evt_msg_print, 0, STRING("stg4_usu_74"), 0, STRING("me"))
        ELSE
            CALL_CHILD(childABC_talk_01)
        END_IF
        CASE_LESS(222)
        CALL_CHILD(childABC_talk_02)
        CASE_LESS_OR_EQUAL(402)
        CALL_CHILD(childABC_talk_03)
        CASE_DEFAULT
        CALL_CHILD(childABC_talk_04)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(childB_init)
    CALL(evt_npc_flag_onoff, 1, STRING("子供Ｂ"), 1536)
    RETURN
EVT_END()

EVT_BEGIN(childB_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        IF_INT_EQUAL(GSWF(1927), 1)
            CALL(evt_msg_print, 0, STRING("stg4_usu_75"), 0, STRING("me"))
        ELSE
            CALL_CHILD(childABC_talk_01)
        END_IF
        CASE_LESS(222)
        CALL_CHILD(childABC_talk_02)
        CASE_LESS_OR_EQUAL(402)
        CALL_CHILD(childABC_talk_03)
        CASE_DEFAULT
        CALL_CHILD(childABC_talk_04)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(childC_init)
    CALL(evt_npc_flag_onoff, 1, STRING("子供Ｃ"), 1536)
    RETURN
EVT_END()

EVT_BEGIN(childC_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        IF_INT_EQUAL(GSWF(1927), 1)
            CALL(evt_msg_print, 0, STRING("stg4_usu_76"), 0, STRING("me"))
        ELSE
            CALL_CHILD(childABC_talk_01)
        END_IF
        CASE_LESS(222)
        CALL_CHILD(childABC_talk_02)
        CASE_LESS_OR_EQUAL(402)
        CALL_CHILD(childABC_talk_03)
        CASE_DEFAULT
        CALL_CHILD(childABC_talk_04)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(gatekeeper_init)
    CALL(evt_npc_flag_onoff, 1, STRING("門番"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(181)
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_npc_set_position, STRING("門番"), 400, 0, -55)
        CASE_LESS(200)
        CALL(evt_npc_set_position, STRING("門番"), 0, -2000, 0)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("門番"), 400, 0, -55)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(gatekeeper_pig_init)
    CALL(evt_npc_flag_onoff, 1, STRING("ブタ門番"), 1536)
    SWITCH(GSW(0))
        CASE_LESS(181)
        CALL(evt_npc_set_position, STRING("ブタ門番"), 0, -2000, 0)
        CASE_LESS(200)
        CALL(evt_npc_set_position, STRING("ブタ門番"), 400, 0, -55)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("ブタ門番"), 0, -2000, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(gatekeeper_talk)
    SWITCH(GSW(0))
        CASE_LESS(179)
        CALL(evt_mario_mov_pos2, 350, -50, 120)
        WAIT_FRAMES(1)
        CALL(evt_mario_set_dir, 90, 300, 0)
        CALL(evt_party_move_behind_mario, 0)
        CALL(evt_cam3d_evt_set, 372, 124, 321, 372, 57, -61, 500, 11)
        WAIT_MS(500)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_msg_print, 0, STRING("stg4_usu_44"), 0, STRING("me"))
        CALL(evt_cam3d_evt_set, 342, 124, 321, 342, 57, -61, 500, 11)
        WAIT_MS(500)
        CALL(evt_party_stop, 0)
        WAIT_MS(200)
        CALL(evt_msg_print_party, STRING("stg4_usu_45"))
        CALL(evt_party_run, 0)
        CALL(evt_cam3d_evt_off, 1000, 11)
        SPAWN_EVENT(village_chief_change_pig_event)
        SET_VALUE(GSW(0), 179)
        RETURN
        CASE_LESS(180)
        CALL(evt_msg_print, 0, STRING("stg4_usu_45_01"), 0, STRING("me"))
        CASE_LESS(181)
        CALL(evt_msg_print, 0, STRING("stg4_usu_46"), 0, STRING("me"))
        CASE_LESS(200)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_VOICE_PIG1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_msg_print, 0, STRING("stg4_usu_48"), 0, STRING("me"))
        CASE_LESS(219)
        CALL(evt_msg_print, 0, STRING("stg4_usu_49"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("stg4_usu_49_1"), 0, STRING("me"))
        CASE_LESS_OR_EQUAL(402)
        CALL(evt_msg_print, 0, STRING("stg4_usu_49_2"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg4_usu_49_2_1"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(crow_f_init)
    CALL(evt_npc_set_position, STRING("me"), 275, 83, 355)
    RETURN
EVT_END()

EVT_BEGIN(crow_g_init)
    CALL(evt_npc_set_position, STRING("me"), 250, 112, 355)
    RETURN
EVT_END()

EVT_BEGIN(crow_h_init)
    CALL(evt_npc_set_position, STRING("me"), -147, 110, 346)
    RETURN
EVT_END()

EVT_BEGIN(crow_i_init)
    CALL(evt_npc_set_position, STRING("me"), -175, 110, 348)
    RETURN
EVT_END()

EVT_BEGIN(crow_j_init)
    CALL(evt_npc_set_position, STRING("me"), -195, 100, -157)
    RETURN
EVT_END()

static EVT_BEGIN(musume_talk)
    SWITCH(GSW(0))
        CASE_LESS(200)
        CALL(evt_npc_get_position, STRING("ブタ３人娘Ａ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_VOICE_PIG1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_msg_print, 0, STRING("stg4_usu_148_1_01"), 0, STRING("ブタ３人娘Ａ"))
        CALL(evt_npc_get_position, STRING("ブタ３人娘Ｂ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_VOICE_PIG1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_msg_print, 0, STRING("stg4_usu_148_1_02"), 0, STRING("ブタ３人娘Ｂ"))
        CALL(evt_npc_get_position, STRING("ブタ３人娘Ｃ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_VOICE_PIG1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_msg_print, 0, STRING("stg4_usu_148_1_03"), 0, STRING("ブタ３人娘Ｃ"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("stg4_usu_148_1_04"), 0, STRING("３人娘Ａ"))
        CALL(evt_msg_print, 0, STRING("stg4_usu_148_1_05"), 0, STRING("３人娘Ｂ"))
        CALL(evt_msg_print, 0, STRING("stg4_usu_148_1_06"), 0, STRING("３人娘Ｃ"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(musume_a_init)
    CALL(evt_npc_set_position, LW(0), -440, 0, 180)
    CALL(evt_npc_set_ry, LW(0), 90)
    CALL(evt_npc_flag_onoff, 1, LW(0), 1536)
    CALL(evt_npc_change_interrupt, LW(0), 6, PTR(musume_talk))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 1, LW(0), 32768)
        CALL(evt_seq_wait, 2)
        CALL(evt_npc_wait_pera, LW(0))
        CALL(evt_npc_flag_onoff, 0, LW(0), 32768)
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(musume_a_entry)
    CALL(evt_npc_entry, STRING("３人娘Ａ"), STRING("c_dance_r"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ａ"), STRING("踊り子"))
    SET_VALUE(LW(0), STRING("３人娘Ａ"))
    CALL_CHILD(musume_a_init)
    RETURN
EVT_END()

EVT_BEGIN(musume_a_pig_entry)
    CALL(evt_npc_entry, STRING("ブタ３人娘Ａ"), STRING("c_bubusan"))
    CALL(evt_npc_set_tribe, STRING("ブタ３人娘Ａ"), STRING("ブタ"))
    SET_VALUE(LW(0), STRING("ブタ３人娘Ａ"))
    CALL_CHILD(musume_a_init)
    RETURN
EVT_END()

EVT_BEGIN(musume_b_init)
    CALL(evt_npc_set_position, LW(0), -410, 0, 150)
    CALL(evt_npc_set_ry, LW(0), 90)
    CALL(evt_npc_flag_onoff, 1, LW(0), 1536)
    CALL(evt_npc_change_interrupt, LW(0), 6, PTR(musume_talk))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 1, LW(0), 32768)
        CALL(evt_seq_wait, 2)
        CALL(evt_npc_wait_pera, LW(0))
        CALL(evt_npc_flag_onoff, 0, LW(0), 32768)
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(musume_b_entry)
    CALL(evt_npc_entry, STRING("３人娘Ｂ"), STRING("c_dance_p"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ｂ"), STRING("踊り子ピンク"))
    SET_VALUE(LW(0), STRING("３人娘Ｂ"))
    CALL_CHILD(musume_b_init)
    RETURN
EVT_END()

EVT_BEGIN(musume_b_pig_entry)
    CALL(evt_npc_entry, STRING("ブタ３人娘Ｂ"), STRING("c_bubusan"))
    CALL(evt_npc_set_tribe, STRING("ブタ３人娘Ｂ"), STRING("ブタ"))
    SET_VALUE(LW(0), STRING("ブタ３人娘Ｂ"))
    CALL_CHILD(musume_b_init)
    RETURN
EVT_END()

EVT_BEGIN(musume_c_init)
    CALL(evt_npc_set_position, LW(0), -380, 0, 180)
    CALL(evt_npc_set_ry, LW(0), 90)
    CALL(evt_npc_flag_onoff, 1, LW(0), 1536)
    CALL(evt_npc_change_interrupt, LW(0), 6, PTR(musume_talk))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 1, LW(0), 32768)
        CALL(evt_seq_wait, 2)
        CALL(evt_npc_wait_pera, LW(0))
        CALL(evt_npc_flag_onoff, 0, LW(0), 32768)
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(musume_c_entry)
    CALL(evt_npc_entry, STRING("３人娘Ｃ"), STRING("c_dance_k"))
    CALL(evt_npc_set_tribe, STRING("３人娘Ｃ"), STRING("踊り子黄緑"))
    SET_VALUE(LW(0), STRING("３人娘Ｃ"))
    CALL_CHILD(musume_c_init)
    RETURN
EVT_END()

EVT_BEGIN(musume_c_pig_entry)
    CALL(evt_npc_entry, STRING("ブタ３人娘Ｃ"), STRING("c_bubusan"))
    CALL(evt_npc_set_tribe, STRING("ブタ３人娘Ｃ"), STRING("ブタ"))
    SET_VALUE(LW(0), STRING("ブタ３人娘Ｃ"))
    CALL_CHILD(musume_c_init)
    RETURN
EVT_END()

static EVT_BEGIN(urouro_move)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741824)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(50.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(urouro_move_semai)
    CALL(evt_npc_flag_onoff, 0, STRING("me"), 1073741824)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(15.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("me"))
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[24] = {
    {"村人Ｆ", 0x40080600, 0x00000000, villagerF_init, 0x00000000, villagerF_talk},
    {"村人Ｇ", 0x00080600, 0x00000000, villagerG_init, urouro_move, villagerG_talk},
    {"村人Ｈ", 0x40080600, 0x00000000, villagerH_init, 0x00000000, villagerH_talk},
    {"村人Ｉ", 0x40080600, 0x00000000, villagerI_init, 0x00000000, villagerI_talk},
    {"村人Ｊ", 0x00080600, 0x00000000, villagerJ_init, urouro_move, villagerJ_talk},
    {"店の主人", 0x40080600, 0x00000000, shop_man_init, 0x00000000, shop_man_talk},
    {"お店の奥さん", 0x40080600, 0x00000000, shop_woman_init, 0x00000000, shop_woman_talk},
    {"キザ野郎", 0x40080600, 0x00000000, kizayarou_init, 0x00000000, kizayarou_talk},
    {"おばさん", 0x40080600, 0x00000000, aunt_init, 0x00000000, aunt_talk},
    {"子供Ａ", 0x40080600, 0x00000000, childA_init, 0x00000000, childA_talk},
    {"子供Ｂ", 0x40080600, 0x00000000, childB_init, 0x00000000, childB_talk},
    {"子供Ｃ", 0x40080600, 0x00000000, childC_init, 0x00000000, childC_talk},
    {"門番", 0x40080600, 0x00000000, gatekeeper_init, 0x00000000, gatekeeper_talk},
    {"ブタキザ野郎", 0x00080600, 0x00000000, kizayarou_pig_init, urouro_move, kizayarou_talk},
    {"ブタおばさん", 0x00080600, 0x00000000, aunt_pig_init, urouro_move, aunt_talk},
    {"ブタ門番", 0x00080600, 0x00000000, gatekeeper_pig_init, 0x00000000, gatekeeper_talk},
    {"ブタ村人Ｉ", 0x00080600, 0x00000000, villagerI_pig_init, urouro_move, villagerI_talk},
    {"ブタ村人Ｊ", 0x00080600, 0x00000000, villagerJ_pig_init, urouro_move, villagerJ_talk},
    {"カラスＦ", 0x40000600, 0x00000000, crow_f_init},
    {"カラスＧ", 0x40000600, 0x00000000, crow_g_init},
    {"カラスＨ", 0x40000600, 0x00000000, crow_h_init},
    {"カラスＩ", 0x40000600, 0x00000000, crow_i_init},
    {"カラスＪ", 0x40000600, 0x00000000, crow_j_init},
    {NULL},
};

EVT_BEGIN(village_chief_change_pig_event)
    LABEL(0)
    WAIT_FRAMES(1)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS_OR_EQUAL(LW(0), -300)
        GOTO(10)
    END_IF
    GOTO(0)
    LABEL(10)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_snd_sfxon, STRING("SFX_STG4_TEMPLE_BELL1"), 0)
    WAIT_MS(500)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_mario_hello_party, 0, 1)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_party_get_pos, 0, LW(3), LW(4), LW(5))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_mario_set_dir, 90, 100, 0)
    ELSE
        CALL(evt_mario_set_dir, 270, 100, 0)
    END_IF
    ADD(LW(0), LW(3))
    DIVIDE(LW(0), 2)
    ADD(LW(2), LW(5))
    DIVIDE(LW(2), 2)
    SET_VALUE(LW(4), LW(1))
    SET_VALUE(LW(5), LW(2))
    ADD(LW(1), 35)
    SUBTRACT(LW(2), 13)
    ADD(LW(4), 100)
    ADD(LW(5), 330)
    CALL(evt_cam3d_evt_set, LW(0), LW(4), LW(5), LW(0), LW(1), LW(2), 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print_party, STRING("stg4_usu_50"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_mon_open)
    IF_INT_LESS(GSW(0), 182)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_msg_print, 0, STRING("stg4_usu_45_02"), 0, STRING("門番"))
        CALL(evt_mario_key_onoff, 1)
    ELSE
        CALL(evt_cam_letter_box_disable, 1)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_monato"), 1)
        CALL(evt_mapobj_get_position, STRING("S_mon01"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_FENCE_OPEN1"), LW(10), LW(11), LW(12), LW(9))
        CALL(evt_sub_intpl_msec_init, 11, 0, 900, 1000)
        LABEL(123)
        CALL(evt_sub_intpl_msec_get_value)
        SET_FLOAT_VALUE(LW(2), LW(0))
        MULTIPLY_FLOAT(LW(2), FLOAT(0.055f))
        SET_FLOAT_VALUE(LW(3), LW(0))
        MULTIPLY_FLOAT(LW(3), FLOAT(-0.094f))
        CALL(evt_mapobj_rotate, 1, STRING("S_mon01"), 0, LW(2), 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_mon02"), 0, LW(3), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(123)
        END_IF
        CALL(evt_hitobj_onoff, STRING("A_mon"), 1, 0)
        CALL(evt_hitobj_onoff, STRING("A_mon02"), 1, 1)
        CALL(evt_snd_sfxoff, LW(9))
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_cam_letter_box_disable, 0)
        SET_VALUE(GSW(0), 183)
        CALL(evt_exit_case_evt)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(cam_up)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_get_pos, 4, LW(0), LW(1), LW(2))
        CALL(evt_cam_get_at, 4, LW(3), LW(4), LW(5))
        ADD(LW(1), 85)
        ADD(LW(4), 85)
        CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 500, 11)
    END_INLINE_EVENT
    WAIT_MS(600)
    RETURN
EVT_END()

static EVT_BEGIN(cam_down)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    RETURN
EVT_END()

EVT_BEGIN(usu01_check_vivian)
    LABEL(0)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 6)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_party_get_status, 6, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        SET_VALUE(GW(0), 0)
        SET_VALUE(GW(1), 0)
        SET_VALUE(GW(2), 0)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(usu01_crowFG_talk_event)
    SET_VALUE(LW(15), 0)
    LABEL(0)
    IF_INT_NOT_EQUAL(GW(0), 0)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("カラスＦ"), LW(3), LW(4), LW(5))
    CALL(evt_sub_area_check, 1, LW(3), LW(5), 20, LW(0), LW(2), LW(6))
    IF_INT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_mario_balloon_tenten)
            SET_VALUE(LW(15), 1)
        END_IF
    ELSE
        IF_INT_EQUAL(LW(15), 1)
            CALL(evt_mario_balloon_off)
            SET_VALUE(LW(15), 0)
        END_IF
    END_IF
    IF_INT_EQUAL(LW(15), 1)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        CALL(evt_mario_party_use_check, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            IF_BITS_SET(LW(0), 256)
                CALL(evt_mario_balloon_off)
                SET_VALUE(LW(15), 0)
                CALL(evt_mario_key_onoff, 0)
                CALL(evt_party_cont_onoff, 0, 0)
                CALL(mainwindow_opendisable)
                CALL_CHILD(cam_up)
                CALL(evt_npc_get_position, STRING("カラスＦ"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_CROW_HOWL1"), LW(0), LW(1), LW(2), 0)
                CALL(evt_msg_print, 0, STRING("stg4_jin_153"), 0, STRING("カラスＦ"))
                CALL_CHILD(cam_down)
                CALL(evt_mario_key_onoff, 1)
                CALL(evt_party_cont_onoff, 0, 1)
                CALL(mainwindow_openenable)
            END_IF
        END_IF
    END_IF
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 6)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_party_get_status, 6, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    IF_INT_EQUAL(LW(15), 1)
        CALL(evt_mario_balloon_off)
        SET_VALUE(LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(GSW(0), 206)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_party_cont_onoff, 0, 0)
            CALL(mainwindow_opendisable)
            CALL_CHILD(cam_up)
            WAIT_MS(1000)
            CALL(evt_npc_get_position, STRING("カラスＦ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_CROW_HOWL1"), LW(0), LW(1), LW(2), 0)
            SWITCH(GSW(0))
                CASE_LESS(210)
                CALL(evt_msg_print, 0, STRING("stg4_usu_137"), 0, STRING("カラスＦ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_138"), 0, STRING("カラスＧ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_139"), 0, STRING("カラスＦ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_140"), 0, STRING("カラスＧ"))
                CALL_CHILD(cam_down)
                IF_INT_EQUAL(GSWF(1928), 0)
                    CALL(evt_party_cont_onoff, 0, 1)
                    LOOP(0)
                        CALL(evt_party_get_status, 6, LW(0))
                        IF_INT_EQUAL(LW(0), 0)
                            BREAK_LOOP
                        END_IF
                        WAIT_FRAMES(1)
                    END_LOOP
                    WAIT_MS(400)
                    CALL(evt_party_stop, 0)
                    CALL(evt_party_move_pos2, 0, 280, 350, FLOAT(80.0f))
                    WAIT_FRAMES(1)
                    CALL(evt_party_set_dir, 0, 270, 200)
                    CALL(evt_mario_set_dir, 90, 200, 0)
                    WAIT_MS(300)
                    CALL(evt_msg_print_party, STRING("stg4_usu_149"))
                    WAIT_MS(300)
                    CALL(evt_mario_set_pose, STRING("M_N_2"))
                    WAIT_MS(500)
                    CALL(evt_mario_set_pose, STRING("M_S_1"))
                    CALL(evt_msg_print_party, STRING("stg4_usu_150"))
                    WAIT_MS(300)
                    SET_VALUE(GSWF(1928), 1)
                    CALL(evt_party_cont_onoff, 0, 0)
                    CALL(evt_party_run, 0)
                END_IF
                CASE_LESS(222)
                CALL(evt_msg_print, 0, STRING("stg4_usu_141"), 0, STRING("カラスＦ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_142"), 0, STRING("カラスＧ"))
                CALL_CHILD(cam_down)
                CASE_LESS_OR_EQUAL(402)
                CALL(evt_msg_print, 0, STRING("stg4_usu_142_1"), 0, STRING("カラスＦ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_142_2"), 0, STRING("カラスＧ"))
                CALL_CHILD(cam_down)
                CASE_DEFAULT
                CALL(evt_msg_print, 0, STRING("stg4_usu_142_2_1"), 0, STRING("カラスＦ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_142_2_2"), 0, STRING("カラスＧ"))
                CALL_CHILD(cam_down)
            END_SWITCH
            CALL(evt_party_cont_onoff, 0, 1)
            CALL(evt_mario_key_onoff, 1)
            CALL(mainwindow_openenable)
            SET_VALUE(GW(0), 1)
        END_IF
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(usu01_crowHI_talk_event)
    SET_VALUE(LW(15), 0)
    LABEL(0)
    IF_INT_NOT_EQUAL(GW(1), 0)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("カラスＨ"), LW(3), LW(4), LW(5))
    CALL(evt_sub_area_check, 1, LW(3), LW(5), 80, LW(0), LW(2), LW(6))
    IF_INT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_mario_balloon_tenten)
            SET_VALUE(LW(15), 1)
        END_IF
    ELSE
        IF_INT_EQUAL(LW(15), 1)
            CALL(evt_mario_balloon_off)
            SET_VALUE(LW(15), 0)
        END_IF
    END_IF
    IF_INT_EQUAL(LW(15), 1)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        CALL(evt_mario_party_use_check, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            IF_BITS_SET(LW(0), 256)
                CALL(evt_mario_balloon_off)
                SET_VALUE(LW(15), 0)
                CALL(evt_mario_key_onoff, 0)
                CALL_CHILD(cam_up)
                CALL(evt_npc_get_position, STRING("カラスＨ"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_CROW_HOWL1"), LW(0), LW(1), LW(2), 0)
                CALL(evt_msg_print, 0, STRING("stg4_jin_153"), 0, STRING("カラスＨ"))
                CALL_CHILD(cam_down)
                CALL(evt_mario_key_onoff, 1)
            END_IF
        END_IF
    END_IF
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 6)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_party_get_status, 6, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    IF_INT_EQUAL(LW(15), 1)
        CALL(evt_mario_balloon_off)
        SET_VALUE(LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(GSW(0), 206)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_party_cont_onoff, 0, 0)
            CALL(mainwindow_opendisable)
            CALL_CHILD(cam_up)
            WAIT_MS(1000)
            CALL(evt_npc_get_position, STRING("カラスＨ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_CROW_HOWL1"), LW(0), LW(1), LW(2), 0)
            SWITCH(GSW(0))
                CASE_LESS(210)
                CALL(evt_msg_print, 0, STRING("stg4_usu_143"), 0, STRING("カラスＨ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_144"), 0, STRING("カラスＩ"))
                CASE_LESS(222)
                CALL(evt_msg_print, 0, STRING("stg4_usu_145"), 0, STRING("カラスＨ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_146"), 0, STRING("カラスＩ"))
                CASE_LESS_OR_EQUAL(402)
                CALL(evt_msg_print, 0, STRING("stg4_usu_146_1"), 0, STRING("カラスＨ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_146_2"), 0, STRING("カラスＩ"))
                CASE_DEFAULT
                CALL(evt_msg_print, 0, STRING("stg4_usu_146_2_1"), 0, STRING("カラスＨ"))
                CALL(evt_msg_print, 0, STRING("stg4_usu_146_2_2"), 0, STRING("カラスＩ"))
            END_SWITCH
            CALL_CHILD(cam_down)
            CALL(evt_party_cont_onoff, 0, 1)
            CALL(evt_mario_key_onoff, 1)
            CALL(mainwindow_openenable)
            SET_VALUE(GW(1), 1)
        END_IF
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(usu01_crowJ_talk_event)
    SET_VALUE(LW(15), 0)
    LABEL(0)
    IF_INT_NOT_EQUAL(GW(2), 0)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("カラスＪ"), LW(3), LW(4), LW(5))
    CALL(evt_sub_area_check, 1, LW(3), LW(5), 20, LW(0), LW(2), LW(6))
    IF_INT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_mario_balloon_tenten)
            SET_VALUE(LW(15), 1)
        END_IF
    ELSE
        IF_INT_EQUAL(LW(15), 1)
            CALL(evt_mario_balloon_off)
            SET_VALUE(LW(15), 0)
        END_IF
    END_IF
    IF_INT_EQUAL(LW(15), 1)
        CALL(evt_key_get_buttontrg, 0, LW(0))
        CALL(evt_mario_party_use_check, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            IF_BITS_SET(LW(0), 256)
                CALL(evt_mario_balloon_off)
                SET_VALUE(LW(15), 0)
                CALL(evt_mario_key_onoff, 0)
                CALL_CHILD(cam_up)
                CALL(evt_npc_get_position, STRING("カラスＪ"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_CROW_HOWL1"), LW(0), LW(1), LW(2), 0)
                CALL(evt_msg_print, 0, STRING("stg4_jin_153"), 0, STRING("カラスＪ"))
                CALL_CHILD(cam_down)
                CALL(evt_mario_key_onoff, 1)
            END_IF
        END_IF
    END_IF
    CALL(evt_mario_get_party, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 6)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_party_get_status, 6, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    IF_INT_EQUAL(LW(15), 1)
        CALL(evt_mario_balloon_off)
        SET_VALUE(LW(15), 0)
        IF_INT_GREATER_OR_EQUAL(GSW(0), 206)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_party_cont_onoff, 0, 0)
            CALL(mainwindow_opendisable)
            CALL_CHILD(cam_up)
            WAIT_MS(1000)
            CALL(evt_npc_get_position, STRING("カラスＪ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_CROW_HOWL1"), LW(0), LW(1), LW(2), 0)
            SWITCH(GSW(0))
                CASE_LESS(210)
                CALL(evt_msg_print, 0, STRING("stg4_usu_147"), 0, STRING("カラスＪ"))
                CASE_LESS(222)
                CALL(evt_msg_print, 0, STRING("stg4_usu_148"), 0, STRING("カラスＪ"))
                CASE_LESS_OR_EQUAL(402)
                CALL(evt_msg_print, 0, STRING("stg4_usu_148_1"), 0, STRING("カラスＪ"))
                CASE_DEFAULT
                CALL(evt_msg_print, 0, STRING("stg4_usu_148_1_1"), 0, STRING("カラスＪ"))
            END_SWITCH
            CALL_CHILD(cam_down)
            CALL(evt_mario_key_onoff, 1)
            CALL(evt_party_cont_onoff, 0, 1)
            CALL(mainwindow_openenable)
            SET_VALUE(GW(2), 1)
        END_IF
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

char* minka_npc[6] = {
    "おばさん", "ブタおばさん", "子供Ａ", "子供Ｂ", "子供Ｃ", NULL,
};

char* minka_item[2] = {
    "iri_20", NULL,
};

char* shop_npc[6] = {
    "お店の奥さん", "キザ野郎", "ブタキザ野郎", "店の主人", "ペラ魔人", NULL,
};

char* shop_mobj[4] = {
    "usu_00_lock", "pera_box", "mobj_box00", NULL,
};

char* shop_item[5] = {
    "item_02", "item_03", "item_04", "item_05", NULL,
};

static EVT_BEGIN(evt_door_1_1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.555f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie1_door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_1_2)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie1_door"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_1)
    SET_FLOAT_VALUE(LW(1), LW(0))
    MULTIPLY_FLOAT(LW(1), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie1_kai1"), LW(1), 0, 0)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie1_kai2"), LW(2), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(door_1_open)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

static EVT_BEGIN(door_1_close)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

static DoorEntry door_1[1] = {{
    0, 3, 2, "S_usu01_ie1_door", "A_door46", "A_door45", "S_usu01_ie1", NULL,
    "S_usu01_ie1_naka", NULL, evt_door_1_1, (s32)evt_door_1_2, evt_wall_1, (const char**)minka_npc, NULL, (const char**)minka_item,
    door_1_open, door_1_close,
}};

EVT_BEGIN(evt_door_2_1_1)
    SET_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(0.555f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie2_door1"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_2_1_2)
    RETURN
EVT_END()

EVT_BEGIN(evt_wall_2_1)
    SET_FLOAT_VALUE(LW(1), LW(0))
    MULTIPLY_FLOAT(LW(1), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie2_kai1"), 0, 0, LW(1))
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie2_kai2"), 0, 0, LW(2))
    RETURN
EVT_END()

USER_FUNC(chk_fuki) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    NpcWork* work = npcGetWorkPtr();
    NpcEntry* npc = work->field_0x10;
    if (npc != NULL && strcmp(npc->description, (const char*)name) == 0) {
        evtSetValue(event, args[1], 1);
        return EVT_RETURN_DONE;
    }
    evtSetValue(event, args[1], 0);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(loop_msg)
    IF_INT_LESS(GSW(0), 200)
        RETURN
    END_IF
    IF_INT_GREATER(GSW(0), 215)
        RETURN
    END_IF
    CALL(evt_mario_wait_movable)
    WAIT_MS(LW(3))
    SET_INT_INDEXED(LW(2), 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(chk_fuki, LW(0), LW(4))
            IF_INT_EQUAL(LW(4), 1)
                GET_INT_INDEXED(LW(2), LW(5))
                IF_INT_EQUAL(LW(5), 1)
                    CALL(evt_msg_close, LW(1))
                END_IF
                SET_INT_INDEXED(LW(2), 0)
            ELSE
                SET_INT_INDEXED(LW(2), 1)
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    LOOP(0)
        LOOP(0)
            GET_INT_INDEXED(LW(2), LW(5))
            IF_INT_EQUAL(LW(5), 1)
                WAIT_FRAMES(1)
                GET_INT_INDEXED(LW(2), LW(5))
                IF_INT_EQUAL(LW(5), 1)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_msg_print, 0, LW(1), 0, LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mise_kiza_msg)
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(0), STRING("キザ野郎"))
        SET_VALUE(LW(1), STRING("stg4_usu_61_3"))
        SET_RAW_VALUE(LW(2), LSWF(2))
        SET_VALUE(LW(3), 700)
        CALL_CHILD(loop_msg)
    END_SIBLING_EVENT
    SET_VALUE(LW(0), STRING("店の主人"))
    SET_VALUE(LW(1), STRING("stg4_usu_61_4"))
    SET_RAW_VALUE(LW(2), LSWF(3))
    SET_VALUE(LW(3), 300)
    CALL_CHILD(loop_msg)
    RETURN
EVT_END()

EVT_BEGIN(mawari_house_delay)
    IF_INT_EQUAL(GSW(0), 180)
        IF_INT_EQUAL(GSWF(1925), 1)
            SPAWN_EVENT(evt_monban_buta2)
        END_IF
    END_IF
    DELETE_EVENT(GW(3))
    CALL(evt_msg_close, STRING("stg4_usu_61_3"))
    CALL(evt_msg_close, STRING("stg4_usu_61_4"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_usu01_ie3"), 1)
    WAIT_MS(1000)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_usu01_ie3"), 1)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(door_2_1_open)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    DELETE_EVENT(GW(4))
    CALL(evt_msg_close, STRING("stg4_usu_61_1"))
    CALL(evt_msg_close, STRING("stg4_usu_61_2"))
    SPAWN_EVENT_GET_ID(PTR(mise_kiza_msg), GW(3))
    RETURN
EVT_END()

DoorEntry door_2_1[1] = {{
    0, 3, 2, "S_usu01_ie2_door1", "A_usu01_ie2_door2_u", "A_usu01_ie2_door2", "S_usu01_ie2", NULL,
    "S_usu01_ie2_naka", NULL, evt_door_2_1_1, 0, evt_wall_2_1, (const char**)shop_npc, (const char**)shop_mobj, (const char**)shop_item,
    door_2_1_open, mawari_house_delay,
}};

USER_FUNC(shop_item_disp_off) {
    s32 i;
    for (i = 0; i < 6; i++) {
        mapObjFlagOn(obj_list[i * 2], 1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(shop_item_disp_on) {
    s32 i;
    for (i = 0; i < 6; i++) {
        mapObjFlagOff(obj_list[i * 2], 1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(ie2_npc_disp_off) {
    const char* off_list[] = {"お店の奥さん", "キザ野郎", "ブタキザ野郎", "店の主人", NULL};
    const char* on_list[] = {"ペラ魔人", NULL};
    s32 i;
    for (i = 0; off_list[i] != NULL; i++) {
        animPoseSetMaterialFlagOff(npcNameToPtr(off_list[i])->poseId, 0x1800);
    }
    for (i = 0; on_list[i] != NULL; i++) {
        animPoseSetMaterialFlagOn(npcNameToPtr(on_list[i])->poseId, 0x1800);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(ie2_npc_disp_on) {
    const char* on_list[] = {"お店の奥さん", "キザ野郎", "ブタキザ野郎", "店の主人", NULL};
    const char* off_list[] = {"ペラ魔人", NULL};
    s32 i;
    for (i = 0; on_list[i] != NULL; i++) {
        animPoseSetMaterialFlagOn(npcNameToPtr(on_list[i])->poseId, 0x1800);
    }
    for (i = 0; off_list[i] != NULL; i++) {
        animPoseSetMaterialFlagOff(npcNameToPtr(off_list[i])->poseId, 0x1800);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_ie2_door2_1)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_usu01_ie2_naka2"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_usu01_ie2_naka2"), 4096)
    CALL(evt_item_delete_check, STRING("item_05"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item_05"), LW(0), LW(1), LW(2))
        CALL(evt_item_set_position, STRING("item_05"), LW(0), -1000, LW(2))
    END_IF
    CALL(evt_mario_mov_pos2, -213, -471, FLOAT(135.0f))
    CALL(evt_party_set_homing_dist, 0, FLOAT(20.0f))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie2_door3"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_party_door_move, -297, -416, FLOAT(135.0f))
    CALL(evt_party_move_behind_mario, 0)
    CALL(evt_party_set_homing_dist, 0, FLOAT(50.0f))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie2_door3"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    DELETE_EVENT(GW(3))
    CALL(evt_msg_close, STRING("stg4_usu_61_3"))
    CALL(evt_msg_close, STRING("stg4_usu_61_4"))
    IF_INT_GREATER_OR_EQUAL(GSW(0), 185)
        CALL(shop_item_disp_off)
    END_IF
    CALL(ie2_npc_disp_off)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_WALL_MOVE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 300, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, STRING("S_usu01_ie2_mkabe"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
        CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie2_door3"), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_FALL1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_ie2_door2_2)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
        CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie2_door3"), LW(0), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_party_set_homing_dist, 0, FLOAT(20.0f))
    CALL(evt_party_move_behind_mario, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_WALL_MOVE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 300, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mapobj_trans, 1, STRING("S_usu01_ie2_mkabe"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 185)
        CALL(shop_item_disp_on)
    END_IF
    CALL(ie2_npc_disp_on)
    SPAWN_EVENT_GET_ID(PTR(mise_kiza_msg), GW(3))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie2_door3"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_party_door_move, -213, -471, FLOAT(135.0f))
    CALL(evt_party_move_behind_mario, 0)
    CALL(evt_party_set_homing_dist, 0, FLOAT(50.0f))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
        CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie2_door3"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_usu01_ie2_naka2"), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_usu01_ie2_naka2"), 4096)
    CALL(evt_item_delete_check, STRING("item_05"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_item_get_position, STRING("item_05"), LW(0), LW(1), LW(2))
        CALL(evt_item_set_position, STRING("item_05"), LW(0), 0, LW(2))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_3_1)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.555f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie3_door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_door_3_2)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie3_door"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_wall_3)
    SET_FLOAT_VALUE(LW(1), LW(0))
    MULTIPLY_FLOAT(LW(1), FLOAT(-0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie3_kai1"), LW(1), 0, 0)
    WAIT_FRAMES(10)
    SET_FLOAT_VALUE(LW(2), LW(0))
    MULTIPLY_FLOAT(LW(2), FLOAT(0.5f))
    CALL(evt_mapobj_rotate, 1, STRING("S_usu01_ie3_kai2"), LW(2), 0, 0)
    RETURN
EVT_END()

char* door_3_item[2] = {
    "item01", NULL,
};

static EVT_BEGIN(door_3_open)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    RETURN
EVT_END()

static EVT_BEGIN(door_3_close)
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    RETURN
EVT_END()

static DoorEntry door_3[1] = {{
    0, 3, 2, "S_usu01_ie3_door", "A_usu01_ie3_doaa_u", "A_usu01_ie3_doaa", "S_usu01_ie3", NULL,
    "S_usu01_ie3_naka", NULL, evt_door_3_1, (s32)evt_door_3_2, evt_wall_3, NULL, NULL, (const char**)door_3_item,
    door_3_open, door_3_close,
}};

static BeroEntry bero_entry_data[3] = {
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gra_00", "w_bero", 1, 1},
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "usu_00", "e_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(usu01_gusun)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 219)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2232), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LABEL(0)
    CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
    IF_INT_GREATER_OR_EQUAL(LW(3), 0)
        IF_INT_LESS_OR_EQUAL(LW(3), 100)
            IF_INT_LESS(GSW(0), 200)
                CALL(evt_msg_toge, 2, 37, 30, -244)
                CALL(evt_msg_print, 0, STRING("stg4_usu_51"), 0, 0)
            ELSE
                CALL(evt_msg_toge, 2, 37, 30, -244)
                CALL(evt_msg_print, 0, STRING("stg4_usu_61_1"), 0, 0)
                CALL(evt_msg_toge, 2, 37, 30, -244)
                CALL(evt_msg_print, 0, STRING("stg4_usu_61_2"), 0, 0)
            END_IF
            RETURN
        END_IF
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(evt_monban_buta_main)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_set_position, STRING("ブタ門番"), 400, 0, -55)
    CALL(evt_npc_set_position, STRING("門番"), 0, -2000, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG4_TEMPLE_BELL1"), 0)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), STRING("e_bero"))
    CALL_CHILD(bero_case_switch_on)
    SET_VALUE(GSW(0), 181)
    RETURN
EVT_END()

EVT_BEGIN(evt_monban_buta)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 181)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_exec_wait, 65536)
    CALL_CHILD(evt_monban_buta_main)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_monban_buta2)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 181)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(1000)
    CALL_CHILD(evt_monban_buta_main)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_monban_look)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(GSW(0), 182)
            RETURN
        END_IF
        IF_INT_GREATER_OR_EQUAL(GSW(0), 181)
            SET_VALUE(LW(0), 420)
            SET_VALUE(LW(1), 0)
            SET_VALUE(LW(2), 0)
            CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
            CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(6))
            IF_INT_LESS(LW(6), 100)
                CALL(evt_mario_key_onoff, 0)
                CALL(evt_mario_normalize)
                CALL(evt_party_yoshi_ride, 0, 0)
                CALL(evt_mario_wait_rideon)
                CALL(evt_mario_set_dir_npc, STRING("ブタ門番"))
                CALL(evt_party_set_dir_npc, 0, STRING("ブタ門番"))
                CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2"), LW(0), LW(1), LW(2), 0)
                CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
                CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
                WAIT_MS(1000)
                CALL(evt_party_stop, 0)
                WAIT_MS(200)
                CALL(evt_msg_print_party, STRING("stg4_usu_47"))
                CALL(evt_party_run, 0)
                CALL(evt_mario_key_onoff, 1)
                SET_VALUE(GSW(0), 182)
                RETURN
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static s32 lock_tbl[2] = {
    0x00000015, 0xFFFFFFFF,
};

static EVT_BEGIN(lock_item_check)
    CALL(evt_pouch_check_item, 21, LW(0))
    IF_INT_GREATER(LW(0), 0)
        CALL(evt_win_item_select, 0, PTR(lock_tbl), LW(0), 0)
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    ELSE
        IF_INT_EQUAL(GSWF(1925), 1)
            CALL(evt_set_dir_to_target, STRING("お店の奥さん"), STRING("mario"))
            CALL(evt_msg_print, 0, STRING("stg4_usu_57"), 0, STRING("お店の奥さん"))
            CALL(evt_mobj_exec_cancel, STRING("me"))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(unlock)
    CALL(evt_run_case_evt, 9, 1, STRING("A_usu01_ie2_door3"), 0, PTR(evt_ie2_door2_1), 0)
    SET_VALUE(GSW(0), 185)
    RETURN
EVT_END()

static char* obj_list[12] = {
    "mi02_S_item_01", "A_item_01", "mi02_mi02_S_item_02", "A_item_02", "mi02_S_item_03",
    "A_item_03", "mi02_S_item_04", "A_item_04", "mi02_S_item_05", "A_item_05", "mi02_S_item_06",
    "A_item_06",
};

// TODO: type
static u32 goods_list[12] = {
    0x00000081, 0x00000014, 0x000000B1, 0x0000000A,
    0x0000008C, 0x0000001E, 0x0000009B, 0x00000014,
    0x00000095, 0x0000000F, 0x00000097, 0x00000028,
};

// TODO: type
static u32 trade_list[24] = {
    0x00000081, 0x000A0000, 0x000000B1, 0x00050000,
    0x0000008C, 0x000F0000, 0x0000009B, 0x000A0000,
    0x00000095, 0x00070000, 0x00000097, 0x00140000,
    0x000000B6, 0x00030000, 0x000000A7, 0x000E0000,
    0x00000098, 0x00040000, 0x000000BF, 0x00C80000,
    0x000000AD, 0x000C0000, 0x00000000, 0x00000000,
};

static char* shopper_data[38] = {
    "お店の奥さん", NULL, NULL, "usugurashop_00", "usugurashop_01", "usugurashop_02", "usugurashop_03",
    "usugurashop_04", "usugurashop_05", "usugurashop_06", "usugurashop_07", "usugurashop_08",
    "usugurashop_09", "usugurashop_10", "usugurashop_11", "usugurashop_12", "usugurashop_13",
    "usugurashop_14", "usugurashop_15", "usugurashop_16", "usugurashop_17", "usugurashop_18",
    "usugurashop_19", "usugurashop_20", "usugurashop_21", "usugurashop_22", "usugurashop_23",
    "usugurashop_24", "usugurashop_25", "usugurashop_26", "usugurashop_27", "usugurashop_28",
    "usugurashop_29", "usugurashop_30", "usugurashop_31", "usugurashop_32", "usugurashop_33",
    "usugurashop_34",
};

EVT_BEGIN(shopper_talk1)
    CALL(evt_msg_print, 0, STRING("stg4_usu_59"), 0, STRING("me"))
    SET_VALUE(LW(0), -100)
    RETURN
EVT_END()

EVT_BEGIN(shopper_talk2)
    CALL(evt_msg_print, 0, STRING("stg4_usu_60"), 0, STRING("me"))
    SET_VALUE(LW(0), -100)
    RETURN
EVT_END()

// TODO: type
static u32 booboo_name[16] = {
    (u32)"ブタキザ野郎", 0x07000000, (u32)"ブタおばさん", 0x01000000,
    (u32)"ブタ門番", 0x03010000, (u32)"ブタ村人Ｉ", 0x05010000,
    (u32)"ブタ村人Ｊ", 0x04010000, (u32)"ブタ３人娘Ａ", 0x04010000,
    (u32)"ブタ３人娘Ｂ", 0x04010000, (u32)"ブタ３人娘Ｃ", 0x04010000,
};

EVT_BEGIN(evt_roll_get)
    RETURN
EVT_END()

static s32 key_tbl[2] = {
    0x00000023, 0xFFFFFFFF,
};

static USER_FUNC(set_fook_evt) {
    s32* args = event->args;
    s32* data = (s32*)evtGetValue(event, args[0]);
    data[1] = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(evt_majin)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_adjust_dir)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -260, -395, FLOAT(80.0f))
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -235, -370, FLOAT(80.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    IF_INT_EQUAL(GSWF(1932), 0)
        CALL(evt_mobj_get_position, STRING("pera_box"), LW(0), LW(1), LW(2))
        ADD(LW(1), 30)
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_01"), 0, 0)
        CALL(evt_mobj_get_position, STRING("pera_box"), LW(3), LW(4), LW(5))
        ADD(LW(4), 30)
        CALL(evt_snd_sfxon_3d, STRING("SFX_MOBJ_BLACKBOX_SWING1"), LW(3), LW(4), LW(5), LW(15))
        CALL(evt_mobj_set_anim, STRING("pera_box"), STRING("A_3"))
        CALL(evt_mobj_wait_animation_end, STRING("pera_box"))
        CALL(evt_snd_sfxoff, LW(15))
        WAIT_MS(300)
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_02"), PTR(majin_callback), 0)
        CALL(evt_mario_set_pose, STRING("M_I_G"))
        WAIT_MS(800)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_03"), PTR(majin_callback), 0)
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_04"), PTR(majin_callback), 0)
        CALL(evt_mario_set_pose, STRING("M_N_5B"))
        WAIT_MS(800)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_05"), PTR(majin_callback), 0)
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_06"), PTR(majin_callback), 0)
        CALL(evt_mario_set_pose, STRING("M_I_Z"))
        WAIT_MS(1600)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
        CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_07"), PTR(majin_callback), 0)
        CALL(evt_mobj_exec_cancel, STRING("pera_box"))
        SET_VALUE(GSWF(1932), 1)
    ELSE
        CALL(evt_pouch_check_item, 35, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            CALL(evt_mobj_get_position, STRING("pera_box"), LW(0), LW(1), LW(2))
            ADD(LW(1), 30)
            CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
            CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_08"), PTR(majin_callback), 0)
            CALL(evt_mobj_exec_cancel, STRING("pera_box"))
        ELSE
            CALL(evt_mobj_get_position, STRING("pera_box"), LW(0), LW(1), LW(2))
            ADD(LW(1), 30)
            CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
            CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_09"), PTR(majin_callback), 0)
            CALL(evt_win_item_select, 0, PTR(key_tbl), LW(0), 0)
            IF_INT_EQUAL(LW(0), -1)
                CALL(evt_mobj_get_position, STRING("pera_box"), LW(0), LW(1), LW(2))
                ADD(LW(1), 30)
                CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
                CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_10"), PTR(majin_callback), 0)
                CALL(evt_mobj_exec_cancel, STRING("pera_box"))
            ELSE
                CALL(evt_pouch_remove_item, 35, LW(0))
                CALL(evt_mobj_get_position, STRING("pera_box"), LW(0), LW(1), LW(2))
                ADD(LW(1), 30)
                CALL(evt_msg_toge, 2, LW(0), LW(1), LW(2))
                CALL(evt_msg_print, 0, STRING("stg4_usu_majin3_11"), PTR(majin_callback), 0)
                CALL(evt_mario_set_pose, STRING("M_I_N"))
            END_IF
        END_IF
    END_IF
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static USER_FUNC(booboo_main) {
    u32 i;
    for (i = 0; i < 8; i++) {
        if (npcNameToPtr_NoAssert((const char*)booboo_name[i * 2]) != NULL) {
            animPoseSetMaterialLightFlagOn(npcNameToPtr((const char*)booboo_name[i * 2])->poseId, 0x10);
        }
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(evt_majin2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mobj_wait_animation_end, STRING("pera_box"))
    CALL(evt_pouch_add_item, 3, LW(0))
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_PERAMAJIN1"))
    CALL(evt_snd_envoff, 1024)
    CALL(bgm_start_wait)
    WAIT_MS(1000)
    CALL(evt_paper_entry, STRING("OFF_d_kusya"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_kusya"))
    CALL(evt_img_alloc_capture, STRING("img"), 0, 0, 1, 0, 0, 608, 480)
    CALL(evt_img_set_virtual_point, STRING("img"), 0, 0, 100)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_img_clear_virtual_point, STRING("img"))
    END_INLINE_EVENT
    CALL(evt_img_onoff, STRING("img"), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PERAMAJIN_APPEAR1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PERAMAJIN_COME1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
    CALL(evt_mario_set_camid, 7)
    CALL(evt_img_wait_animend, STRING("img"))
    CALL(lect_cam_save)
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    WAIT_FRAMES(1)
    CALL_CHILD(roll_majin_lecture_init)
    CALL_CHILD(roll_majin_lecture)
    LOOP(6)
        CALL(evt_mario_dispflag_onoff, 1, 1)
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_PERAMAJIN_BLINKING1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_dispflag_onoff, 0, 1)
        WAIT_MS(100)
    END_LOOP
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_mario_set_pos, LW(10), LW(11), LW(12))
    CALL(evt_mario_dispflag_onoff, 0, 1)
    CALL(evt_mario_init_camid)
    CALL(lect_cam_load, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PERAMAJIN_GOOUT1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff_f, 513, 2000)
    CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_2"))
    CALL(evt_img_wait_animend, STRING("img"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_PERAMAJIN_GOOUT2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_img_release, STRING("img"))
    CALL(evt_paper_delete, STRING("OFF_d_kusya"))
    SET_VALUE(GSW(0), 187)
    SET_VALUE(GSWF(1931), 1)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_envoff, 513)
    CALL(evt_snd_envon, 288, 0)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(tree_onoff)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_mapobj_get_position, LW(10), LW(3), LW(4), LW(5))
        CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(9))
        IF_INT_GREATER(LW(9), LW(11))
            IF_INT_EQUAL(LF(0), 1)
                CALL(evt_mapobj_flag_onoff, 1, 1, LW(10), 1)
                SET_VALUE(LF(0), 0)
            END_IF
        ELSE
            IF_INT_EQUAL(LF(0), 0)
                CALL(evt_mapobj_flag_onoff, 1, 0, LW(10), 1)
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_ki_1)
    CALL(evt_npc_set_anim, STRING("カラスＪ"), STRING("A_1"))
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("カラスＪ"), STRING("S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(evt_ki_2)
    CALL(evt_npc_set_anim, STRING("カラスＨ"), STRING("A_1"))
    CALL(evt_npc_set_anim, STRING("カラスＩ"), STRING("A_1"))
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("カラスＨ"), STRING("S_1"))
    CALL(evt_npc_set_anim, STRING("カラスＩ"), STRING("S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(evt_ki_3)
    CALL(evt_npc_set_anim, STRING("カラスＦ"), STRING("A_1"))
    CALL(evt_npc_set_anim, STRING("カラスＧ"), STRING("A_1"))
    WAIT_MS(1500)
    CALL(evt_npc_set_anim, STRING("カラスＦ"), STRING("S_1"))
    CALL(evt_npc_set_anim, STRING("カラスＧ"), STRING("S_1"))
    RETURN
EVT_END()

static TreeAccessEntry tree_tbl[5] = {
    {2, "A_ki_1", "S_usu01_ki01", NULL, NULL, evt_ki_1},
    {2, "A_ki_2", "S_usu01_ki02", NULL, NULL, evt_ki_2},
    {2, "A_ki_3", "S_usu01_ki03", NULL, NULL, evt_ki_3},
    {2, "A_ki_4", "S_usu01_ki05", NULL, NULL, NULL},
    {7, NULL, NULL, NULL, NULL, NULL},
};

EVT_BEGIN(usu_01_tree_init)
    SET_VALUE(LW(0), PTR(tree_tbl))
    CALL_CHILD(evt_sub_tree_access_entry)
    RETURN
EVT_END()

static EVT_BEGIN(mobj_box00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 274, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static USER_FUNC(npc_process) {
    NpcWork* work = npcGetWorkPtr();
    NpcEntry* npc = work->entries;
    s32 i;
    for (i = 0; i < work->total; i++, npc++) {
        if (npc->flags & 1) {
            if (npc->position.y <= -1000.0f) {
                npc->flags |= 0x800;
            } else {
                npc->flags &= ~0x800;
            }
        }
    }
    return EVT_RETURN_BLOCK;
}

static void majin_callback(s32 type) {
    MapObjectEntry* obj = mobjNameToPtr("pera_box");

    if (type == 0) {
        animPoseSetAnim(obj->poseId, "A_2", 1);
    } else if (type == 5) {
        if (animPoseGetLoopTimes(obj->poseId) >= 1.0f) {
            animPoseSetAnim(obj->poseId, "A_2", 1);
        }
    } else if (type == 2) {
        animPoseSetAnim(obj->poseId, "S_1", 1);
    }
}

USER_FUNC(box_r) {
    MapObjectEntry* obj = mobjNameToPtr("pera_box");
    obj->rotation.y = 135.0f;
    obj->flags |= 4;
    return EVT_RETURN_DONE;
}

static USER_FUNC(bgm_start_wait) {
    return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

USER_FUNC(mobj_ry) {
    MapObjectEntry* obj = mobjNameToPtr("mobj_box00");
    obj->rotation.y = 135.0f;
    obj->flags |= 4;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(usu_01_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_USU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_USU1"))
    CALL(evt_kagemario_init_29_text_1A0C)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(evt_usu_kagemario_party_kill_29_data_104A8)
    CALL(evt_npc_setup, PTR(npcEnt))
    SWITCH(GSW(0))
        CASE_BETWEEN(178, 200)
        CALL_CHILD(musume_a_pig_entry)
        CALL_CHILD(musume_b_pig_entry)
        CALL_CHILD(musume_c_pig_entry)
        CASE_BETWEEN(200, 215)
        CALL_CHILD(musume_a_entry)
        CALL_CHILD(musume_b_entry)
        CALL_CHILD(musume_c_entry)
    END_SWITCH
    IF_INT_LESS(GSW(0), 185)
        CALL(evt_mobj_lock, STRING("usu_00_lock"), 21, FLOAT(-263.5f), FLOAT(10.1f), FLOAT(-432.05f), 135, PTR(lock_item_check), PTR(unlock), GSWF(1926))
    ELSE
        CALL(evt_run_case_evt, 9, 1, STRING("A_usu01_ie2_door3"), 0, PTR(evt_ie2_door2_1), 0)
    END_IF
    SET_VALUE(LW(0), PTR(door_1))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_2_1))
    CALL_CHILD(evt_door_entry)
    SET_VALUE(LW(0), PTR(door_3))
    CALL_CHILD(evt_door_entry)
    CALL(evt_run_case_evt, 9, 1, STRING("A_usu01_ie2_door3_u"), 0, PTR(evt_ie2_door2_2), 0)
    SPAWN_EVENT(evt_monban_look)
    IF_INT_EQUAL(GSW(0), 180)
        IF_INT_EQUAL(GSWF(1925), 1)
            SPAWN_EVENT(evt_monban_buta)
        END_IF
    END_IF
    SPAWN_EVENT_GET_ID(PTR(usu01_gusun), GW(4))
    IF_INT_GREATER_OR_EQUAL(GSW(0), 185)
        IF_INT_LESS(GSW(0), 200)
            CALL(set_fook_evt, PTR(shopper_data), PTR(shopper_talk1))
        ELSE
            IF_INT_LESS(GSW(0), 213)
                CALL(set_fook_evt, PTR(shopper_data), PTR(shopper_talk2))
            END_IF
        END_IF
        CALL(evt_shop_setup, PTR(obj_list), PTR(goods_list), PTR(shopper_data), PTR(trade_list))
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("mi02_S_items"), 16384)
    SET_VALUE(GW(0), 0)
    SET_VALUE(GW(1), 0)
    SET_VALUE(GW(2), 0)
    SPAWN_EVENT(usu01_check_vivian)
    SPAWN_EVENT(usu01_crowFG_talk_event)
    SPAWN_EVENT(usu01_crowHI_talk_event)
    SPAWN_EVENT(usu01_crowJ_talk_event)
    IF_INT_LESS(GSW(0), 183)
        CALL(evt_run_case_evt, 9, 1, STRING("A_mon"), 0, PTR(evt_mon_open), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_monato"), 1)
        CALL(evt_hitobj_onoff, STRING("A_mon02"), 1, 0)
    ELSE
        CALL(evt_mapobj_rotate, 1, STRING("S_mon01"), 0, 50, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_mon02"), 0, -85, 0)
        CALL(evt_hitobj_onoff, STRING("A_mon"), 1, 0)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(booboo_main)
    END_INLINE_EVENT
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ki1"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ki2"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ki3"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ki4"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("ki5"), 2)
    CALL_CHILD(usu_01_tree_init)
    SET_VALUE(LW(10), STRING("ki3"))
    SET_VALUE(LW(11), 450)
    SPAWN_EVENT(tree_onoff)
    CALL(evt_item_entry, STRING("item_01"), 125, 210, 0, 265, 16, GSWF(5630), 0)
    CALL(evt_item_entry, STRING("item_02"), 151, -275, 0, -310, 16, GSWF(1935), 0)
    CALL(evt_item_entry, STRING("item_03"), 134, -305, 0, -380, 16, GSWF(1936), 0)
    CALL(evt_item_entry, STRING("item_04"), 156, -335, 0, -410, 16, GSWF(1937), 0)
    CALL(evt_item_entry, STRING("item_05"), 164, -250, 0, -500, 16, GSWF(1939), 0)
    CALL(evt_mobj_itembox, STRING("mobj_box00"), -255, 0, -288, 0, 0, PTR(mobj_box00), GSWF(1934))
    CALL(mobj_ry)
    BEGIN_INLINE_EVENT
        CALL(npc_process)
    END_INLINE_EVENT
    IF_INT_GREATER_OR_EQUAL(GSW(0), 187)
        SET_VALUE(GSWF(1931), 1)
    END_IF
    CALL(evt_mobj_itembox, STRING("pera_box"), -289, 0, -341, 3, PTR(evt_majin), PTR(evt_majin2), GSWF(1931))
    CALL(evt_mobj_flag_onoff, 0, STRING("pera_box"), 64)
    CALL(box_r)
    IF_INT_EQUAL(GSWF(5394), 1)
        CALL(evt_item_entry, STRING("iri_20"), 217, -345, 20, -132, 16, GSWF(1938), 0)
    END_IF
    SPAWN_EVENT(evt_sub_mail_event)
    RETURN
EVT_END()
// clang-format on

#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_shop.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_puni_flag_onoff[];
extern s32 evt_puni_init_puni[];
extern s32 evt_puni_mem_init[];
extern s32 evt_puni_set_hamari[];
extern s32 evt_puni_set_mode[];
extern s32 evt_puni_set_yuka[];
extern s32 mri_countdown[];
extern s32 mri_guide_goryu[];
extern s32 mri_puni_init[];
extern s32 puni_disp_info[];

// clang-format off
extend_data(32,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(33,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 obj_list[12] = {
    (u32)"S_item_01", (u32)"A_item_01", (u32)"S_item_02", (u32)"A_item_02",
    (u32)"S_item_03", (u32)"A_item_03", (u32)"S_item_04", (u32)"A_item_04",
    (u32)"S_item_05", (u32)"A_item_05", (u32)"S_item_06", (u32)"A_item_06",
};

// TODO: type
static u32 goods_list[12] = {
    0x00000090, 0x0000000A, 0x00000083, 0x0000000F,
    0x000000A3, 0x00000003, 0x00000092, 0x00000008,
    0x0000009A, 0x00000005, 0x00000094, 0x00000003,
};

// TODO: type
static u32 trade_list[24] = {
    0x00000090, 0x00050000, 0x00000083, 0x00070000,
    0x000000A3, 0x00010000, 0x00000092, 0x00040000,
    0x0000009A, 0x00020000, 0x00000094, 0x00010000,
    0x00000095, 0x000E0000, 0x00000096, 0x004B0000,
    0x00000097, 0x002B0000, 0x000000A2, 0x00040000,
    0x0000008E, 0x00110000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shopper_data[38] = {
    (u32)"プニ店員", 0x00000000, 0x00000000, (u32)"stg2_shop_00",
    (u32)"stg2_shop_01", (u32)"stg2_shop_02", (u32)"stg2_shop_03", (u32)"stg2_shop_04",
    (u32)"stg2_shop_05", (u32)"stg2_shop_06", (u32)"stg2_shop_07", (u32)"stg2_shop_08",
    (u32)"stg2_shop_09", (u32)"stg2_shop_10", (u32)"stg2_shop_11", (u32)"stg2_shop_12",
    (u32)"stg2_shop_13", (u32)"stg2_shop_14", (u32)"stg2_shop_15", (u32)"stg2_shop_16",
    (u32)"stg2_shop_17", (u32)"stg2_shop_18", (u32)"stg2_shop_19", (u32)"stg2_shop_20",
    (u32)"stg2_shop_21", (u32)"stg2_shop_22", (u32)"stg2_shop_23", (u32)"stg2_shop_24",
    (u32)"stg2_shop_25", (u32)"stg2_shop_26", (u32)"stg2_shop_27", (u32)"stg2_shop_28",
    (u32)"stg2_shop_29", (u32)"stg2_shop_30", (u32)"stg2_shop_31", (u32)"stg2_shop_32",
    (u32)"stg2_shop_33", (u32)"stg2_shop_34",
};

EVT_BEGIN(chibitoge_nakama)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_TOGETTI_JUMP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_jump_position_nohit, STRING("me"), -167, 44, 41, 400, 50)
    CALL_CHILD(mri_guide_goryu)
    CALL(evt_puni_flag_onoff, 1, 0, 262144)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイドsp"), 1536)
    CALL(evt_msg_print, 0, STRING("stg2_mri_f01_00"), 0, STRING("ガイドsp"))
    CALL(evt_cam3d_evt_set, 211, 151, 548, 211, 56, 7, 0, 11)
    CALL(evt_npc_set_ry_lr, STRING("プニ店員"), 0)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg2_mri_f01_01"), 0, STRING("プニ店員"))
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_party_stop, 0)
    CALL(evt_msg_print_party, STRING("stg2_mri_f01_02"))
    CALL(evt_msg_select, 0, STRING("stg2_mri_f01_yesno"))
    CALL(evt_msg_continue)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_TOGETTI_JUMP1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_jump_position_nohit, STRING("ちびトゲ"), -138, 0, 72, 300, 30)
        WAIT_MS(200)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_TOGETTI_MOVE1"), LW(0), LW(1), LW(2), LW(9))
        ADD(LW(0), 200)
        CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 0, FLOAT(180.0f), 0)
        CALL(evt_snd_sfxoff, LW(9))
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CALL(evt_msg_print, 0, STRING("stg2_mri_f01_03"), 0, STRING("ガイドsp"))
        CALL(evt_cam3d_evt_set, 211, 151, 548, 211, 56, 7, 0, 11)
        WAIT_FRAMES(1)
        CALL(evt_msg_print, 0, STRING("stg2_mri_f01_04"), 0, STRING("プニ店員"))
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_FRAMES(1)
    ELSE
        CALL(evt_msg_print_party, STRING("stg2_mri_f01_05"))
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 512)
        CALL(evt_msg_toge, 1, 0, 0, 0)
        CALL(evt_msg_print, 0, STRING("stg2_mri_f01_06"), 0, 0)
        CALL(evt_npc_flag_onoff, 1, STRING("me"), 512)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG2_TOGETTI_JUMP1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_npc_jump_position_nohit, STRING("me"), -138, 0, 72, 300, 30)
        WAIT_MS(500)
        CALL(evt_cam3d_evt_set, 211, 151, 548, 211, 56, 7, 0, 11)
        WAIT_FRAMES(1)
        CALL(evt_msg_print, 0, STRING("stg2_mri_f01_07"), 0, STRING("プニ店員"))
        CALL(evt_cam3d_evt_off, 0, 11)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CALL(evt_puni_init_puni, 3, 1, LW(0), LW(1), LW(2), 0)
        SET_VALUE(GSWF(2878), 1)
    END_IF
    CALL(evt_puni_flag_onoff, 1, 1, 262144)
    CALL(evt_puni_set_mode, 1, 1)
    CALL(evt_npc_flag_onoff, 0, STRING("ガイドsp"), 1536)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

static EVT_BEGIN(chibitoge_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(102, 105)
        CASE_GREATER_OR_EQUAL(118)
        CASE_DEFAULT
        IF_INT_EQUAL(GSWF(2836), 0)
            IF_INT_EQUAL(GSWF(2878), 0)
                CALL(evt_npc_set_position, STRING("me"), -170, 0, 0)
                CALL(evt_npc_set_ry, STRING("me"), 90)
            END_IF
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(chibitoge_talk)
    CALL_CHILD(chibitoge_nakama)
    RETURN
EVT_END()

EVT_BEGIN(punishop_init)
    CALL(evt_npc_set_position, STRING("me"), 207, 0, -6)
    CALL(evt_npc_set_ry, STRING("me"), 270)
    RETURN
EVT_END()

EVT_BEGIN(punishop_talk)
    RETURN
EVT_END()

EVT_BEGIN(puniinfo_init)
    CALL(evt_npc_set_position, STRING("me"), 235, 0, 124)
    CALL(evt_npc_set_ry, STRING("me"), 270)
    RETURN
EVT_END()

EVT_BEGIN(puniinfo_talk)
    SWITCH(GSW(0))
        CASE_LESS(112)
        SWITCH(LSW(714))
            CASE_EQUAL(0)
            CALL(evt_msg_print, 0, STRING("stg2_mri_f00_00"), 0, STRING("me"))
            SET_VALUE(LSW(714), 1)
            CASE_EQUAL(1)
            CALL(evt_msg_print, 0, STRING("stg2_mri_f00_01"), 0, STRING("me"))
            SET_VALUE(LSW(714), 2)
            CASE_EQUAL(2)
            CALL(evt_msg_print, 0, STRING("stg2_mri_f00_02"), 0, STRING("me"))
            SET_VALUE(LSW(714), 0)
        END_SWITCH
        CASE_LESS(401)
        SWITCH(LSW(714))
            CASE_EQUAL(0)
            CALL(evt_msg_print, 0, STRING("stg2_mri_f00_02_01"), 0, STRING("me"))
            SET_VALUE(LSW(714), 1)
            CASE_EQUAL(1)
            CALL(evt_msg_print, 0, STRING("stg2_mri_f00_02_02"), 0, STRING("me"))
            SET_VALUE(LSW(714), 2)
            CASE_EQUAL(2)
            CALL(evt_msg_print, 0, STRING("stg2_mri_f00_02_03"), 0, STRING("me"))
            SET_VALUE(LSW(714), 0)
        END_SWITCH
        CASE_DEFAULT
        SWITCH(LSW(714))
            CASE_EQUAL(0)
            CALL(evt_msg_print, 0, STRING("stg2_mri_f00_02_04"), 0, STRING("me"))
            SET_VALUE(LSW(714), 1)
            CASE_EQUAL(1)
            CALL(evt_msg_print, 0, STRING("stg2_mri_f00_02_05"), 0, STRING("me"))
            SET_VALUE(LSW(714), 2)
            CASE_EQUAL(2)
            CALL(evt_msg_print, 0, STRING("stg2_mri_f00_02_06"), 0, STRING("me"))
            SET_VALUE(LSW(714), 0)
        END_SWITCH
    END_SWITCH
    RETURN
EVT_END()

// TODO: type
static u32 npcEnt[92] = {
    (u32)"ちびトゲ", 0x00000600, 0x00000000, (u32)&chibitoge_init,
    0x00000000, (u32)&chibitoge_talk, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"プニ店員",
    0x00000600, 0x00000000, (u32)&punishop_init, 0x00000000,
    (u32)&punishop_talk, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"プニ情報屋", 0x00000600,
    0x00000000, (u32)&puniinfo_init, 0x00000000, (u32)&puniinfo_talk,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 bero_entry_data[30] = {
    (u32)"e_bero", 0x00000000, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"mri_06", (u32)"w_bero",
    0x00010001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(mri_07_init_evt)
    CALL(evt_puni_mem_init)
    CALL(evt_puni_set_yuka, 7, 0, -310, -1000, -1000, 0, 0)
    CALL(evt_puni_set_hamari, -129, 0, 96, 2, -2, -1000, -1000)
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
    CALL(evt_map_playanim, STRING("mizu_01"), 1, 0)
    CALL(evt_map_playanim, STRING("mizu_02"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_01"), 1, 0)
    CALL(evt_map_playanim, STRING("awa_02"), 1, 0)
    SWITCH(GSW(0))
        CASE_BETWEEN(109, 110)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI9"))
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_MRI1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_MRI1"))
    END_SWITCH
    CALL(evt_mobj_recovery_blk, STRING("mobj_recover"), 8, 290, 82, 120, 0, 0)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_shop_setup, PTR(obj_list), PTR(goods_list), PTR(shopper_data), PTR(trade_list))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

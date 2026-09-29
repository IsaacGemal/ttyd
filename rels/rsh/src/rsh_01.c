#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_fade.h"
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
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_rsh_01_rnd_npc_entry[];
extern s32 evt_rsh_door_entry[];
extern s32 evt_rsh_door_init[];
extern s32 evt_rsh_konarikin_irai_party_talk[];
extern s32 evt_rsh_moamoa_on_window[];
extern s32 evt_rsh_window[];

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

EVT_BEGIN(s_bero_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.33301f))
    CALL(evt_mapobj_trans, 0, STRING("s_tobira"), LW(0), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(s_bero_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.33301f))
    CALL(evt_mapobj_trans, 0, STRING("s_tobira"), LW(0), 0, 0)
    RETURN
EVT_END()

static BeroEntry bero_data_a[4] = {
    {"w_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_02_a", "e_bero", 1, 1, (s32)"w_door_close", (s32)"w_door_open"},
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_00_a", "w_bero", 1, 1, (s32)"e_door_close", (s32)"e_door_open"},
    {"s_bero", 1, 0x11, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "pik_00", "n_bero", 5, 5, (s32)s_bero_close, (s32)s_bero_open},
    {NULL},
};

static BeroEntry bero_data_b[4] = {
    {"w_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_02_b", "e_bero", 1, 1, (s32)"w_door_close", (s32)"w_door_open"},
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_00_b", "w_bero", 1, 1, (s32)"e_door_close", (s32)"e_door_open"},
    {"s_bero", 1, 0x11, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hom_00", "n_bero_1", 5, 5, (s32)s_bero_close, (s32)s_bero_open},
    {NULL},
};

static BeroEntry bero_data_c[4] = {
    {"w_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_02_c", "e_bero", 1, 1, (s32)"w_door_close", (s32)"w_door_open"},
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_00_c", "w_bero", 1, 1, (s32)"e_door_close", (s32)"e_door_open"},
    {"s_bero", 1, 0x11, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_01_c", "s_bero", 5, 5, (s32)s_bero_close, (s32)s_bero_open},
    {NULL},
};

static char* door_3[7] = {
    "heya_3", "haikei_3", "tobira_3", "a_tobira_3",
    "3_door_open", "a_tobira_3_heya", "3_door_close",
};

static char* door_4[7] = {
    "heya_4", "haikei_4", "tobira_4", "a_tobira_4",
    "4_door_open", "a_tobira_4_heya", "4_door_close",
};

static EVT_BEGIN(init_toron)
    SWITCH(GSW(0))
        CASE_LESS(283)
        CALL(evt_npc_set_position, STRING("me"), -45, 0, -175)
        CASE_BETWEEN(283, 286)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_BETWEEN(287, 302)
        CALL(evt_npc_set_position, STRING("me"), -45, 0, -175)
        CASE_BETWEEN(303, 309)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_GREATER_OR_EQUAL(310)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(1), 0)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(talk_toron)
    SWITCH(GSW(0))
        CASE_LESS(287)
        IF_INT_EQUAL(GSWF(3427), 0)
            SET_VALUE(GSWF(3427), 1)
            CALL(evt_msg_print, 0, STRING("stg6_rsh_09"), 0, 0)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg6_rsh_10"), 0, 0)
        END_IF
        CASE_LESS(291)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_61"), 0, 0)
        CASE_LESS(294)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_106"), 0, 0)
        SET_VALUE(GSWF(3437), 1)
        CALL_CHILD(evt_rsh_konarikin_irai_party_talk)
        CASE_LESS(301)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_61"), 0, 0)
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("stg6_rsh_147_02"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(init_madam)
    SWITCH(GSW(0))
        CASE_LESS(328)
        CALL(evt_npc_set_position, STRING("me"), -275, 0, -225)
        CASE_BETWEEN(328, 332)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_GREATER_OR_EQUAL(333)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(1), 0)
        CALL(evt_npc_flag_onoff, 0, STRING("me"), 131072)
    END_IF
    RETURN
EVT_END()

static s32 item_tbl_madam[] = {
    75, -1,
};

static EVT_BEGIN(talk_madam_ring_return)
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_200"), 0, STRING("マダム"))
    CALL(evt_win_item_select, 0, PTR(item_tbl_madam), LW(1), 0)
    IF_INT_NOT_EQUAL(LW(1), -1)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_201"), 0, STRING("マダム"))
        CALL(evt_sub_get_coin, 30)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_202"), 0, STRING("マダム"))
        CALL(evt_pouch_remove_item, 75, LW(0))
    ELSE
        CALL(evt_msg_print, 0, STRING("stg6_rsh_201_2"), 0, STRING("マダム"))
    END_IF
    SWITCH(GSW(0))
        CASE_BETWEEN(310, 325)
        CALL(evt_snd_bgmoff, 26624)
        CASE_BETWEEN(334, 344)
        CALL(evt_snd_bgmoff, 26624)
        CASE_DEFAULT
        CALL(evt_snd_bgmon, 160, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(talk_madam)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 288)
        IF_INT_EQUAL(GSWF(3430), 0)
            CALL(evt_snd_bgmoff, 18432)
            IF_INT_EQUAL(GSWF(3429), 0)
                SET_VALUE(GSWF(3429), 1)
                CALL(evt_msg_print, 0, STRING("stg6_rsh_80"), 0, 0)
            ELSE
                CALL(evt_msg_print, 0, STRING("stg6_rsh_83"), 0, 0)
            END_IF
            CALL(evt_msg_select, 0, STRING("stg6_rsh_83_yn"))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_msg_print_add, 0, STRING("stg6_rsh_81"))
                SET_VALUE(GSWF(3430), 1)
            ELSE
                CALL(evt_msg_print_add, 0, STRING("stg6_rsh_82"))
            END_IF
            SWITCH(GSW(0))
                CASE_BETWEEN(310, 325)
                CALL(evt_snd_bgmoff, 26624)
                CASE_BETWEEN(334, 344)
                CALL(evt_snd_bgmoff, 26624)
                CASE_DEFAULT
                CALL(evt_snd_bgmon, 160, 0)
            END_SWITCH
            RETURN
        END_IF
    END_IF
    CALL(evt_pouch_check_item, 75, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(talk_madam_ring_return)
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(284)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_11"), 0, 0)
        CASE_LESS(287)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_53"), 0, 0)
        CASE_LESS(288)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_62"), 0, 0)
        CASE_LESS(301)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_84"), 0, 0)
        CASE_LESS(302)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_147_03"), 0, 0)
        CASE_LESS(310)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_160"), 0, 0)
        CASE_LESS(326)
        IF_INT_EQUAL(GSWF(3441), 0)
            SET_VALUE(GSWF(3441), 1)
            CALL(evt_msg_print, 0, STRING("stg6_rsh_204"), 0, 0)
            CALL(evt_msg_print, 0, STRING("stg6_rsh_205"), 0, 0)
        ELSE
            CALL(evt_msg_print, 0, STRING("stg6_rsh_205"), 0, 0)
        END_IF
        CALL(evt_msg_select, 0, STRING("stg6_rsh_205_01"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("stg6_rsh_206"))
        ELSE
            CALL(evt_msg_print_add, 0, STRING("stg6_rsh_207"))
        END_IF
        CALL(evt_msg_print, 0, STRING("stg6_rsh_208"), 0, 0)
        CASE_LESS(327)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_221"), 0, 0)
    END_SWITCH
    RETURN
EVT_END()

static NpcSetupInfo npc_data[7] = {
    {"トロン", 0x40020610, 0, init_toron, NULL, talk_toron},
    {"マダム", 0x40020610, 0, init_madam, NULL, talk_madam},
    {"ポワン探偵", 0x40000600},
    {"車掌", 0x40000600},
    {"コック", 0x40000600},
    {"ウェイトレス", 0x40000600},
    {NULL},
};

NpcSetupInfo npc_data_2[3] = {
    {"トロン", 0x40020610, 0, init_toron, NULL, talk_toron},
    {"マダム", 0x40020610, 0, init_madam, NULL, talk_madam},
    {NULL},
};

static EVT_BEGIN(evt_daiji_kami)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yuka_3_kami"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yuka_3"), 1)
    SET_VALUE(GSW(0), 305)
    CALL(evt_item_entry, STRING("daiji"), 80, 0, 0, 0, 17, -1, 0)
    CALL(evt_item_get_item, STRING("daiji"))
    CALL(evt_item_get_item_end_wait, STRING("daiji"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(evt_pikary_arrival)
    CALL(evt_npc_set_position, STRING("ポワン探偵"), -100, 0, 0)
    CALL(evt_npc_set_position, STRING("車掌"), 0, 0, 0)
    CALL(evt_npc_set_position, STRING("コック"), 30, 0, 10)
    CALL(evt_npc_set_position, STRING("ウェイトレス"), 50, 0, 40)
    CALL(evt_mario_set_pos, -35, 0, 30)
    CALL(evt_mario_set_dir_npc, STRING("ウェイトレス"))
    CALL(evt_party_set_pos, 0, -50, 0, 20)
    CALL(evt_party_set_dir_npc, 0, STRING("ウェイトレス"))
    CALL(evt_npc_flag_onoff, 1, STRING("ポワン探偵"), 131088)
    CALL(evt_npc_flag_onoff, 1, STRING("車掌"), 131088)
    CALL(evt_npc_flag_onoff, 1, STRING("コック"), 131088)
    CALL(evt_npc_flag_onoff, 1, STRING("ウェイトレス"), 131088)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_248"), 0, STRING("車掌"))
    WAIT_MS(300)
    CALL(evt_npc_set_autotalkpose, STRING("ウェイトレス"), STRING("S_1"), STRING("T_4"))
    CALL(evt_msg_print, 0, STRING("stg6_rsh_249"), 0, STRING("ウェイトレス"))
    CALL(evt_npc_set_autotalkpose, STRING("ウェイトレス"), STRING("S_1"), STRING("T_1"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_250"), 0, STRING("コック"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_251"), 0, STRING("ポワン探偵"))
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg6_rsh_252"), 0, STRING("車掌"))
    WAIT_MS(300)
    WAIT_MS(1000)
    SET_VALUE(GSW(0), 333)
    CALL(evt_fade_set_mapchange_type, 1, 2, 500, 1, 500)
    CALL(evt_bero_mapchange, STRING("pik_00"), STRING("resha"))
    RETURN
EVT_END()

static char* access_tbl[7] = {
    "tana_5", "stg6_rsh_262", "stg6_rsh_267", "tana_4",
    "stg6_rsh_263", "stg6_rsh_267", NULL,
};

static EVT_BEGIN(evt_access)
    CALL(evt_mario_key_onoff, 0)
    IF_INT_LESS(GSW(0), 345)
        CALL(evt_msg_print, 0, LW(1), 0, 0)
    ELSE
        CALL(evt_msg_print, 0, LW(2), 0, 0)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_access_entry)
    SET_INT_READ_ADDR(PTR(access_tbl))
    LOOP(0)
        READ_INT3(LW(0), LW(1), LW(2))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_run_case_evt, 9, 1, LW(0), 0, PTR(evt_access), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(rsh_01_init_evt)
    SWITCH(GSW(0))
        CASE_BETWEEN(310, 325)
        CASE_GREATER_OR_EQUAL(333)
        CASE_DEFAULT
        CALL(evt_map_playanim, STRING("yure"), 1, 0)
        CALL(evt_map_playanim, STRING("hai_sora"), 1, 0)
        CALL(evt_map_playanim, STRING("hai_yama"), 1, 0)
        CALL(evt_map_playanim, STRING("hai_iwayama"), 1, 0)
        CALL(evt_map_playanim, STRING("hai_saboten"), 1, 0)
    END_SWITCH
    CALL_CHILD(evt_rsh_door_init)
    SET_VALUE(LW(0), PTR(door_3))
    CALL_CHILD(evt_rsh_door_entry)
    SET_VALUE(LW(0), PTR(door_4))
    CALL_CHILD(evt_rsh_door_entry)
    CALL_CHILD(evt_access_entry)
    SWITCH(GSW(0))
        CASE_BETWEEN(310, 325)
        CASE_GREATER_OR_EQUAL(333)
        CASE_DEFAULT
        SET_VALUE(LW(0), 2)
        CALL_CHILD(bero_case_switch_off)
        SET_VALUE(LW(1), STRING("stg6_rsh_261"))
        SET_VALUE(LW(2), STRING("stg6_rsh_261"))
        CALL(evt_run_case_evt, 9, 1, STRING("s_bero"), 0, PTR(evt_access), 0)
    END_SWITCH
    SWITCH(GSW(0))
        CASE_BETWEEN(284, 286)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_POWAN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1"))
        CASE_BETWEEN(304, 309)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_POWAN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1"))
        CASE_BETWEEN(310, 325)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_HOM1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
        CASE_BETWEEN(327, 328)
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1_3"))
        CASE_BETWEEN(329, 331)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1"))
        CASE_BETWEEN(334, 344)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
        CASE_DEFAULT
        CALL(evt_sub_get_mapname, LW(0))
        IF_STR_EQUAL(LW(0), STRING("rsh_01_a"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH1"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1"))
        END_IF
        IF_STR_EQUAL(LW(0), STRING("rsh_01_b"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH3"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1"))
        END_IF
        IF_STR_EQUAL(LW(0), STRING("rsh_01_c"))
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH2"))
            CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1"))
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(rsh_01_a_init_evt)
    SWITCH(GSW(0))
        CASE_EQUAL(332)
        SET_VALUE(LW(0), PTR(bero_data_a))
        CALL(evt_bero_get_info)
        CALL_CHILD(bero_case_entry)
        SPAWN_EVENT(evt_pikary_arrival)
        CASE_DEFAULT
        SET_VALUE(LW(0), PTR(bero_data_a))
        CALL(evt_bero_get_info)
        CALL_CHILD(evt_bero_info_run)
    END_SWITCH
    CALL(evt_npc_setup, PTR(npc_data))
    CALL_CHILD(rsh_01_init_evt)
    IF_INT_EQUAL(GSW(0), 304)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yuka_3_kami"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yuka_3"), 1)
        CALL(evt_run_case_evt, 5, 1, STRING("a_kami"), 0, PTR(evt_daiji_kami), 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("yuka_3_kami"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("yuka_3"), 1)
    END_IF
    CALL_CHILD(evt_rsh_moamoa_on_window)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 2)
        RETURN
    END_IF
    IF_INT_LESS(GSW(0), 282)
        CALL(evt_mario_set_pos, 100, 0, 0)
        CALL(evt_party_set_pos, 0, 130, 0, 0)
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_mario_adjust_dir)
        CALL(evt_cam_road_reset)
        SET_VALUE(GSW(0), 282)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(rsh_01_b_init_evt)
    SET_VALUE(LW(0), PTR(bero_data_b))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npc_data_2))
    CALL_CHILD(rsh_01_init_evt)
    RETURN
EVT_END()

static char* window_tbl[3] = {
    "mado_uturi_3", "mado_uturi_4", NULL,
};

EVT_BEGIN(rsh_01_c_init_evt)
    SET_VALUE(LW(0), PTR(bero_data_c))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npc_data_2))
    CALL_CHILD(rsh_01_init_evt)
    CALL(evt_map_playanim, STRING("ranpu"), 1, 0)
    SET_VALUE(LW(0), PTR(window_tbl))
    CALL_CHILD(evt_rsh_window)
    RETURN
EVT_END()

EVT_BEGIN(talk_syasyou)
    IF_INT_EQUAL(GSWF(3581), 0)
        CALL(evt_msg_print, 0, STRING("stg6_rsh_c_14"), 0, 0)
    ELSE
        CALL(evt_msg_print, 0, STRING("stg6_rsh_c_09_1"), 0, 0)
    END_IF
    RETURN
EVT_END()

static BeroEntry bero_data_a_return[4] = {
    {"w_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_02_a", "e_bero", 1, 1, (s32)"w_door_close", (s32)"w_door_open"},
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_08_a", "e_bero", 1, 1, (s32)"e_door_close", (s32)"e_door_open"},
    {"s_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "pik_00", "n_bero", 5, 5, (s32)s_bero_close, (s32)s_bero_open},
    {NULL},
};

static BeroEntry bero_data_b_return[4] = {
    {"w_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_02_b", "e_bero", 1, 1, (s32)"w_door_close", (s32)"w_door_open"},
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_08_a", "e_bero", 1, 1, (s32)"e_door_close", (s32)"e_door_open"},
    {"s_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hom_00", "n_bero_1", 5, 5, (s32)s_bero_close, (s32)s_bero_open},
    {NULL},
};

static BeroEntry bero_data_c_return[4] = {
    {"w_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_02_c", "e_bero", 1, 1, (s32)"w_door_close", (s32)"w_door_open"},
    {"e_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_08_a", "e_bero", 1, 1, (s32)"e_door_close", (s32)"e_door_open"},
    {"s_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "rsh_01_c", "s_bero", 5, 5, (s32)s_bero_close, (s32)s_bero_open},
    {NULL},
};

BeroEntry gor_bero[2] = {
    {"s_bero", 1, 8, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gor_04", "n_bero", 5, 5, (s32)s_bero_close, (s32)s_bero_open},
    {NULL},
};

EVT_BEGIN(rsh_01_init_evt_nidome)
    IF_INT_EQUAL(GSWF(3579), 0)
        IF_INT_EQUAL(GSWF(3580), 0)
            IF_INT_EQUAL(GSWF(3581), 0)
                CALL(evt_map_playanim, STRING("yure"), 1, 0)
                IF_INT_EQUAL(GSWF(3578), 0)
                    CALL(evt_map_playanim, STRING("hai_sora"), 1, 0)
                    CALL(evt_map_playanim, STRING("hai_yama"), 1, 0)
                    CALL(evt_map_playanim, STRING("hai_iwayama"), 1, 0)
                    CALL(evt_map_playanim, STRING("hai_saboten"), 1, 0)
                ELSE
                    CALL(evt_map_playanim, STRING("rt_hai_sora"), 1, 0)
                    CALL(evt_map_playanim, STRING("rt_hai_yama"), 1, 0)
                    CALL(evt_map_playanim, STRING("rt_hai_iwayama"), 1, 0)
                    CALL(evt_map_playanim, STRING("rt_hai_saboten"), 1, 0)
                END_IF
            END_IF
        END_IF
    END_IF
    CALL_CHILD(evt_rsh_door_init)
    SET_VALUE(LW(0), PTR(door_3))
    CALL_CHILD(evt_rsh_door_entry)
    SET_VALUE(LW(0), PTR(door_4))
    CALL_CHILD(evt_rsh_door_entry)
    CALL_CHILD(evt_access_entry)
    IF_INT_EQUAL(GSWF(3579), 0)
        IF_INT_EQUAL(GSWF(3580), 0)
            IF_INT_EQUAL(GSWF(3581), 0)
                BEGIN_INLINE_EVENT
                    CALL(evt_bero_exec_wait, 65536)
                    SET_VALUE(LW(0), STRING("s_bero"))
                    CALL_CHILD(bero_case_switch_off)
                    SET_VALUE(LW(1), STRING("stg6_rsh_261"))
                    SET_VALUE(LW(2), STRING("stg6_rsh_261"))
                    CALL(evt_run_case_evt, 9, 1, STRING("s_bero"), 0, PTR(evt_access), 0)
                END_INLINE_EVENT
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(GSWF(3460), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_PIK1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_bgmoff, 26624)
    ELSE
        IF_INT_EQUAL(GSWF(3461), 1)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
            CALL(evt_snd_envoff, 512)
            CALL(evt_snd_bgmoff, 26624)
        ELSE
            CALL(evt_sub_get_mapname, LW(0))
            IF_STR_EQUAL(LW(0), STRING("rsh_01_a"))
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH1"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1"))
            END_IF
            IF_STR_EQUAL(LW(0), STRING("rsh_01_b"))
            END_IF
            IF_STR_EQUAL(LW(0), STRING("rsh_01_c"))
                CALL(evt_snd_bgmon, 512, STRING("BGM_STG6_RSH2"))
                CALL(evt_snd_envon, 272, STRING("ENV_STG6_RSH1"))
            END_IF
        END_IF
    END_IF
    CALL_CHILD(evt_rsh_01_rnd_npc_entry)
    IF_INT_EQUAL(GSWF(3578), 1)
        CALL(evt_npc_set_position, STRING("車掌"), 395, 0, -90)
        CALL(evt_npc_flag_onoff, 1, STRING("車掌"), 1073874448)
        CALL(evt_npc_set_ry, STRING("車掌"), 270)
        CALL(evt_npc_change_interrupt, STRING("車掌"), 6, PTR(talk_syasyou))
        SET_VALUE(LW(0), STRING("e_bero"))
        CALL_CHILD(bero_case_switch_off)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(rsh_01_a_init_evt_nidome)
    IF_INT_EQUAL(GSWF(3578), 0)
        SET_VALUE(LW(0), PTR(bero_data_a))
    ELSE
        SET_VALUE(LW(0), PTR(bero_data_a_return))
    END_IF
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_01_init_evt_nidome)
    CALL(evt_npc_setup, PTR(npc_data_2))
    SET_VALUE(GSWF(3453), 1)
    SET_VALUE(GSWF(3454), 0)
    SET_VALUE(GSWF(3455), 0)
    IF_INT_EQUAL(GSWF(3461), 1)
        SET_VALUE(LW(0), STRING("s_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("s_bero"), PTR(gor_bero))
        SET_VALUE(LW(0), STRING("s_bero"))
        CALL_CHILD(bero_case_switch_on)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(rsh_01_b_init_evt_nidome)
    IF_INT_EQUAL(GSWF(3578), 0)
        SET_VALUE(LW(0), PTR(bero_data_b))
    ELSE
        SET_VALUE(LW(0), PTR(bero_data_b_return))
    END_IF
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_01_init_evt_nidome)
    CALL(evt_npc_setup, PTR(npc_data_2))
    SET_VALUE(GSWF(3453), 0)
    SET_VALUE(GSWF(3454), 1)
    SET_VALUE(GSWF(3455), 0)
    RETURN
EVT_END()

EVT_BEGIN(rsh_01_c_init_evt_nidome)
    IF_INT_EQUAL(GSWF(3578), 0)
        SET_VALUE(LW(0), PTR(bero_data_c))
    ELSE
        SET_VALUE(LW(0), PTR(bero_data_c_return))
    END_IF
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(rsh_01_init_evt_nidome)
    CALL(evt_npc_setup, PTR(npc_data_2))
    CALL(evt_map_playanim, STRING("ranpu"), 1, 0)
    SET_VALUE(LW(0), PTR(window_tbl))
    CALL_CHILD(evt_rsh_window)
    SET_VALUE(GSWF(3453), 0)
    SET_VALUE(GSWF(3454), 0)
    SET_VALUE(GSWF(3455), 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

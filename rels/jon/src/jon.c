#include "jon.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
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
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "npc_event.h"
#include "party/party_motion.h"
#include "data/mapdata.h"
#include "data/npc_data.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "driver/npcdrv.h"
#include "manager/evtmgr_cmd.h"
#include "math_ext.h"
#include "memory.h"
#include "mario/mariost.h"
#include <string.h>
#include <dolphin/gx.h>

#pragma pool_data off

#pragma force_active on
extern s32 basabasa2_dead_event[];
extern s32 basabasa2_find_event[];
extern s32 basabasa2_init_event[];
extern s32 basabasa2_lost_event[];
extern s32 basabasa2_move_event[];
extern s32 basabasa2_return_event[];
extern s32 bubble_find_event[];
extern s32 bubble_init_event[];
extern s32 bubble_lost_event[];
extern s32 bubble_move_event[];
extern s32 bubble_return_event[];
extern s32 chorobon_find_event[];
extern s32 chorobon_init_event[];
extern s32 chorobon_lost_event[];
extern s32 chorobon_return_event[];
extern s32 dokugassun_find_event[];
extern s32 dokugassun_init_event[];
extern s32 dokugassun_lost_event[];
extern s32 dokugassun_move_event[];
extern s32 dokugassun_return_event[];
extern s32 enemy_common_blow_event[];
extern s32 hannya_find_event[];
extern s32 hannya_init_event[];
extern s32 hannya_lost_event[];
extern s32 hannya_move_event[];
extern s32 hannya_return_event[];
extern s32 hbom_find_event[];
extern s32 hbom_init_event[];
extern s32 hbom_lost_event[];
extern s32 hbom_move_event[];
extern s32 hbom_return_event[];
extern s32 honenoko2_find_event[];
extern s32 honenoko2_init_event[];
extern s32 honenoko2_lost_event[];
extern s32 honenoko2_move_event[];
extern s32 honenoko2_return_event[];
extern s32 karon_blow_event[];
extern s32 karon_dead_event[];
extern s32 karon_find_event[];
extern s32 karon_init_event[];
extern s32 karon_lost_event[];
extern s32 karon_move_event[];
extern s32 karon_return_event[];
extern s32 kuriboo_find_event[];
extern s32 kuriboo_init_event[];
extern s32 kuriboo_lost_event[];
extern s32 kuriboo_move_event[];
extern s32 kuriboo_return_event[];
extern s32 mahoon_dead_event[];
extern s32 mahoon_find_event[];
extern s32 mahoon_init_event[];
extern s32 mahoon_lost_event[];
extern s32 mahoon_move_event[];
extern s32 mahoon_return_event[];
extern s32 met_find_event[];
extern s32 met_init_event[];
extern s32 met_lost_event[];
extern s32 met_move_event[];
extern s32 met_return_event[];
extern s32 nokonoko_find_event[];
extern s32 nokonoko_init_event[];
extern s32 nokonoko_lost_event[];
extern s32 nokonoko_move_event[];
extern s32 nokonoko_return_event[];
extern s32 pakkun_find_event[];
extern s32 pakkun_init_event[];
extern s32 pakkun_move_event[];
extern s32 pakkun_return_event[];
extern s32 pansy_find_event[];
extern s32 pansy_init_event[];
extern s32 pansy_lost_event[];
extern s32 pansy_move_event[];
extern s32 pansy_return_event[];
extern s32 patakuri_find_event[];
extern s32 patakuri_init_event[];
extern s32 patakuri_lost_event[];
extern s32 patakuri_move_event[];
extern s32 patakuri_return_event[];
extern s32 patamet_find_event[];
extern s32 patamet_init_event[];
extern s32 patamet_lost_event[];
extern s32 patamet_move_event[];
extern s32 patamet_return_event[];
extern s32 patapata_find_event[];
extern s32 patapata_init_event[];
extern s32 patapata_lost_event[];
extern s32 patapata_move_event[];
extern s32 patapata_return_event[];
extern s32 piders_blow_event[];
extern s32 piders_find_event[];
extern s32 piders_init_event[];
extern s32 piders_move_event[];
extern s32 sambo_find_event[];
extern s32 sambo_init_event[];
extern s32 sambo_lost_event[];
extern s32 sambo_move_event[];
extern s32 sambo_return_event[];
extern s32 sinemon_find_event[];
extern s32 sinemon_init_event[];
extern s32 sinemon_lost_event[];
extern s32 sinemon_move_event[];
extern s32 sinemon_return_event[];
extern s32 sinnosuke_find_event[];
extern s32 sinnosuke_init_event[];
extern s32 sinnosuke_lost_event[];
extern s32 sinnosuke_move_event[];
extern s32 sinnosuke_return_event[];
extern s32 teresa_find_event[];
extern s32 teresa_init_event[];
extern s32 teresa_lost_event[];
extern s32 teresa_move_event[];
extern s32 teresa_return_event[];
extern s32 togedaruma_find_event[];
extern s32 togedaruma_init_event[];
extern s32 togedaruma_lost_event[];
extern s32 togedaruma_move_event[];
extern s32 togedaruma_return_event[];
extern s32 togenoko_find_event[];
extern s32 togenoko_init_event[];
extern s32 togenoko_lost_event[];
extern s32 togenoko_move_event[];
extern s32 togenoko_return_event[];
extern s32 wanwan_find_event[];
extern s32 wanwan_init_event[];
extern s32 wanwan_move_event[];
static USER_FUNC(gonbaba_wait_landon);
s32 enemy_setup[1055];
s32 idouya_setup[38];
s32 gyousyou_setup[47];
s32 dokan_open[74];
s32 dokan_open2[70];
static s32 evt_kanban[25];
s32 setup_boss[9];
s32 setup_return[190];
s32 setup_prev_return[69];
s32 setup_normal[53];
s32 iri_12_dokan_in[16];
s32 iri_12_enemy_dead_event[18];
static s32 iri_12_makkino_talk[55];
static s32 iri_12_init[65];
s32 evt_iri_30_touch_rakugaki[14];
s32 evt_iri_30_bomb_rakugaki[154];
s32 zonbaba_first_event[677];

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

EVT_BEGIN(jon_init_evt)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(evt_sub_get_mapname, LW(0))
        IF_STR_EQUAL(LW(0), STRING("jon_00"))
            SET_VALUE(GSW(1321), 0)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("jon_01"))
            SET_VALUE(GSW(1321), 50)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("jon_02"))
            SET_VALUE(GSW(1321), 80)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("jon_03"))
            SET_VALUE(GSW(1321), 9)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("jon_04"))
            SET_VALUE(GSW(1321), 49)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("jon_05"))
            SET_VALUE(GSW(1321), 79)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("jon_06"))
            SET_VALUE(GSW(1321), 99)
        END_IF
    END_IF
    SET_VALUE(LSW(0), GSW(1321))
    SET_VALUE(LW(0), LSW(0))
    ADD(LW(0), 1)
    IF_INT_GREATER(LW(0), GSW(33))
        SET_VALUE(GSW(33), LW(0))
    END_IF
    CALL(evt_run_case_evt, 9, 1, STRING("a_kanban"), 0, PTR(evt_kanban), 0)
    CALL_CHILD(idouya_setup)
    CALL_CHILD(gyousyou_setup)
    CALL_CHILD(enemy_setup)
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    SET_VALUE(LW(1), LW(0))
    MODULO(LW(1), 10)
    SET_VALUE(LW(2), LW(0))
    DIVIDE(LW(2), 10)
    SWITCH(LW(1))
        CASE_EQUAL(0)
        IF_INT_EQUAL(LW(2), 10)
            CALL_CHILD(setup_boss)
        ELSE
            CALL_CHILD(setup_return)
            CALL(evt_mobj_floatswitch_blue, 0, STRING("sw"), -75, 60, 0, PTR(dokan_open2), 0)
        END_IF
        CASE_EQUAL(9)
        CALL_CHILD(setup_prev_return)
        CALL(evt_npc_check, STRING("敵"), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 1)
            CALL(evt_hit_bind_mapobj, STRING("a_dokan_1"), STRING("dokan_1_s"))
            CALL(evt_mapobj_trans, 1, STRING("dokan_1_s"), 0, 30, 0)
            CALL(evt_hit_bind_update, STRING("a_dokan_1"))
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dokan_1_k"), 1)
        END_IF
        CALL(evt_npc_check, STRING("移動屋"), LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_hit_bind_mapobj, STRING("a_dokan_1"), STRING("dokan_1_s"))
            CALL(evt_mapobj_trans, 1, STRING("dokan_1_s"), 0, -30, 0)
            CALL(evt_hit_bind_update, STRING("a_dokan_1"))
            CALL(evt_mobj_floatswitch_blue, 0, STRING("idou_sw"), 75, 60, 0, PTR(dokan_open), 0)
        END_IF
        CASE_DEFAULT
        CALL_CHILD(setup_normal)
        CALL(evt_npc_check, STRING("敵"), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 1)
            CALL(evt_hit_bind_mapobj, STRING("a_dokan_1"), STRING("dokan_1_s"))
            CALL(evt_mapobj_trans, 1, STRING("dokan_1_s"), 0, 30, 0)
            CALL(evt_hit_bind_update, STRING("a_dokan_1"))
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dokan_1_k"), 1)
        END_IF
        CALL(evt_npc_check, STRING("移動屋"), LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_hit_bind_mapobj, STRING("a_dokan_1"), STRING("dokan_1_s"))
            CALL(evt_mapobj_trans, 1, STRING("dokan_1_s"), 0, -30, 0)
            CALL(evt_hit_bind_update, STRING("a_dokan_1"))
            CALL(evt_mobj_floatswitch_blue, 0, STRING("idou_sw"), 75, 60, 0, PTR(dokan_open), 0)
        END_IF
    END_SWITCH
    CALL_CHILD(iri_12_init)
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("jon_04"))
        IF_INT_EQUAL(GSW(1321), 49)
            IF_INT_EQUAL(GSWF(5404), 0)
                SET_VALUE(LF(0), 0)
                IF_INT_EQUAL(GSW(1420), 30)
                    IF_INT_EQUAL(GSWF(5406), 1)
                        SET_VALUE(LF(0), 1)
                    END_IF
                END_IF
                IF_INT_EQUAL(LF(0), 0)
                    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("after"), 1)
                    CALL(evt_run_case_evt, 9, 1, STRING("a_rakugaki"), 0, PTR(evt_iri_30_touch_rakugaki), LW(0))
                    IF_INT_EQUAL(GSW(1420), 30)
                        IF_INT_EQUAL(GSWF(5405), 1)
                            CALL(evt_run_case_evt, 15, 1, STRING("a_rakugaki"), 0, PTR(evt_iri_30_bomb_rakugaki), LW(1))
                            CALL(evt_set_case_wrk, LW(1), 15, LW(0))
                        END_IF
                    END_IF
                ELSE
                    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("before"), 1)
                    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_rakugaki"), 1)
                END_IF
            ELSE
                CALL(evt_mapobj_flag_onoff, 1, 1, STRING("before"), 1)
                CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_rakugaki"), 1)
            END_IF
        ELSE
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("after"), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_rakugaki"), 1)
        END_IF
    END_IF
    IF_INT_NOT_EQUAL(GSW(1321), 99)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_100DN1"))
        CALL(evt_snd_envoff, 512)
        CALL(evt_snd_set_rev_mode, 1)
    ELSE
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envon, 272, STRING("ENV_STG0_DAN1"))
        CALL(evt_snd_set_rev_mode, 2)
    END_IF
    RETURN
EVT_END()

u8 enemy_100[100] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x06,
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x10, 0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18, 0x19, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1A,
    0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22,
    0x23, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x29, 0x2A, 0x2B,
    0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x2E, 0x2F,
    0x30, 0x31, 0x2E, 0x33,
};

// TODO: type
u32 npcEnt_kuriboo[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)kuriboo_init_event,
    (u32)kuriboo_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)kuriboo_find_event,
    (u32)kuriboo_lost_event, (u32)kuriboo_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_patakuri[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)patakuri_init_event,
    (u32)patakuri_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)patakuri_find_event,
    (u32)patakuri_lost_event, (u32)patakuri_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_hannya[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)hannya_init_event,
    (u32)hannya_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)hannya_find_event,
    (u32)hannya_lost_event, (u32)hannya_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_chorobon[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)chorobon_init_event,
    (u32)chorobon_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)chorobon_find_event,
    (u32)chorobon_lost_event, (u32)chorobon_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_sinnosuke[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)sinnosuke_init_event,
    (u32)sinnosuke_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)sinnosuke_find_event,
    (u32)sinnosuke_lost_event, (u32)sinnosuke_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_sinemon[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)sinemon_init_event,
    (u32)sinemon_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)sinemon_find_event,
    (u32)sinemon_lost_event, (u32)sinemon_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_nokonoko[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)nokonoko_init_event,
    (u32)nokonoko_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)nokonoko_find_event,
    (u32)nokonoko_lost_event, (u32)nokonoko_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_patapata[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)patapata_init_event,
    (u32)patapata_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)patapata_find_event,
    (u32)patapata_lost_event, (u32)patapata_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_met[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)met_init_event,
    (u32)met_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)met_find_event,
    (u32)met_lost_event, (u32)met_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_patamet[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)patamet_init_event,
    (u32)patamet_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)patamet_find_event,
    (u32)patamet_lost_event, (u32)patamet_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_sambo[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)sambo_init_event,
    (u32)sambo_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)sambo_find_event,
    (u32)sambo_lost_event, (u32)sambo_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_honenoko[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)honenoko2_init_event,
    (u32)honenoko2_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)honenoko2_find_event,
    (u32)honenoko2_lost_event, (u32)honenoko2_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_karon[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)karon_init_event,
    (u32)karon_move_event, 0x00000000, (u32)karon_dead_event, (u32)karon_find_event,
    (u32)karon_lost_event, (u32)karon_return_event, (u32)karon_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_dokugassun[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)dokugassun_init_event,
    (u32)dokugassun_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)dokugassun_find_event,
    (u32)dokugassun_lost_event, (u32)dokugassun_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_piders[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)piders_init_event,
    (u32)piders_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)piders_find_event,
    0x00000000, 0x00000000, (u32)piders_blow_event, 0x00000002,
    0x00000000, 0x430C0000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_teresa[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)teresa_init_event,
    (u32)teresa_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)teresa_find_event,
    (u32)teresa_lost_event, (u32)teresa_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_hbom[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)hbom_init_event,
    (u32)hbom_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)hbom_find_event,
    (u32)hbom_lost_event, (u32)hbom_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_bubble[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)bubble_init_event,
    (u32)bubble_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)bubble_find_event,
    (u32)bubble_lost_event, (u32)bubble_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_wanwan[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)wanwan_init_event,
    (u32)wanwan_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)wanwan_find_event,
    0x00000000, 0x00000000, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_mahoon[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)mahoon_init_event,
    (u32)mahoon_move_event, 0x00000000, (u32)mahoon_dead_event, (u32)mahoon_find_event,
    (u32)mahoon_lost_event, (u32)mahoon_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_pakkun[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)pakkun_init_event,
    (u32)pakkun_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)pakkun_find_event,
    0x00000000, (u32)pakkun_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_togenoko[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)togenoko_init_event,
    (u32)togenoko_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)togenoko_find_event,
    (u32)togenoko_lost_event, (u32)togenoko_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_basabasa2[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)basabasa2_init_event,
    (u32)basabasa2_move_event, 0x00000000, (u32)basabasa2_dead_event, (u32)basabasa2_find_event,
    (u32)basabasa2_lost_event, (u32)basabasa2_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_togedaruma[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)togedaruma_init_event,
    (u32)togedaruma_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)togedaruma_find_event,
    (u32)togedaruma_lost_event, (u32)togedaruma_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_pansy[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, (u32)pansy_init_event,
    (u32)pansy_move_event, 0x00000000, (u32)enemy_common_dead_event, (u32)pansy_find_event,
    (u32)pansy_lost_event, (u32)pansy_return_event, (u32)enemy_common_blow_event, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 npcEnt_zonbaba[46] = {
    (u32)"敵", 0x1000000A, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)enemy_common_dead_event, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x42C80000, 0x42C80000, 0x43480000, 0x43960000,
    0x447A0000, 0x43B40000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(enemy_setup)
    CALL(evt_npc_check, STRING("移動屋"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    IF_INT_NOT_EQUAL(LW(0), 100)
        MODULO(LW(0), 10)
        IF_INT_EQUAL(LW(0), 0)
            RETURN
        END_IF
    END_IF
    CALL(enemy_info, GSW(1321), LW(0), LW(1), LW(2))
    CALL(evt_npc_entry, STRING("敵"), LW(0))
    CALL(evt_npc_set_tribe, STRING("敵"), LW(1))
    CALL(evt_sub_random, 100, LW(3))
    IF_INT_LESS(LW(3), 50)
        CALL(evt_sub_random, 50, LW(3))
        ADD(LW(3), 80)
        CALL(evt_sub_random, 200, LW(4))
        ADD(LW(4), -100)
    ELSE
        CALL(evt_sub_random, 50, LW(3))
        ADD(LW(3), -130)
        CALL(evt_sub_random, 200, LW(4))
        ADD(LW(4), -100)
    END_IF
    IF_STR_EQUAL(LW(1), STRING("クリボー"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("トゲクリ"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ハイパークリボー"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ハイパートゲクリ"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ヤミクリボー"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ヤミトゲクリ"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("パタクリ"))
        CALL(evt_npc_setup, PTR(npcEnt_patakuri))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 50, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ハイパーパタクリ"))
        CALL(evt_npc_setup, PTR(npcEnt_patakuri))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 50, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ヤミパタクリ"))
        CALL(evt_npc_setup, PTR(npcEnt_patakuri))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 50, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ヒンニャー"))
        CALL(evt_npc_setup, PTR(npcEnt_hannya))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ハンニャー"))
        CALL(evt_npc_setup, PTR(npcEnt_hannya))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ヘンニャー"))
        CALL(evt_npc_setup, PTR(npcEnt_hannya))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("チョロボン"))
        CALL(evt_npc_setup, PTR(npcEnt_chorobon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("グリーンチョロボン"))
        CALL(evt_npc_setup, PTR(npcEnt_chorobon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("フラワーチョロボン"))
        CALL(evt_npc_setup, PTR(npcEnt_chorobon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("シンエモン"))
        CALL(evt_npc_setup, PTR(npcEnt_sinemon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ハイパーシンエモン"))
        CALL(evt_npc_setup, PTR(npcEnt_sinemon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("モノクロシンエモン"))
        CALL(evt_npc_setup, PTR(npcEnt_sinemon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("シンノスケ"))
        CALL(evt_npc_setup, PTR(npcEnt_sinnosuke))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ハイパーシンノスケ"))
        CALL(evt_npc_setup, PTR(npcEnt_sinnosuke))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ノコノコ敵"))
        CALL(evt_npc_setup, PTR(npcEnt_nokonoko))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ヤミノコノコ"))
        CALL(evt_npc_setup, PTR(npcEnt_nokonoko))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ウラノコ"))
        CALL(evt_npc_setup, PTR(npcEnt_nokonoko))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("パタパタ"))
        CALL(evt_npc_setup, PTR(npcEnt_patapata))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 50, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ヤミパタパタ"))
        CALL(evt_npc_setup, PTR(npcEnt_patapata))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 50, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("メット"))
        CALL(evt_npc_setup, PTR(npcEnt_met))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("パタメット"))
        CALL(evt_npc_setup, PTR(npcEnt_patamet))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 50, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("パタトゲメット"))
        CALL(evt_npc_setup, PTR(npcEnt_patamet))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 50, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("サンボ"))
        CALL(evt_npc_setup, PTR(npcEnt_sambo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("サンボマミー"))
        CALL(evt_npc_setup, PTR(npcEnt_sambo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ホネノコ"))
        CALL(evt_npc_setup, PTR(npcEnt_honenoko))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("カロン"))
        CALL(evt_npc_setup, PTR(npcEnt_karon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ドクガッスン"))
        CALL(evt_npc_setup, PTR(npcEnt_dokugassun))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 10, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("クロクモーン"))
        CALL(evt_npc_setup, PTR(npcEnt_dokugassun))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 10, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ブリザード"))
        CALL(evt_npc_setup, PTR(npcEnt_dokugassun))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 10, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("モノクロクロクモーン"))
        CALL(evt_npc_setup, PTR(npcEnt_dokugassun))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 10, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("パイダース"))
        CALL(evt_npc_setup, PTR(npcEnt_piders))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 140, LW(4))
        CALL(evt_npc_set_home_position, STRING("敵"), LW(3), 140, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("チュランタラー"))
        CALL(evt_npc_setup, PTR(npcEnt_piders))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 140, LW(4))
        CALL(evt_npc_set_home_position, STRING("敵"), LW(3), 140, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ボロドー"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("バッジボロドー"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ジュゲム"))
        CALL(evt_npc_setup, PTR(npcEnt_dokugassun))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 10, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ボムへい"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("テレサ"))
        CALL(evt_npc_setup, PTR(npcEnt_teresa))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 30, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("パープルテレサ"))
        CALL(evt_npc_setup, PTR(npcEnt_teresa))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 30, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ヘビーボム"))
        CALL(evt_npc_setup, PTR(npcEnt_hbom))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ジャイアントボム"))
        CALL(evt_npc_setup, PTR(npcEnt_hbom))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("バブル"))
        CALL(evt_npc_setup, PTR(npcEnt_bubble))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 20, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ファントム"))
        CALL(evt_npc_setup, PTR(npcEnt_bubble))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 20, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ワンワン"))
        CALL(wanwan_set_home_pos, LW(3), 0, LW(4))
        CALL(evt_npc_setup, PTR(npcEnt_wanwan))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("バーストワンワン"))
        CALL(wanwan_set_home_pos, LW(3), 0, LW(4))
        CALL(evt_npc_setup, PTR(npcEnt_wanwan))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ジュゲム"))
        CALL(evt_npc_setup, PTR(npcEnt_bubble))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 20, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ハイパージュゲム"))
        CALL(evt_npc_setup, PTR(npcEnt_bubble))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 20, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("マホーン"))
        CALL(evt_npc_setup, PTR(npcEnt_mahoon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 20, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("スーパーマホーン"))
        CALL(evt_npc_setup, PTR(npcEnt_mahoon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 20, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("マホーンカスタム"))
        CALL(evt_npc_setup, PTR(npcEnt_mahoon))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 20, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("パックンフラワー"))
        CALL(evt_npc_setup, PTR(npcEnt_pakkun))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("アイスパックン"))
        CALL(evt_npc_setup, PTR(npcEnt_pakkun))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ダークキーパー"))
        CALL(evt_npc_setup, PTR(npcEnt_kuriboo))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("トゲノコ"))
        CALL(evt_npc_setup, PTR(npcEnt_togenoko))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("トゲノコエース"))
        CALL(evt_npc_setup, PTR(npcEnt_togenoko))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("バサバサ"))
        CALL(evt_npc_setup, PTR(npcEnt_basabasa2))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 80, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("バサバサチュルル"))
        CALL(evt_npc_setup, PTR(npcEnt_basabasa2))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 80, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("バサバサグリーン"))
        CALL(evt_npc_setup, PTR(npcEnt_basabasa2))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 80, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("トゲダルマー"))
        CALL(evt_npc_setup, PTR(npcEnt_togedaruma))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ヤミトゲダルマー"))
        CALL(evt_npc_setup, PTR(npcEnt_togedaruma))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("パンジーさん"))
        CALL(evt_npc_setup, PTR(npcEnt_pansy))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("きらめくパンジーさん"))
        CALL(evt_npc_setup, PTR(npcEnt_pansy))
        CALL(evt_npc_set_position, STRING("敵"), LW(3), 0, LW(4))
    END_IF
    IF_STR_EQUAL(LW(1), STRING("ゾンババ"))
        CALL(evt_npc_setup, PTR(npcEnt_zonbaba))
        CALL(evt_npc_set_position, STRING("敵"), 0, 0, 0)
        CALL(evt_npc_set_anim, STRING("敵"), STRING("GNB_H_3"))
        CALL(evt_npc_flag_onoff, 1, STRING("敵"), 33554496)
        CALL(evt_npc_pera_onoff, STRING("敵"), 0)
        CALL(evt_npc_set_ry, STRING("敵"), 0)
        SPAWN_EVENT(zonbaba_first_event)
    END_IF
    CALL(evt_npc_get_position, STRING("敵"), LW(3), LW(4), LW(5))
    CALL(evt_npc_set_battle_info, STRING("敵"), LW(2))
    CALL(setup_npc_ex_para, STRING("敵"))
    CALL(yattukeFlag)
    BEGIN_INLINE_EVENT
        LABEL(1)
        WAIT_FRAMES(1)
        CALL(evt_npc_status_check, STRING("敵"), 4, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            GOTO(1)
        END_IF
        SET_VALUE(LW(0), GSW(1321))
        ADD(LW(0), 1)
        IF_INT_NOT_EQUAL(LW(0), 100)
            SPAWN_EVENT(dokan_open)
        ELSE
            SET_VALUE(GSWF(5085), 1)
        END_IF
        SPAWN_EVENT(iri_12_enemy_dead_event)
    END_INLINE_EVENT
    RETURN
EVT_END()

// TODO: type
u32 treasure_tbl[10] = {
    0x000000F5, 0x000000FC, 0x0000011F, 0x00000132,
    0x00000001, 0x000000FE, 0x000000FF, 0x00000142,
    0x0000012F, 0x00000123,
};

EVT_BEGIN(evt_open_box)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    DIVIDE(LW(0), 10)
    SET_INT_READ_ADDR(PTR(treasure_tbl))
    LOOP(LW(0))
        READ_INT(LW(1))
    END_LOOP
    CALL(evt_mobj_get_position, STRING("box"), LW(10), LW(11), LW(12))
    CALL(evt_item_entry, STRING("item"), LW(1), LW(10), LW(11), LW(12), 17, -1, 0)
    CALL(evt_mobj_wait_animation_end, STRING("box"))
    CALL(evt_item_get_item, STRING("item"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(move_event)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 132112)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    CALL(evt_sub_random, 360, LW(3))
    LABEL(0)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(500)
    CALL(evt_npc_reverse_ry, STRING("me"))
    WAIT_MS(1000)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_loiter_dir, LW(3), FLOAT(120.0f), FLOAT(240.0f))
    CALL(evt_npc_add_dirdist, LW(0), LW(2), LW(3), FLOAT(100.0f))
    CALL(evt_npc_move_position, STRING("me"), LW(0), LW(2), 2000, FLOAT(40.0f), 4)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(get_map_name)
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    SWITCH(LW(0))
        CASE_LESS(10)
        SET_VALUE(LW(0), STRING("jon_00"))
        CASE_EQUAL(10)
        SET_VALUE(LW(0), STRING("jon_03"))
        CASE_LESS(20)
        SET_VALUE(LW(0), STRING("jon_00"))
        CASE_EQUAL(20)
        SET_VALUE(LW(0), STRING("jon_03"))
        CASE_LESS(30)
        SET_VALUE(LW(0), STRING("jon_00"))
        CASE_EQUAL(30)
        SET_VALUE(LW(0), STRING("jon_03"))
        CASE_LESS(40)
        SET_VALUE(LW(0), STRING("jon_00"))
        CASE_EQUAL(40)
        SET_VALUE(LW(0), STRING("jon_03"))
        CASE_LESS(50)
        SET_VALUE(LW(0), STRING("jon_00"))
        CASE_EQUAL(50)
        SET_VALUE(LW(0), STRING("jon_04"))
        CASE_LESS(60)
        SET_VALUE(LW(0), STRING("jon_01"))
        CASE_EQUAL(60)
        SET_VALUE(LW(0), STRING("jon_04"))
        CASE_LESS(70)
        SET_VALUE(LW(0), STRING("jon_01"))
        CASE_EQUAL(70)
        SET_VALUE(LW(0), STRING("jon_04"))
        CASE_LESS(80)
        SET_VALUE(LW(0), STRING("jon_01"))
        CASE_EQUAL(80)
        SET_VALUE(LW(0), STRING("jon_05"))
        CASE_LESS(90)
        SET_VALUE(LW(0), STRING("jon_02"))
        CASE_EQUAL(90)
        SET_VALUE(LW(0), STRING("jon_05"))
        CASE_LESS(100)
        SET_VALUE(LW(0), STRING("jon_02"))
        CASE_EQUAL(100)
        SET_VALUE(LW(0), STRING("jon_06"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(talk_idouya)
    CALL(evt_msg_print, 0, STRING("msg_jon_mover"), 0, 0)
    CALL(evt_msg_select, 0, STRING("msg_jon_mover_select"))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(2), 10)
        CASE_EQUAL(1)
        SET_VALUE(LW(2), 30)
        CASE_EQUAL(2)
        SET_VALUE(LW(2), 10)
        CASE_EQUAL(3)
        CALL(evt_msg_continue)
        RETURN
    END_SWITCH
    CALL(evt_pouch_get_coin, LW(3))
    IF_INT_LESS(LW(3), LW(2))
        CALL(evt_msg_print_add, 0, STRING("msg_jon_mover_nocoin"))
        RETURN
    END_IF
    MULTIPLY(LW(2), -1)
    CALL(evt_pouch_add_coin, LW(2))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        ADD(GSW(1321), 2)
        CALL_CHILD(get_map_name)
        SET_VALUE(LW(1), STRING("dokan_2"))
        CASE_EQUAL(1)
        ADD(GSW(1321), 5)
        CALL_CHILD(get_map_name)
        SET_VALUE(LW(1), STRING("dokan_2"))
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("tik_06"))
        SET_VALUE(LW(1), STRING("dokan_3"))
    END_SWITCH
    IF_STR_EQUAL(LW(0), STRING("tik_06"))
        IF_INT_EQUAL(GSW(1420), 12)
            IF_INT_EQUAL(GSWF(5406), 1)
                SET_VALUE(GSWF(5407), 1)
            END_IF
        END_IF
    END_IF
    CALL(evt_msg_print_add, 1, STRING("<scrl_auto><once_stop>"))
    CALL(evt_msg_print_add, 0, STRING("msg_jon_mover_2"))
    BEGIN_INLINE_EVENT
        CALL(evt_raster)
    END_INLINE_EVENT
    WAIT_MS(5000)
    CALL(evt_fade_set_mapchange_type, 0, 10, 300, 9, 300)
    CALL(evt_fade_set_mapchange_type, 1, 10, 300, 9, 300)
    IF_STR_EQUAL(LW(0), STRING("tik_06"))
        SET_VALUE(GSW(1321), 0)
    END_IF
    CALL(evt_mario_goodbye_party, 0)
    CALL(evt_bero_mapchange, LW(0), LW(1))
    RETURN
EVT_END()

// TODO: type
u32 npcEnt_idouya[46] = {
    (u32)"移動屋", 0x10000600, 0x00000000, 0x00000000,
    (u32)&move_event, (u32)&talk_idouya, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(idouya_setup)
    IF_INT_EQUAL(GSW(1321), 99)
        RETURN
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(1321), 95)
        RETURN
    END_IF
    CALL(evt_sub_random, 1000, LW(0))
    IF_INT_GREATER(LW(0), 50)
        RETURN
    END_IF
    CALL(evt_npc_entry, STRING("移動屋"), STRING("c_pjoho"))
    CALL(evt_npc_set_tribe, STRING("移動屋"), STRING("プニ情報屋"))
    CALL(evt_npc_setup, PTR(npcEnt_idouya))
    CALL(evt_npc_set_position, STRING("移動屋"), -100, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(talk_gyousyou)
    CALL(evt_msg_print, 0, STRING("100kai_item_01"), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("100kai_item_00"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("100kai_item_02"))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    LABEL(0)
    CALL(evt_win_coin_on, 0, LW(12))
    CALL(evt_win_other_select, 15)
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_win_coin_off, LW(12))
        CALL(evt_msg_print, 0, STRING("100kai_item_10"), 0, STRING("me"))
        RETURN
    END_IF
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_LESS(LW(0), LW(3))
        CALL(evt_win_coin_off, LW(12))
        CALL(evt_msg_print, 0, STRING("100kai_item_03"), 0, STRING("me"))
        GOTO(0)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(1), 240)
        CALL(evt_sub_get_language, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_insert, 0, STRING("100kai_item_04"), 0, STRING("me"), LW(3), LW(2), LW(4))
        ELSE
            CALL(evt_msg_print_insert, 0, STRING("100kai_item_04"), 0, STRING("me"), LW(2), LW(3), LW(4))
        END_IF
    ELSE
        CALL(evt_sub_get_language, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_insert, 0, STRING("100kai_item_04"), 0, STRING("me"), LW(3), LW(2))
        ELSE
            CALL(evt_msg_print_insert, 0, STRING("100kai_item_04"), 0, STRING("me"), LW(2), LW(3))
        END_IF
    END_IF
    CALL(evt_msg_select, 0, STRING("100kai_item_00"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_win_coin_off, LW(12))
        CALL(evt_msg_print_add, 0, STRING("100kai_item_05"))
        RETURN
    END_IF
    CALL(evt_pouch_add_item, LW(1), LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(evt_win_coin_off, LW(12))
        IF_INT_GREATER_OR_EQUAL(LW(1), 240)
            CALL(evt_msg_print_add, 0, STRING("100kai_item_09"))
        ELSE
            CALL(evt_msg_print_add, 0, STRING("100kai_item_09"))
        END_IF
        RETURN
    END_IF
    MULTIPLY(LW(3), -1)
    CALL(evt_pouch_add_coin, LW(3))
    CALL(evt_win_coin_wait, LW(12))
    WAIT_MS(200)
    CALL(evt_win_coin_off, LW(12))
    CALL(evt_pouch_get_coin, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("100kai_item_06"))
        RETURN
    END_IF
    CALL(evt_msg_print_add, 0, STRING("100kai_item_07"))
    CALL(evt_msg_select, 0, STRING("100kai_item_00"))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_msg_print_add, 0, STRING("100kai_item_08"))
        RETURN
    END_IF
    CALL(evt_msg_continue)
    GOTO(0)
    RETURN
EVT_END()

// TODO: type
u32 npcEnt_gyousyou[46] = {
    (u32)"行商人", 0x10000600, 0x00000000, 0x00000000,
    (u32)&move_event, (u32)&talk_gyousyou, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

EVT_BEGIN(gyousyou_setup)
    IF_INT_EQUAL(GSW(1321), 99)
        RETURN
    END_IF
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    MODULO(LW(0), 10)
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_sub_random, 1000, LW(0))
    IF_INT_GREATER(LW(0), 330)
        RETURN
    END_IF
    CALL(evt_npc_entry, STRING("行商人"), STRING("c_botta"))
    CALL(evt_npc_set_tribe, STRING("行商人"), STRING("ボッタクール"))
    CALL(evt_npc_setup, PTR(npcEnt_gyousyou))
    CALL(evt_npc_set_position, STRING("行商人"), 100, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(shikake_setup)
    IF_INT_EQUAL(GSW(1321), 99)
        RETURN
    END_IF
    CALL(evt_npc_check, STRING("敵"), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(evt_sub_random, 1000, LW(0))
    IF_INT_GREATER(LW(0), 200)
        RETURN
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_seq_wait, 2)
        CALL(evt_bero_exec_wait, 65536)
        CALL(evt_npc_stop_for_event)
        CALL(evt_mario_wait_landon)
        WAIT_MS(100)
        CALL(evt_fade_entry, 17, 150, 0, 0, 0)
        CALL(evt_fade_end_wait)
        CALL(evt_fade_entry, 16, 150, 0, 0, 0)
        CALL(evt_fade_end_wait)
        WAIT_MS(200)
        CALL(evt_sub_random, 1000, LW(0))
        SWITCH(LW(0))
            CASE_LESS(200)
            CALL(evt_msg_print, 0, STRING("msg_jon_trick_1"), 0, 0)
            CALL(shikakeFlag, 0)
            CASE_LESS(400)
            CALL(evt_msg_print, 0, STRING("msg_jon_trick_2"), 0, 0)
            CALL(shikakeFlag, 1)
            CASE_LESS(600)
            CALL(evt_msg_print, 0, STRING("msg_jon_trick_3"), 0, 0)
            CALL(shikakeFlag, 2)
            CASE_DEFAULT
            CALL(evt_msg_print, 0, STRING("msg_jon_trick_4"), 0, 0)
            CALL(shikakeFlag, 3)
        END_SWITCH
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_npc_start_for_event)
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(dokan_open)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(1000)
    CALL(evt_cam_letter_box_wait)
    WAIT_MS(500)
    SET_VALUE(LW(2), STRING("a_dokan_1"))
    SET_VALUE(LW(3), STRING("dokan_1_s"))
    CALL(evt_mapobj_get_position, LW(3), LW(7), LW(8), LW(9))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOKAN_APPEAR1"), LW(7), LW(8), LW(9), 0)
    CALL(evt_hit_bind_mapobj, LW(2), LW(3))
    CALL(evt_sub_intpl_msec_init, 11, 0, 300, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
    CALL(evt_mapobj_trans, 1, LW(3), 0, LW(0), 0)
    CALL(evt_hit_bind_update, LW(2))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dokan_1_k"), 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(dokan_open2)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(400)
    SET_VALUE(LW(2), STRING("a_dokan_3"))
    SET_VALUE(LW(3), STRING("dokan_1_s"))
    CALL(evt_mapobj_get_position, LW(3), LW(7), LW(8), LW(9))
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOKAN_APPEAR1"), LW(7), LW(8), LW(9), 0)
    CALL(evt_hit_bind_mapobj, LW(2), LW(3))
    CALL(evt_sub_intpl_msec_init, 11, 0, 300, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
    CALL(evt_mapobj_trans, 1, LW(3), 0, LW(0), 0)
    CALL(evt_hit_bind_update, LW(2))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("dokan_1_k"), 1)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_kanban)
    CALL(evt_npc_stop_for_event)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    CALL(evt_msg_print_insert, 0, STRING("msg_jon_kanban_1"), 0, 0, LW(0))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_npc_start_for_event)
    RETURN
EVT_END()

static EVT_BEGIN(evt_kanban2)
    CALL(evt_npc_stop_for_event)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_msg_print, 0, STRING("msg_jon_kanban_2"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_npc_start_for_event)
    RETURN
EVT_END()

EVT_BEGIN(floor_inc)
    SPAWN_EVENT(iri_12_dokan_in)
    ADD(GSW(1321), 1)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(floor_clear)
    SET_VALUE(GSW(1321), 0)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

BeroEntry bero_boss[3] = {
    {"dokan_2", 2, 0, 0x08, 100000, 0, 0, -1, NULL, 6, NULL, NULL, NULL, 1, 1},
    {"dokan_1", 2, 0, 0x02, 100000, 0, 0, -1, NULL, 10, floor_clear, "tik_06", "dokan_3", 1, 1},
    {NULL},
};

EVT_BEGIN(setup_boss)
    SET_VALUE(LW(0), PTR(bero_boss))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    RETURN
EVT_END()

BeroEntry bero_return[4] = {
    {"dokan_2", 2, 0, 0x08, 100000, 0, 0, -1, NULL, 6, NULL, NULL, NULL, 1, 1},
    {"dokan_1", 2, 0, 0x02, 100000, 0, 0, -1, NULL, 10, floor_clear, "tik_06", "dokan_3", 1, 1},
    {"dokan_3", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_00", "dokan_2", 1, 1},
    {NULL},
};

BeroEntry bero_return2[2] = {
    {"dokan_3", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_01", "dokan_2", 1, 1},
    {NULL},
};

BeroEntry bero_return3[2] = {
    {"dokan_3", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_02", "dokan_2", 1, 1},
    {NULL},
};

EVT_BEGIN(setup_return)
    SET_VALUE(LW(0), PTR(bero_return))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    SWITCH(LW(0))
        CASE_LESS(50)
        CASE_BETWEEN(50, 70)
        SET_VALUE(LW(0), STRING("dokan_3"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan_3"), PTR(bero_return2))
        SET_VALUE(LW(0), STRING("dokan_3"))
        CALL_CHILD(bero_case_switch_on)
        CASE_GREATER(70)
        SET_VALUE(LW(0), STRING("dokan_3"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan_3"), PTR(bero_return3))
        SET_VALUE(LW(0), STRING("dokan_3"))
        CALL_CHILD(bero_case_switch_on)
    END_SWITCH
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    SWITCH(LW(0))
        CASE_EQUAL(10)
        CALL(evt_mobj_itembox, STRING("box"), 0, 20, -100, 1, 0, PTR(evt_open_box), GSWF(5075))
        CASE_EQUAL(20)
        CALL(evt_mobj_itembox, STRING("box"), 0, 20, -100, 1, 0, PTR(evt_open_box), GSWF(5076))
        CASE_EQUAL(30)
        CALL(evt_mobj_itembox, STRING("box"), 0, 20, -100, 1, 0, PTR(evt_open_box), GSWF(5077))
        CASE_EQUAL(40)
        CALL(evt_mobj_itembox, STRING("box"), 0, 20, -100, 1, 0, PTR(evt_open_box), GSWF(5078))
        CASE_EQUAL(50)
        CALL(evt_mobj_itembox, STRING("box"), 0, 20, -100, 1, 0, PTR(evt_open_box), GSWF(5079))
        CASE_EQUAL(60)
        CALL(evt_mobj_itembox, STRING("box"), 0, 20, -100, 1, 0, PTR(evt_open_box), GSWF(5080))
        CASE_EQUAL(70)
        CALL(evt_mobj_itembox, STRING("box"), 0, 20, -100, 1, 0, PTR(evt_open_box), GSWF(5081))
        CASE_EQUAL(80)
        CALL(evt_mobj_itembox, STRING("box"), 0, 20, -100, 1, 0, PTR(evt_open_box), GSWF(5082))
        CASE_EQUAL(90)
        CALL(evt_mobj_itembox, STRING("box"), 0, 20, -100, 1, 0, PTR(evt_open_box), GSWF(5083))
        CASE_EQUAL(100)
        CALL(evt_mobj_itembox, STRING("box"), 270, 0, 200, 1, 0, PTR(evt_open_box), GSWF(5084))
    END_SWITCH
    CALL(evt_mobj_signboard, STRING("board"), 150, 0, -75, PTR(evt_kanban2), LSWF(0))
    RETURN
EVT_END()

BeroEntry bero_prev_return[3] = {
    {"dokan_2", 2, 0, 0x08, 100000, 0, 0, -1, NULL, 6, NULL, NULL, NULL, 1, 1},
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_03", "dokan_2", 1, 1},
    {NULL},
};

BeroEntry bero_prev_return2[2] = {
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_04", "dokan_2", 1, 1},
    {NULL},
};

BeroEntry bero_prev_return3[2] = {
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_05", "dokan_2", 1, 1},
    {NULL},
};

BeroEntry bero_prev_return4[2] = {
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_06", "dokan_2", 1, 1},
    {NULL},
};

EVT_BEGIN(setup_prev_return)
    SET_VALUE(LW(0), PTR(bero_prev_return))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    SWITCH(LW(0))
        CASE_LESS(49)
        CASE_BETWEEN(49, 69)
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan_1"), PTR(bero_prev_return2))
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_on)
        CASE_BETWEEN(70, 98)
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan_1"), PTR(bero_prev_return3))
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_on)
        CASE_DEFAULT
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan_1"), PTR(bero_prev_return4))
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_on)
    END_SWITCH
    RETURN
EVT_END()

BeroEntry bero_normal[3] = {
    {"dokan_2", 2, 0, 0x08, 100000, 0, 0, -1, NULL, 6, NULL, NULL, NULL, 1, 1},
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_00", "dokan_2", 1, 1},
    {NULL},
};

BeroEntry bero_normal2[2] = {
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_01", "dokan_2", 1, 1},
    {NULL},
};

BeroEntry bero_normal3[2] = {
    {"dokan_1", 2, 0, 0x09, 100000, 0, 0, -1, NULL, 6, floor_inc, "jon_02", "dokan_2", 1, 1},
    {NULL},
};

EVT_BEGIN(setup_normal)
    SET_VALUE(LW(0), PTR(bero_normal))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    SWITCH(LW(0))
        CASE_LESS(50)
        CASE_BETWEEN(50, 80)
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan_1"), PTR(bero_normal2))
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_on)
        CASE_GREATER(80)
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_bero_overwrite, STRING("dokan_1"), PTR(bero_normal3))
        SET_VALUE(LW(0), STRING("dokan_1"))
        CALL_CHILD(bero_case_switch_on)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(iri_12_dokan_in_sub)
    LOOP(0)
        CALL(evt_npc_get_unitwork, STRING("マッキノ"), 0, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 32)
    CALL(evt_npc_set_ry, STRING("マッキノ"), 90)
    CALL(evt_npc_jump_position_nohit, STRING("マッキノ"), 0, 29, 0, 400, 29)
    WAIT_MS(200)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        SET_FLOAT_VALUE(LW(0), FLOAT(1.0f))
        LOOP(0)
            SUBTRACT_FLOAT(LW(0), FLOAT(0.007f))
            CALL(evt_npc_set_scale, STRING("マッキノ"), LW(0), LW(0), LW(0))
            WAIT_FRAMES(1)
            IF_FLOAT_LESS(LW(0), FLOAT(0.0f))
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_npc_set_scale, STRING("マッキノ"), FLOAT(0.0f), FLOAT(0.0f), FLOAT(0.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_pera_onoff, STRING("マッキノ"), 0)
        CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 33816576)
        CALL(evt_paper_entry, STRING("p_dokan_y"))
        CALL(evt_npc_set_paper, STRING("マッキノ"), STRING("p_dokan_y"))
        CALL(evt_npc_set_paper_anim, STRING("マッキノ"), STRING("PM_D_1A"))
        CALL(iri_12_paper_init)
        CALL(evt_npc_set_ry, STRING("マッキノ"), 0)
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
        SET_VALUE(LW(2), 0)
        LOOP(0)
            ADD_FLOAT(LW(1), FLOAT(1.0f))
            IF_FLOAT_GREATER(LW(1), FLOAT(30.0f))
                SET_FLOAT_VALUE(LW(1), FLOAT(30.0f))
            END_IF
            ADD(LW(2), 1)
            IF_INT_GREATER(LW(2), 10)
                SUBTRACT_FLOAT(LW(0), LW(1))
                ADD_FLOAT(LW(1), FLOAT(0.1f))
                IF_FLOAT_GREATER(LW(1), FLOAT(30.0f))
                    SET_FLOAT_VALUE(LW(1), FLOAT(30.0f))
                END_IF
            END_IF
            IF_FLOAT_LESS(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            CALL(evt_npc_set_ry, STRING("マッキノ"), LW(0))
            WAIT_FRAMES(1)
            IF_INT_GREATER(LW(2), 18)
                BREAK_LOOP
            END_IF
        END_LOOP
        LOOP(0)
            SUBTRACT_FLOAT(LW(0), LW(1))
            IF_FLOAT_LESS(LW(0), FLOAT(0.0f))
                ADD_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            CALL(evt_npc_set_ry, STRING("マッキノ"), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(300)
    CALL(evt_npc_get_position, STRING("マッキノ"), LW(2), LW(3), LW(4))
    LOOP(0)
        SUBTRACT_FLOAT(LW(3), FLOAT(1.0f))
        CALL(evt_npc_set_position, STRING("マッキノ"), LW(2), LW(3), LW(4))
        WAIT_FRAMES(1)
        IF_FLOAT_LESS_OR_EQUAL(LW(3), FLOAT(-30.0f))
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(iri_12_dokan_in)
    IF_INT_EQUAL(GSW(1420), 12)
        IF_INT_EQUAL(GSWF(5406), 1)
            IF_INT_EQUAL(GSWF(5407), 0)
                SPAWN_EVENT(iri_12_dokan_in_sub)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_return_dokan)
    CALL(evt_npc_set_ry, STRING("マッキノ"), 90)
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("マッキノ"))
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 33816576)
    CALL(evt_npc_flag_onoff, 0, STRING("マッキノ"), 131088)
    CALL(evt_paper_entry, STRING("p_dokan_x"))
    CALL(evt_npc_set_paper, STRING("マッキノ"), STRING("p_dokan_x"))
    CALL(evt_npc_set_paper_anim, STRING("マッキノ"), STRING("PM_D_1A"))
    CALL(iri_12_paper_init)
    CALL(evt_npc_set_ry, STRING("マッキノ"), 180)
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 1073741824)
    CALL(evt_npc_get_position, STRING("マッキノ"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(2), 3)
    CALL(evt_npc_set_position, STRING("マッキノ"), LW(0), LW(1), LW(2))
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_status_onoff, 0, STRING("マッキノ"), 2)
        CALL(evt_npc_get_position, STRING("マッキノ"), LW(0), LW(1), LW(2))
        ADD(LW(0), 60)
        CALL(evt_npc_move_position, STRING("マッキノ"), LW(0), LW(2), 0, 60, 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_FRAMES(20)
    CALL(evt_npc_set_rotate_offset, STRING("マッキノ"), 0, 16, 0)
    BEGIN_SIBLING_EVENT
        CALL(evt_npc_get_position, STRING("マッキノ"), LW(2), LW(3), LW(4))
        SET_FLOAT_VALUE(LW(0), FLOAT(1.0f))
        LOOP(0)
            SUBTRACT_FLOAT(LW(0), FLOAT(0.02f))
            IF_FLOAT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                CALL(evt_npc_set_scale, STRING("マッキノ"), 0, 0, 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_set_scale, STRING("マッキノ"), LW(0), LW(0), LW(0))
            SET_FLOAT_VALUE(LW(1), LW(0))
            MULTIPLY_FLOAT(LW(1), FLOAT(16.0f))
            CALL(evt_npc_set_rotate_offset, STRING("マッキノ"), 0, LW(1), 0)
            SET_FLOAT_VALUE(LW(2), FLOAT(16.0f))
            SUBTRACT_FLOAT(LW(2), LW(1))
            ADD_FLOAT(LW(2), LW(3))
            CALL(iri_12_makkino_set_y, LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(1), FLOAT(30.0f))
        LOOP(0)
            ADD_FLOAT(LW(0), LW(1))
            IF_FLOAT_GREATER_OR_EQUAL(LW(0), FLOAT(360.0f))
                SUBTRACT_FLOAT(LW(0), FLOAT(360.0f))
            END_IF
            CALL(evt_npc_set_rotate, STRING("マッキノ"), LW(0), 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_WHILE(GF(0))
    CALL(evt_npc_set_position, STRING("マッキノ"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_return)
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 32)
    CALL(evt_npc_move_position, STRING("マッキノ"), 72, 0, 0, 120, 4)
    CALL(evt_msg_print, 0, STRING("irai_12_011"), 0, STRING("マッキノ"))
    CALL(evt_npc_move_position, STRING("マッキノ"), 98, -123, 0, 120, 4)
    CALL_CHILD(iri_12_makkino_return_dokan)
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_return_b100)
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 32)
    CALL(evt_npc_move_position, STRING("マッキノ"), -205, -75, 0, 120, 4)
    CALL(evt_msg_print, 0, STRING("irai_12_011"), 0, STRING("マッキノ"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_npc_move_position, STRING("マッキノ"), -76, -310, 0, 180, 4)
    CALL(evt_npc_move_position, STRING("マッキノ"), 98, -486, 0, 180, 4)
    CALL_CHILD(iri_12_makkino_return_dokan)
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_return_zonbaba)
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 32)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("マッキノ"), -337, -124, 0, 120, 4)
    CALL(evt_msg_print, 0, STRING("irai_12_011"), 0, STRING("マッキノ"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_npc_move_position, STRING("マッキノ"), -76, -310, 0, 180, 4)
    CALL(evt_npc_move_position, STRING("マッキノ"), 98, -486, 0, 180, 4)
    CALL_CHILD(iri_12_makkino_return_dokan)
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_move_dokan_side)
    CALL(evt_npc_set_unitwork, STRING("マッキノ"), 0, 1)
    CALL(evt_npc_get_reglid, STRING("マッキノ"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_change_interrupt, STRING("マッキノ"), 1, 0)
    CALL(evt_npc_change_interrupt, STRING("マッキノ"), 6, 0)
    LOOP(0)
        CALL(evt_npc_status_check, STRING("マッキノ"), 16, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_area_check, 1, -40, 0, 15, LW(0), LW(2), LW(3))
        IF_INT_EQUAL(LW(3), 1)
            CALL(evt_mario_mov_pos2, -40, -40, 60)
        END_IF
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 32)
    CALL(evt_npc_move_position, STRING("マッキノ"), -40, 0, 0, 150, 4)
    CALL(evt_npc_flag_onoff, 0, STRING("マッキノ"), 32)
    CALL(wait_landon)
    CALL(evt_npc_change_interrupt, STRING("マッキノ"), 6, PTR(iri_12_makkino_talk))
    CALL(evt_npc_set_unitwork, STRING("マッキノ"), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(iri_12_enemy_dead_event)
    IF_INT_EQUAL(GSW(1420), 12)
        IF_INT_EQUAL(GSWF(5406), 1)
            IF_INT_EQUAL(GSWF(5407), 0)
                WAIT_MS(800)
                SPAWN_EVENT(iri_12_makkino_move_dokan_side)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_urouro)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("マッキノ"), -175, 109, 0, 120, 4)
        WAIT_FRAMES(1)
        CALL(evt_npc_move_position, STRING("マッキノ"), -73, 109, 0, 120, 4)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_buruburu)
    CALL(evt_npc_move_position, STRING("マッキノ"), -440, -124, 0, 120, 4)
    CALL(evt_npc_get_position, STRING("マッキノ"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 1)
    LOOP(0)
        ADD(LW(0), 2)
        CALL(evt_npc_set_position, STRING("マッキノ"), LW(0), LW(1), LW(2))
        WAIT_MS(50)
        SUBTRACT(LW(0), 2)
        CALL(evt_npc_set_position, STRING("マッキノ"), LW(0), LW(1), LW(2))
        WAIT_MS(50)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_okiru)
    CALL(evt_npc_set_anim, STRING("マッキノ"), STRING("O_1"))
    CALL(evt_npc_wait_anim, STRING("マッキノ"), 1)
    CALL(evt_npc_set_balloontype, STRING("マッキノ"), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 1536)
    CALL(evt_npc_flag_onoff, 0, STRING("マッキノ"), 16777216)
    CALL(evt_npc_status_onoff, 1, STRING("マッキノ"), 2)
    CALL(evt_set_dir_to_target, STRING("マッキノ"), STRING("mario"))
    RETURN
EVT_END()

static EVT_BEGIN(iri_12_makkino_talk)
    CALL(evt_npc_check, STRING("敵"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        IF_INT_EQUAL(GSWF(5406), 0)
            CALL(evt_msg_print, 0, STRING("irai_12_008"), PTR(iri_12_msgcb), STRING("マッキノ"))
            SET_VALUE(GSWF(5406), 1)
            CALL(evt_npc_change_interrupt, STRING("マッキノ"), 6, 0)
            SPAWN_EVENT(iri_12_makkino_move_dokan_side)
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_12_010"), 0, STRING("マッキノ"))
        END_IF
    ELSE
        IF_INT_EQUAL(GSWF(5406), 0)
            CALL(evt_msg_print, 0, STRING("irai_12_007"), 0, STRING("マッキノ"))
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_12_009"), 0, STRING("マッキノ"))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_fall)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 2)
    CALL(evt_bero_get_start_position, LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 32)
    CALL(evt_npc_set_position, STRING("マッキノ"), LW(0), LW(1), LW(2))
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 131120)
    CALL(wait_landon)
    CALL(evt_npc_check, STRING("敵"), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(iri_12_makkino_move_dokan_side)
    ELSE
        CALL(evt_npc_move_position, STRING("マッキノ"), -55, 109, 0, 120, 4)
        CALL(evt_npc_check, STRING("敵"), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_npc_change_interrupt, STRING("マッキノ"), 1, PTR(iri_12_makkino_urouro))
            CALL(evt_npc_restart_regular_event, STRING("マッキノ"))
        END_IF
    END_IF
    CALL(evt_npc_change_interrupt, STRING("マッキノ"), 6, PTR(iri_12_makkino_talk))
    CALL(evt_npc_flag_onoff, 0, STRING("マッキノ"), 131088)
    RETURN
EVT_END()

EVT_BEGIN(iri_12_makkino_fall_return)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_seq_wait, 2)
    CALL(evt_bero_exec_wait, 2)
    CALL(evt_bero_get_start_position, LW(0), LW(1), LW(2))
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 32)
    CALL(evt_npc_set_position, STRING("マッキノ"), LW(0), LW(1), LW(2))
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 131120)
    CALL(wait_landon)
    IF_INT_EQUAL(GSW(1321), 99)
        IF_INT_EQUAL(GSWF(5085), 1)
            SPAWN_EVENT(iri_12_makkino_return_b100)
            SET_VALUE(GSWF(5407), 1)
        ELSE
            CALL(evt_npc_change_interrupt, STRING("マッキノ"), 1, PTR(iri_12_makkino_buruburu))
            CALL(evt_npc_restart_regular_event, STRING("マッキノ"))
            CALL(evt_mario_key_onoff, 1)
        END_IF
    ELSE
        CALL_CHILD(iri_12_makkino_return)
        SET_VALUE(GSWF(5407), 1)
        CALL(evt_mario_key_onoff, 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(iri_12_init_fall)
    CALL(evt_npc_entry, STRING("マッキノ"), STRING("c_suifu_k"))
    CALL(evt_npc_set_tribe, STRING("マッキノ"), STRING("水夫"))
    CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 1536)
    CALL(evt_npc_status_onoff, 1, STRING("マッキノ"), 2)
    SET_VALUE(LW(0), GSW(1321))
    ADD(LW(0), 1)
    MODULO(LW(0), 10)
    IF_INT_EQUAL(LW(0), 0)
        SPAWN_EVENT(iri_12_makkino_fall_return)
    ELSE
        SPAWN_EVENT(iri_12_makkino_fall)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(iri_12_init)
    IF_INT_EQUAL(GSW(1420), 12)
        SWITCH(GSW(1321))
            CASE_EQUAL(17)
            IF_INT_EQUAL(GSWF(5406), 0)
                CALL(evt_npc_entry, STRING("マッキノ"), STRING("c_suifu_k"))
                CALL(evt_npc_set_tribe, STRING("マッキノ"), STRING("水夫"))
                CALL(evt_npc_set_position, STRING("マッキノ"), -50, 0, -120)
                CALL(evt_npc_set_ry, STRING("マッキノ"), 90)
                CALL(evt_npc_flag_onoff, 1, STRING("マッキノ"), 16777216)
                CALL(evt_npc_change_interrupt, STRING("マッキノ"), 6, PTR(iri_12_makkino_talk))
                CALL(evt_npc_set_anim, STRING("マッキノ"), STRING("D_2"))
                CALL(evt_npc_set_balloontype, STRING("マッキノ"), 0)
            ELSE
                CALL_CHILD(iri_12_init_fall)
            END_IF
            CASE_DEFAULT
            IF_INT_EQUAL(GSWF(5406), 1)
                IF_INT_EQUAL(GSWF(5407), 0)
                    CALL_CHILD(iri_12_init_fall)
                END_IF
            END_IF
        END_SWITCH
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_iri_30_touch_rakugaki)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("irai_30_007"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_iri_30_bomb_rakugaki)
    CALL(evt_msg_close, STRING("irai_30_007"))
    CALL(evt_del_case_evt, 1, LW(15))
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mapobj_get_position, STRING("s_rakugaki"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_WALL_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_paper_entry, STRING("OFF_d_bom_ver2"))
    CALL(evt_offscreen_entry, STRING("ofs"))
    CALL(evt_img_entry, STRING("img"))
    CALL(evt_img_onoff, STRING("img"), 0)
    CALL(evt_img_set_paper, STRING("img"), STRING("OFF_d_bom_ver2"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("s_rakugaki"), STRING("ofs"))
    CALL(evt_offscreen_get_boundingbox2, STRING("ofs"), LW(0), LW(1), LW(2), LW(3))
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(LW(2), 0)
        IF_INT_EQUAL(LW(3), 0)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_img_alloc_capture, STRING("img"), 0, STRING("ofs"), 1, LW(0), LW(1), LW(2), LW(3))
        WAIT_FRAMES(1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_rakugaki"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("before"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("after"), 1)
        CALL(evt_img_clear_virtual_point, STRING("img"))
        CALL(evt_img_onoff, STRING("img"), 1)
        CALL(evt_img_set_paper_anim, STRING("img"), STRING("A_1"))
        CALL(evt_img_wait_animend, STRING("img"))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("s_rakugaki"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("before"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("after"), 1)
    END_IF
    CALL(evt_img_release, STRING("img"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("s_rakugaki"))
    CALL(evt_paper_delete, STRING("OFF_d_bom_ver2"))
    CALL(evt_offscreen_delete, STRING("ofs"))
    SET_VALUE(GSWF(5406), 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(gonbaba_standup_shake)
    WAIT_MS(3700)
    CALL(evt_npc_get_position, STRING("敵"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_GNB_STEP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 800)
    RETURN
EVT_END()

static EVT_BEGIN(gonbaba_fog_change)
    CALL(evt_map_get_fog, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
    SET_VALUE(GW(0), LW(5))
    SET_VALUE(GW(1), LW(6))
    CONVERT_MS_TO_FRAME(LW(10), 8000)
    ADD(LW(10), 1)
    BEGIN_INLINE_EVENT
        LOOP(LW(10))
            CALL(evt_map_set_fog, LW(4), GW(0), GW(1), LW(7), LW(8), LW(9))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_init, 11, 0, 400, LW(10))
        LOOP(LW(10))
            CALL(evt_sub_intpl_get_value)
            SET_VALUE(GW(0), LW(0))
            ADD(GW(0), LW(5))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_sub_intpl_init, 11, 0, 1600, LW(10))
    LOOP(LW(10))
        CALL(evt_sub_intpl_get_value)
        SET_VALUE(GW(1), LW(0))
        ADD(GW(1), LW(6))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(gonbaba_down_shake)
    CALL(evt_npc_get_position, STRING("敵"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon, STRING("SFX_STG1_GNB_DOWN1"), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_GNB_DOWN2"), LW(0), LW(1), LW(2), LW(8))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 90, 0, 10000)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_snd_sfx_vol, LW(8), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        CALL(evt_snd_sfxoff, LW(8))
    END_INLINE_EVENT
    CALL(evt_cam_shake, 4, FLOAT(0.05f), FLOAT(0.0f), 500)
    CALL(evt_sub_intpl_init, 11, 50, 5, 50)
    LOOP(0)
        CALL(evt_sub_intpl_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
        CALL(evt_cam_shake, 4, LW(0), FLOAT(0.0f), 50)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 2000)
    RETURN
EVT_END()

EVT_BEGIN(zonbaba_first_event)
    IF_INT_EQUAL(GSWF(5085), 1)
        CALL(evt_npc_set_position, STRING("敵"), 0, -1000, 0)
        CALL(evt_npc_flag_onoff, 0, STRING("敵"), 131096)
        CALL(evt_mobj_itembox, STRING("box"), 270, 0, 200, 1, 0, PTR(evt_open_box), GSWF(5084))
        CALL(evt_hitobj_onoff, STRING("baba"), 1, 0)
        RETURN
    END_IF
    CALL(evt_map_set_fog, 2, 400, 1000, 0, 10, 40)
    CALL(evt_map_fog_onoff, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_npc_set_position, STRING("敵"), 175, 0, 0)
    CALL(evt_npc_set_anim, STRING("敵"), STRING("GNB_S_1"))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oFF_jon_06"), 1)
    CALL(evt_seq_wait, 2)
    WAIT_MS(3500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 96)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 96)
    CALL(evt_mario_set_pose, STRING("M_N_5B"))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(3), LW(0))
        ADD(LW(3), -40)
        CALL(evt_party_move_pos, 0, LW(3), LW(2), 250)
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_set_dir_npc, STRING("敵"))
    CALL(evt_party_set_dir_npc, 0, STRING("敵"))
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_GONBABA_FLY2"))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon_3d_ex, STRING("SFX_STG1_GNB_ROAR2"), 0, 0, 0, 255, 255, 4300, 16, 0)
        CALL(evt_npc_set_anim, STRING("敵"), STRING("GNB_U_1"))
        CALL(evt_npc_wait_anim, STRING("敵"), FLOAT(1.0f))
        CALL(evt_npc_set_anim, STRING("敵"), STRING("GNB_S_1"))
    END_INLINE_EVENT
    SPAWN_EVENT(gonbaba_standup_shake)
    SPAWN_EVENT(gonbaba_fog_change)
    CALL(evt_cam3d_evt_set, -755, 24, 376, -84, 114, -22, 4500, 11)
    WAIT_MS(4000)
    LOOP(30)
        CALL(evt_cam3d_evt_set, -755, 24, 376, -84, 114, -22, 15, 11)
        WAIT_MS(15)
        CALL(evt_cam3d_evt_set, -745, 23, 366, -84, 114, -22, 15, 11)
        WAIT_MS(15)
    END_LOOP
    WAIT_MS(3000)
    CALL(evt_npc_set_battle_info, STRING("敵"), 51)
    CALL(evt_npc_battle_start, STRING("敵"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_cam3d_evt_set, -769, 50, 527, -22, 114, 7, 1, 11)
    CALL(evt_npc_set_anim, STRING("敵"), STRING("GNB_H_1"))
    CALL(evt_map_fog_onoff, 0)
    CALL(evt_npc_set_position, STRING("敵"), 0, 60, 0)
    CALL(evt_mario_set_pos, 25, 10, 150)
    CALL(evt_party_set_pos, 0, -25, 10, 150)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 90, -1)
    SPAWN_EVENT(gonbaba_down_shake)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_npc_flag_onoff, 1, STRING("敵"), 131088)
        CALL(gonbaba_wait_landon)
        CALL(evt_npc_set_anim, STRING("敵"), STRING("GNB_H_2"))
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("敵"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_GNB_DOWN2"), LW(0), LW(1), LW(2), LW(3))
        CALL(evt_cam_shake, 4, FLOAT(0.05f), FLOAT(0.0f), 500)
        CALL(evt_sub_intpl_init, 11, 50, 5, 50)
        LOOP(0)
            CALL(evt_sub_intpl_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(1000.0f))
            CALL(evt_cam_shake, 4, LW(0), FLOAT(0.0f), 50)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 2000)
        CALL(evt_snd_sfxoff, LW(3))
    END_INLINE_EVENT
    WAIT_MS(1000)
    WAIT_MS(5000)
    CALL(evt_npc_get_position, STRING("敵"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_GNB_DOWN3"), LW(0), LW(1), LW(2), 0)
    WAIT_MS(600)
    CALL(evt_cam3d_evt_set, -145, 80, 882, 104, 114, 81, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mobj_itembox, STRING("box"), 250, 60, 0, 1, 0, PTR(evt_open_box), GSWF(5084))
    CALL(evt_mobj_flag_onoff, 0, STRING("box"), 64)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("敵"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_GNB_THROAT_MOVE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1000)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_GMB_MOUTH_MOVE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(200)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_GMB_MOUTH_MOVE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(200)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_GMB_MOUTH_MOVE1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(1100)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_GMB_MOUTH_OPEN1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("敵"), STRING("GNB_H_4"))
    CALL(evt_npc_wait_anim, STRING("敵"), FLOAT(0.78f))
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_mobj_get_position, STRING("box"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_TBOX_JUMP1"), LW(0), LW(1), LW(2), GW(0))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(60.0f))
        SET_FLOAT_VALUE(LW(1), FLOAT(25.0f))
        SET_FLOAT_VALUE(LW(2), FLOAT(-0.6f))
        SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(4), FLOAT(1.3f))
        SET_VALUE(LF(0), 0)
        LOOP(0)
            ADD_FLOAT(LW(0), LW(1))
            ADD_FLOAT(LW(3), LW(4))
            CALL(evt_mobj_set_y_position, STRING("box"), LW(0))
            CALL(evt_mobj_set_z_position, STRING("box"), LW(3))
            ADD_FLOAT(LW(1), LW(2))
            IF_INT_LESS(LW(1), -14)
                SET_FLOAT_VALUE(LW(1), FLOAT(-14.0f))
            END_IF
            IF_INT_LESS_OR_EQUAL(LW(0), 0)
                IF_INT_EQUAL(LF(0), 0)
                    CALL(evt_snd_sfxoff, GW(0))
                    SET_VALUE(LF(0), 1)
                END_IF
                SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
                MULTIPLY_FLOAT(LW(1), FLOAT(-0.5f))
                CALL(evt_mobj_get_position, STRING("box"), LW(5), LW(6), LW(7))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG8_TBOX_LANDING1"), LW(5), LW(6), LW(7), 0)
                IF_INT_LESS_OR_EQUAL(LW(1), 2)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mobj_set_y_position, STRING("box"), 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_npc_set_anim, STRING("敵"), STRING("GNB_H_3"))
    WAIT_MS(200)
    CALL(evt_cam3d_evt_set, 255, 105, 648, 255, 55, 150, 500, 11)
    WAIT_MS(1800)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_set_dir, 90, 300, 0)
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("oFF_jon_06"), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("敵"), 32)
    CALL(evt_npc_flag_onoff, 0, STRING("敵"), 8)
    SET_VALUE(GSWF(5085), 1)
    IF_INT_EQUAL(GSW(1420), 12)
        IF_INT_EQUAL(GSWF(5406), 1)
            IF_INT_EQUAL(GSWF(5407), 0)
                CALL(evt_npc_get_reglid, STRING("マッキノ"), LW(0))
                DELETE_EVENT(LW(0))
                CALL(evt_npc_change_interrupt, STRING("マッキノ"), 1, 0)
                CALL(evt_mario_key_onoff, 0)
                SPAWN_EVENT(iri_12_makkino_return_zonbaba)
                SET_VALUE(GSWF(5407), 1)
            END_IF
        END_IF
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// clang-format on

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];
void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("jon_00", jon_init_evt);
    relSetEvtAddr("jon_01", jon_init_evt);
    relSetEvtAddr("jon_02", jon_init_evt);
    relSetEvtAddr("jon_03", jon_init_evt);
    relSetEvtAddr("jon_04", jon_init_evt);
    relSetEvtAddr("jon_05", jon_init_evt);
    relSetEvtAddr("jon_06", jon_init_evt);
    relSetBtlAddr("jon", btlsetup_jon_tbl, btl_jon_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}

void raster_disp(CameraId camera, void* param) {
    s32 row;
    s32 col;
    CameraEntry* cam = camGetPtr(camera);
    EventEntry* event = param;
    f32* eventf = (f32*)&event->lwData[0];
    f32 indMtx[2][3];
    GXTexObj texObj;
    u8* tex;
    u8* image;
    f32 amplitude;
    f32 phase;
    s32 i, j;
    amplitude = eventf[0];
    phase = eventf[1];
    if (amplitude > 127.0f) {
        amplitude = 127.0f;
    }

    tex = smartAlloc(GXGetTexBufferSize(0x260, 0x1e0, GX_TF_RGBA8, GX_FALSE, GX_FALSE), 3)->address;
    image = smartAlloc(GXGetTexBufferSize(0x40, 0x40, GX_TF_IA8, GX_FALSE, GX_FALSE), 3)->address;
    phase = 6.2831855f * phase;

    for (i = 0; i < 0x40; i++) {
#ifdef MUST_MATCH
        // MUST_MATCH: reusing `q` as the row base (instead of `row = q * 32;`) puts MWCC's callee-saved GPRs in
        // the target's order (row/cam/0x4330/gp in r31/r28/r29/r30); the #else form is the natural one.
        s32 q = i / 4;
        col = i % 4;
        q = q * 32;
        row = q;
#else
        s32 q = i / 4;
        row = q * 32;
        col = i % 4;
#endif
        for (j = 0; j < 0x40; j++) {
            f32 k = 0.25f;
            f32 v = (f32)sin((f32)j + ((f32)gp->retraceCount * k + phase));
            // Two-statement offset keeps the target's `row + (j/4)*512` operand order; the one-expression
            // form reassociates it to `(j/4)*512 + row` and loses the match.
            s32 off = row + (j / 4) * 512;
            off += (col + (j % 4) * 4) * 2;
            image[off] = (s32)(amplitude * v + 128.0f);
            image[off + 1] = 0x80;
        }
    }

    DCFlushRange(image, 0x2000);
    GXSetTexCopySrc(0, 0, 0x260, 0x1e0);
    GXSetTexCopyDst(0x260, 0x1e0, GX_TF_RGBA8, GX_FALSE);
    GXCopyTex(tex, GX_FALSE);
    GXPixModeSync();

    GXInitTexObj(&texObj, tex, 0x260, 0x1e0, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);

    GXInitTexObj(&texObj, image, 0x40, 0x40, GX_TF_IA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP1);

    GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_OR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, 0x7d);

    {
        indMtx[0][0] = 0.5f;
        indMtx[0][1] = 0.0f;
        indMtx[0][2] = 0.0f;
        indMtx[1][0] = 0.0f;
        indMtx[1][1] = 0.5f;
        indMtx[1][2] = 0.0f;
        GXSetIndTexMtx(GX_ITM_0, indMtx, 1);
    }

    GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP1);
    GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_8, GX_ITS_8);
    GXSetTevIndWarp(GX_TEVSTAGE0, GX_INDTEXSTAGE0, 1, 0, GX_ITM_0);
    GXSetNumIndStages(1);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXLoadPosMtxImm(cam->view, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        // Named sizes: literal operands get other FPRs and operand order.
        f32 w = 608.0f;
        f32 h = 480.0f;
        f32 top = 240.0f;
        f32 right = 304.0f;
        f32 half = 0.5f;
        GXPosition3f32(-w * half, top, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(right, top, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(right, -h * half, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-w * half, -h * half, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }
    GXSetNumIndStages(0);
    GXSetTevDirect(GX_TEVSTAGE0);
}

s32 evt_raster(EventEntry* event, BOOL isFirstCall) {
    f32* eventf = (f32*)&event->lwData[0];

    if (isFirstCall) {
        eventf[0] = 0.0f;
        eventf[1] = 0.0f;
    }
    eventf[0] += 0.2f;
    eventf[1] += 0.01f;
    dispEntry(CAMERA_2D, 0.0f, 3, raster_disp, event);
    return EVT_RETURN_BLOCK;
}

s32 enemy_info(EventEntry* event, BOOL isFirstCall) {
    s32* args;
    s32 index;
    u8 kind;
    NpcTribeInfo* tribe;
    const char* name;
    s32 found;
    BattleSetupData* setup;

    args = event->args;
    index = evtGetValue(event, args[0]);
    kind = enemy_100[index];
    found = 0;
    setup = &btlsetup_jon_tbl[kind];

    for (tribe = npcTribe; tribe->nameJp != NULL; tribe++) {
        if (strcmp(tribe->nameJp, setup->name) == 0) {
            found = 1;
            break;
        }
    }
    if (!found) {
        name = "クリボー";
        tribe = npcGetTribe(name);
    } else {
        name = tribe->nameJp;
    }
    evtSetValue(event, args[1], (s32)tribe->modelName);
    evtSetValue(event, args[2], (s32)name);
    evtSetValue(event, args[3], kind);
    return EVT_RETURN_DONE;
}

s32 yattukeFlag(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("敵");

    npc->battleInfo.flags |= NPC_BATTLE_NO_FIRST_STRIKE_WIN;
    return EVT_RETURN_DONE;
}

s32 setup_npc_ex_para(EventEntry* event, BOOL isFirstCall) {
    s32 name = evtGetValue(event, event->args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)name);

    npc->territoryLoiter.y = 50.0f;
    npc->territoryHoming.y = 180.0f;
    return EVT_RETURN_DONE;
}

s32 wanwan_set_home_pos(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 x = evtGetValue(event, args[0]);
    s32 y = evtGetValue(event, args[1]);
    s32 z = evtGetValue(event, args[2]);

    ((NpcSetupInfo*)npcEnt_wanwan)->territoryBase.x = (f32)x;
    ((NpcSetupInfo*)npcEnt_wanwan)->territoryBase.y = (f32)y;
    ((NpcSetupInfo*)npcEnt_wanwan)->territoryBase.z = (f32)z;
    return EVT_RETURN_DONE;
}

s32 shikakeFlag(EventEntry* event, BOOL isFirstCall) {
    s32 flag = evtGetValue(event, event->args[0]);
    NpcEntry* npc = npcNameToPtr("敵");

    switch (flag) {
    case 0:
        npc->battleInfo.flags |= NPC_BATTLE_NO_PARTY_CHANGE;
        break;
    case 1:
        npc->battleInfo.flags |= NPC_BATTLE_NO_HAMMER;
        break;
    case 2:
        npc->battleInfo.flags |= NPC_BATTLE_NO_JUMP;
        break;
    case 3:
        npc->battleInfo.flags |= NPC_BATTLE_COIN_BONUS;
        break;
    }
    return EVT_RETURN_DONE;
}

s32 iri_12_paper_init(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("マッキノ");

    animPosePaperPeraOff(npc->poseId);
    animPoseSetMaterialLightFlagOff(npc->poseId, 1);
    return EVT_RETURN_DONE;
}

s32 iri_12_makkino_set_y(EventEntry* event, BOOL isFirstCall) {
    f32 y = evtGetFloat(event, event->args[0]);
    NpcEntry* npc = npcNameToPtr("マッキノ");

    npc->position.y = y;
    return EVT_RETURN_DONE;
}

s32 wait_landon(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("マッキノ");

    if (!(npc->flags & 0x10) || npc->groundHit != NULL) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

void iri_12_msgcb(s32 type, smartEntry* smart) {
    MessageData* data = smart->address;

    if (type == 3 && data->choiceIndex == 1) {
        evtEntry(iri_12_makkino_okiru, 0, 0);
    }
}

static s32 gonbaba_wait_landon(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("敵");

    return npc->groundHit != NULL ? 2 : 0;
}

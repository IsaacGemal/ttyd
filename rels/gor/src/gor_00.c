#include "gor_00.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_item.h"
#include "evt/evt_light.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_memcard.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_nannpc.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_telop.h"
#include "evt/evt_urouro.h"
#include "evt/evt_win.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "kpa_act.h"
#include "party/party_motion.h"
#include "memory.h"
#include "mario/mario_pouch.h"
#include "mario/mario.h"
#include "party.h"
#include "manager/evtmgr_cmd.h"
#include "manager/evtmgr.h"
#include "driver/mapdrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include <string.h>
#include "driver/msgdrv.h"
#include "driver/npcdrv.h"
#include "motion/mot_ship.h"
#include "system.h"
#include "math_ext.h"
#include "eff/eff_mahojin.h"
#include "eff/eff_funemizu.h"
#include "eff/n64/eff_water_damage_n64.h"
#include <dolphin/gx.h>

#pragma pool_data off

#pragma force_active on
extern s32 luigi_gor_first_talk[];
static USER_FUNC(get_yoshi_color);
static USER_FUNC(scale_func);
static USER_FUNC(fune_eff);
static USER_FUNC(cam_kaiten_set_para);
static USER_FUNC(cam_kaiten_get_para);
static USER_FUNC(cam_kaiten_set_direct);
static USER_FUNC(cam_kaiten_init);
static USER_FUNC(delete_itemsel_table);
static USER_FUNC(make_itemsel_table);
static f32 accdec_type(s32 target, s32 count, f32 start, f32 end);
static USER_FUNC(alpha_set_func);
static USER_FUNC(alpha_init_func);
static void alpha_tev(MapTevCallback* param);
static s32 sanders_talk[8];
static u8* _ap;
static s32* itemsel_table_ram;
static f32* ckp0;
static f32* ckp1;
static f32* ckpw;
u32 inemuri_sfx_id;
char* unk_gor_bss00000000[1];
s32 tentyou_talk[14];
s32 tenin_talk[14];
static s32 borodo2_talk[13];
NpcSetupInfo rakganEnt[2];
s32 christine_osoware[357];
s32 gundan1000tai_beforebattle[524];
s32 gundan1000tai_afterbattle[1201];
s32 mony_peton_after[37];
s32 shukko_event[498];
s32 mony_leave[108];
s32 peton_leave[88];
s32 mony_peton_restart[61];
static s32 peach_mail[209];
static s32 option_ship_mario[139];
s32 gor_muj_ship_start[258];

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
u32 nannpc_entry_data[123] = {
    (u32)"c_zako_n", (u32)"S_1", 0x00000000, (u32)"c_zako_n",
    (u32)"S_1", 0x40000000, (u32)"c_zako_n", (u32)"S_1",
    0x40800000, (u32)"c_zako_n", (u32)"S_1", 0x40C00000,
    (u32)"c_zako_n", (u32)"S_1", 0x41000000, (u32)"c_zako_n",
    (u32)"S_1", 0x41200000, (u32)"c_zako_n", (u32)"S_1",
    0x41400000, (u32)"c_zako_n", (u32)"S_1", 0x41600000,
    (u32)"c_zako_n", (u32)"S_1", 0x41800000, (u32)"c_zako_n",
    (u32)"S_1", 0x41900000, (u32)"c_zako_n", (u32)"S_1",
    0x41A00000, (u32)"c_zako_n", (u32)"S_1", 0x41B00000,
    (u32)"c_zako_n", (u32)"S_1", 0x41C00000, (u32)"c_zako_n",
    (u32)"S_1", 0x41D00000, (u32)"c_zako_n", (u32)"S_1",
    0x41E00000, (u32)"c_zako_n", (u32)"S_1", 0x41F00000,
    (u32)"c_zako_n", (u32)"S_1", 0x42000000, (u32)"c_zako_n",
    (u32)"S_1", 0x42080000, (u32)"c_zako_n", (u32)"S_1",
    0x42100000, (u32)"c_zako_n", (u32)"S_1", 0x42180000,
    (u32)"c_zako_n", (u32)"R_1", 0x00000000, (u32)"c_zako_n",
    (u32)"R_1", 0x3F800000, (u32)"c_zako_n", (u32)"R_1",
    0x40000000, (u32)"c_zako_n", (u32)"R_1", 0x40400000,
    (u32)"c_zako_n", (u32)"R_1", 0x40800000, (u32)"c_zako_n",
    (u32)"R_1", 0x40A00000, (u32)"c_zako_n", (u32)"R_1",
    0x40C00000, (u32)"c_zako_n", (u32)"R_1", 0x40E00000,
    (u32)"c_zako_n", (u32)"R_1", 0x41000000, (u32)"c_zako_n",
    (u32)"R_1", 0x41100000, (u32)"c_zako_n", (u32)"R_1",
    0x41200000, (u32)"c_zako_n", (u32)"R_1", 0x41300000,
    (u32)"c_zako_n", (u32)"R_1", 0x41400000, (u32)"c_zako_n",
    (u32)"R_1", 0x41500000, (u32)"c_zako_n", (u32)"R_1",
    0x41600000, (u32)"c_zako_n", (u32)"R_1", 0x41700000,
    (u32)"c_zako_n", (u32)"R_1", 0x41800000, (u32)"c_zako_n",
    (u32)"R_1", 0x41880000, (u32)"c_zako_n", (u32)"R_1",
    0x41900000, (u32)"c_zako_n", (u32)"R_1", 0x41980000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 dai3_animtbl[21] = {
    0x00000000, 0x00010001, 0x00020002, 0x00030003,
    0x00040004, 0x00050005, 0x00060006, 0x00070007,
    0x00080008, 0x00090009, 0x000A000A, 0x000B000B,
    0x000C000C, 0x000D000D, 0x000E000E, 0x000F000F,
    0x00100010, 0x00110011, 0x00120012, 0x00130013,
    0xFFFF0000,
};

// TODO: type
u32 dai3_run_animtbl[11] = {
    0x00140015, 0x00160017, 0x00180019, 0x001A001B,
    0x001C001D, 0x001E001F, 0x00200021, 0x00220023,
    0x00240025, 0x00260027, 0xFFFF0000,
};

EVT_BEGIN(no00_custom)
    IF_INT_EQUAL(GSW(0), 5)
        CALL(evt_mario_wait_landon)
        CALL(evt_mario_flag_onoff, 1, 256)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
        ADD(LW(12), -80)
        CALL(evt_mario_mov_pos2, LW(10), LW(12), FLOAT(150.0f))
        CALL(evt_mario_dispflag_onoff, 1, 1)
        CALL(evt_party_dispflg_onoff, 1, 1, 1)
        SET_VALUE(GF(10), 0)
        SET_VALUE(LW(0), 1)
        RETURN
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[2] = {
    {"n_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, no00_custom, "gor_01", "s_bero", 1, 1},
    {NULL},
};

const GXColor zero_gor_000126e4 = {0, 0, 0, 0};

static EVT_BEGIN(mony_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(122, 123)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), 130, 15, 250)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(gor_00_mony_regl)
    SWITCH(GSW(0))
        CASE_EQUAL(278)
        CALL_CHILD(mony_leave)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(mony_peton_talk)
    CALL(evt_snd_bgmoff_f, 1024, 3000)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(500)
    CALL(evt_set_dir_to_target, STRING("モニー"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("mac_2_072"), 0, STRING("モニー"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_2_073"), 0, STRING("モニー"))
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 1073741856)
    CALL(evt_party_stop, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(0), 150)
            CALL(evt_mario_mov_pos2, LW(0), 235, FLOAT(120.0f))
        END_IF
        CALL(evt_mario_mov_pos2, 80, 240, FLOAT(120.0f))
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 55, 235, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 157, 115, 562, 157, 55, 217, 1000, 11)
    CALL(evt_npc_set_ry, STRING("モニー"), 90)
    CALL(evt_npc_set_ry, STRING("ピートン"), 270)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("mac_2_074"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_2_075"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("mac_2_076"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_2_077"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("mac_2_078"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_2_079"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("mac_2_080"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_2_081"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("mac_2_082"), 0, STRING("モニー"))
    CALL(evt_npc_get_position, STRING("モニー"), LW(0), LW(1), LW(2))
    CALL(evt_mario_get_pos, 0, LW(3), LW(1), LW(2))
    IF_INT_GREATER(LW(0), LW(3))
        CALL(evt_npc_set_ry, STRING("モニー"), 270)
        CALL(evt_npc_get_position, STRING("モニー"), LW(0), LW(1), LW(2))
    ELSE
        CALL(evt_npc_get_position, STRING("ピートン"), LW(0), LW(1), LW(2))
    END_IF
    CALL(evt_cam3d_evt_set, LW(0), 115, 562, LW(0), 55, 217, 300, 11)
    WAIT_MS(600)
    CALL(evt_msg_print, 0, STRING("mac_2_083"), 0, STRING("モニー"))
    CALL(evt_msg_select, 0, STRING("mac_2_083_yesno"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("mac_2_084"))
        SET_VALUE(GSWF(1176), 1)
    ELSE
        CALL(evt_msg_print_add, 0, STRING("mac_2_086"))
        SET_VALUE(GSWF(1176), 0)
    END_IF
    CALL(evt_npc_flag_onoff, 0, STRING("モニー"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("ピートン"), 1073741856)
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_party_run, 0)
    CALL(evt_run_case_evt, 0, 1, STRING("A_oku_yuka_01"), 0, PTR(mony_peton_after), 0)
    SET_VALUE(GSW(0), 123)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgmoff_f, 513, 2000)
    CALL(evt_snd_bgmon_f, 288, 0, 2000)
    RETURN
EVT_END()

static EVT_BEGIN(mony_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(122)
        CALL_CHILD(mony_peton_talk)
        CASE_EQUAL(123)
        IF_INT_EQUAL(GSWF(1176), 0)
            CALL(evt_msg_print, 0, STRING("mac_2_087"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("mac_2_085"), 0, STRING("me"))
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(peton_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(122, 123)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_set_position, STRING("me"), 185, 15, 250)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(gor_00_peton_regl)
    SWITCH(GSW(0))
        CASE_EQUAL(278)
        CALL_CHILD(peton_leave)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(peton_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(122)
        CALL_CHILD(mony_peton_talk)
        CASE_EQUAL(123)
        IF_INT_EQUAL(GSWF(1176), 0)
            CALL(evt_msg_print, 0, STRING("mac_2_087_01"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("mac_2_085_01"), 0, STRING("me"))
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(marco_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), 77, 51, 110)
        CALL(evt_npc_entry, STRING("第三部下"), STRING("c_zako_n"))
        CALL(evt_npc_set_tribe, STRING("第三部下"), STRING("第三勢力部下"))
        CALL(evt_npc_set_position, STRING("第三部下"), 0, -1000, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(marco_party_move)
    CALL(evt_party_stop, 0)
    CALL(evt_party_force_reset_outofscreen, 0)
    CALL(evt_party_get_pos, 0, LW(10), LW(11), LW(12))
    IF_INT_LESS_OR_EQUAL(LW(10), -110)
        GOTO(10)
    END_IF
    IF_INT_LESS_OR_EQUAL(LW(12), -155)
        GOTO(10)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(11), 77)
        GOTO(40)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(10), 40)
        IF_INT_LESS_OR_EQUAL(LW(10), 140)
            IF_INT_GREATER_OR_EQUAL(LW(12), 180)
                GOTO(30)
            END_IF
        END_IF
    END_IF
    IF_INT_LESS_OR_EQUAL(LW(10), 40)
        IF_INT_GREATER_OR_EQUAL(LW(12), 150)
            GOTO(20)
        END_IF
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(10), 140)
        IF_INT_GREATER_OR_EQUAL(LW(12), 150)
            GOTO(20)
        END_IF
    END_IF
    GOTO(40)
    LABEL(10)
    CALL(evt_party_set_pos, 0, -133, 15, 220)
    LABEL(20)
    CALL(evt_party_move_pos2, 0, 75, 220, FLOAT(120.0f))
    LABEL(30)
    CALL(evt_party_jump_pos, 0, 90, 50, 150, 500, 0, -1000)
    CALL(evt_party_set_dir, 0, 270, 200)
    CALL(evt_party_run, 0)
    RETURN
    LABEL(40)
    CALL(evt_party_move_pos2, 0, 90, 150, FLOAT(120.0f))
    CALL(evt_party_set_dir, 0, 270, 200)
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

static EVT_BEGIN(marco_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(223)
        CALL(evt_party_stop, 0)
        BEGIN_INLINE_EVENT
            CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_LESS_OR_EQUAL(LW(1), 45)
                CALL(evt_party_jump_pos, 0, 75, 15, 166, 500, 0, -1000)
            END_IF
            CALL(evt_party_move_pos2, 0, 120, 135, FLOAT(60.0f))
            CALL(evt_party_set_dir, 0, 270, 200)
        END_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("mac_4_030"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_031"), 0, STRING("me"))
        CALL(evt_msg_print, 0, STRING("mac_4_032"), 0, STRING("ガラの悪い水夫"))
        CALL(evt_cam3d_evt_set, 110, 170, 472, 110, 110, 127, 500, 11)
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_033"), 0, STRING("ガラの悪い水夫"))
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_msg_print, 0, STRING("mac_4_034"), 0, STRING("me"))
        CALL(evt_cam3d_evt_set, 160, 170, 472, 160, 110, 127, 500, 11)
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_035"), 0, STRING("ガラの悪い水夫"))
        CALL(evt_cam3d_evt_set, 60, 170, 472, 60, 110, 127, 500, 11)
        WAIT_MS(800)
        CALL(evt_msg_print, 0, STRING("mac_4_036"), 0, STRING("me"))
        CALL(evt_cam3d_evt_set, 95, 207, 763, 95, 89, 98, 0, 11)
        WAIT_FRAMES(1)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        WAIT_MS(600)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        WAIT_MS(600)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        WAIT_MS(600)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        WAIT_MS(900)
        CALL(evt_msg_print, 0, STRING("mac_4_037"), 0, STRING("me"))
        CALL(evt_npc_set_ry, STRING("me"), 270)
        BEGIN_INLINE_EVENT
            CALL(evt_msg_print, 0, STRING("mac_4_038"), 0, STRING("ガラの悪い水夫"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("mac_4_039"), 0, STRING("オドオド水夫"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(1500)
            CALL(evt_msg_print, 0, STRING("mac_4_041"), 0, STRING("水夫Ｄ"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(2000)
            CALL(evt_msg_togelr, 0)
            CALL(evt_msg_print, 0, STRING("mac_4_043_01"), 0, STRING("店長"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(2500)
            CALL(evt_msg_print, 0, STRING("mac_4_042"), 0, STRING("水夫ボム兵Ｃ"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(3000)
            CALL(evt_msg_print, 0, STRING("mac_4_043"), 0, STRING("めがね水夫"))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(3500)
            CALL(evt_msg_togelr, 0)
            CALL(evt_msg_print, 0, STRING("mac_4_043_02"), 0, STRING("店員"))
        END_INLINE_EVENT
        WAIT_MS(4500)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("mac_4_044"), 0, STRING("me"))
        CALL(evt_cam3d_evt_off, 500, 11)
        CALL(evt_party_run, 0)
        SET_VALUE(GSW(0), 224)
        CASE_EQUAL(224)
        IF_INT_EQUAL(GSWF(1215), 0)
            CALL(evt_msg_print, 0, STRING("mac_4_045"), 0, STRING("me"))
        ELSE
            CALL(evt_msg_print, 0, STRING("mac_4_045_01"), 0, STRING("me"))
        END_IF
        CASE_LESS(227)
        CALL(evt_msg_print, 0, STRING("mac_4_045_02"), 0, STRING("me"))
        CASE_EQUAL(228)
        SPAWN_EVENT(marco_party_move)
        IF_INT_EQUAL(GSWF(1177), 1)
            CALL(evt_msg_print, 0, STRING("mac_4_091"), 0, STRING("me"))
            GOTO(100)
        END_IF
        CALL(evt_msg_print, 0, STRING("mac_4_089"), 0, STRING("me"))
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_MS(500)
        CALL(evt_npc_reverse_ry, STRING("me"))
        WAIT_MS(500)
        CALL(evt_msg_print, 0, STRING("mac_4_090"), 0, STRING("me"))
        SET_VALUE(GSWF(1177), 1)
        LABEL(100)
        CALL(evt_msg_select, 0, STRING("mac_4_090_yesno"))
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_msg_print_add, 0, STRING("mac_4_092"))
            RETURN
        END_IF
        CALL(evt_snd_bgmoff_f, 512, 2000)
        CALL(evt_msg_continue)
        SPAWN_EVENT(shukko_event)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(garawaru_urouro2)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("me"), 450, -200, 0, FLOAT(40.0f), 0)
        WAIT_MS(1200)
        CALL(evt_npc_move_position, STRING("me"), -200, -200, 0, FLOAT(40.0f), 0)
        WAIT_MS(1200)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(garawaru_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    SWITCH(GSW(0))
        CASE_LESS(4)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), -200, 15, -175)
        CASE_EQUAL(217)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_change_interrupt, STRING("me"), 1, 0)
        CALL(evt_npc_restart_regular_event, STRING("me"))
        CASE_BETWEEN(223, 229)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_set_position, STRING("me"), 160, 73, 135)
        CASE_LESS(229)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), -200, 15, -200)
        CASE_DEFAULT
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(garawaru_regl)
    SWITCH(GSW(0))
        CASE_LESS(7)
        IF_INT_EQUAL(GSWF(1179), 1)
            RETURN
        END_IF
        LOOP(0)
            CALL(evt_npc_move_position, STRING("me"), 450, -175, 0, FLOAT(40.0f), 0)
            WAIT_MS(1000)
            CALL(evt_npc_move_position, STRING("me"), -200, -175, 0, FLOAT(40.0f), 0)
            IF_INT_EQUAL(GSWF(1179), 1)
                BREAK_LOOP
            END_IF
            WAIT_MS(1000)
            IF_INT_EQUAL(GSWF(1179), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CASE_LESS(223)
        CALL_CHILD(garawaru_urouro2)
        CASE_BETWEEN(223, 229)
        CASE_GREATER(229)
        CALL_CHILD(garawaru_urouro2)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(garawaru_talk)
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_msg_print, 0, STRING("gor_00_06_01"), 0, STRING("me"))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_00_06_02"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_00_06_03"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_00_06_04"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_00_06_05"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("gor_00_06_06"), 0, STRING("me"))
        CASE_LESS(223)
        CALL(evt_msg_print, 0, STRING("gor_00_06_07"), 0, STRING("me"))
        CASE_LESS(228)
        CALL(evt_msg_print, 0, STRING("gor_00_06_08"), 0, STRING("me"))
        CASE_LESS(229)
        CALL(evt_msg_print, 0, STRING("mac_4_025"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(megane_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_set_position, STRING("me"), 220, 71, 80)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(megane_regl)
    RETURN
EVT_END()

static EVT_BEGIN(megane_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_msg_print, 0, STRING("mac_4_029"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(odoodo_init)
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), -225, 15, -400)
        CASE_EQUAL(217)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), -208, 15, 192)
        CALL(evt_npc_change_interrupt, STRING("me"), 1, 0)
        CALL(evt_npc_restart_regular_event, STRING("me"))
        CASE_BETWEEN(223, 229)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), 34, 77, 40)
        CASE_DEFAULT
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), -200, 15, -400)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(odoodo_regl)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CASE_EQUAL(268)
        CASE_GREATER_OR_EQUAL(23)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
        CALL(urouro_main_func, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(odoodo_talk)
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_msg_print, 0, STRING("gor_00_05_01"), 0, STRING("me"))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_00_05_02"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_00_05_03"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_00_05_04"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_00_05_05"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("gor_00_05_06"), 0, STRING("me"))
        CASE_LESS(223)
        CALL(evt_msg_print, 0, STRING("gor_00_05_07"), 0, STRING("me"))
        CASE_LESS(228)
        CALL(evt_msg_print, 0, STRING("gor_00_05_08"), 0, STRING("me"))
        CASE_LESS(229)
        CALL(evt_msg_print, 0, STRING("mac_4_024"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_00_05_09"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_00_05_10"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_00_05_11"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_00_05_12"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(odoodo_ani_urouro)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("me"), 450, -200, 0, FLOAT(40.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_reverse_ry, STRING("me"))
        CALL(evt_npc_move_position, STRING("me"), -200, -200, 0, FLOAT(40.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_reverse_ry, STRING("me"))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(odoodo_ani_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(269)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), -200, 15, -200)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(odoodo_ani_regl)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(268)
        CALL_CHILD(odoodo_ani_urouro)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(odoodo_ani_talk)
    SWITCH(GSW(0))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_00_07_09"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_00_07_10"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_00_07_11"), 0, STRING("me"))
        CASE_GREATER_OR_EQUAL(401)
        CALL(evt_msg_print, 0, STRING("gor_00_07_12"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_a_init)
    SWITCH(GSW(0))
        CASE_EQUAL(217)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_change_interrupt, STRING("me"), 1, 0)
        CALL(evt_npc_restart_regular_event, STRING("me"))
        CASE_DEFAULT
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), -215, 15, 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifu_a_regl)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CASE_EQUAL(268)
        CASE_DEFAULT
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
        CALL(urouro_main_func, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_a_talk)
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_msg_print, 0, STRING("gor_00_03_01"), 0, STRING("me"))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_00_03_02"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_00_03_03"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_00_03_04"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_00_03_05"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("gor_00_03_06"), 0, STRING("me"))
        CASE_LESS(223)
        CALL(evt_msg_print, 0, STRING("gor_00_03_07"), 0, STRING("me"))
        CASE_LESS(224)
        CALL(evt_msg_print, 0, STRING("gor_00_03_08"), 0, STRING("me"))
        CASE_LESS(228)
        CALL(evt_msg_print, 0, STRING("gor_00_03_08_01"), 0, STRING("me"))
        CASE_LESS(229)
        CALL(evt_msg_print, 0, STRING("gor_00_03_08_02"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_00_03_09"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_00_03_10"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_00_03_11"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_00_03_12"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifu_b_init)
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CASE_EQUAL(217)
        CALL(evt_npc_set_position, STRING("me"), -191, 15, 36)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_change_interrupt, STRING("me"), 1, 0)
        CALL(evt_npc_restart_regular_event, STRING("me"))
        CASE_DEFAULT
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), 0, 15, -300)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifu_b_regl)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CASE_EQUAL(268)
        CASE_GREATER_OR_EQUAL(23)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
        CALL(urouro_main_func, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifu_b_talk)
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_msg_print, 0, STRING("gor_00_04_01"), 0, STRING("me"))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_00_04_02"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_00_04_03"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_00_04_04"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_00_04_05"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("gor_00_04_06"), 0, STRING("me"))
        CASE_LESS(223)
        CALL(evt_msg_print, 0, STRING("gor_00_04_07"), 0, STRING("me"))
        CASE_LESS(224)
        CALL(evt_msg_print, 0, STRING("gor_00_04_08"), 0, STRING("me"))
        CASE_LESS(228)
        CALL(evt_msg_print, 0, STRING("gor_00_04_08_01"), 0, STRING("me"))
        CASE_LESS(229)
        CALL(evt_msg_print, 0, STRING("gor_00_04_08_02"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_00_04_09"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_00_04_10"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_00_04_11"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_00_04_12"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_d_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_npc_set_position, STRING("me"), 125, 49, 20)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_d_talk)
    CALL(evt_msg_print, 0, STRING("mac_4_027"), 0, STRING("me"))
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_a_urouro2)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("me"), -215, 215, 0, FLOAT(40.0f), 0)
        WAIT_MS(800)
        CALL(evt_npc_reverse_ry, STRING("me"))
        CALL(evt_npc_move_position, STRING("me"), -215, -150, 0, FLOAT(40.0f), 0)
        WAIT_MS(800)
        CALL(evt_npc_reverse_ry, STRING("me"))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_a_init)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), -215, 15, 175)
        CASE_EQUAL(217)
        CALL(evt_npc_set_position, STRING("me"), -160, 15, 207)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_change_interrupt, STRING("me"), 1, 0)
        CALL(evt_npc_restart_regular_event, STRING("me"))
        CASE_DEFAULT
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_set_position, STRING("me"), -215, 15, -150)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_a_regl)
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
        CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
        CALL(urouro_main_func, STRING("me"))
        CASE_BETWEEN(223, 229)
        CASE_DEFAULT
        CALL_CHILD(suifubomb_a_urouro2)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_a_talk)
    IF_INT_EQUAL(GSW(1420), 6)
        IF_INT_EQUAL(GSWF(5405), 0)
            CALL(evt_msg_print, 0, STRING("irai_06_001"), 0, STRING("me"))
            SET_VALUE(GSWF(5405), 1)
        ELSE
            CALL(evt_msg_print, 0, STRING("irai_06_002"), 0, STRING("me"))
            CALL(make_itemsel_table, LW(11))
            IF_INT_EQUAL(LW(11), 0)
                CALL(evt_msg_print_add, 0, STRING("irai_06_006"))
                CALL(delete_itemsel_table)
                RETURN
            END_IF
            CALL(evt_msg_continue)
            CALL(evt_win_item_select, 0, LW(10), LW(0), 0)
            CALL(delete_itemsel_table)
            IF_INT_EQUAL(LW(0), -1)
                CALL(evt_msg_print, 0, STRING("irai_06_006"), 0, STRING("me"))
                RETURN
            END_IF
            CALL(evt_pouch_remove_item, LW(0), LW(1))
            CALL(evt_msg_print, 0, STRING("irai_06_003"), 0, STRING("me"))
            CALL(evt_sub_get_coin, 11)
            CALL(evt_msg_print, 0, STRING("irai_06_005"), 0, STRING("me"))
            SET_VALUE(GSW(1420), 0)
            SET_VALUE(GSWF(5380), 1)
            CALL(evt_msg_print, 0, STRING("sys_request_finish"), 0, 0)
        END_IF
        RETURN
    END_IF
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_msg_print, 0, STRING("gor_00_01_01"), 0, STRING("me"))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_00_01_02"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_00_01_03"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_00_01_04"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_00_01_05"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("gor_00_01_06"), 0, STRING("me"))
        CASE_LESS(223)
        CALL(evt_mario_dispflag_onoff, 1, 2)
        CALL(evt_msg_print, 0, STRING("gor_00_01_07"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(1000)
        CALL(evt_msg_print, 0, STRING("gor_00_01_07_01"), 0, STRING("me"))
        CALL(evt_mario_dispflag_onoff, 0, 2)
        CASE_LESS(224)
        CALL(evt_msg_print, 0, STRING("gor_00_01_08"), 0, STRING("me"))
        CASE_LESS(228)
        CALL(evt_msg_print, 0, STRING("gor_00_01_08_01"), 0, STRING("me"))
        CASE_LESS(229)
        CALL(evt_msg_print, 0, STRING("gor_00_01_08_02"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_00_01_09"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_00_01_10"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_00_01_11"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_00_01_12"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_b_urouro2)
    LOOP(0)
        CALL(evt_npc_move_position, STRING("me"), 175, -350, 0, FLOAT(40.0f), 0)
        WAIT_MS(1000)
        CALL(evt_npc_move_position, STRING("me"), -250, -350, 0, FLOAT(40.0f), 0)
        WAIT_MS(1000)
    END_LOOP
    WAIT_WHILE(1)
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_b_init)
    CALL(evt_npc_set_position, STRING("me"), -250, 15, -350)
    CALL(evt_npc_set_ry, STRING("me"), 90)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    IF_INT_EQUAL(GSW(0), 217)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CALL(evt_npc_change_interrupt, STRING("me"), 1, 0)
        CALL(evt_npc_restart_regular_event, STRING("me"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_b_regl)
    SWITCH(GSW(0))
        CASE_LESS(7)
        IF_INT_EQUAL(GSWF(1179), 1)
            RETURN
        END_IF
        LOOP(0)
            CALL(evt_npc_move_position, STRING("me"), 0, -350, 0, FLOAT(40.0f), 0)
            WAIT_MS(1000)
            CALL(evt_npc_move_position, STRING("me"), -250, -350, 0, FLOAT(40.0f), 0)
            IF_INT_EQUAL(GSWF(1179), 1)
                BREAK_LOOP
            END_IF
            WAIT_MS(1000)
            IF_INT_EQUAL(GSWF(1179), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_npc_set_ry, STRING("me"), 90)
        CASE_LESS(223)
        CALL_CHILD(suifubomb_b_urouro2)
        CASE_BETWEEN(223, 229)
        CASE_GREATER(229)
        CALL_CHILD(suifubomb_b_urouro2)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_b_talk)
    SWITCH(GSW(0))
        CASE_LESS(7)
        CALL(evt_msg_print, 0, STRING("gor_00_02_01"), 0, STRING("me"))
        CASE_LESS(56)
        CALL(evt_msg_print, 0, STRING("gor_00_02_02"), 0, STRING("me"))
        CASE_LESS(112)
        CALL(evt_msg_print, 0, STRING("gor_00_02_03"), 0, STRING("me"))
        CASE_LESS(164)
        CALL(evt_msg_print, 0, STRING("gor_00_02_04"), 0, STRING("me"))
        CASE_LESS(215)
        CALL(evt_msg_print, 0, STRING("gor_00_02_05"), 0, STRING("me"))
        CASE_LESS(222)
        CALL(evt_msg_print, 0, STRING("gor_00_02_06"), 0, STRING("me"))
        CASE_LESS(223)
        CALL(evt_msg_print, 0, STRING("gor_00_02_07"), 0, STRING("me"))
        CASE_LESS(224)
        CALL(evt_msg_print, 0, STRING("gor_00_02_08"), 0, STRING("me"))
        CASE_LESS(228)
        CALL(evt_msg_print, 0, STRING("gor_00_02_08_01"), 0, STRING("me"))
        CASE_LESS(229)
        CALL(evt_msg_print, 0, STRING("gor_00_02_08_02"), 0, STRING("me"))
        CASE_LESS(339)
        CALL(evt_msg_print, 0, STRING("gor_00_02_09"), 0, STRING("me"))
        CASE_LESS(373)
        CALL(evt_msg_print, 0, STRING("gor_00_02_10"), 0, STRING("me"))
        CASE_LESS(401)
        CALL(evt_msg_print, 0, STRING("gor_00_02_11"), 0, STRING("me"))
        CASE_DEFAULT
        CALL(evt_msg_print, 0, STRING("gor_00_02_12"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_c_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_npc_set_position, STRING("me"), 140, 15, 200)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(suifubomb_c_talk)
    CALL(evt_msg_print, 0, STRING("mac_4_028"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(sanders_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_npc_entry, STRING("サンダース"), STRING("c_bomt_n"))
        CALL(evt_npc_set_tribe, STRING("サンダース"), STRING("サンダース"))
        CALL(evt_npc_set_ry, STRING("サンダース"), 90)
        CALL(evt_npc_change_interrupt, STRING("サンダース"), 6, PTR(sanders_talk))
        IF_INT_EQUAL(GSW(0), 228)
            CALL(evt_npc_set_position, STRING("サンダース"), 100, 50, 80)
        ELSE
            CALL(evt_npc_set_position, STRING("サンダース"), 0, -1000, 0)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(sanders_talk)
    CALL(evt_msg_print, 0, STRING("mac_4_088"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(korutesu_init)
    SWITCH(GSW(0))
        CASE_EQUAL(404)
        CASE_GREATER_OR_EQUAL(268)
        CALL(evt_npc_set_ry, STRING("me"), 270)
        CALL(evt_npc_set_position, STRING("me"), 210, 40, 92)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(korutesu_talk)
    CALL(evt_msg_print, 0, STRING("gor_00_025"), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("gor_00_025_yesno"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("gor_00_027"))
        SET_VALUE(GSWF(3125), 1)
        SET_VALUE(GF(13), 1)
        CALL_CHILD(gor_muj_ship_start)
    ELSE
        CALL(evt_msg_print_add, 0, STRING("gor_00_026"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(tentyou_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_npc_entry, STRING("店長"), STRING("c_gshop_m"))
        CALL(evt_npc_set_tribe, STRING("店長"), STRING("店員男"))
        CALL(evt_npc_set_ry, STRING("店長"), 90)
        CALL(evt_npc_set_position, STRING("店長"), -12, 128, 40)
        CALL(evt_npc_change_interrupt, STRING("店長"), 6, PTR(tentyou_talk))
        CALL(evt_npc_flag_onoff, 1, STRING("店長"), 1536)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(tentyou_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_msg_print, 0, STRING("mac_4_029_01"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(tenin_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_npc_entry, STRING("店員"), STRING("c_gshop_w"))
        CALL(evt_npc_set_tribe, STRING("店員"), STRING("店員女"))
        CALL(evt_npc_set_ry, STRING("店員"), 90)
        CALL(evt_npc_set_position, STRING("店員"), -30, 128, 120)
        CALL(evt_npc_change_interrupt, STRING("店員"), 6, PTR(tenin_talk))
        CALL(evt_npc_flag_onoff, 1, STRING("店員"), 1536)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(tenin_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(223, 229)
        CALL(evt_msg_print, 0, STRING("mac_4_029_02"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(borodo2_init)
    IF_INT_EQUAL(GSW(1420), 4)
        IF_INT_EQUAL(GSWF(5405), 1)
            IF_INT_EQUAL(GSWF(5406), 1)
                IF_INT_EQUAL(GSWF(5407), 0)
                    CALL(evt_npc_entry, STRING("ボロドー２"), STRING("c_borodo_n"))
                    CALL(evt_npc_set_tribe, STRING("ボロドー２"), STRING("盗賊"))
                    CALL(evt_npc_flag_onoff, 1, STRING("ボロドー２"), 1536)
                    CALL(evt_npc_set_position, STRING("ボロドー２"), -168, 15, -540)
                    CALL(evt_npc_change_interrupt, STRING("ボロドー２"), 6, PTR(borodo2_talk))
                END_IF
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(borodo2_escape)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("me"), 1073741856)
    CALL(evt_npc_set_ry, STRING("me"), 90)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("me"), -168, -420, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_move_position, STRING("me"), 60, -420, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(1500)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(borodo2_talk)
    CALL(evt_msg_print, 0, STRING("irai_04_004"), 0, STRING("me"))
    SET_VALUE(GSWF(5407), 1)
    SPAWN_EVENT(borodo2_escape)
    RETURN
EVT_END()

EVT_BEGIN(mokorim_init)
    CALL_CHILD(npc_init_evt)
    SWITCH(GSW(0))
        CASE_BETWEEN(18, 177)
        CALL(evt_npc_set_position, STRING("me"), 525, 15, -270)
        CASE_GREATER_OR_EQUAL(345)
        CALL(evt_npc_set_position, STRING("me"), 525, 15, -270)
    END_SWITCH
    CALL(evt_npc_set_ry, STRING("me"), 270)
    RETURN
EVT_END()

EVT_BEGIN(mokorim_talk)
    SET_VALUE(LF(15), 0)
    SWITCH(GSW(0))
        CASE_BETWEEN(18, 177)
        IF_INT_EQUAL(GSW(116), 3)
            CALL(evt_msg_print, 0, STRING("salari_before_010"), 0, 0)
            RETURN
        END_IF
        CALL(evt_cam3d_evt_set, 500, 120, 90, 500, 55, -280, 800, 11)
        WAIT_MS(800)
        IF_INT_EQUAL(GSWF(1182), 0)
            SET_VALUE(GSWF(1182), 1)
            CALL(evt_snd_bgmoff, 1024)
            CALL(evt_snd_bgmon, 1, STRING("BGM_EVT_DANGER3"))
            SET_VALUE(LF(15), 1)
            CALL(evt_msg_print, 0, STRING("salari_before_000"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_N_2"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print, 0, STRING("salari_before_001"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_N_1"))
            WAIT_MS(500)
            CALL(evt_msg_print, 0, STRING("salari_before_002"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_I_Z"))
            WAIT_MS(500)
        ELSE
            CALL(evt_msg_print, 0, STRING("salari_before_003"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_I_Z"))
            WAIT_MS(500)
        END_IF
        CALL(evt_unitwin_alloc, 0)
        CALL(unitwincoin_disp_onoff, 1)
        CALL(evt_msg_print, 0, STRING("salari_before_004"), 0, 0)
        CALL(evt_msg_select, 0, STRING("salari_before_005"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_pouch_get_coin, LW(0))
            IF_INT_GREATER_OR_EQUAL(LW(0), 100)
                CALL(evt_msg_continue)
                CALL(evt_mario_set_pose, STRING("M_I_2"))
                WAIT_MS(500)
                CALL(evt_mario_set_pose, STRING("M_S_1"))
                CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
                LOOP(2)
                    CALL(evt_npc_jump_position_nohit, STRING("me"), LW(0), LW(1), LW(2), 200, 10)
                    WAIT_FRAMES(1)
                END_LOOP
                ADD(GSW(116), 1)
                IF_INT_EQUAL(GSW(116), 3)
                    CALL(evt_msg_print, 0, STRING("salari_before_007"), 0, 0)
                ELSE
                    CALL(evt_msg_print, 0, STRING("salari_before_006"), 0, 0)
                END_IF
                CALL(evt_pouch_add_coin, -100)
                CALL(unitwincoin_wait)
            ELSE
                CALL(evt_mario_set_pose, STRING("M_S_1"))
                CALL(evt_msg_print_add, 0, STRING("salari_before_009"))
            END_IF
        ELSE
            CALL(evt_msg_continue)
            CALL(evt_mario_set_pose, STRING("M_N_B"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_msg_print, 0, STRING("salari_before_008"), 0, 0)
        END_IF
        CALL(unitwincoin_disp_onoff, 0)
        CALL(evt_unitwin_free)
        CALL(evt_cam3d_evt_off, 500, 11)
        WAIT_MS(500)
        CASE_GREATER_OR_EQUAL(345)
        IF_INT_EQUAL(GSWF(1183), 0)
            SET_VALUE(GSWF(1183), 1)
            CALL(evt_cam3d_evt_set, 500, 120, 90, 500, 55, -280, 800, 11)
            WAIT_MS(800)
            CALL(evt_msg_print, 0, STRING("salari_after_000"), 0, 0)
            CALL(evt_mario_set_pose, STRING("M_N_2"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_cam3d_evt_set, 515, 105, 5, 515, 55, -280, 800, 11)
            WAIT_MS(800)
            CALL(evt_msg_print, 0, STRING("salari_after_000_1"), 0, 0)
            CALL(evt_cam3d_evt_set, 515, 90, -70, 515, 55, -280, 800, 11)
            WAIT_MS(800)
            CALL(evt_msg_print, 0, STRING("salari_after_000_2"), 0, 0)
            CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
            LOOP(2)
                CALL(evt_npc_jump_position_nohit, STRING("me"), LW(0), LW(1), LW(2), 200, 10)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_msg_print, 0, STRING("salari_after_001"), 0, 0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_mario_set_pose, STRING("M_N_7"))
            WAIT_MS(500)
            CALL(evt_mario_set_pose, STRING("M_S_1"))
            CALL(evt_cam3d_evt_set, 500, 120, 90, 500, 55, -280, 800, 11)
            WAIT_MS(800)
            IF_INT_GREATER(GSW(116), 0)
                CALL(evt_msg_print, 0, STRING("salari_after_002"), 0, 0)
                SWITCH(GSW(116))
                    CASE_EQUAL(1)
                    SET_VALUE(LW(0), 300)
                    CASE_EQUAL(2)
                    SET_VALUE(LW(0), 500)
                    CASE_EQUAL(3)
                    SET_VALUE(LW(0), 999)
                END_SWITCH
                CALL(evt_unitwin_alloc, 0)
                CALL(unitwincoin_disp_onoff, 1)
                CALL(evt_msg_print_insert, 0, STRING("salari_after_003"), 0, 0, LW(0))
                CALL(evt_pouch_add_coin, LW(0))
                CALL(unitwincoin_wait)
                CALL(unitwincoin_disp_onoff, 0)
                CALL(evt_unitwin_free)
                CALL(evt_msg_print, 0, STRING("salari_after_004"), 0, 0)
            ELSE
                CALL(evt_msg_print, 0, STRING("salari_after_005"), 0, 0)
            END_IF
            CALL(evt_cam3d_evt_off, 500, 11)
        ELSE
            CALL(evt_msg_print, 0, STRING("salari_after_006"), 0, 0)
            CALL(evt_msg_select, 0, STRING("salari_after_006_1"))
            IF_INT_EQUAL(LW(0), 0)
                LOOP(0)
                    CALL(evt_msg_print_add, 0, STRING("salari_after_007"))
                    CALL(evt_msg_select, 0, STRING("salari_after_008"))
                    CALL(evt_msg_continue)
                    CALL(evt_npc_flag_onoff, 0, STRING("me"), 512)
                    SWITCH(LW(0))
                        CASE_EQUAL(0)
                        CALL(evt_msg_print, 0, STRING("salari_after_009"), 0, 0)
                        CASE_EQUAL(1)
                        CALL(evt_msg_print, 0, STRING("salari_after_010"), 0, 0)
                        CASE_EQUAL(2)
                        CALL(evt_msg_print, 0, STRING("salari_after_011"), 0, 0)
                        CASE_EQUAL(3)
                        CALL(evt_msg_print, 0, STRING("salari_after_012"), 0, 0)
                        CASE_EQUAL(4)
                        CALL(evt_msg_print, 0, STRING("salari_after_013"), 0, 0)
                        CASE_EQUAL(5)
                        CALL(evt_msg_print, 0, STRING("salari_after_014"), 0, 0)
                        CASE_EQUAL(6)
                        CALL(evt_msg_print, 0, STRING("salari_after_015"), 0, 0)
                    END_SWITCH
                    CALL(evt_npc_flag_onoff, 1, STRING("me"), 512)
                    CALL(evt_msg_print, 0, STRING("salari_after_016"), 0, 0)
                    CALL(evt_msg_select, 0, STRING("salari_after_017"))
                    IF_INT_EQUAL(LW(0), 0)
                    ELSE
                        CALL(evt_msg_print_add, 0, STRING("salari_after_018"))
                        BREAK_LOOP
                    END_IF
                END_LOOP
            ELSE
                CALL(evt_msg_print_add, 0, STRING("salari_after_018"))
            END_IF
        END_IF
    END_SWITCH
    IF_INT_EQUAL(LF(15), 1)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(baba_init)
    CALL_CHILD(npc_init_evt)
    CALL(evt_npc_status_onoff, 1, STRING("me"), 2)
    RETURN
EVT_END()

static EVT_BEGIN(luigi_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(261, 338)
        CALL(evt_npc_set_position, STRING("me"), 420, 15, -210)
        CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(luigi_talk)
    CALL_CHILD(luigi_gor_first_talk)
    RETURN
EVT_END()

static NpcSetupInfo luigiEnt[2] = {
    {"ルイージ", 0x00080600, 0, luigi_init, NULL, luigi_talk},
    {NULL},
};

static EVT_BEGIN(luigi_npcEnt)
    SWITCH(GSW(0))
        CASE_BETWEEN(261, 338)
        CALL(evt_npc_entry, STRING("ルイージ"), STRING("c_luigi"))
        CALL(evt_npc_set_tribe, STRING("ルイージ"), STRING("ルイージ"))
        CALL(evt_npc_setup, PTR(luigiEnt))
        CALL(evt_npc_entry, STRING("ラクガン"), STRING("c_lp_pansy"))
        CALL(evt_npc_set_tribe, STRING("ラクガン"), STRING("ラクガン"))
        CALL(evt_npc_setup, PTR(rakganEnt))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(rakgan_init)
    CALL(evt_npc_set_position, STRING("me"), 460, 15, -210)
    CALL(evt_npc_set_ry_lr, STRING("me"), 0)
    RETURN
EVT_END()

EVT_BEGIN(rakgan_talk)
    CALL(evt_msg_print, 0, STRING("luigi_npc_04"), 0, STRING("me"))
    RETURN
EVT_END()

NpcSetupInfo rakganEnt[2] = {
    {"ラクガン", 0x00080600, 0, rakgan_init, NULL, rakgan_talk},
    {NULL},
};

static NpcSetupInfo npcEnt[17] = {
    {"モニー", 0x00080600, 0, mony_init, gor_00_mony_regl, mony_talk},
    {"ピートン", 0x00080600, 0, peton_init, gor_00_peton_regl, peton_talk},
    {"マルコ", 0x00080600, 0, marco_init, NULL, marco_talk},
    {"ガラの悪い水夫", 0x00080600, 0, garawaru_init, garawaru_regl, garawaru_talk},
    {"めがね水夫", 0x00080600, 0, megane_init, megane_regl, megane_talk},
    {"オドオド水夫", 0x00080600, 0, odoodo_init, odoodo_regl, odoodo_talk},
    {"オドオド水夫兄", 0x00080600, 0, odoodo_ani_init, odoodo_ani_regl, odoodo_ani_talk},
    {"水夫Ａ", 0x00080600, 0, suifu_a_init, suifu_a_regl, suifu_a_talk},
    {"水夫Ｂ", 0x00080600, 0, suifu_b_init, suifu_b_regl, suifu_b_talk},
    {"水夫Ｄ", 0x00080600, 0, suifu_d_init, NULL, suifu_d_talk},
    {"水夫ボム兵Ａ", 0x00080600, 0, suifubomb_a_init, suifubomb_a_regl, suifubomb_a_talk},
    {"水夫ボム兵Ｂ", 0x00080600, 0, suifubomb_b_init, suifubomb_b_regl, suifubomb_b_talk},
    {"水夫ボム兵Ｃ", 0x00080600, 0, suifubomb_c_init, NULL, suifubomb_c_talk},
    {"コルテス", 0x00000600, 0, korutesu_init, NULL, korutesu_talk},
    {"モコリム", 0x00080600, 0, mokorim_init, NULL, mokorim_talk},
    {"ババ", 0, 0, baba_init},
    {NULL},
};

EVT_BEGIN(ship_a_kaji_animation)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, 5, -5, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(0), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, -5, 5, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(0), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(ship_a_wakka_animation)
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    LOOP(0)
        IF_INT_EQUAL(GF(4), 0)
            IF_INT_EQUAL(GF(0), 0)
                ADD_FLOAT(LW(0), FLOAT(0.1f))
                IF_INT_GREATER_OR_EQUAL(LW(0), FLOAT(10.0f))
                    SET_FLOAT_VALUE(LW(0), FLOAT(10.0f))
                END_IF
            ELSE
                SUBTRACT_FLOAT(LW(0), FLOAT(0.07f))
                IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
                END_IF
            END_IF
            IF_INT_GREATER(GW(1), FLOAT(360.0f))
                SUBTRACT_FLOAT(GW(1), FLOAT(360.0f))
            END_IF
            IF_INT_LESS(GW(1), FLOAT(0.0f))
                ADD_FLOAT(GW(1), FLOAT(360.0f))
            END_IF
            ADD_FLOAT(GW(1), LW(0))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(ship_a_entotsu_animation)
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    LOOP(0)
        IF_INT_EQUAL(GF(4), 0)
            IF_INT_EQUAL(GF(0), 0)
                ADD_FLOAT(LW(0), FLOAT(0.1f))
                IF_INT_GREATER_OR_EQUAL(LW(0), FLOAT(10.0f))
                    SET_FLOAT_VALUE(LW(0), FLOAT(10.0f))
                END_IF
            ELSE
                SUBTRACT_FLOAT(LW(0), FLOAT(0.07f))
                IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
                END_IF
            END_IF
            IF_INT_GREATER(GW(2), FLOAT(360.0f))
                SUBTRACT_FLOAT(GW(2), FLOAT(360.0f))
            END_IF
            IF_INT_LESS(GW(2), FLOAT(0.0f))
                ADD_FLOAT(GW(2), FLOAT(360.0f))
            END_IF
            ADD_FLOAT(GW(2), LW(0))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(ship_a_neji_animation)
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    LOOP(0)
        IF_INT_EQUAL(GF(4), 0)
            IF_INT_EQUAL(GF(0), 0)
                ADD_FLOAT(LW(0), FLOAT(0.1f))
                IF_INT_GREATER_OR_EQUAL(LW(0), FLOAT(10.0f))
                    SET_FLOAT_VALUE(LW(0), FLOAT(10.0f))
                END_IF
            ELSE
                SUBTRACT_FLOAT(LW(0), FLOAT(0.07f))
                IF_INT_LESS_OR_EQUAL(LW(0), FLOAT(0.0f))
                    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
                END_IF
            END_IF
            IF_INT_GREATER(GW(3), FLOAT(360.0f))
                SUBTRACT_FLOAT(GW(3), FLOAT(360.0f))
            END_IF
            IF_INT_LESS(GW(3), FLOAT(0.0f))
                ADD_FLOAT(GW(3), FLOAT(360.0f))
            END_IF
            ADD_FLOAT(GW(3), LW(0))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(ship_a_yure_animation)
    CALL(fune_yure_func_init, FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(6), GW(6))
        WAIT_FRAMES(1)
        LOOP(0)
            IF_INT_EQUAL(GF(4), 0)
                CALL(fune_yure_func, 0, LW(6), GW(5), LW(10))
                IF_INT_EQUAL(LW(10), 1)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_FLOAT_VALUE(LW(6), GW(6))
        LOOP(0)
            IF_INT_EQUAL(GF(4), 0)
                CALL(fune_yure_func, 1, LW(6), GW(5), LW(10))
                IF_INT_EQUAL(LW(10), 1)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(ship_a_animation)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_fune_a"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_a_hikinami"), 1)
    CALL(evt_hitobj_onoff, STRING("A_fune_a"), 1, 1)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_a_hikinami"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_a_hikinami"), 256)
    CALL(evt_hit_bind_mapobj, STRING("A_fune_a"), STRING("S_fune_a"))
    SPAWN_EVENT_GET_ID(PTR(ship_a_kaji_animation), LW(10))
    SPAWN_EVENT_GET_ID(PTR(ship_a_wakka_animation), LW(11))
    SPAWN_EVENT_GET_ID(PTR(ship_a_entotsu_animation), LW(12))
    SPAWN_EVENT_GET_ID(PTR(ship_a_neji_animation), LW(13))
    SPAWN_EVENT_GET_ID(PTR(ship_a_yure_animation), LW(14))
    LOOP(0)
        CALL(evt_mapobj_trans, 1, STRING("S_fune_a"), GW(4), GW(8), 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_fune_a"), 0, GW(7), GW(5))
        IF_INT_EQUAL(GF(5), 1)
            SET_FLOAT_VALUE(GW(11), GW(4))
        END_IF
        CALL(evt_mapobj_trans, 1, STRING("S_a_hikinami"), GW(11), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_a_hikinami"), 0, GW(7), 0)
        CALL(evt_hit_bind_update, STRING("A_fune_a"))
        CALL(evt_mapobj_rotate, 1, STRING("S_f_drin"), GW(0), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_f_wa_l"), 0, 0, GW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_f_wa_r"), 0, 0, GW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_f_en01"), 0, GW(2), 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_skr"), GW(3), 0, 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(ship_pera)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_kaiten"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_fune_a"), STRING("offscreen"))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_alloc_capture, STRING("capture_img"), 1, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_set_virtual_point, STRING("capture_img"), 0, 0, 375)
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("Z_1"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    CALL(evt_snd_sfxon, STRING("SFX_OFF_SHIP_REVERSE1"), 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_a"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a_hikinami"), 1)
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_fune_a"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_a_hikinami"), 1)
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_fune_a"))
    SET_VALUE(GW(7), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    RETURN
EVT_END()

EVT_BEGIN(enter_gorotsuki_town_init)
    CALL(evt_npc_entry, STRING("クリスチーヌ"), STRING("c_pkr"))
    CALL(evt_npc_set_tribe, STRING("クリスチーヌ"), STRING("クリスチーヌ"))
    CALL(evt_npc_entry, STRING("第三勢力幹部"), STRING("c_kanbu_ibt"))
    CALL(evt_npc_set_tribe, STRING("第三勢力幹部"), STRING("第三勢力幹部"))
    CALL(evt_npc_entry, STRING("第三勢力部下Ａ"), STRING("c_zako_n"))
    CALL(evt_npc_set_tribe, STRING("第三勢力部下Ａ"), STRING("第三勢力部下"))
    CALL(evt_npc_entry, STRING("第三勢力部下Ｂ"), STRING("c_zako_n"))
    CALL(evt_npc_set_tribe, STRING("第三勢力部下Ｂ"), STRING("第三勢力部下"))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(christine_osoware_setup)
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 175, 15, -300)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1073741856)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), 225, 15, -300)
    CALL(evt_npc_set_ry, STRING("第三勢力幹部"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1073741856)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), 255, 15, -330)
    CALL(evt_npc_set_ry, STRING("第三勢力部下Ａ"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ａ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ａ"), 1073741856)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), 280, 15, -330)
    CALL(evt_npc_set_ry, STRING("第三勢力部下Ｂ"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ｂ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ｂ"), 1073741856)
    RETURN
EVT_END()

EVT_BEGIN(enter_gorotsuki_town)
    CALL(evt_snd_bgm_scope, 0, 1)
    SET_TYPE(0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 0, 700, 1420, 0, -10, 0, 0, 11)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_VALUE(GF(0), 0)
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_VALUE(GF(1), 0)
    SET_FLOAT_VALUE(GW(2), FLOAT(0.0f))
    SET_VALUE(GF(2), 0)
    SET_FLOAT_VALUE(GW(3), FLOAT(0.0f))
    SET_VALUE(GF(3), 0)
    SET_FLOAT_VALUE(GW(4), FLOAT(-600.0f))
    SET_FLOAT_VALUE(GW(11), GW(4))
    SET_VALUE(GF(4), 0)
    SET_VALUE(GW(7), 180)
    SET_VALUE(GF(5), 1)
    SPAWN_EVENT(ship_a_animation)
    WAIT_MS(3000)
    SET_VALUE(GF(10), 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(GW(6), FLOAT(5.0f))
        CALL(evt_sub_intpl_msec_init, 0, GW(4), -270, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(0), 1)
        SET_VALUE(GF(1), 1)
        SET_VALUE(GF(2), 1)
        SET_VALUE(GF(3), 1)
        SET_VALUE(GW(6), 2)
        SET_VALUE(GW(6), FLOAT(3.0f))
        CALL(evt_sub_intpl_msec_init, 4, -270, -75, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(10), 0)
        SET_VALUE(GW(6), FLOAT(1.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_MOVE1"), LW(0), LW(1), LW(2), LW(10))
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(10), LW(0), LW(1), LW(2))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 2, 225, 1015, 2, 50, 243, 3000, 11)
    WAIT_MS(3000)
    WAIT_WHILE(GF(10))
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mario_set_pos, 0, 15, 225)
    CALL(evt_mario_set_dir, 90, 0, 0)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_0_014"), 0, 0)
    WAIT_MS(2000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_0_015"), 0, 0)
    WAIT_MS(1000)
    SET_VALUE(GF(4), 1)
    DEBUG_PUT_MSG(STRING("アニメ停止"))
    SET_VALUE(GF(10), 1)
    CALL(evt_npc_set_position, STRING("船ダミー"), 0, GW(8), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL_CHILD(ship_pera)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("船ダミー"), LW(0), LW(1), LW(2))
            SET_FLOAT_VALUE(GW(8), LW(1))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_JUMP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_jump_position_nohit, STRING("船ダミー"), 0, GW(8), 0, 1800, 100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_LANDING1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(GF(10), 0)
    DEBUG_PUT_MSG(STRING("アニメ開始"))
    SET_VALUE(GF(4), 0)
    WAIT_MS(2000)
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    SET_VALUE(GF(2), 0)
    SET_VALUE(GF(3), 0)
    SET_VALUE(GW(6), FLOAT(3.0f))
    SET_VALUE(GF(10), 1)
    WAIT_FRAMES(1)
    SET_VALUE(GF(5), 0)
    SET_FLOAT_VALUE(GW(11), GW(4))
    ADD_FLOAT(GW(11), FLOAT(-400.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, -75, -270, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GW(6), FLOAT(5.0f))
        CALL(evt_sub_intpl_msec_init, 0, -270, -1260, 6000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(10), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_a"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a_hikinami"), 1)
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_MOVE1"), LW(0), LW(1), LW(2), LW(9))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(9), LW(0), LW(1), LW(2))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(9))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(6000)
        CALL(evt_sub_intpl_msec_init, 0, 100, 0, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_snd_sfx_vol, LW(9), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_map_set_playrate, STRING("hiki_nami"), FLOAT(0.25f))
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 1500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(3500)
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 1500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_off, 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_telop_entry, 0)
    END_INLINE_EVENT
    SET_VALUE(GSW(0), 3)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(LW(0), 0)
    CALL(evt_snd_bgm_scope, 0, 0)
    SPAWN_EVENT(christine_osoware)
    RETURN
EVT_END()

EVT_BEGIN(christine_osoware)
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_mario_set_pos, 0, 15, 250)
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_seq_wait, 2)
        CALL(evt_mario_cont_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
    END_IF
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(2), -100)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(1179), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 4194304)
    CALL(evt_npc_sound_data_set, STRING("ガラの悪い水夫"), 0, 0, 0, 0, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫ボム兵Ｂ"), 4194304)
    CALL(evt_npc_sound_data_set, STRING("水夫ボム兵Ｂ"), 0, 0, 0, 0, 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
    CALL(evt_snd_envoff, 512)
    CALL_CHILD(christine_osoware_setup)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_print, 0, STRING("mac_0_016"), 0, STRING("クリスチーヌ"))
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_set_reardir)
    CALL(evt_cam3d_evt_set, 100, 127, 45, 100, 75, -250, 500, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("クリスチーヌ"), LW(0), LW(1), LW(2))
        ADD(LW(0), -100)
        CALL(evt_npc_move_position, STRING("クリスチーヌ"), LW(0), LW(2), 0, FLOAT(180.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_npc_get_position, STRING("第三勢力幹部"), LW(0), LW(1), LW(2))
        ADD(LW(0), -100)
        CALL(evt_npc_move_position, STRING("第三勢力幹部"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(350)
        CALL(evt_npc_get_position, STRING("第三勢力部下Ａ"), LW(0), LW(1), LW(2))
        ADD(LW(0), -100)
        CALL(evt_npc_move_position, STRING("第三勢力部下Ａ"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(350)
        CALL(evt_npc_get_position, STRING("第三勢力部下Ｂ"), LW(0), LW(1), LW(2))
        ADD(LW(0), -100)
        CALL(evt_npc_move_position, STRING("第三勢力部下Ｂ"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    WAIT_MS(1400)
    CALL(evt_msg_print, 0, STRING("mac_0_017"), 0, STRING("第三勢力幹部"))
    CALL(evt_msg_print, 0, STRING("mac_0_018"), 0, STRING("クリスチーヌ"))
    CALL(evt_cam3d_evt_set, 120, 105, -53, 120, 70, -250, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_0_019"), 0, STRING("第三勢力幹部"))
    CALL(evt_msg_print, 0, STRING("mac_0_020"), 0, STRING("第三勢力部下Ａ"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 100, 127, 45, 100, 75, -250, 500, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("第三勢力幹部"), LW(0), LW(1), LW(2))
        ADD(LW(0), -20)
        CALL(evt_npc_move_position, STRING("第三勢力幹部"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_npc_get_position, STRING("第三勢力部下Ａ"), LW(0), LW(1), LW(2))
        ADD(LW(0), -20)
        CALL(evt_npc_move_position, STRING("第三勢力部下Ａ"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(150)
        CALL(evt_npc_get_position, STRING("第三勢力部下Ｂ"), LW(0), LW(1), LW(2))
        ADD(LW(0), -20)
        CALL(evt_npc_move_position, STRING("第三勢力部下Ｂ"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    WAIT_MS(50)
    CALL(evt_npc_get_position, STRING("クリスチーヌ"), LW(0), LW(1), LW(2))
    ADD(LW(0), -20)
    CALL(evt_npc_move_position, STRING("クリスチーヌ"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    CALL(evt_msg_print, 0, STRING("mac_0_021"), 0, STRING("クリスチーヌ"))
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 4)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_nannpc_init, PTR(nannpc_entry_data), 0, 0, 0, 1000)
    CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
    SET_VALUE(LW(0), 0)
    SPAWN_EVENT(gundan1000tai_beforebattle)
    RETURN
EVT_END()

EVT_BEGIN(gundan1000tai_beforebattle)
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_mario_set_pos, -220, 15, 90)
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_mario_cont_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        CALL(evt_npc_set_position, STRING("クリスチーヌ"), 55, 15, -300)
        CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
        CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1536)
        CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1073741856)
        CALL(evt_npc_set_position, STRING("第三勢力幹部"), 105, 15, -300)
        CALL(evt_npc_set_ry, STRING("第三勢力幹部"), 270)
        CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), 135, 15, -330)
        CALL(evt_npc_set_ry, STRING("第三勢力部下Ａ"), 270)
        CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), 160, 15, -330)
        CALL(evt_npc_set_ry, STRING("第三勢力部下Ｂ"), 270)
        CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1536)
        CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ａ"), 1536)
        CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ｂ"), 1536)
        CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1073741856)
        CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ａ"), 1073741856)
        CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ｂ"), 1073741856)
    END_IF
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS_OR_EQUAL(LW(2), -100)
            IF_INT_GREATER_OR_EQUAL(LW(0), -25)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_envoff, 512)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SET_VALUE(LF(0), 0)
    IF_INT_LESS_OR_EQUAL(LW(2), -420)
        SET_VALUE(LW(2), -420)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(2), -220)
        SET_VALUE(LW(2), -220)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        ADD(LW(0), 20)
        IF_INT_GREATER_OR_EQUAL(LW(0), 10)
            SET_VALUE(LW(0), 20)
        END_IF
        CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(120.0f))
    END_IF
    CALL(evt_mario_set_dir, 90, 100, 0)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 270)
    WAIT_MS(200)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("クリスチーヌ"), 0, 0, 0, 0, 0, 0, 0, 30)
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_I_N"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), -35)
    CALL(evt_npc_move_position, STRING("クリスチーヌ"), LW(0), LW(2), 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("mac_0_022"), 0, STRING("クリスチーヌ"))
    CALL(evt_cam3d_evt_set, 36, 149, 224, 37, 53, -317, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_023"), 0, STRING("第三勢力幹部"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_jump_pos, LW(0), LW(1), LW(2), 200, 0, -1000)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    WAIT_MS(150)
    CALL(evt_mario_set_dir, 270, 100, 0)
    WAIT_MS(150)
    CALL(evt_mario_set_dir, 90, 100, 0)
    WAIT_MS(150)
    CALL(evt_mario_set_dir, 270, 100, 0)
    WAIT_MS(150)
    CALL(evt_mario_set_dir, 90, 100, 0)
    WAIT_MS(150)
    CALL(evt_msg_print, 0, STRING("mac_0_024"), 0, STRING("第三勢力幹部"))
    CALL(evt_cam3d_evt_set, 0, 149, 224, 0, 53, -317, 500, 11)
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_2"), LW(10), LW(11), LW(12), 0)
    CALL(evt_mario_set_pose, STRING("M_N_H"))
    BEGIN_INLINE_EVENT
        ADD(LW(10), 80)
        ADD(LW(12), -30)
        CALL(evt_npc_move_position, STRING("第三勢力部下Ａ"), LW(10), LW(12), 0, FLOAT(180.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        ADD(LW(10), 105)
        ADD(LW(12), -30)
        CALL(evt_npc_move_position, STRING("第三勢力部下Ｂ"), LW(10), LW(12), 0, FLOAT(180.0f), 0)
    END_INLINE_EVENT
    ADD(LW(10), 50)
    CALL(evt_npc_move_position, STRING("第三勢力幹部"), LW(10), LW(12), 0, FLOAT(180.0f), 0)
    CALL(L_evt_pouch_get_hp, LW(0))
    IF_INT_LESS(LW(0), 5)
        CALL(L_evt_pouch_set_hp, 5)
    END_IF
    CALL(evt_npc_set_battle_info, STRING("第三勢力幹部"), 2)
    CALL(evt_npc_battle_start, STRING("第三勢力幹部"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_cam3d_evt_set, 80, 108, -45, 80, 60, -316, 0, 11)
    CALL(evt_mario_set_pos, -40, 15, -300)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), -77, 15, -300)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1073741856)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), 75, 15, -300)
    CALL(evt_npc_set_ry, STRING("第三勢力幹部"), 270)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), 105, 15, -330)
    CALL(evt_npc_set_ry, STRING("第三勢力部下Ａ"), 270)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), 130, 15, -330)
    CALL(evt_npc_set_ry, STRING("第三勢力部下Ｂ"), 270)
    SET_VALUE(GSW(0), 5)
    CALL(evt_npc_get_reglid, STRING("ガラの悪い水夫"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_get_reglid, STRING("水夫ボム兵Ｂ"), LW(0))
    DELETE_EVENT(LW(0))
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -200, 15, -175)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    CALL(evt_npc_set_position, STRING("水夫ボム兵Ｂ"), -250, 15, -350)
    CALL(evt_npc_set_ry, STRING("水夫ボム兵Ｂ"), 90)
    SET_VALUE(LW(0), 0)
    CALL_CHILD(gundan1000tai_afterbattle)
    RETURN
EVT_END()

static EVT_BEGIN(_kemuri_sub)
    CALL(evt_sub_random, 200, LW(0))
    ADD(LW(0), -50)
    CALL(evt_sub_random, 100, LW(1))
    ADD(LW(1), 15)
    CALL(evt_sub_random, 100, LW(2))
    ADD(LW(2), -250)
    CALL(evt_sub_random, 100, LW(3))
    ADD_FLOAT(LW(3), FLOAT(50.0f))
    DIVIDE_FLOAT(LW(3), FLOAT(100.0f))
    CALL(evt_eff, 0, STRING("kemuri_test"), 2, LW(0), LW(1), LW(2), LW(3), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_ATTACK3"), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(gundan1000tai_kemuri)
    LOOP(0)
        SPAWN_EVENT(_kemuri_sub)
        SPAWN_EVENT(_kemuri_sub)
        SPAWN_EVENT(_kemuri_sub)
        CALL(evt_sub_random, 200, LW(0))
        ADD(LW(0), 200)
        WAIT_MS(LW(0))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(gundan1000tai_zaza_se)
    BEGIN_INLINE_EVENT
        LOOP(10)
            CALL(gundan1000tai_circle_pos_func, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_TOGETHER1"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(100)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(20)
            WAIT_MS(25)
            CALL(gundan1000tai_circle_pos_func, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_TOGETHER1L"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(25)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(20)
            CALL(gundan1000tai_circle_pos_func, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_TOGETHER1R"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(50)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(gundan1000tai_jump_se)
    LOOP(0)
        CALL(gundan1000tai_circle_pos_func, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_ATTACK2"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(200)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(gundan1000tai_yure)
    LOOP(0)
        BEGIN_INLINE_EVENT
            CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 500)
        END_INLINE_EVENT
        CALL(evt_sub_random, 4500, LW(0))
        ADD(LW(0), 500)
        WAIT_MS(LW(0))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(gundan1000tai_afterbattle)
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_cam3d_evt_set, 80, 108, -45, 80, 60, -316, 0, 11)
        CALL(evt_mario_set_pos, -40, 15, -300)
        CALL(evt_mario_set_dir, 90, 0, 0)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
        CALL(evt_npc_set_position, STRING("クリスチーヌ"), -77, 15, -300)
        CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
        CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1536)
        CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1073741856)
        CALL(evt_npc_set_position, STRING("第三勢力幹部"), 75, 15, -300)
        CALL(evt_npc_set_ry, STRING("第三勢力幹部"), 270)
        CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), 105, 15, -330)
        CALL(evt_npc_set_ry, STRING("第三勢力部下Ａ"), 270)
        CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), 130, 15, -330)
        CALL(evt_npc_set_ry, STRING("第三勢力部下Ｂ"), 270)
        CALL(evt_seq_wait, 2)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1073743392)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ａ"), 1073743392)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力部下Ｂ"), 1073743392)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(dai3_animtbl))
        CALL(evt_nannpc_flag_onoff, LW(0), 0, 12)
        CALL(evt_nannpc_set_size, LW(0), 30, 5)
        ADD(LW(0), 1)
        IF_INT_EQUAL(LW(0), 1000)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_025"), 0, STRING("第三勢力幹部"))
    CALL(evt_cam3d_evt_set, 30, 325, 334, 30, 65, -316, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_026"), 0, STRING("第三勢力幹部"))
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        WAIT_MS(200)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 30)
        LOOP(0)
            WAIT_MS(150)
            CALL(evt_mario_set_dir, 270, 100, 0)
            WAIT_MS(150)
            CALL(evt_mario_set_dir, 90, 100, 0)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(LW(11))
        WAIT_MS(220)
        CALL(evt_eff_fukidashi, 1, STRING(""), STRING("クリスチーヌ"), 0, 0, 0, 0, 0, 0, 0, 30)
        LOOP(0)
            WAIT_MS(500)
            CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 270)
            WAIT_MS(500)
            CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
        END_LOOP
    END_INLINE_EVENT
    SET_VALUE(GF(10), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.005f), 300)
            IF_INT_EQUAL(GF(10), 1)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_INLINE_EVENT
    CALL(gundan1000tai_init)
    CALL(evt_snd_sfxon, STRING("SFX_STG0_3RD_TOGETHER1_2"), 0)
    LOOP(0)
        SET_VALUE(LW(0), 0)
        SET_VALUE(LW(1), 1)
        LOOP(0)
            CALL(evt_nannpc_get_work, LW(0), 0, LW(2))
            ADD(LW(0), 1)
            IF_INT_EQUAL(LW(2), 0)
                SET_VALUE(LW(1), 0)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(0), 1000)
                BREAK_LOOP
            END_IF
        END_LOOP
        IF_INT_EQUAL(LW(1), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    DELETE_EVENT(LW(10))
    DELETE_EVENT(LW(11))
    SET_VALUE(GF(10), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 100, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("mac_0_027"), 0, STRING("第三勢力幹部"))
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    CALL(gundan1000tai_totsugeki)
    SPAWN_EVENT_GET_ID(PTR(gundan1000tai_kemuri), LW(10))
    SPAWN_EVENT_GET_ID(PTR(gundan1000tai_jump_se), LW(11))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_ATTACK1"), 0, 15, -300, LW(12))
    SPAWN_EVENT_GET_ID(PTR(gundan1000tai_yure), LW(13))
    WAIT_MS(1500)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 0, -1000, 0)
    WAIT_MS(4300)
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 85, 15, -190)
    CALL(evt_npc_move_position, STRING("クリスチーヌ"), 300, -180, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 270)
    DELETE_EVENT(LW(13))
    CALL(evt_cam3d_evt_set, 285, 117, 50, 285, 25, -315, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_0_028"), 0, STRING("クリスチーヌ"))
    CALL(evt_mario_set_pos, 55, 15, -180)
    CALL(evt_mario_dispflag_onoff, 0, 1)
    CALL(evt_cam3d_evt_off, 1000, 11)
    CALL(evt_mario_mov_pos2, 270, -180, FLOAT(120.0f))
    CALL(evt_msg_print, 0, STRING("mac_0_029"), 0, STRING("クリスチーヌ"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(1000)
    CALL(evt_npc_get_position, STRING("クリスチーヌ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_get_dir, STRING("クリスチーヌ"), LW(3))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 0, -1000, 0)
    CALL(evt_mario_set_party_pos, 1, 13, LW(0), LW(1), LW(2))
    CALL(evt_party_set_dir, 1, LW(3), -1)
    CALL(evt_party_run, 1)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GF(10), 1)
    LOOP(0)
        CALL(gundan1000tai_areacheck, LW(0), LW(1))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_mario_set_hosei_xyz, LW(0), 0, 0)
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_party_set_hosei_xyz, 1, LW(1), 0, 0)
        END_IF
        IF_INT_EQUAL(GF(10), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), 30, 15, -300)
    CALL(evt_cam3d_evt_set, 30, 325, 334, 30, 65, -316, 0, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_030"), 0, STRING("第三勢力幹部"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_0_031"), 0, STRING("第三勢力幹部"))
    CALL(evt_npc_set_position, STRING("第三勢力部下Ａ"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("第三勢力部下Ｂ"), 0, -1000, 0)
    CALL(gundan1000tai_totsuend)
    DELETE_EVENT(LW(11))
    CALL(evt_snd_sfxoff, LW(12))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        DELETE_EVENT(LW(10))
    END_INLINE_EVENT
    LOOP(0)
        SET_VALUE(LW(0), 0)
        SET_VALUE(LW(1), 1)
        LOOP(0)
            CALL(evt_nannpc_get_work, LW(0), 0, LW(2))
            ADD(LW(0), 1)
            IF_INT_EQUAL(LW(2), 0)
                SET_VALUE(LW(1), 0)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(0), 1000)
                BREAK_LOOP
            END_IF
        END_LOOP
        IF_INT_EQUAL(LW(1), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_reverse_ry, STRING("第三勢力幹部"))
    WAIT_MS(800)
    CALL(evt_npc_reverse_ry, STRING("第三勢力幹部"))
    WAIT_MS(800)
    CALL(evt_npc_reverse_ry, STRING("第三勢力幹部"))
    WAIT_MS(800)
    CALL(evt_npc_reverse_ry, STRING("第三勢力幹部"))
    WAIT_MS(800)
    WAIT_MS(1350)
    CALL(evt_msg_print, 0, STRING("mac_0_032"), 0, STRING("第三勢力幹部"))
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -130, 15, -354, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -98, 15, -370, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -130, 15, -300, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), -130, 15, -354, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(140)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -89, 15, -242, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 10, 15, -160, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -120, 15, -315, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), -89, 15, -242, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -71, 15, -369, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -114, 15, -222, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -23, 15, -351, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), -71, 15, -369, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(180)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -40, 15, -190, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -15, 15, -165, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -55, 15, -189, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), -40, 15, -190, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(150)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -12, 15, -405, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 20, 15, -394, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, -5, 15, -188, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), -12, 15, -405, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(220)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 50, 15, -408, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 74, 15, -384, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 0, 15, -420, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), 50, 15, -408, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(100)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 68, 15, -201, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 41, 15, -175, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 90, 15, -186, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), 68, 15, -201, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(120)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 138, 15, -239, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 162, 15, -216, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 111, 15, -191, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), 138, 15, -239, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(180)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 149, 15, -374, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 95, 15, -360, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 173, 15, -316, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), 149, 15, -374, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(230)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 198, 15, -300, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 187, 15, -268, 30, 0, 60)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, 185, 15, -221, 30, 0, 60)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_QUESTION_MARK1"), 198, 15, -300, 0)
    END_INLINE_EVENT
    WAIT_MS(1500)
    CALL(evt_cam3d_evt_set, 30, 133, -25, 30, 60, -316, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("第三勢力幹部"), LW(0), LW(1), LW(2))
    CALL(evt_npc_jump_position_nohit, STRING("第三勢力幹部"), LW(0), LW(1), LW(2), 225, 30)
    CALL(evt_npc_set_ry, STRING("第三勢力幹部"), 90)
    WAIT_MS(700)
    CALL(evt_msg_print, 0, STRING("mac_0_033"), 0, STRING("第三勢力幹部"))
    WAIT_MS(1000)
    SET_VALUE(GSW(0), 6)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_fade_set_mapchange_type, 0, 2, 500, 1, 500)
    CALL(evt_bero_mapchange, STRING("gor_01"), STRING("s_bero"))
    RETURN
EVT_END()

EVT_BEGIN(mony_peton_after)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    LOOP(0)
        CALL(evt_mario_party_use_check, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_party_stop, 0)
    CALL(evt_msg_print_party, STRING("mac_2_088"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSWF(1181), 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

EVT_BEGIN(shukko_event_move_ship_se00)
    LOOP(6)
        WAIT_MS(2000)
        CALL(evt_mapobj_get_position, STRING("S_fune_c"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_SHIP_MOVE1"), LW(0), LW(1), LW(2), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(shukko_event_move_ship_se01)
    LOOP(8)
        WAIT_MS(1400)
        CALL(evt_mapobj_get_position, STRING("S_fune_c"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_SHIP_MAST1"), LW(0), LW(1), LW(2), 0)
    END_LOOP
    RETURN
EVT_END()

char* shukko_npc_list[10] = {
    "ガラの悪い水夫", "マルコ", "水夫Ｄ", "めがね水夫", "オドオド水夫", "水夫ボム兵Ｃ", "サンダース", "店長", "店員", NULL,
};

EVT_BEGIN(shukko_event_move_ship)
    SPAWN_EVENT(shukko_event_move_ship_se00)
    SPAWN_EVENT(shukko_event_move_ship_se01)
    CALL(evt_mapobj_get_position, STRING("S_fune_c"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_WAVE1"), LW(0), LW(1), LW(2), LW(9))
    CALL(evt_hit_bind_mapobj, STRING("A_fune_C"), STRING("S_fune_c"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_c_hikinami"), 1)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_c_hikinami"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_c_hikinami"), 256)
    CALL(alpha_set_func, 0)
    SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
    CALL(evt_sub_intpl_msec_init, 1, 0, 100, 6000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_fune_c"), LW(0), 0, 0)
        CALL(evt_hit_bind_update, STRING("A_fune_C"))
        CALL(evt_mapobj_trans, 1, STRING("S_c_hikinami"), LW(0), 0, 0)
        SET_FLOAT_VALUE(LW(4), LW(0))
        SUBTRACT_FLOAT(LW(4), LW(3))
        CALL(hosei_mario_pos, LW(4), 0, 0)
        CALL(hosei_party_pos, LW(4), 0, 0)
        SET_INT_READ_ADDR(PTR(shukko_npc_list))
        LOOP(0)
            READ_INT(LW(5))
            IF_INT_EQUAL(LW(5), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, LW(5), LW(6), LW(7), LW(8))
            ADD_FLOAT(LW(6), LW(4))
            CALL(evt_npc_set_position, LW(5), LW(6), LW(7), LW(8))
        END_LOOP
        SET_FLOAT_VALUE(LW(3), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_FLOAT_VALUE(LW(3), FLOAT(100.0f))
    CALL(evt_sub_intpl_msec_init, 0, 100, 600, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_fune_c"), LW(0), 0, 0)
        CALL(evt_hit_bind_update, STRING("A_fune_C"))
        CALL(evt_mapobj_trans, 1, STRING("S_c_hikinami"), LW(0), 0, 0)
        SET_FLOAT_VALUE(LW(4), LW(0))
        SUBTRACT_FLOAT(LW(4), LW(3))
        CALL(hosei_mario_pos, LW(4), 0, 0)
        CALL(hosei_party_pos, LW(4), 0, 0)
        SET_INT_READ_ADDR(PTR(shukko_npc_list))
        LOOP(0)
            READ_INT(LW(5))
            IF_INT_EQUAL(LW(5), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, LW(5), LW(6), LW(7), LW(8))
            ADD_FLOAT(LW(6), LW(4))
            CALL(evt_npc_set_position, LW(5), LW(6), LW(7), LW(8))
        END_LOOP
        SET_FLOAT_VALUE(LW(3), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_snd_sfxoff, LW(9))
    RETURN
EVT_END()

EVT_BEGIN(shukko_event)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_flag_onoff, 1, 1024)
    CALL(evt_party_flg_onoff, 0, 16777216, 1)
    CALL(evt_cam3d_evt_set, 85, 142, 398, 85, 90, 102, 1000, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos, 125, 140, 1000)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos, 0, 120, 125, 1000)
    END_INLINE_EVENT
    CALL(evt_npc_get_position, STRING("マルコ"), LW(0), LW(1), LW(2))
    ADD(LW(0), 20)
    CALL(evt_npc_move_position, STRING("マルコ"), LW(0), LW(2), 1000, 0, 0)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("mac_4_093"), 0, STRING("マルコ"))
    CALL(evt_cam3d_evt_set, 95, 207, 763, 95, 89, 98, 400, 11)
    WAIT_MS(400)
    CALL(evt_msg_print, 0, STRING("mac_4_094"), 0, 0)
    CALL(evt_npc_move_position, STRING("水夫ボム兵Ｃ"), 90, 200, 0, 120, 0)
    CALL(evt_npc_jump_position_nohit, STRING("水夫ボム兵Ｃ"), 55, 51, 135, 700, 50)
    CALL(evt_npc_set_ry, STRING("水夫ボム兵Ｃ"), 90)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_SAILING1"))
    CALL(evt_cam3d_evt_set, 122, 360, 1112, 122, 180, 90, 2000, 11)
    WAIT_MS(2000)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_STARTING1"), 0)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SPAWN_EVENT(shukko_event_move_ship)
    WAIT_MS(2000)
    SET_INT_READ_ADDR(PTR(shukko_npc_list))
    LOOP(0)
        READ_INT(LW(5))
        IF_INT_EQUAL(LW(5), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_npc_flag_onoff, 1, LW(5), 294912)
    END_LOOP
    CALL(evt_cam3d_evt_set, 300, 0, 515, 300, 60, 91, 0, 11)
    WAIT_MS(2000)
    SET_INT_READ_ADDR(PTR(shukko_npc_list))
    LOOP(0)
        READ_INT(LW(5))
        IF_INT_EQUAL(LW(5), 0)
            BREAK_LOOP
        END_IF
        CALL(evt_npc_flag_onoff, 0, LW(5), 32768)
    END_LOOP
    CALL(cam_kaiten_init, 770, 235, -250, 350, 100, 80, -60, 39, 439, 350, 100, 80)
    BEGIN_INLINE_EVENT_GET_ID(LW(7))
        LOOP(0)
            CALL(cam_kaiten_set_direct)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_kaiten_get_para, 0, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LW(0), LF(7))
        CALL(cam_kaiten_get_para, 1, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LW(1), LF(7))
        ADD_FLOAT(LW(1), FLOAT(360.0f))
        CALL(evt_sub_intpl_msec_init, PTR(lineardec_type), LW(0), LW(1), 4500)
        LOOP(0)
            CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            CALL(evt_sub_intpl_msec_get_value_para, LW(6), LW(9))
            CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            IF_INT_EQUAL(LW(9), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_kaiten_get_para, 0, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LF(6), LW(0))
        CALL(cam_kaiten_get_para, 1, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LF(6), LW(1))
        CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 4500)
        LOOP(0)
            CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            CALL(evt_sub_intpl_msec_get_value_para, LW(7), LW(9))
            CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            IF_INT_EQUAL(LW(9), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_kaiten_get_para, 0, LF(0), LW(0), LF(2), LF(3), LF(4), LF(5), LF(6), LF(7))
        CALL(cam_kaiten_get_para, 1, LF(0), LW(1), LF(2), LF(3), LF(4), LF(5), LF(6), LF(7))
        CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 4500)
        LOOP(0)
            CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            CALL(evt_sub_intpl_msec_get_value_para, LW(1), LW(9))
            CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            IF_INT_EQUAL(LW(9), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(4500)
    DELETE_EVENT(LW(7))
    WAIT_MS(1500)
    CALL(evt_cam3d_evt_set, 122, 360, 1112, 122, 180, 90, 0, 11)
    WAIT_MS(4500)
    CALL(evt_cam3d_evt_set, 0, 99, -302, -130, 64, -452, 0, 11)
    WAIT_MS(1500)
    CALL(evt_npc_flag_onoff, 1, STRING("第三部下"), 1536)
    CALL(evt_npc_status_onoff, 1, STRING("第三部下"), 2)
    CALL(evt_npc_set_position, STRING("第三部下"), -170, 15, -580)
    CALL(evt_npc_move_position, STRING("第三部下"), -170, -500, 0, FLOAT(180.0f), 0)
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("第三部下"), 90)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("mac_4_095"), 0, STRING("第三部下"))
    SET_VALUE(GSW(0), 229)
    CALL(evt_bero_mapchange, STRING("sys_01"), STRING("stg_5"))
    RETURN
EVT_END()

// TODO: type
u32 nannpc_entry_data01[123] = {
    (u32)"c_boro_k", (u32)"S_1", 0x00000000, (u32)"c_boro_k",
    (u32)"S_1", 0x40000000, (u32)"c_boro_k", (u32)"S_1",
    0x40800000, (u32)"c_boro_k", (u32)"S_1", 0x40C00000,
    (u32)"c_boro_k", (u32)"S_1", 0x41000000, (u32)"c_boro_k",
    (u32)"S_1", 0x41200000, (u32)"c_boro_k", (u32)"S_1",
    0x41400000, (u32)"c_boro_k", (u32)"S_1", 0x41600000,
    (u32)"c_boro_k", (u32)"S_1", 0x41800000, (u32)"c_boro_k",
    (u32)"S_1", 0x41900000, (u32)"c_boro_k", (u32)"S_1",
    0x41A00000, (u32)"c_boro_k", (u32)"S_1", 0x41B00000,
    (u32)"c_boro_k", (u32)"S_1", 0x41C00000, (u32)"c_boro_k",
    (u32)"S_1", 0x41D00000, (u32)"c_boro_k", (u32)"S_1",
    0x41E00000, (u32)"c_boro_k", (u32)"S_1", 0x41F00000,
    (u32)"c_boro_k", (u32)"S_1", 0x42000000, (u32)"c_boro_k",
    (u32)"S_1", 0x42080000, (u32)"c_boro_k", (u32)"S_1",
    0x42100000, (u32)"c_boro_k", (u32)"S_1", 0x42180000,
    (u32)"c_boro_k", (u32)"R_1", 0x00000000, (u32)"c_boro_k",
    (u32)"R_1", 0x3F800000, (u32)"c_boro_k", (u32)"R_1",
    0x40000000, (u32)"c_boro_k", (u32)"R_1", 0x40400000,
    (u32)"c_boro_k", (u32)"R_1", 0x40800000, (u32)"c_boro_k",
    (u32)"R_1", 0x40A00000, (u32)"c_boro_k", (u32)"R_1",
    0x40C00000, (u32)"c_boro_k", (u32)"R_1", 0x40E00000,
    (u32)"c_boro_k", (u32)"R_1", 0x41000000, (u32)"c_boro_k",
    (u32)"R_1", 0x41100000, (u32)"c_boro_k", (u32)"R_1",
    0x41200000, (u32)"c_boro_k", (u32)"R_1", 0x41300000,
    (u32)"c_boro_k", (u32)"R_1", 0x41400000, (u32)"c_boro_k",
    (u32)"R_1", 0x41500000, (u32)"c_boro_k", (u32)"R_1",
    0x41600000, (u32)"c_boro_k", (u32)"R_1", 0x41700000,
    (u32)"c_boro_k", (u32)"R_1", 0x41800000, (u32)"c_boro_k",
    (u32)"R_1", 0x41880000, (u32)"c_boro_k", (u32)"R_1",
    0x41900000, (u32)"c_boro_k", (u32)"R_1", 0x41980000,
    0x00000000, 0x00000000, 0x00000000,
};

// Non-const on purpose: MWCC schedules the 12-byte copy in suifu100taikikan_seq01 case 99 as the
// original only when the source object is mutable, but the original symbol lives in .rodata, so pin it there.
__declspec(section ".rodata") Vec vec3_gor_00013858 = {0.0f, -1000.0f, 0.0f};

// TODO: type
static u32 suifu_animtbl[21] = {
    0x00000000, 0x00010001, 0x00020002, 0x00030003,
    0x00040004, 0x00050005, 0x00060006, 0x00070007,
    0x00080008, 0x00090009, 0x000A000A, 0x000B000B,
    0x000C000C, 0x000D000D, 0x000E000E, 0x000F000F,
    0x00100010, 0x00110011, 0x00120012, 0x00130013,
    0xFFFF0000,
};

// TODO: type
static u32 suifu_run_animtbl[11] = {
    0x00140015, 0x00160017, 0x00180019, 0x001A001B,
    0x001C001D, 0x001E001F, 0x00200021, 0x00220023,
    0x00240025, 0x00260027, 0xFFFF0000,
};

// TODO: type
u32 suifu100kaidanDOWN[12] = {
    0x439A8000, 0x41700000, 0xC3BB8000, 0x43A10000,
    0x41700000, 0xC3C68000, 0x43A90000, 0x41700000,
    0xC3CD0000, 0x43B28000, 0x41700000, 0xC3D58000,
};

// TODO: type
u32 suifu100kaidanUP[12] = {
    0x430F0000, 0x42E60000, 0xC4008000, 0x43270000,
    0x42E60000, 0xC4058000, 0x433D0000, 0x42E60000,
    0xC40A4000, 0x43490000, 0x42E60000, 0xC40C8000,
};

EVT_BEGIN(unk_evt_gor_000277f4)
    CALL(evt_npc_set_ry, STRING("マルコ"), 270)
    CALL(evt_npc_move_position, STRING("マルコ"), -130, 200, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_move_position, STRING("マルコ"), -230, -170, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_move_position, STRING("マルコ"), -230, -330, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_set_ry, STRING("マルコ"), 90)
    CALL(evt_npc_move_position, STRING("マルコ"), 250, -330, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_set_ry, STRING("マルコ"), 270)
    CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 270, 35, -400, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 260, 55, -450, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 230, 75, -480, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 210, 95, -520, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 150, 115, -530, 400, 20)
    CALL(evt_npc_move_position, STRING("マルコ"), 0, -570, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_position, STRING("マルコ"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(gorotsuki_kikan)
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_cont_onoff, 1)
    WAIT_FRAMES(2)
    CALL(evt_mario_set_pos, 190, 15, 225)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_pos, 0, 225, 15, 220)
    CALL(evt_party_set_dir, 0, 270, -1)
    CALL(evt_npc_set_position, STRING("コルテス"), 85, 80, 50)
    CALL(evt_npc_set_ry, STRING("コルテス"), 90)
    CALL(evt_npc_set_position, STRING("マルコ"), 145, 15, 225)
    CALL(evt_npc_set_ry, STRING("マルコ"), 270)
    CALL(evt_npc_set_position, STRING("水夫Ａ"), 50, 15, 225)
    CALL(evt_npc_set_ry, STRING("水夫Ａ"), 90)
    CALL(evt_npc_set_position, STRING("水夫Ｂ"), 0, 15, 225)
    CALL(evt_npc_set_ry, STRING("水夫Ｂ"), 90)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), 35, 15, 250)
    CALL(evt_npc_set_ry, STRING("オドオド水夫"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫Ａ"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫Ｂ"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 1073741856)
    CALL(evt_nannpc_init, PTR(nannpc_entry_data01), 0, 0, 0, 100)
    CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(suifu_animtbl))
        ADD(LW(0), 1)
        IF_INT_EQUAL(LW(0), 100)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(suifu100taikikan_init)
    CALL(evt_cam3d_evt_set, 60, 225, 432, 125, 225, 177, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_000_00"), 0, 0)
    CALL(evt_cam3d_evt_set, 165, 120, 586, 170, 55, 217, 1500, 11)
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("mac_5_000_01"), 0, STRING("マルコ"))
    CALL(evt_msg_print, 0, STRING("mac_5_000_02"), 0, STRING("水夫Ａ"))
    CALL(evt_cam3d_evt_set, 90, 165, 759, 90, 70, 217, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_000_03"), 0, STRING("水夫Ａ"))
    CALL(suifu100taikikan_away)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(suifu_run_animtbl))
        ADD(LW(0), 1)
        IF_INT_EQUAL(LW(0), 100)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    SET_VALUE(GSW(0), 269)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("水夫Ａ"), 270)
        CALL(evt_npc_move_position, STRING("水夫Ａ"), -146, 218, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_move_position, STRING("水夫Ａ"), -215, 0, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_restart_regular_event, STRING("水夫Ａ"))
        CALL(evt_npc_flag_onoff, 0, STRING("水夫Ａ"), 1073741856)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("水夫Ｂ"), 270)
        CALL(evt_npc_move_position, STRING("水夫Ｂ"), -168, 209, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_move_position, STRING("水夫Ｂ"), -165, -234, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_set_ry, STRING("水夫Ｂ"), 90)
        CALL(evt_npc_move_position, STRING("水夫Ｂ"), 0, -300, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_restart_regular_event, STRING("水夫Ｂ"))
        CALL(evt_npc_flag_onoff, 0, STRING("水夫Ｂ"), 1073741856)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("オドオド水夫"), 270)
        CALL(evt_npc_move_position, STRING("オドオド水夫"), -143, 219, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_move_position, STRING("オドオド水夫"), -200, -400, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_restart_regular_event, STRING("オドオド水夫"))
        CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 1073741856)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_npc_set_ry, STRING("オドオド水夫兄"), 90)
        CALL(evt_npc_set_position, STRING("オドオド水夫兄"), -200, 15, -200)
        CALL(evt_npc_restart_regular_event, STRING("オドオド水夫兄"))
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_000_04"), 0, STRING("マルコ"))
    CALL(evt_npc_set_ry, STRING("マルコ"), 90)
    WAIT_MS(200)
    CALL(evt_cam3d_evt_set, 165, 127, 628, 165, 71, 184, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_000_05"), 0, STRING("マルコ"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_5_000_06"), 0, STRING("マルコ"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_5_000_07"), 0, STRING("コルテス"))
    CALL(evt_cam3d_evt_set, 85, 190, 498, 85, 140, 50, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_000_08"), 0, STRING("コルテス"))
    CALL(evt_cam3d_evt_set, 165, 120, 586, 170, 55, 217, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("mac_5_000_09"), 0, STRING("マルコ"))
    CALL(evt_mario_set_dir, 270, 200, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("マルコ"), 270)
        CALL(evt_npc_move_position, STRING("マルコ"), -130, 200, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_move_position, STRING("マルコ"), -230, -170, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_move_position, STRING("マルコ"), -230, -330, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_set_ry, STRING("マルコ"), 90)
        CALL(evt_npc_move_position, STRING("マルコ"), 250, -330, 0, FLOAT(240.0f), 0)
        CALL(evt_npc_set_ry, STRING("マルコ"), 270)
        CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 270, 35, -400, 400, 20)
        CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 260, 55, -450, 400, 20)
        CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 230, 75, -480, 400, 20)
        CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 210, 95, -520, 400, 20)
        CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 150, 115, -530, 400, 20)
        CALL(evt_npc_move_position, STRING("マルコ"), 0, -570, 0, FLOAT(180.0f), 0)
        CALL(evt_npc_set_position, STRING("マルコ"), 0, -1000, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 205, 127, 628, 205, 71, 184, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("mac_5_000_10"))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgm_scope, 0, 0)
    SPAWN_EVENT(peach_mail)
    RETURN
EVT_END()

EVT_BEGIN(into_gor_ship)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    ADD_FLOAT(GW(13), FLOAT(600.0f))
    SET_FLOAT_VALUE(LW(10), FLOAT(600.0f))
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST_SHIP1"))
    SET_FLOAT_VALUE(LW(0), FLOAT(120.0f))
    ADD_FLOAT(LW(0), LW(10))
    SET_FLOAT_VALUE(LW(1), FLOAT(150.0f))
    ADD_FLOAT(LW(1), LW(10))
    SET_FLOAT_VALUE(LW(2), FLOAT(180.0f))
    ADD_FLOAT(LW(2), LW(10))
    SET_FLOAT_VALUE(LW(3), FLOAT(217.0f))
    ADD_FLOAT(LW(3), LW(10))
    CALL(evt_mario_set_pos, LW(0), 40, 65)
    CALL(evt_party_set_pos, 0, LW(1), 40, 35)
    CALL(evt_party_set_pos, 1, LW(2), 40, 68)
    CALL(evt_npc_set_position, STRING("コルテス"), LW(3), 40, 86)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_party_set_dir, 1, 90, 0)
    CALL(evt_npc_set_ry, STRING("コルテス"), 270)
    IF_INT_EQUAL(GSW(0), 277)
        SET_FLOAT_VALUE(LW(4), FLOAT(145.0f))
        ADD_FLOAT(LW(4), LW(10))
        SET_FLOAT_VALUE(LW(5), FLOAT(185.0f))
        ADD_FLOAT(LW(5), LW(10))
        CALL(evt_npc_set_position, STRING("モニー"), LW(4), 40, 110)
        CALL(evt_npc_set_position, STRING("ピートン"), LW(5), 40, 110)
        CALL(evt_npc_set_ry, STRING("モニー"), 90)
        CALL(evt_npc_set_ry, STRING("ピートン"), 270)
    END_IF
    SPAWN_EVENT_GET_ID(PTR(option_ship_mario), LW(9))
    CALL(evt_cam3d_evt_set, 188, 193, 753, 188, 74, 63, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(alpha_set_func, 255)
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_ARRIVAL1_3"), 0)
    SET_FLOAT_VALUE(LW(0), GW(13))
    SET_FLOAT_VALUE(LW(1), GW(13))
    SUBTRACT_FLOAT(LW(1), FLOAT(600.0f))
    CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 6000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        SET_FLOAT_VALUE(GW(13), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_bgmoff, 512)
    WAIT_MS(1000)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
    CALL(evt_snd_bgmoff, 30720)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_cont_onoff, 1, 1)
    IF_INT_EQUAL(GSW(0), 277)
        CALL_CHILD(mony_peton_restart)
    END_IF
    DELETE_EVENT(LW(9))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(mony_leave)
    CALL(evt_npc_jump_position_nohit, STRING("モニー"), 145, 15, 225, 400, 20)
    CALL(evt_npc_set_ry, STRING("モニー"), 270)
    CALL(evt_npc_move_position, STRING("モニー"), -130, 200, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_move_position, STRING("モニー"), -230, -170, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_move_position, STRING("モニー"), -230, -400, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_set_ry, STRING("モニー"), 90)
    CALL(evt_npc_move_position, STRING("モニー"), 250, -330, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_set_ry, STRING("モニー"), 270)
    CALL(evt_npc_jump_position_nohit, STRING("モニー"), 270, 35, -400, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("モニー"), 260, 55, -450, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("モニー"), 230, 75, -480, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("モニー"), 210, 95, -520, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("モニー"), 150, 115, -530, 400, 20)
    CALL(evt_npc_move_position, STRING("モニー"), 0, -570, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_position, STRING("モニー"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(peton_leave)
    CALL(evt_npc_move_position, STRING("ピートン"), -230, -170, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_move_position, STRING("ピートン"), -230, -400, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_set_ry, STRING("ピートン"), 90)
    CALL(evt_npc_move_position, STRING("ピートン"), 250, -330, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_set_ry, STRING("ピートン"), 270)
    CALL(evt_npc_jump_position_nohit, STRING("ピートン"), 270, 35, -400, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("ピートン"), 260, 55, -450, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("ピートン"), 230, 75, -480, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("ピートン"), 210, 95, -520, 400, 20)
    CALL(evt_npc_jump_position_nohit, STRING("ピートン"), 150, 115, -530, 400, 20)
    CALL(evt_npc_move_position, STRING("ピートン"), 0, -570, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_position, STRING("ピートン"), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(mony_peton_restart)
    SET_VALUE(GSW(0), 278)
    CALL(evt_msg_print, 0, STRING("mac_5_038"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("mac_5_039"), 0, STRING("ピートン"))
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 1073741856)
    CALL(evt_npc_set_ry, STRING("モニー"), 270)
    WAIT_MS(300)
    CALL(evt_npc_restart_regular_event, STRING("モニー"))
    WAIT_MS(60)
    CALL(evt_npc_jump_position_nohit, STRING("ピートン"), 185, 15, 225, 400, 20)
    CALL(evt_npc_set_ry, STRING("ピートン"), 270)
    CALL(evt_npc_move_position, STRING("ピートン"), -130, 200, 0, FLOAT(240.0f), 0)
    CALL(evt_npc_restart_regular_event, STRING("ピートン"))
    RETURN
EVT_END()

EVT_BEGIN(gor_00_koopa_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_set_mode, 2)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, 250, -100, 285)
    CALL(evt_mario_set_dispdir, 0)
    CALL(evt_cam3d_evt_set, 167, 195, 920, 167, 68, 227, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(kpa_chg_pose, STRING("KPA_D_1"))
    WAIT_MS(1000)
    SET_VALUE(GW(0), -100)
    SET_VALUE(GW(1), 285)
    CALL(evt_eff, STRING(""), STRING("splash"), 3, 250, 0, 285, FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 4, -100, 75, 400)
        LABEL(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(0)
        END_IF
        CALL(evt_sub_intpl_msec_init, 1, 75, 15, 400)
        LABEL(3)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(3)
        END_IF
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 285, 235, 900)
        LABEL(1)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(GW(1), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(1)
        END_IF
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_WATER_OUT1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_sub_intpl_msec_init, 11, 250, 200, 900)
    LABEL(2)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_mario_set_pos, LW(0), GW(0), GW(1))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(2)
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_WATER_IN1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_KOOPA_VEXATIOUS1_2"), LW(0), LW(1), LW(2), 0)
    CALL(kpa_chg_pose, STRING("KPA_D_2"))
    CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.005f), 500)
    WAIT_MS(1000)
    CALL(evt_mario_cont_onoff, 1)
    WAIT_FRAMES(1)
    CALL(kpa_chg_pose, STRING("KPA_S_2"))
    CALL(evt_cam3d_evt_set, 167, 161, 737, 167, 68, 227, 800, 11)
    WAIT_MS(800)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_2"), STRING("KPA_T_4"))
    CALL(evt_msg_print, 0, STRING("kpa_stg4_011"), 0, STRING("mario"))
    CALL(evt_cam3d_evt_set, -127, 161, 737, -127, 68, 227, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("kpa_stg4_012"), 0, STRING("水夫ボム兵Ａ"))
    BEGIN_INLINE_EVENT
        WAIT_MS(600)
        CALL(evt_msg_print, 0, STRING("kpa_stg4_013"), 0, STRING("水夫Ｂ"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("kpa_stg4_014"), 0, STRING("オドオド水夫"))
    END_INLINE_EVENT
    WAIT_MS(2100)
    CALL(evt_npc_get_position, STRING("水夫ボム兵Ａ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_NOKO_ESCAPE1"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("水夫ボム兵Ａ"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 30)
        SUBTRACT(LW(2), 200)
        CALL(evt_npc_move_position, STRING("水夫ボム兵Ａ"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("水夫Ｂ"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 30)
        SUBTRACT(LW(2), 200)
        CALL(evt_npc_move_position, STRING("水夫Ｂ"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("オドオド水夫"), LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 25)
        SUBTRACT(LW(2), 200)
        CALL(evt_npc_move_position, STRING("オドオド水夫"), LW(0), LW(2), 0, FLOAT(120.0f), 0)
    END_INLINE_EVENT
    WAIT_MS(1500)
    CALL(evt_cam3d_evt_set, 10, 161, 36, 10, 78, -273, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_npc_set_position, STRING("ババ"), 135, 35, -330)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_kamek_move_position, STRING("ババ"), 10, -330, 10, 0, FLOAT(60.0f), 0)
    SET_VALUE(GF(0), 1)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ババ"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("水夫ボム兵Ａ"), LW(0), LW(1), LW(2))
        ADD(LW(0), 50)
        SUBTRACT(LW(2), 310)
        CALL(evt_npc_set_position, STRING("水夫ボム兵Ａ"), LW(0), LW(1), LW(2))
        ADD(LW(0), 350)
        CALL(evt_npc_move_position, STRING("水夫ボム兵Ａ"), LW(0), LW(2), 0, FLOAT(180.0f), 0)
        CALL(evt_npc_flag_onoff, 1, STRING("水夫ボム兵Ａ"), 128)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("水夫Ｂ"), LW(0), LW(1), LW(2))
        ADD(LW(0), 50)
        SUBTRACT(LW(2), 270)
        CALL(evt_npc_set_position, STRING("水夫Ｂ"), LW(0), LW(1), LW(2))
        ADD(LW(0), 350)
        CALL(evt_npc_move_position, STRING("水夫Ｂ"), LW(0), LW(2), 0, FLOAT(180.0f), 0)
        CALL(evt_npc_flag_onoff, 1, STRING("水夫Ｂ"), 128)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("オドオド水夫"), LW(0), LW(1), LW(2))
        ADD(LW(0), 50)
        SUBTRACT(LW(2), 310)
        CALL(evt_npc_set_position, STRING("オドオド水夫"), LW(0), LW(1), LW(2))
        ADD(LW(0), 350)
        CALL(evt_npc_move_position, STRING("オドオド水夫"), LW(0), LW(2), 0, FLOAT(180.0f), 0)
        CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 128)
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_npc_set_ry, STRING("ババ"), 90)
    WAIT_MS(500)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ババ"), 1, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1200)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_flag_onoff, 1, STRING("ババ"), 1073741856)
    CALL(evt_npc_kamek_move_position, STRING("ババ"), 20, -120, 10, 0, FLOAT(90.0f), 0)
    SET_VALUE(GF(0), 1)
    CALL(evt_npc_set_position, STRING("ババ"), 80, 35, 125)
    CALL(evt_cam3d_evt_set, 145, 193, 737, 145, 83, 225, 0, 11)
    WAIT_FRAMES(1)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FLY1"), LW(0), LW(1), LW(2), 0)
            WAIT_FRAMES(10)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_kamek_move_position, STRING("ババ"), 82, 225, 8, 0, FLOAT(90.0f), 0)
    SET_VALUE(GF(0), 1)
    CALL(evt_npc_flag_onoff, 0, STRING("ババ"), 1073741856)
    CALL(evt_msg_print, 0, STRING("kpa_stg4_015"), 0, STRING("ババ"))
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_7"), STRING("KPA_T_7"))
    CALL(evt_msg_print, 0, STRING("kpa_stg4_016"), 0, STRING("mario"))
    CALL(evt_npc_set_autotalkpose, STRING("ババ"), STRING("K_2"), STRING("T_12"))
    CALL(evt_msg_print, 0, STRING("kpa_stg4_017"), 0, STRING("ババ"))
    SET_VALUE(LSWF(6), 0)
    SET_VALUE(GF(0), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(LSWF(6), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(100)
        CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.005f), 800)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(LSWF(6), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(100)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_KOOPA_ANGRY2_1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_koopa_fire, 1000)
        CALL(evt_mario_dispflag_onoff, 0, 2)
        WAIT_MS(100)
        CALL(kpa_chg_pose, STRING("KPA_S_4"))
        CALL(evt_npc_set_color, STRING("ババ"), 128, 128, 128, 255)
        CALL(evt_npc_set_color, STRING("houki"), 128, 128, 128, 255)
        CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
        CALL(evt_npc_get_dir, STRING("ババ"), LW(3))
        SET_VALUE(LW(4), LW(1))
        SET_VALUE(LW(5), LW(2))
        ADD(LW(5), 5)
        CALL(evt_npc_set_ry, STRING("houki"), LW(3))
        CALL(evt_npc_set_position, STRING("houki"), LW(0), LW(4), LW(5))
        ADD(LW(1), 5)
        CALL(evt_npc_set_position, STRING("ババ"), LW(0), LW(1), LW(2))
        CALL(evt_npc_set_anim, STRING("ババ"), STRING("D_5"))
        SET_VALUE(LW(0), 100)
        LOOP(6)
            WAIT_MS(LW(0))
            CALL(evt_npc_flag_onoff, 1, STRING("houki"), 128)
            WAIT_MS(LW(0))
            CALL(evt_npc_flag_onoff, 0, STRING("houki"), 128)
            DIVIDE(LW(0), 2)
        END_LOOP
        CALL(evt_npc_set_position, STRING("houki"), 0, -1000, 0)
        CALL(evt_npc_get_position, STRING("ババ"), LW(2), LW(3), LW(4))
        CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_FALL1"), LW(2), LW(3), LW(4), 0)
        CALL(evt_sub_intpl_msec_init, 1, LW(3), 15, 200)
        LABEL(4)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_position, STRING("ババ"), LW(2), LW(0), LW(4))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 1)
            GOTO(4)
        END_IF
        CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_DOWN1"), LW(2), 0, LW(4), 0)
        CALL(evt_npc_set_anim, STRING("ババ"), STRING("D_3"))
        WAIT_FRAMES(1)
        SET_VALUE(GF(0), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(LSWF(6), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(300)
        CALL(evt_mario_dispflag_onoff, 1, 2)
    END_INLINE_EVENT
    SET_VALUE(GF(1), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            IF_INT_NOT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_npc_get_position, STRING("ババ"), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        LOOP(0)
            IF_INT_EQUAL(GF(1), 1)
                BREAK_LOOP
            END_IF
            CALL(evt_sub_random, 30, LW(3))
            SUBTRACT(LW(3), 15)
            ADD(LW(3), LW(0))
            CALL(evt_sub_random, 15, LW(4))
            ADD(LW(4), 10)
            ADD(LW(4), LW(1))
            CALL(evt_snd_sfxon_3d, STRING("SFX_KUPPA_KAMEBABA_KOGE1"), LW(3), LW(4), LW(2), 0)
            CALL(evt_eff64, 0, STRING("kemuri12_n64"), 0, LW(3), LW(4), LW(2), FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
            WAIT_MS(600)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_4"), STRING("KPA_T_3"))
    CALL(evt_msg_print, 0, STRING("kpa_stg4_018"), PTR(gor_stg4_018_cb), STRING("mario"))
    LOOP(0)
        IF_INT_NOT_EQUAL(GF(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_cam3d_evt_set, 80, 103, 494, 80, 45, 225, 800, 11)
    WAIT_MS(800)
    CALL(evt_npc_set_autotalkpose, STRING("ババ"), STRING("D_3"), STRING("T_8"))
    CALL(evt_msg_print, 0, STRING("kpa_stg4_019"), 0, STRING("ババ"))
    CALL(evt_cam3d_evt_set, 145, 193, 737, 145, 83, 225, 800, 0)
    WAIT_MS(800)
    CALL(evt_npc_set_autotalkpose, STRING("mario"), STRING("KPA_S_4"), STRING("KPA_T_3"))
    CALL(evt_msg_print, 0, STRING("kpa_stg4_020"), 0, STRING("mario"))
    SET_VALUE(GSW(0), 218)
    WAIT_MS(2000)
    CALL(evt_snd_bgmoff_f, 512, 1000)
    CALL(evt_fade_entry, 6, 300, 0, 0, 0)
    CALL(evt_fade_end_wait)
    SET_VALUE(GF(1), 1)
    WAIT_MS(2000)
    SET_VALUE(LF(10), 0)
    SET_VALUE(LW(0), 12)
    CALL_CHILD(evt_memcard_save)
    CALL(evt_mario_set_mode, 0)
    CALL(evt_fade_set_mapchange_type, 1, 59, 0, 58, 0)
    CALL(evt_bero_mapchange, STRING("usu_00"), 0)
    RETURN
EVT_END()

static EVT_BEGIN(peach_mail)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_mario_get_name_hitobj_ride, LW(3))
        IF_INT_LESS_OR_EQUAL(LW(2), 250)
            IF_INT_LESS_OR_EQUAL(LW(0), -120)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING("BGM_FF_MAIL_RECEPTION1"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), 40)
    CALL(evt_party_move_pos2, 0, LW(0), LW(2), FLOAT(60.0f))
    CALL(evt_party_set_dir, 0, 270, -1)
    CALL(evt_msg_print_party, STRING("gor_00_mail5_00"))
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
    CALL(evt_cam3d_evt_set, -110, 109, 512, -110, 57, 217, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_N_9"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MAIL_RECEPTION2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_10"))
    WAIT_MS(500)
    CALL(evt_mario_set_dir, 90, 300, 0)
    CALL(evt_msg_print, 0, STRING("gor_00_mail5_01"), 0, 0)
    CALL(evt_msg_print_party, STRING("gor_00_mail5_02"))
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_shift_reset)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 270)
    CALL(evt_pouch_receive_mail, 4)
    CALL(evt_pouch_open_mail, 4)
    RETURN
EVT_END()

f32 gor_ship_puka_data[16] = {
    -0.0f, -0.5f, -1.0f, -1.5f, -2.0f, -2.2f, -2.5f, -2.7f,
    -2.7f, -2.7f, -2.5f, -2.2f, -2.0f, -1.5f, -1.0f, -0.5f,
};

EVT_BEGIN(gor_ship_puka)
    SET_VALUE(LW(10), 0)
    LOOP(0)
        CALL(gor_ship_puka_func, LW(10), LW(1), LW(11))
        SET_FLOAT_INDEXED(LW(0), LW(11))
        ADD(LW(10), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(gor_muj_ship_sub)
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), GW(13))
        SET_FLOAT_VALUE(LW(1), GW(14))
        SET_FLOAT_VALUE(LW(2), GW(15))
        CALL(evt_mapobj_trans, 1, STRING("S_fune_b"), LW(0), LW(1), 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_fune_b"), 0, LW(2), 0)
        CALL(evt_hit_bind_update, STRING("A_fune_b"))
        SET_FLOAT_VALUE(LW(3), LW(2))
        ADD_FLOAT(LW(3), FLOAT(180.0f))
        CALL(evt_mapobj_trans, 1, STRING("S_c_hikinami"), LW(0), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_c_hikinami"), 0, LW(3), 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(option_ship_mario)
    SET_FLOAT_VALUE(LW(10), GW(13))
    SET_FLOAT_VALUE(LW(11), GW(14))
    LOOP(0)
        SET_FLOAT_VALUE(LW(4), GW(13))
        SUBTRACT_FLOAT(LW(4), LW(10))
        SET_FLOAT_VALUE(LW(5), GW(14))
        SUBTRACT_FLOAT(LW(5), LW(11))
        CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_mario_set_pos, LW(1), LW(2), LW(3))
        CALL(evt_party_get_pos, 0, LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_party_set_pos, 0, LW(1), LW(2), LW(3))
        CALL(evt_party_get_pos, 1, LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_party_set_pos, 1, LW(1), LW(2), LW(3))
        CALL(evt_npc_get_position, STRING("コルテス"), LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        CALL(evt_npc_set_position, STRING("コルテス"), LW(1), LW(2), LW(3))
        IF_INT_EQUAL(GSW(0), 277)
            CALL(evt_npc_get_position, STRING("モニー"), LW(1), LW(2), LW(3))
            ADD_FLOAT(LW(1), LW(4))
            ADD_FLOAT(LW(2), LW(5))
            CALL(evt_npc_set_position, STRING("モニー"), LW(1), LW(2), LW(3))
            CALL(evt_npc_get_position, STRING("ピートン"), LW(1), LW(2), LW(3))
            ADD_FLOAT(LW(1), LW(4))
            ADD_FLOAT(LW(2), LW(5))
            CALL(evt_npc_set_position, STRING("ピートン"), LW(1), LW(2), LW(3))
        END_IF
        SET_FLOAT_VALUE(LW(10), GW(13))
        SET_FLOAT_VALUE(LW(11), GW(14))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(gor_muj_ship_init)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_fune_b"), 1)
    CALL(evt_hitobj_onoff, STRING("A_fune_b"), 1, 1)
    CALL(evt_hitobj_onoff, STRING("A_b_bero02"), 1, 0)
    CALL(evt_hit_bind_mapobj, STRING("A_fune_b"), STRING("S_fune_b"))
    SET_FLOAT_VALUE(GW(13), FLOAT(-25.0f))
    SET_FLOAT_VALUE(GW(14), FLOAT(0.0f))
    SET_VALUE(GW(15), 0)
    SPAWN_EVENT(gor_muj_ship_sub)
    SET_RAW_VALUE(LW(0), GW(14))
    SET_FLOAT_VALUE(LW(1), FLOAT(1.5f))
    SPAWN_EVENT_GET_ID(PTR(gor_ship_puka), GW(10))
    RETURN
EVT_END()

const Vec vec3_gor_00013c54 = {0.0f, 0.0f, 0.0f};
const Vec vec3_gor_00013c60 = {0.0f, 0.0f, 0.0f};
const Vec vec3_gor_00013c6c = {0.0f, 0.0f, 0.0f};

EVT_BEGIN(gor_muj_ship_pera)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_kaiten"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_offscreen_set_texture_fmt, STRING("offscreen"), 6, 6)
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_fune_b"), STRING("offscreen"))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_alloc_capture, STRING("capture_img"), 1, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_set_virtual_point, STRING("capture_img"), 191, 0, 50)
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("Z_1"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    CALL(evt_snd_sfxon, STRING("SFX_OFF_SHIP_REVERSE1"), 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_b"), 1)
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_fune_b"), 1)
    SET_VALUE(GW(15), 180)
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_fune_b"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_paper_delete, STRING("OFF_d_kaiten"))
    RETURN
EVT_END()

EVT_BEGIN(gor_muj_ship_start)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_c_hikinami"), 1)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_c_hikinami"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_c_hikinami"), 256)
    CALL(alpha_set_func, 0)
    CALL(evt_map_set_playrate, STRING("hiki_nami"), FLOAT(1.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 100, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(scale_func, LW(0))
            CALL(evt_npc_set_scale, STRING("コルテス"), LW(0), LW(0), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_party_cont_onoff, 1, 0)
        CALL(evt_mario_set_pos, 0, -1000, 0)
        CALL(evt_party_set_pos, 0, 0, -1000, 0)
        CALL(evt_party_set_pos, 1, 0, -1000, 0)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 184, 278, 1091, 184, 158, 99, 500, 11)
    WAIT_MS(1000)
    DELETE_EVENT(GW(10))
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST_SHIP1"))
    SET_VALUE(GF(10), 1)
    CALL(evt_npc_set_position, STRING("船ダミー"), 0, GW(14), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL_CHILD(gor_muj_ship_pera)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("船ダミー"), LW(0), LW(1), LW(2))
            SET_FLOAT_VALUE(GW(14), LW(1))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("S_fune_b"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_JUMP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_jump_position_nohit, STRING("船ダミー"), 0, GW(14), 0, 1800, 150)
    CALL(evt_mapobj_get_position, STRING("S_fune_b"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_LANDING1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(GF(10), 0)
    SET_RAW_VALUE(LW(0), GW(14))
    SET_FLOAT_VALUE(LW(1), FLOAT(1.5f))
    SPAWN_EVENT_GET_ID(PTR(gor_ship_puka), GW(10))
    BEGIN_INLINE_EVENT
        WAIT_MS(2000)
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 5000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_STARTING1_3"), 0)
    SET_FLOAT_VALUE(LW(0), GW(13))
    SET_FLOAT_VALUE(LW(1), GW(13))
    ADD_FLOAT(LW(1), FLOAT(800.0f))
    CALL(evt_sub_intpl_msec_init, 1, LW(0), LW(1), 10000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        SET_FLOAT_VALUE(GW(13), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_bero_mapchange, STRING("muj_20"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(tsumini)
    IF_INT_LESS_OR_EQUAL(GSW(0), 215)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ni_03"), 1)
        CALL(evt_hitobj_onoff, STRING("A_ni_03"), 1, 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ni_01"), 1)
        CALL(evt_hitobj_onoff, STRING("A_ni_01"), 1, 0)
    END_IF
    CALL(evt_seq_wait, 2)
    CALL(funeflag_save)
    SET_VALUE(GF(12), 0)
    SET_VALUE(GF(13), 0)
    LOOP(0)
        CALL(evt_mario_get_motion, LW(0))
        IF_INT_EQUAL(LW(0), 25)
            CALL(funechange_func, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                SET_VALUE(GF(12), 1)
            END_IF
            SET_VALUE(LF(0), GF(12))
        ELSE
            SET_VALUE(LF(0), 0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(0), -100)
                IF_INT_GREATER_OR_EQUAL(LW(2), -70)
                    SET_VALUE(LF(0), 1)
                END_IF
            ELSE
                IF_INT_GREATER_OR_EQUAL(LW(2), -280)
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 0)
            CALL(funeflag_reset)
            SWITCH(GSW(0))
                CASE_BETWEEN(223, 229)
                CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 128)
                CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 128)
                CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 128)
                CALL(evt_npc_flag_onoff, 1, STRING("めがね水夫"), 128)
                CALL(evt_npc_flag_onoff, 1, STRING("サンダース"), 128)
                CALL(evt_npc_flag_onoff, 1, STRING("水夫Ｄ"), 128)
                CALL(evt_npc_flag_onoff, 1, STRING("店長"), 128)
                CALL(evt_npc_flag_onoff, 1, STRING("店員"), 128)
                CASE_DEFAULT
                CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
            END_SWITCH
        ELSE
            CALL(funeflag_load)
            SWITCH(GSW(0))
                CASE_BETWEEN(223, 229)
                CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 128)
                CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 128)
                CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 128)
                CALL(evt_npc_flag_onoff, 0, STRING("めがね水夫"), 128)
                CALL(evt_npc_flag_onoff, 0, STRING("サンダース"), 128)
                CALL(evt_npc_flag_onoff, 0, STRING("水夫Ｄ"), 128)
                CALL(evt_npc_flag_onoff, 0, STRING("店長"), 128)
                CALL(evt_npc_flag_onoff, 0, STRING("店員"), 128)
                CASE_DEFAULT
                CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
            END_SWITCH
        END_IF
        IF_INT_EQUAL(GF(13), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
static u32 dmgRetPoint[16] = {
    (u32)"A_yuka", 0x00000000, 0xC47A0000, 0x00000000,
    (u32)"Ship_Change01", 0xC4098000, 0x41700000, 0x00000000,
    (u32)"Ship_Change02", 0x00000000, 0xC47A0000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(mobj_box00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 295, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 258)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_npc_entry)
    CALL(epilogue_npc_clear)
    CALL(evt_npc_entry, STRING("ピーチ姫"), STRING("c_peach"))
    CALL(evt_npc_set_tribe, STRING("ピーチ姫"), STRING("ピーチ姫"))
    CALL(evt_npc_set_position, STRING("ピーチ姫"), 125, 15, 235)
    CALL(evt_npc_set_ry, STRING("ピーチ姫"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("ピーチ姫"), 1568)
    CALL(evt_npc_set_autotalkpose, STRING("ピーチ姫"), STRING("P_S_7"), STRING("P_T_13"))
    CALL(evt_npc_set_anim, STRING("ピーチ姫"), STRING("P_S_7"))
    CALL(evt_npc_entry, STRING("キノじい"), STRING("c_kino_ji"))
    CALL(evt_npc_set_tribe, STRING("キノじい"), STRING("キノじい"))
    CALL(evt_npc_set_position, STRING("キノじい"), 165, 15, 205)
    CALL(evt_npc_set_ry, STRING("キノじい"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("キノじい"), 1568)
    CALL(evt_npc_entry, STRING("クリハカセ"), STRING("c_kuri_h"))
    CALL(evt_npc_set_tribe, STRING("クリハカセ"), STRING("クリハカセ"))
    CALL(evt_npc_set_position, STRING("クリハカセ"), 30, 15, 200)
    CALL(evt_npc_set_ry, STRING("クリハカセ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("クリハカセ"), 1568)
    CALL(evt_npc_entry, STRING("クリスチーヌ"), STRING("c_pkr"))
    CALL(evt_npc_set_tribe, STRING("クリスチーヌ"), STRING("クリスチーヌ"))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 10, 15, 235)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1568)
    CALL(evt_npc_entry, STRING("ノコタロウ"), STRING("c_pnk"))
    CALL(evt_npc_set_tribe, STRING("ノコタロウ"), STRING("ノコタロウ"))
    CALL(evt_npc_set_position, STRING("ノコタロウ"), -15, 15, 220)
    CALL(evt_npc_set_ry, STRING("ノコタロウ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("ノコタロウ"), 1568)
    CALL(evt_npc_entry, STRING("クラウダ"), STRING("c_windy"))
    CALL(evt_npc_set_tribe, STRING("クラウダ"), STRING("クラウダ"))
    CALL(evt_npc_set_position, STRING("クラウダ"), -40, 15, 200)
    CALL(evt_npc_set_ry, STRING("クラウダ"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("クラウダ"), 1568)
    CALL(get_yoshi_color, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(0), STRING("c_babyyoshi"))
        CASE_EQUAL(1)
        SET_VALUE(LW(0), STRING("c_babyyoshi2"))
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("c_babyyoshi3"))
        CASE_EQUAL(3)
        SET_VALUE(LW(0), STRING("c_babyyoshi4"))
        CASE_EQUAL(4)
        SET_VALUE(LW(0), STRING("c_babyyoshi5"))
        CASE_EQUAL(5)
        SET_VALUE(LW(0), STRING("c_babyyoshi6"))
        CASE_EQUAL(6)
        SET_VALUE(LW(0), STRING("c_babyyoshi7"))
    END_SWITCH
    CALL(evt_npc_entry, STRING("チビヨッシー"), LW(0))
    CALL(evt_npc_set_tribe, STRING("チビヨッシー"), STRING("ヨッシー"))
    CALL(evt_npc_set_position, STRING("チビヨッシー"), -45, 15, 240)
    CALL(evt_npc_set_ry, STRING("チビヨッシー"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("チビヨッシー"), 1568)
    CALL(evt_npc_entry, STRING("ビビアン"), STRING("c_vivian"))
    CALL(evt_npc_set_tribe, STRING("ビビアン"), STRING("ビビアン"))
    CALL(evt_npc_set_position, STRING("ビビアン"), -65, 15, 220)
    CALL(evt_npc_set_ry, STRING("ビビアン"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("ビビアン"), 1568)
    CALL(evt_npc_entry, STRING("サンダース"), STRING("c_bomt_n"))
    CALL(evt_npc_set_tribe, STRING("サンダース"), STRING("サンダース"))
    CALL(evt_npc_set_position, STRING("サンダース"), -90, 15, 205)
    CALL(evt_npc_set_ry, STRING("サンダース"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("サンダース"), 1568)
    CALL(evt_mario_chk_join_party, LW(0), 7)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_npc_entry, STRING("チュチュリーナ"), STRING("c_tyutyu"))
        CALL(evt_npc_set_tribe, STRING("チュチュリーナ"), STRING("チュチュリーナ"))
        CALL(evt_npc_set_position, STRING("チュチュリーナ"), -95, 15, 235)
        CALL(evt_npc_set_ry, STRING("チュチュリーナ"), 90)
        CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 1568)
    END_IF
    CALL(evt_npc_entry, STRING("プニオ"), STRING("c_pguide"))
    CALL(evt_npc_set_tribe, STRING("プニオ"), STRING("ガイド"))
    CALL(evt_npc_set_position, STRING("プニオ"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("プニオ"), 1568)
    CALL(evt_npc_status_onoff, 1, STRING("プニオ"), 2)
    CALL(evt_npc_entry, STRING("プニコ"), STRING("c_psister"))
    CALL(evt_npc_set_tribe, STRING("プニコ"), STRING("ガイド妹"))
    CALL(evt_npc_set_position, STRING("プニコ"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("プニコ"), 1568)
    CALL(evt_npc_status_onoff, 1, STRING("プニコ"), 2)
    RETURN
EVT_END()

EVT_BEGIN(punio_kinoko)
    LOOP(0)
        CALL(evt_npc_get_position, STRING("プニオ"), LW(0), LW(1), LW(2))
        ADD_FLOAT(LW(1), FLOAT(15.0f))
        CALL(evt_item_set_position, STRING("punio_kino"), LW(0), LW(1), LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(puniko_kinoko)
    LOOP(0)
        CALL(evt_npc_get_position, STRING("プニコ"), LW(0), LW(1), LW(2))
        ADD_FLOAT(LW(1), FLOAT(15.0f))
        CALL(evt_item_set_position, STRING("puniko_kino"), LW(0), LW(1), LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_set_pos, 100, 15, 210)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_cam3d_evt_set, 0, 700, 1420, 0, -10, 0, 0, 11)
    CALL(evt_item_entry, STRING("punio_kino"), 148, 0, -1000, 0, 17, -1, 0)
    CALL(evt_item_entry, STRING("puniko_kino"), 148, 0, -1000, 0, 17, -1, 0)
    CALL(evt_map_set_blend, 0, 216, 208, 255, 255)
    CALL(evt_map_set_blend, 1, 216, 208, 255, 255)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1500)
    CALL(evt_msg_print, 0, STRING("ep_01"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, 65, 102, 513, 65, 50, 217, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("ep_02"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, 60, 164, 836, 60, 55, 220, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("ep_03"), 0, STRING("クリスチーヌ"))
    CALL(evt_set_dir_to_target, STRING("クリハカセ"), STRING("クリスチーヌ"))
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("クリハカセ"))
    WAIT_MS(250)
    CALL(evt_msg_print, 0, STRING("ep_04"), 0, STRING("クリハカセ"))
    CALL(evt_set_dir_to_target, STRING("クリハカセ"), STRING("mario"))
    WAIT_FRAMES(1)
    CALL(evt_npc_wait_pera, STRING("クリハカセ"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("ep_05"), 0, STRING("クリハカセ"))
    CALL(evt_cam3d_evt_set, 15, 116, 565, 15, 55, 220, 400, 11)
    WAIT_MS(900)
    CALL(evt_msg_print, 0, STRING("ep_06"), 0, STRING("クリスチーヌ"))
    CALL(evt_cam3d_evt_set, -10, 116, 565, -10, 55, 220, 400, 11)
    WAIT_MS(900)
    CALL(evt_msg_print, 0, STRING("ep_07"), 0, STRING("ノコタロウ"))
    CALL(evt_cam3d_evt_set, -35, 116, 565, -35, 55, 220, 400, 11)
    WAIT_MS(900)
    CALL(evt_msg_print, 0, STRING("ep_08"), 0, STRING("クラウダ"))
    CALL(evt_cam3d_evt_set, -40, 116, 565, -40, 55, 220, 400, 11)
    WAIT_MS(900)
    CALL(evt_msg_print, 0, STRING("ep_09"), 0, STRING("チビヨッシー"))
    CALL(evt_cam3d_evt_set, -60, 116, 565, -60, 55, 220, 400, 11)
    WAIT_MS(900)
    CALL(evt_msg_print, 0, STRING("ep_10"), 0, STRING("ビビアン"))
    CALL(evt_cam3d_evt_set, -85, 116, 565, -85, 55, 220, 400, 11)
    WAIT_MS(900)
    CALL(evt_msg_print, 0, STRING("ep_11"), 0, STRING("サンダース"))
    CALL(evt_npc_check, STRING("チュチュリーナ"), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_cam3d_evt_set, -90, 116, 565, -90, 55, 220, 400, 11)
        WAIT_MS(900)
        CALL(evt_msg_print, 0, STRING("ep_12"), 0, STRING("チュチュリーナ"))
    END_IF
    CALL(evt_cam3d_evt_set, 60, 164, 836, 60, 55, 220, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("ep_13"), 0, STRING("キノじい"))
    CALL(evt_msg_print, 0, STRING("ep_14"), 0, 0)
    WAIT_MS(200)
    CALL(evt_npc_set_position, STRING("プニオ"), 20, 115, -660)
    CALL(evt_npc_flag_onoff, 1, STRING("プニオ"), 131088)
    CALL(evt_npc_set_position, STRING("プニコ"), -20, 115, -660)
    CALL(evt_npc_flag_onoff, 1, STRING("プニコ"), 131088)
    CALL(evt_item_set_position, STRING("punio_kino"), 20, 130, -660)
    CALL(evt_item_set_position, STRING("puniko_kino"), -20, 130, -660)
    SET_VALUE(GF(0), 1)
    SET_VALUE(GF(1), 1)
    SPAWN_EVENT_GET_ID(PTR(punio_kinoko), LW(14))
    SPAWN_EVENT_GET_ID(PTR(puniko_kinoko), LW(15))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("プニオ"), 20, -570, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニオ"), 196, -570, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニオ"), 302, -335, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニオ"), -215, -335, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニオ"), -240, -205, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニオ"), -129, 210, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニオ"), 65, 205, 0, 180, 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("プニコ"), -20, -570, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニコ"), 156, -570, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニコ"), 302, -335, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニコ"), -255, -335, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニコ"), -240, -205, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニコ"), -129, 210, 0, 180, 0)
        CALL(evt_npc_move_position, STRING("プニコ"), 55, 240, 0, 180, 0)
        SET_VALUE(GF(1), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(0))
    WAIT_WHILE(GF(1))
    CALL(evt_msg_print, 0, STRING("ep_15"), 0, STRING("プニオ"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("ep_16"), 0, STRING("プニコ"))
    CALL(evt_msg_print, 0, STRING("ep_17"), 0, STRING("プニオ"))
    CALL(evt_msg_print, 0, STRING("ep_18"), 0, STRING("プニコ"))
    CALL(evt_cam3d_evt_set, 95, 116, 565, 95, 55, 220, 500, 11)
    CALL(evt_npc_status_onoff, 0, STRING("プニオ"), 2)
    CALL(evt_npc_move_position, STRING("プニオ"), 75, 215, 0, 40, 0)
    DELETE_EVENT(LW(14))
    WAIT_FRAMES(1)
    CALL(evt_mario_set_pose, STRING("M_C_3"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 20)
    ADD(LW(1), 20)
    ADD(LW(2), 5)
    CALL(evt_item_move_3d_position, STRING("punio_kino"), LW(0), LW(1), LW(2), 300, 0)
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_item_delete, STRING("punio_kino"))
    CALL(evt_npc_status_onoff, 0, STRING("プニコ"), 2)
    CALL(evt_npc_move_position, STRING("プニコ"), 110, 240, 0, 40, 0)
    DELETE_EVENT(LW(15))
    WAIT_FRAMES(1)
    CALL(evt_npc_set_anim, STRING("ピーチ姫"), STRING("P_S_8"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("ピーチ姫"), LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 5)
    ADD(LW(1), 20)
    ADD(LW(2), 5)
    CALL(evt_item_move_3d_position, STRING("puniko_kino"), LW(0), LW(1), LW(2), 300, 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("プニコ"), 55, 240, 0, 40, 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_npc_move_position, STRING("プニオ"), 65, 205, 0, 40, 0)
    WAIT_WHILE(GF(0))
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("ピーチ姫"), STRING("P_S_7"))
    CALL(evt_item_delete, STRING("puniko_kino"))
    CALL(evt_npc_set_autotalkpose, STRING("ピーチ姫"), STRING("P_S_7"), STRING("P_T_13"))
    CALL(evt_msg_print, 0, STRING("ep_20"), 0, STRING("ピーチ姫"))
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 1, STRING("ピーチ姫"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("キノじい"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("クリハカセ"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("ノコタロウ"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("クラウダ"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("チビヨッシー"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("プニオ"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("プニコ"), 32768)
    CALL(evt_cam3d_evt_set, -200, 120, 505, 75, 60, 220, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(1000)
    CALL(evt_npc_set_autotalkpose, STRING("ピーチ姫"), STRING("P_S_7"), STRING("P_T_13"))
    CALL(evt_msg_print, 0, STRING("ep_21"), 0, STRING("ピーチ姫"))
    CALL(evt_cam3d_evt_set, 30, 88, 466, 33, 45, 220, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("ep_22"), 0, STRING("クリハカセ"))
    CALL(evt_npc_flag_onoff, 0, STRING("ピーチ姫"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("キノじい"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("クリハカセ"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("クリスチーヌ"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("ノコタロウ"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("クラウダ"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("チビヨッシー"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("プニオ"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("プニコ"), 32768)
    CALL(evt_cam3d_evt_set, 110, 133, 663, 110, 55, 220, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(500)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("キノじい"), 0, 0, 0, 0, 0, 0, 0, 48)
    WAIT_MS(1500)
    CALL(evt_msg_print, 0, STRING("ep_23"), 0, STRING("キノじい"))
    CALL(evt_mario_set_dir_npc, STRING("ピーチ姫"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(600)
    CALL(evt_mario_set_dir, 270, 300, 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 2, 225, 1015, 2, 50, 243, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("ep_24"), 0, STRING("ピーチ姫"))
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_VALUE(GF(0), 0)
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_VALUE(GF(1), 0)
    SET_FLOAT_VALUE(GW(2), FLOAT(0.0f))
    SET_VALUE(GF(2), 0)
    SET_FLOAT_VALUE(GW(3), FLOAT(0.0f))
    SET_VALUE(GF(3), 0)
    SET_FLOAT_VALUE(GW(4), FLOAT(-600.0f))
    SET_FLOAT_VALUE(GW(11), GW(4))
    SET_VALUE(GF(4), 0)
    SET_VALUE(GW(7), 180)
    SET_VALUE(GF(5), 1)
    SPAWN_EVENT(ship_a_animation)
    WAIT_MS(500)
    SET_VALUE(GF(10), 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(GW(6), FLOAT(5.0f))
        CALL(evt_sub_intpl_msec_init, 0, GW(4), -270, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(0), 1)
        SET_VALUE(GF(1), 1)
        SET_VALUE(GF(2), 1)
        SET_VALUE(GF(3), 1)
        SET_VALUE(GW(6), 2)
        SET_VALUE(GW(6), FLOAT(3.0f))
        CALL(evt_sub_intpl_msec_init, 4, -270, -75, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(10), 0)
        SET_VALUE(GW(6), FLOAT(1.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_MOVE1"), LW(0), LW(1), LW(2), LW(10))
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(10), LW(0), LW(1), LW(2))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(500)
    WAIT_WHILE(GF(10))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ピーチ姫"), LW(0), LW(1), LW(2))
        CALL(evt_npc_move_position, STRING("ピーチ姫"), 0, LW(2), 0, 60, 0)
        CALL(evt_npc_flag_onoff, 1, STRING("ピーチ姫"), 128)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("キノじい"), LW(0), LW(1), LW(2))
        CALL(evt_npc_move_position, STRING("キノじい"), 0, LW(2), 0, 60, 0)
        CALL(evt_npc_flag_onoff, 1, STRING("キノじい"), 128)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_mario_mov_pos2, 0, LW(2), 60)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    WAIT_MS(800)
    CALL(evt_msg_print, 0, STRING("ep_25"), 0, 0)
    WAIT_MS(2000)
    SET_VALUE(GF(4), 1)
    DEBUG_PUT_MSG(STRING("アニメ停止"))
    SET_VALUE(GF(10), 1)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL_CHILD(ship_pera)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_JUMP1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, PTR(lineardec_type), 0, 80, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_VALUE(GW(8), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(600)
        CALL(evt_sub_intpl_msec_init, 1, 80, 0, 600)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_VALUE(GW(8), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_LANDING1"), LW(0), LW(1), LW(2), 0)
        SET_VALUE(GF(10), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(10))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("ep_26"), 0, 0)
    DEBUG_PUT_MSG(STRING("アニメ開始"))
    SET_VALUE(GF(4), 0)
    WAIT_MS(2000)
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    SET_VALUE(GF(2), 0)
    SET_VALUE(GF(3), 0)
    SET_VALUE(GW(6), FLOAT(3.0f))
    SET_VALUE(GF(10), 1)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, -75, -270, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GW(6), FLOAT(5.0f))
        CALL(evt_sub_intpl_msec_init, 0, -270, -1260, 6000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(10), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_a"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a_hikinami"), 1)
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_MOVE1"), LW(0), LW(1), LW(2), LW(9))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(9), LW(0), LW(1), LW(2))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(9))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(6000)
        CALL(evt_sub_intpl_msec_init, 0, 100, 0, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_snd_sfx_vol, LW(9), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_set_ry, STRING("プニオ"), 270)
        WAIT_MS(300)
        CALL(evt_npc_set_ry, STRING("プニコ"), 270)
        WAIT_MS(300)
        CALL(evt_npc_set_ry, STRING("クリハカセ"), 270)
        WAIT_MS(300)
        CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 270)
        WAIT_MS(200)
        CALL(evt_npc_set_ry, STRING("ノコタロウ"), 270)
        WAIT_MS(200)
        CALL(evt_npc_set_ry, STRING("クラウダ"), 270)
        WAIT_MS(150)
        CALL(evt_npc_set_ry, STRING("チビヨッシー"), 270)
        WAIT_MS(200)
        CALL(evt_npc_set_ry, STRING("ビビアン"), 270)
        WAIT_MS(200)
        CALL(evt_npc_set_ry, STRING("サンダース"), 270)
        CALL(evt_npc_check, STRING("チュチュリーナ"), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            WAIT_MS(200)
            CALL(evt_npc_set_ry, STRING("チュチュリーナ"), 270)
        END_IF
    END_INLINE_EVENT
    WAIT_MS(3000)
    WAIT_WHILE(GF(10))
    CALL(evt_snd_bgmoff_f, 512, 4000)
    CALL(evt_cam3d_evt_set, 0, 700, 1420, 0, -10, 0, 3500, 11)
    WAIT_MS(3000)
    CALL(evt_bero_mapchange, STRING("muj_21"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(epilogue_npc_entry2)
    CALL(evt_npc_entry, STRING("クリスチーヌ"), STRING("c_pkr"))
    CALL(evt_npc_set_tribe, STRING("クリスチーヌ"), STRING("クリスチーヌ"))
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 0, 15, 220)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1073742880)
    CALL(evt_npc_status_onoff, 1, STRING("クリスチーヌ"), 2)
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt2)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_cam3d_evt_set, 120, 40, 586, -25, 50, 214, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_fade_end_wait)
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("ep_46"), 0, 0)
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("クリスチーヌ"), -150, 220, 0, 40, 0)
        CALL(evt_npc_move_position, STRING("クリスチーヌ"), -245, -250, 0, 40, 0)
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_snd_bgmoff_f, 512, 6500)
    WAIT_MS(1000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 900, 9, 900)
    CALL(evt_bero_mapchange, STRING("aaa_00"), STRING("epilogue2"))
    RETURN
EVT_END()

EVT_BEGIN(return_gorotsuki_town_init)
    CALL(evt_npc_entry, STRING("クリスチーヌ"), STRING("c_pkr"))
    CALL(evt_npc_set_tribe, STRING("クリスチーヌ"), STRING("クリスチーヌ"))
    CALL(evt_npc_flag_onoff, 1, STRING("クリスチーヌ"), 1024)
    CALL(evt_npc_set_position, STRING("クリスチーヌ"), 175, 15, 225)
    CALL(evt_npc_set_ry, STRING("クリスチーヌ"), 270)
    RETURN
EVT_END()

EVT_BEGIN(return_gorotsuki_town)
    CALL(evt_snd_bgm_scope, 0, 1)
    SET_TYPE(0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_set_party, 0, 1)
    CALL(evt_cam3d_evt_set, 0, 700, 1420, 0, -10, 0, 0, 11)
    SET_FLOAT_VALUE(GW(0), FLOAT(0.0f))
    SET_VALUE(GF(0), 0)
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_VALUE(GF(1), 0)
    SET_FLOAT_VALUE(GW(2), FLOAT(0.0f))
    SET_VALUE(GF(2), 0)
    SET_FLOAT_VALUE(GW(3), FLOAT(0.0f))
    SET_VALUE(GF(3), 0)
    SET_FLOAT_VALUE(GW(4), FLOAT(-600.0f))
    SET_FLOAT_VALUE(GW(11), GW(4))
    SET_VALUE(GF(4), 0)
    SET_VALUE(GW(7), 180)
    SET_VALUE(GF(5), 1)
    SPAWN_EVENT(ship_a_animation)
    CALL(evt_seq_wait, 2)
    WAIT_MS(3000)
    SET_VALUE(GF(10), 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(GW(6), FLOAT(5.0f))
        CALL(evt_sub_intpl_msec_init, 0, GW(4), -270, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(0), 1)
        SET_VALUE(GF(1), 1)
        SET_VALUE(GF(2), 1)
        SET_VALUE(GF(3), 1)
        SET_VALUE(GW(6), 2)
        SET_VALUE(GW(6), FLOAT(3.0f))
        CALL(evt_sub_intpl_msec_init, 4, -270, -75, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(10), 0)
        SET_VALUE(GW(6), FLOAT(1.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_MOVE1"), LW(0), LW(1), LW(2), LW(10))
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(10), LW(0), LW(1), LW(2))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("クリスチーヌ"), 0, 0, 0, 0, 0, 0, 0, 48)
    CALL(evt_cam3d_evt_set, 2, 225, 1015, 2, 50, 243, 3000, 11)
    WAIT_MS(3000)
    WAIT_WHILE(GF(10))
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_pos, -20, 15, 225)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_pos, 0, 20, 15, 225)
    CALL(evt_party_set_dir, 0, 270, 0)
    WAIT_MS(1000)
    CALL(evt_npc_move_position, STRING("クリスチーヌ"), 20, 225, 0, FLOAT(120.0f), 0)
    CALL(evt_npc_delete, STRING("クリスチーヌ"))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("mac_8_01"), 0, 0)
    SET_VALUE(GF(4), 1)
    SET_VALUE(GF(10), 1)
    CALL(evt_npc_set_position, STRING("船ダミー"), 0, GW(8), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL_CHILD(ship_pera)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("船ダミー"), LW(0), LW(1), LW(2))
            SET_FLOAT_VALUE(GW(8), LW(1))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_JUMP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_jump_position_nohit, STRING("船ダミー"), 0, GW(8), 0, 1800, 100)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_LANDING1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(GF(10), 0)
    DEBUG_PUT_MSG(STRING("アニメ開始"))
    SET_VALUE(GF(4), 0)
    WAIT_MS(2000)
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    SET_VALUE(GF(2), 0)
    SET_VALUE(GF(3), 0)
    SET_VALUE(GW(6), FLOAT(3.0f))
    SET_VALUE(GF(10), 1)
    WAIT_FRAMES(1)
    SET_VALUE(GF(5), 0)
    SET_FLOAT_VALUE(GW(11), GW(4))
    ADD_FLOAT(GW(11), FLOAT(-400.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, -75, -270, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GW(6), FLOAT(5.0f))
        CALL(evt_sub_intpl_msec_init, 0, -270, -1260, 6000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            SET_FLOAT_VALUE(GW(4), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(GF(10), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_a"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a_hikinami"), 1)
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_MOVE1"), LW(0), LW(1), LW(2), LW(9))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("S_fune_a"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(9), LW(0), LW(1), LW(2))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_snd_sfxoff, LW(9))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(6000)
        CALL(evt_sub_intpl_msec_init, 0, 100, 0, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            CALL(evt_snd_sfx_vol, LW(9), LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_map_set_playrate, STRING("hiki_nami"), FLOAT(0.25f))
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_sub_intpl_msec_init, 0, 0, 255, 1500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(2000)
        CALL(evt_sub_intpl_msec_init, 0, 255, 0, 1500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(alpha_set_func, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(3000)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("mac_8_02"), 0, STRING("party"))
    SET_VALUE(GSW(0), 405)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgm_scope, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(gor_00_init_evt)
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("pointLight1"), STRING("S_fune_b"), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("pointLight2"), STRING("S_fune_b"), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("pointLight3"), STRING("S_fune_b"), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("pointLight4"), STRING("S_fune_b"), 0)
    END_INLINE_EVENT
    SWITCH(GSW(0))
        CASE_BETWEEN(0, 3)
        IF_INT_NOT_EQUAL(GSW(0), 2)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
        ELSE
            CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
        END_IF
        CALL(evt_snd_bgm_scope, 0, 1)
        CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR0"))
        CASE_BETWEEN(4, 6)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
        CALL(evt_snd_envoff, 512)
        CASE_EQUAL(217)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_KUPPA1"))
        CALL(evt_snd_bgm_scope, 0, 1)
        CALL(evt_snd_bgmon, 512, STRING("ENV_STG0_GOR0"))
        CASE_EQUAL(402)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("epilogue2"))
            CALL(evt_snd_bgmon_f, 768, STRING("BGM_ENDING2"), 800)
            CALL(evt_snd_bgmoff, 34816)
            CALL(evt_snd_envoff, 512)
        ELSE
            CALL(evt_snd_bgmon_f, 768, STRING("BGM_ENDING2"), 800)
            CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR0"))
        END_IF
        CASE_EQUAL(268)
        CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG0_GOR1"), 2000)
        CALL(evt_snd_bgmoff, 30784)
        CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR0"))
        CASE_DEFAULT
        IF_INT_NOT_EQUAL(GSWF(1178), 1)
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG0_GOR1"))
            CALL(evt_snd_bgmoff, 30720)
        END_IF
        CALL(evt_snd_envon, 272, STRING("ENV_STG0_GOR0"))
    END_SWITCH
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL_CHILD(luigi_npcEnt)
    CALL(evt_npc_setup, PTR(npcEnt))
    IF_INT_GREATER_OR_EQUAL(GSW(0), 277)
        CALL(evt_npc_set_tribe, STRING("モニー"), STRING("モニー（指輪）"))
        CALL(evt_npc_set_tribe, STRING("ピートン"), STRING("ピートン（指輪）"))
    END_IF
    CALL(alpha_init_func)
    CALL_CHILD(sanders_init)
    CALL_CHILD(tentyou_init)
    CALL_CHILD(tenin_init)
    CALL(evt_npc_entry, STRING("船ダミー"), STRING("c_suifu_b"))
    CALL(evt_npc_flag_onoff, 1, STRING("船ダミー"), 1073741984)
    CALL_CHILD(borodo2_init)
    IF_INT_NOT_EQUAL(GSW(0), 217)
        CALL(evt_mobj_save_blk, STRING("mobj_save"), -275, 75, 100, 0, 0)
    END_IF
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("nami_02"), 1, 0)
    CALL(evt_map_playanim, STRING("hiki_nami"), 1, 0)
    CALL(evt_paper_entry, STRING("OFF_d_kaiten"))
    CALL(evt_hit_damage_return_set, PTR(dmgRetPoint))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_a"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a_hikinami"), 1)
    CALL(evt_hitobj_onoff, STRING("A_fune_a"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_b"), 1)
    CALL(evt_hitobj_onoff, STRING("A_fune_b"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_c"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_c_hikinami"), 1)
    CALL(evt_hitobj_onoff, STRING("A_fune_C"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_fune_c"), 2)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 110, 15, -440, STRING("S_ground"), 125, GSWF(5568))
    CALL(evt_mobj_itembox, STRING("mobj_box00"), -500, 15, -200, 0, 0, PTR(mobj_box00), GSWF(1247))
    CALL(evt_item_entry, STRING("item_00"), 125, -390, 15, -65, 16, GSWF(5567), 0)
    IF_INT_NOT_EQUAL(GSW(0), 402)
        SPAWN_EVENT(tsumini)
    END_IF
    IF_INT_EQUAL(GSW(0), 269)
        SPAWN_EVENT(peach_mail)
    END_IF
    IF_INT_EQUAL(GSW(0), 123)
        IF_INT_EQUAL(GSWF(1181), 0)
            CALL(evt_run_case_evt, 0, 1, STRING("A_oku_yuka_01"), 0, PTR(mony_peton_after), 0)
        END_IF
    END_IF
    SWITCH(GSW(0))
        CASE_EQUAL(2)
        CALL_CHILD(enter_gorotsuki_town_init)
        SPAWN_EVENT(enter_gorotsuki_town)
        CASE_EQUAL(3)
        CALL_CHILD(enter_gorotsuki_town_init)
        SET_VALUE(LW(0), 1)
        SPAWN_EVENT(christine_osoware)
        CASE_EQUAL(4)
        CALL_CHILD(enter_gorotsuki_town_init)
        CALL_CHILD(christine_osoware_setup)
        CALL(evt_nannpc_init, PTR(nannpc_entry_data), 0, 0, 0, 1000)
        CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
        SET_VALUE(LW(0), 1)
        SPAWN_EVENT(gundan1000tai_beforebattle)
        CASE_EQUAL(5)
        CALL_CHILD(enter_gorotsuki_town_init)
        CALL(evt_nannpc_init, PTR(nannpc_entry_data), 0, 0, 0, 1000)
        CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
        SET_VALUE(LW(0), 1)
        SPAWN_EVENT(gundan1000tai_afterbattle)
        CASE_EQUAL(217)
        CALL(evt_mario_kill_party, 0)
        CALL(evt_npc_entry, STRING("houki"), STRING("c_kamek_be"))
        CALL(evt_npc_set_anim, STRING("houki"), STRING("H_2"))
        CALL(evt_npc_flag_onoff, 1, STRING("houki"), 32)
        CALL(evt_npc_flag_onoff, 1, STRING("houki"), 64)
        CALL(evt_npc_set_position, STRING("houki"), 0, -1000, 0)
        CALL(evt_npc_set_ry, STRING("houki"), 90)
        SPAWN_EVENT(gor_00_koopa_evt)
        CASE_BETWEEN(172, 229)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_fune_c"), 1)
        CALL(evt_hitobj_onoff, STRING("A_fune_C"), 1, 1)
        CASE_EQUAL(402)
        CALL(evt_mario_kill_party, 0)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("epilogue2"))
            CALL_CHILD(epilogue_npc_entry2)
            SPAWN_EVENT(epilogue_evt2)
        ELSE
            CALL_CHILD(epilogue_npc_entry)
            SPAWN_EVENT(epilogue_evt)
        END_IF
        CASE_EQUAL(268)
        SPAWN_EVENT(gorotsuki_kikan)
        CASE_EQUAL(404)
        CALL_CHILD(return_gorotsuki_town_init)
        SPAWN_EVENT(return_gorotsuki_town)
    END_SWITCH
    IF_INT_NOT_EQUAL(GSW(0), 402)
        IF_INT_NOT_EQUAL(GSW(0), 404)
            IF_INT_GREATER_OR_EQUAL(GSW(0), 268)
                CALL_CHILD(gor_muj_ship_init)
                IF_INT_EQUAL(GSWF(1180), 1)
                    SET_VALUE(GSWF(1180), 0)
                    CALL(evt_mario_set_pos, 190, 15, 225)
                    CALL(evt_party_set_pos, 0, 225, 15, 220)
                ELSE
                    IF_INT_EQUAL(GSWF(1178), 1)
                        SET_VALUE(GSWF(1178), 0)
                        SET_VALUE(GSWF(39), 0)
                        SPAWN_EVENT(into_gor_ship)
                    END_IF
                END_IF
            END_IF
        END_IF
    END_IF
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change01"), STRING("Ship_Change01"))
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change02"), STRING("Ship_Change02"))
    CALL(evt_hitobj_onoff, STRING("A_c_bero"), 1, 0)
    CALL(evt_hitobj_onoff, STRING("A_b_bero02"), 1, 0)
    BEGIN_INLINE_EVENT
        CALL(fune_eff)
    END_INLINE_EVENT
    RETURN
EVT_END()

// clang-format on

static inline GXColor alphaColor(u8 a) {
    GXColor c = zero_gor_000126e4;

    c.a = a;
    return c;
}

static void alpha_tev(MapTevCallback* param) {
    s32 stage = param->tevStage;
    u8* ap = _ap;
    GXColor color;

    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    color = alphaColor(*ap);
    GXSetTevColor(GX_TEVREG0, color);
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    param->tevStage = param->tevStage + 1;
}

static USER_FUNC(alpha_init_func) {
    _ap = _mapAlloc(1);
    *_ap = 0xFF;
    return EVT_RETURN_DONE;
}

static USER_FUNC(alpha_set_func) {
    *_ap = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

f32 lineardec_type(s32 target, s32 count, f32 start, f32 end) {
    s32 i;
    s32 init = count * 20 / 100;
    s32 lim = count * 80 / 100;
    f32 value;
    f32 sum;
    f32 result;

    value = (f32)init;
    sum = value;
    for (i = 0; i < count; i++) {
        if (i >= lim) {
            value -= 1.0f;
        }
        sum += value;
        if (i == target) {
            result = sum;
        }
    }
    return start + (end - start) * result / sum;
}

static f32 accdec_type(s32 target, s32 count, f32 start, f32 end) {
    s32 i;
    s32 hi = count * 10 / 100;
    s32 lo = count * 90 / 100;
    f32 result;
    f32 value;
    f32 sum;

    value = 0.0f;
    sum = value;

    for (i = 0; i < count; i++) {
        if (i <= hi) {
            value += 1.0f;
        } else if (i >= lo) {
            value -= 1.0f;
        }
        sum += value;
        if (i == target) {
            result = sum;
        }
    }
    return start + (end - start) * result / sum;
}

static USER_FUNC(make_itemsel_table) {
    s32* args = event->args;
    PouchData* pouch = pouchGetPtr();
    s32* table;
    s32 i;
    s32 count;
    s32 item;

    itemsel_table_ram = _mapAlloc(0x54);
    table = itemsel_table_ram;
    count = 0;
    for (i = 0; i < MAX_HELD_ITEMS; i++) {
        item = pouch->heldItems[i];
        if (item >= 0xB3 && item < 0xEC) {
            *table = item;
            table++;
            count++;
        }
        if (item >= 0x94 && item < 0xA1) {
            *table = item;
            table++;
            count++;
        }
        switch (item) {
        case 0xA6:
        case 0xA7:
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0xAD:
        case 0xAE:
        case 0xAF:
        case 0xB0:
            *table = item;
            table++;
            count++;
            break;
        }
    }
    *table = -1;
    evtSetValue(event, args[0], count);
    event->lwData[10] = (s32)itemsel_table_ram;
    return EVT_RETURN_DONE;
}

static USER_FUNC(delete_itemsel_table) {
    _mapFree(itemsel_table_ram);
    return EVT_RETURN_DONE;
}

USER_FUNC(fune_yure_func_init) {
    *(f32*)&event->lwData[0] = evtGetFloat(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(fune_yure_func) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    f32 target = evtGetFloat(event, args[1]);
    s32 done;
    f32 speed;
    f32* value = (f32*)&event->lwData[0];

    if (target == 0.0f) {
        evtSetFloat(event, args[2], 0.0f);
        evtSetValue(event, args[3], 1);
        return EVT_RETURN_DONE;
    }

    done = 0;
    speed = 2.0f * target / sysMsec2FrameFloat(2000.0f);
    if (type == 0) {
        *value += speed;
        if (*value >= target) {
            *value = target;
            done = 1;
        }
    } else {
        *value -= speed;
        if (*value <= -target) {
            *value = -target;
            done = 1;
        }
    }
    evtSetFloat(event, args[2], *value);
    evtSetValue(event, args[3], done);
    return EVT_RETURN_DONE;
}

void geundan1000tai_circle_pos(Vec* out) {
    f32 s;
    f32 angle;
    f32 radius;
    f32 c;

    radius = (f32)(irand(0xB4) + 0x78);
    angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
    s = sin(angle);
    c = cos(angle);

    out->x = 0.9f * (radius * s);
    out->y = 15.0f;
    out->z = 0.5f * (radius * c) - 300.0f;
}

USER_FUNC(gundan1000tai_circle_pos_func) {
    s32* args = event->args;
    f32 radius;
    f32 s;
    f32 c;
    f32 angle;

    radius = (f32)(irand(0xB4) + 0x78);
    angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
    s = sin(angle);
    c = cos(angle);

    evtSetFloat(event, args[0], 0.9f * (radius * s));
    evtSetFloat(event, args[1], 15.0f);
    evtSetFloat(event, args[2], 0.5f * (radius * c) - 300.0f);
    return EVT_RETURN_DONE;
}

// Splitting the angle product (marked above) puts the eight pooled float constants in the original's
// registers; assigning base before field_0x60 puts the 90/-280 and 270/340 address temps in r3/r4.
// With the lifetime optimiser on, the allocator splits the two halves' live ranges and the second half's
// angle/s come out in f22/f21; off, and with radius/s/angle/c/c2 declared in this order, both halves keep
// angle in f21 and s in f22. The second half's cos result needs its own temp (c2) to stay in f1.
#pragma opt_lifetimes off
USER_FUNC(gundan1000tai_init) {
    s32 i;
    NanNpcEntry* entry;
    NanNpcWork* work;
    f32 radius;
    f32 s;
    f32 angle;
    f32 c;
    f32 c2;
    f32 base;

    work = nanNPCWork;
    for (i = 0; i < work->count; i++) {
        entry = &work->entries[i];
        radius = (f32)(irand(0xB4) + 0x78);
        angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
        s = sin(angle);
        c = cos(angle);
        entry->hoseiPosition.x = 0.9f * (radius * s);
        entry->hoseiPosition.y = 15.0f;
        entry->hoseiPosition.z = 0.5f * (radius * c) - 300.0f;
        if (entry->hoseiPosition.x < 30.0f) {
            base = -280.0f;
            entry->facingAngle = 90.0f;
        } else {
            base = 340.0f;
            entry->facingAngle = 270.0f;
        }
        radius = (f32)irand(0x64);
        // The named product keeps the eight float constants in the original's f28-f31/f24-f27 registers.
        {
            f32 tmp = 6.2831855f * (f32)irand(0x168);
            angle = tmp / 360.0f;
        }
        s = sin(angle);
        c2 = cos(angle);
        entry->position.x = radius * s + base;
        entry->position.y = 15.0f;
        entry->position.z = radius * c2 - 300.0f;
        entry->callback = gundan1000tai_seq00;
        entry->state = 0;
        entry->index = i;
    }
    return EVT_RETURN_DONE;
}

#pragma opt_lifetimes reset
USER_FUNC(gundan1000tai_totsugeki) {
    s32 i;
    NanNpcEntry* entry;
    NanNpcWork* work;

    work = nanNPCWork;
    for (i = 0; i < work->count; i++) {
        entry = &work->entries[i];
        entry->callback = gundan1000tai_seq01;
        entry->state = 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(gundan1000tai_areacheck) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(1);
    s32 area;

    if (mario->position.y > 70.0f) {
        area = 0;
    } else if (mario->position.x < 60.0f) {
        area = 0x10;
    } else if (mario->position.x < 80.0f) {
        area = 4;
    } else if (mario->position.x < 100.0f) {
        area = 2;
    } else if (mario->position.x < 120.0f) {
        area = 1;
    } else {
        area = 0;
    }
    evtSetValue(event, args[0], area);

    if (party->position.y > 70.0f) {
        area = 0;
    } else if (party->position.x < 60.0f) {
        area = 0x10;
    } else if (party->position.x < 80.0f) {
        area = 4;
    } else if (party->position.x < 100.0f) {
        area = 2;
    } else if (party->position.x < 120.0f) {
        area = 1;
    } else {
        area = 0;
    }
    evtSetValue(event, args[1], area);
    return EVT_RETURN_DONE;
}

USER_FUNC(gundan1000tai_totsuend) {
    s32 i;
    NanNpcWork* work = nanNPCWork;

    for (i = 0; i < work->count; i++) {
        work->entries[i].endFlag = 1;
    }
    return EVT_RETURN_DONE;
}

void gundan1000tai_seq00(NanNpcEntry* entry) {
    marioGetPtr();
    switch (entry->state) {
    case 0: {
        s32 tmp;

        entry->startTime = gp->renderTime;
        tmp = (entry->index % 6) * 500;
        entry->timer = tmp + irand(500);
        entry->state++;
        // fallthrough
    }
    case 1: {
        s32 elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);

        if (elapsed > entry->timer) {
            entry->state++;
        }
        break;
    }
    case 2: {
        f32 dist = distABf(entry->position.x, entry->position.z, entry->hoseiPosition.x, entry->hoseiPosition.z);
        f32 angle = angleABf(entry->position.x, entry->position.z, entry->hoseiPosition.x, entry->hoseiPosition.z);
        f32 half = 0.5f;
        f32 scaled = (f32)(irand(0xB4) + 0xB4) + dist * half;
        f32 duration = sysFrame2SecFloat(scaled * half);

        entry->field_0x78 = duration;
        entry->field_0x7C = angle;
        entry->field_0x70 = (s32)(dist / duration);
        entry->timer = 0;
        entry->moveDone = 0;
        evt_nannpc_set_animtbl_sub(entry, (s16*)dai3_run_animtbl);
        entry->state++;
        // fallthrough
    }
    case 3:
        movePos(&entry->position.x, &entry->position.z, entry->field_0x78, entry->field_0x7C);
        entry->timer++;
        if (entry->timer >= entry->field_0x70) {
            entry->position.x = entry->hoseiPosition.x;
            entry->position.z = entry->hoseiPosition.z;
            entry->moveDone = 1;
            evt_nannpc_set_animtbl_sub(entry, (s16*)dai3_animtbl);
            entry->state++;
        }
        break;
    case 4:
        entry->basePos = entry->position;
        break;
    }
}

void gundan1000tai_seq01(NanNpcEntry* entry) {
    marioGetPtr();
    switch (entry->state) {
    case 0: {
        s32 r;

        entry->basePos = entry->position;
        entry->moveDone = 0;
        entry->endFlag = 0;
        r = irand(0x3E8);
        if (r < 0x32) {
            entry->state = 10;
            break;
        }
        if (r < 0x96) {
            entry->state = 1;
            entry->timer = irand(0x1F4) + 0x12C;
            entry->startTime = gp->renderTime;
        } else {
            entry->state = 1;
            entry->timer = irand(0x9C4) + 0x1F4;
            entry->startTime = gp->renderTime;
        }
        break;
    }
    case 1: {
        s32 elapsed;

        if (entry->endFlag != 0) {
            entry->state = 100;
            break;
        }
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed > entry->timer) {
            evt_nannpc_set_animtbl_sub(entry, (s16*)dai3_run_animtbl);
            entry->state = 10;
        }
        break;
    }
    case 10:
    case 11: {
        f32 radius;
        f32 s;
        f32 angle;
        f32 c;

        entry->startTime = gp->renderTime;
        entry->hoseiPosition = entry->position;
        if (entry->state == 10) {
            radius = (f32)(irand(0x19) + 0x19);
        } else {
            radius = (f32)(irand(0xB4) + 0x78);
        }
        angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
        s = sin(angle);
        c = cos(angle);
        entry->targetPos.x = 0.9f * (radius * s) + 30.0f;
        entry->targetPos.y = 15.0f;
        entry->targetPos.z = 0.5f * (radius * c) - 300.0f;
        entry->field_0x78 = -4.0f * (f32)(irand(0x3C) + 0x32);
        entry->field_0x7C = (entry->targetPos.y - entry->hoseiPosition.y) - entry->field_0x78;
        entry->timer = irand(0x1F4) + 0x258;
        if (entry->state == 10) {
            entry->field_0x70 = 0x15;
            if (entry->hoseiPosition.x < entry->targetPos.x) {
                entry->facingAngle = 90.0f;
            } else {
                entry->facingAngle = 270.0f;
            }
        } else {
            entry->field_0x70 = 0x16;
            if (entry->hoseiPosition.x < entry->targetPos.x) {
                entry->facingAngle = 270.0f;
            } else {
                entry->facingAngle = 90.0f;
            }
        }
        entry->state = 20;
        evt_nannpc_set_animtbl_sub(entry, (s16*)dai3_run_animtbl);
        break;
    }
    case 20: {
        s32 elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);

        if (elapsed < entry->timer) {
            f32 t = (f32)elapsed / (f32)entry->timer;

            entry->position.x = intplGetValue(0, elapsed, entry->timer, entry->hoseiPosition.x, entry->targetPos.x);
            entry->position.z = intplGetValue(0, elapsed, entry->timer, entry->hoseiPosition.z, entry->targetPos.z);
            entry->position.y = entry->hoseiPosition.y + t * (entry->field_0x7C + entry->field_0x78 * t);
        } else {
            entry->position = entry->targetPos;
            entry->state = entry->field_0x70;
            entry->startTime = gp->renderTime;
            entry->timer = irand(0x7D0) + 0x1F4;
        }
        break;
    }
    case 21: {
        s32 elapsed;

        if (entry->endFlag != 0) {
            entry->state = 100;
            break;
        }
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed > entry->timer) {
            entry->state = 11;
        }
        break;
    }
    case 22:
        if (entry->endFlag != 0) {
            entry->state = 102;
            break;
        }
        entry->timer = irand(0x7D0) + 0x1F4;
        entry->startTime = gp->renderTime;
        entry->basePos = entry->position;
        entry->field_0x78 = 15.0f + entry->basePos.x;
        entry->field_0x7C = entry->basePos.x - 15.0f;
        if (irand(1) != 0) {
            entry->stepX = 1.0f;
            entry->facingAngle = 90.0f;
        } else {
            entry->stepX = -1.0f;
            entry->facingAngle = 270.0f;
        }
        entry->state = 30;
        break;
    case 30: {
        s32 elapsed;

        if (entry->endFlag != 0) {
            entry->state = 102;
            break;
        }
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed > entry->timer) {
            entry->state = 10;
            break;
        }
        if (entry->field_0x78 < entry->position.x) {
            entry->stepX = -1.0f;
            entry->facingAngle = 270.0f;
        }
        if (entry->field_0x7C > entry->position.x) {
            entry->stepX = 1.0f;
            entry->facingAngle = 90.0f;
        }
        entry->position.x += entry->stepX;
        break;
    }
    case 100: {
        f32 angle;
        f32 s;
        f32 radius;
        f32 c;
        f32 dist;
        f32 a;

        radius = (f32)(irand(0xB4) + 0x78);
        angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
        s = sin(angle);
        c = cos(angle);
        entry->targetPos.x = 0.9f * (radius * s) + 30.0f;
        entry->targetPos.z = 0.5f * (radius * c) - 300.0f;
        dist = distABf(entry->position.x, entry->position.z, entry->hoseiPosition.x, entry->hoseiPosition.z);
        a = angleABf(entry->position.x, entry->position.z, entry->hoseiPosition.x, entry->hoseiPosition.z);
        {
            f32 half = 0.5f;
            f32 duration = sysFrame2SecFloat((f32)(irand(0xB4) + 0xB4) + dist * half);

            entry->field_0x78 = duration;
            entry->field_0x7C = a;
            entry->field_0x70 = (s32)(dist / duration);
        }
        entry->timer = 0;
        entry->state = 101;
        // fallthrough
    }
    case 101:
        movePos(&entry->position.x, &entry->position.z, entry->field_0x78, entry->field_0x7C);
        entry->timer++;
        if (entry->timer >= entry->field_0x70) {
            entry->position.x = entry->hoseiPosition.x;
            entry->position.z = entry->hoseiPosition.z;
            entry->state = 102;
        }
        break;
    case 102:
        evt_nannpc_set_animtbl_sub(entry, (s16*)dai3_animtbl);
        entry->moveDone = 1;
        break;
    }
}

// Taken from the identical matched functions in win_06.c/tik_05.c: the dx/dz locals (rather than the
// inlined -(b.x - a.x)) plus the Vec declaration order give the original's register allocation.
static USER_FUNC(cam_kaiten_init) {
    s32* args = event->args;
    Vec a;
    Vec p;
    f32 dist;
    f32 angle;
    f32 dx;
    f32 dz;

    ckp0 = _mapAlloc(0x20);
    memset(ckp0, 0, 0x20);
    ckp1 = _mapAlloc(0x20);
    memset(ckp1, 0, 0x20);
    ckpw = _mapAlloc(0x20);
    memset(ckp1, 0, 0x20);

    ckp0[0] = evtGetFloat(event, args[0]);
    ckp0[1] = evtGetFloat(event, args[1]);
    ckp0[2] = evtGetFloat(event, args[2]);
    ckp0[3] = evtGetFloat(event, args[3]);
    ckp0[4] = evtGetFloat(event, args[4]);
    ckp0[5] = evtGetFloat(event, args[5]);
    ckp1[0] = evtGetFloat(event, args[6]);
    ckp1[1] = evtGetFloat(event, args[7]);
    ckp1[2] = evtGetFloat(event, args[8]);
    ckp1[3] = evtGetFloat(event, args[9]);
    ckp1[4] = evtGetFloat(event, args[10]);
    ckp1[5] = evtGetFloat(event, args[11]);

    p = *(Vec*)&ckp0[0];
    a = *(Vec*)&ckp0[3];
    a.y = 0.0f;
    p.y = 0.0f;
    dist = PSVECDistance(&p, &a);
    dx = a.x - p.x;
    dz = a.z - p.z;
    angle = 57.29579f * (f32)atan2(-dx, dz);
    ckp0[6] = angle;
    ckp0[7] = dist;

    p = *(Vec*)&ckp1[0];
    a = *(Vec*)&ckp1[3];
    a.y = 0.0f;
    p.y = 0.0f;
    dist = PSVECDistance(&p, &a);
    dx = a.x - p.x;
    dz = a.z - p.z;
    angle = 57.29579f * (f32)atan2(-dx, dz);
    ckp1[6] = angle;
    ckp1[7] = dist;

    *(Vec*)&ckpw[0] = *(Vec*)&ckp0[0];
    *(Vec*)&ckpw[3] = *(Vec*)&ckp0[3];
    ckpw[6] = ckp0[6];
    ckpw[7] = ckp0[7];
    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_set_direct) {
    CameraEntry* cam = camGetPtr(4);
    f32 s = sin(6.2831855f * ckpw[6] / 360.0f);
    f32 c = cos(6.2831855f * ckpw[6] / 360.0f);

    ckpw[0] = ckpw[7] * s + ckpw[3];
    ckpw[2] = ckpw[5] - ckpw[7] * c;
    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = *(Vec*)ckpw;
    cam->storedAt = *(Vec*)&ckpw[3];
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 0xB;
    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_get_para) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    f32* p;

    switch (index) {
    case 0:
        p = ckp0;
        break;
    case 1:
        p = ckp1;
        break;
    case 2:
    default:
        p = ckpw;
        break;
    }
    evtSetFloat(event, args[1], p[0]);
    evtSetFloat(event, args[2], p[1]);
    evtSetFloat(event, args[3], p[2]);
    evtSetFloat(event, args[4], p[3]);
    evtSetFloat(event, args[5], p[4]);
    evtSetFloat(event, args[6], p[5]);
    evtSetFloat(event, args[7], p[6]);
    evtSetFloat(event, args[8], p[7]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_set_para) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    f32* p;

    switch (index) {
    case 0:
        p = ckp0;
        break;
    case 1:
        p = ckp1;
        break;
    case 2:
    default:
        p = ckpw;
        break;
    }
    p[0] = evtGetFloat(event, args[1]);
    p[1] = evtGetFloat(event, args[2]);
    p[2] = evtGetFloat(event, args[3]);
    p[3] = evtGetFloat(event, args[4]);
    p[4] = evtGetFloat(event, args[5]);
    p[5] = evtGetFloat(event, args[6]);
    p[6] = evtGetFloat(event, args[7]);
    p[7] = evtGetFloat(event, args[8]);
    return EVT_RETURN_DONE;
}

USER_FUNC(hosei_mario_pos) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    MarioWork* mario = marioGetPtr();

    mario->position.x += x;
    mario->position.y += y;
    mario->position.z += z;
    return EVT_RETURN_DONE;
}

USER_FUNC(hosei_party_pos) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    PartyEntry* party = partyGetPtr(partyCtrlNo);

    if (party == NULL) {
        return EVT_RETURN_DONE;
    }
    party->position.x += x;
    party->position.y += y;
    party->position.z += z;
    return EVT_RETURN_DONE;
}

USER_FUNC(suifu100taikikan_init) {
    s32 i;
    NanNpcEntry* entry;
    NanNpcWork* work;

    work = nanNPCWork;
    for (i = 0; i < work->count; i++) {
        entry = &work->entries[i];

        entry->facingAngle = 90.0f;
        entry->position.x = (f32)(irand(0x87) - 100);
        entry->position.y = 15.0f;
        entry->position.z = (f32)(irand(0x32) + 190);
        entry->callback = suifu100taikikan_seq00;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(suifu100taikikan_away) {
    s32 i;
    NanNpcEntry* entry;
    NanNpcWork* work;

    work = nanNPCWork;
    for (i = 0; i < work->count; i++) {
        entry = &work->entries[i];

        entry->facingAngle = 90.0f;
        entry->callback = suifu100taikikan_seq01;
        entry->state = 0;
    }
    return EVT_RETURN_DONE;
}

void suifu100taikikan_seq00(NanNpcEntry* entry) {
    entry->state = 0;
}

void suifu100taikikan_seq01(NanNpcEntry* entry) {
    switch (entry->state) {
    case 0:
        entry->hoseiPosition = entry->position;
        entry->hoseiPosition.x = -180.0f + (f32)irand(0x32);
        entry->field_0x78 = 140.0f + (f32)irand(0x78);
        entry->facingAngle = 270.0f;
        entry->field_0x70 = 1;
        entry->state = 100;
        break;
    case 1:
        entry->hoseiPosition = entry->position;
        entry->hoseiPosition.x = -255.0f + (f32)irand(0x1E);
        entry->hoseiPosition.z = -230.0f + (f32)irand(0x1E);
        entry->field_0x70 = 10;
        entry->state = 100;
        break;
    case 10:
        entry->hoseiPosition = entry->position;
        entry->hoseiPosition.z = -350.0f + (f32)irand(0x32);
        entry->field_0x70 = 20;
        entry->state = 100;
        break;
    case 20:
        entry->hoseiPosition = entry->position;
        entry->hoseiPosition.x += 520.0f + (f32)irand(0x32);
        entry->facingAngle = 90.0f;
        entry->field_0x70 = 30;
        entry->state = 100;
        break;
    case 30:
        entry->hoseiPosition = *(Vec*)&suifu100kaidanDOWN[irand(3) * 3];
        entry->field_0x70 = 40;
        entry->state = 100;
        break;
    case 40:
        entry->hoseiPosition = *(Vec*)&suifu100kaidanUP[irand(3) * 3];
        entry->facingAngle = 270.0f;
        entry->field_0x70 = 50;
        entry->state = 100;
        break;
    case 50:
        entry->hoseiPosition = entry->position;
        entry->hoseiPosition.x = -30.0f + (f32)irand(0x3C);
        entry->field_0x70 = 60;
        entry->state = 100;
        break;
    case 60:
        entry->hoseiPosition = entry->position;
        entry->hoseiPosition.z -= 100.0f;
        entry->field_0x70 = 99;
        entry->state = 100;
        break;
    case 99:
        entry->position = vec3_gor_00013858;
        break;
    case 100:
        entry->startTime = gp->renderTime;
        entry->basePos = entry->position;
        entry->state = 101;
        // fallthrough
    case 101: {
        Vec v;
        Vec out;
        f32 mag;
        s32 elapsed;
        s32 total;

        PSVECSubtract(&entry->hoseiPosition, &entry->basePos, &v);
        mag = PSVECMag(&v);
        total = (s32)(1000.0f * mag / entry->field_0x78);
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        PSVECScale(&v, &out, (f32)elapsed / (f32)total);
        if (elapsed < total) {
            PSVECAdd(&entry->basePos, &out, &entry->position);
        } else {
            entry->position = entry->hoseiPosition;
            entry->state = entry->field_0x70;
        }
        break;
    }
    }
}

void gor_stg4_018_cb(s32 type, smartEntry* smart) {
    MessageData* data = smart->address;

    switch (type) {
    case 3:
        if (data->choiceIndex == 1) {
            evtSetValue(NULL, LSWF(6), 1);
        }
        break;
    }
}

static USER_FUNC(fune_eff) {
    f32 oldX;
    f32 oldZ;
    f32 dist;
    f32 angle;
    f32 sign;
    EffectEntry* e1;
    EffectEntry* e2;
    Vec pos;

    if (isFirstCall) {
        event->userdata[0] = (s32)effFunemizuEntry(0, 0.0f, -1000.0f, 0.0f, 0.0f, 0.0f);
        event->userdata[1] = (s32)effFunemizuEntry(2, 0.0f, -1000.0f, 0.0f, 0.0f, 0.0f);
        mapObjGetPos("S_fune_a", &pos);
        event->userdataf[2] = pos.x;
        *(f32*)&event->field_84 = pos.z;
    }

    e1 = (EffectEntry*)event->userdata[0];
    e2 = (EffectEntry*)event->userdata[1];
    oldX = event->userdataf[2];
    oldZ = *(f32*)&event->field_84;
    mapObjGetPos("S_fune_a", &pos);
    event->userdataf[2] = pos.x;
    *(f32*)&event->field_84 = pos.z;
    dist = distABf(oldX, oldZ, pos.x, pos.z);
    angle = angleABf(oldX, oldZ, pos.x, pos.z);
    sign = (angle < 180.0f) ? 1.0f : -1.0f;

    ((EffFunemizuData*)e1->userdata)->scale = 3.0f;
    ((EffFunemizuData*)e1->userdata)->x = 85.0f * sign + pos.x;
    ((EffFunemizuData*)e1->userdata)->y = pos.y - 51.0f;
    ((EffFunemizuData*)e1->userdata)->z = 30.0f + pos.z;
    ((EffFunemizuData*)e1->userdata)->speed = dist;
    if (angle < 180.0f) {
        ((EffFunemizuData*)e1->userdata)->yaw = 135.0f + angle;
    } else {
        ((EffFunemizuData*)e1->userdata)->yaw = 45.0f + angle;
    }

    ((EffFunemizuData*)e2->userdata)->x = pos.x - 15.0f * sign;
    ((EffFunemizuData*)e2->userdata)->y = pos.y - 51.0f;
    ((EffFunemizuData*)e2->userdata)->z = 90.0f + pos.z;
    ((EffFunemizuData*)e2->userdata)->speed = dist;
    ((EffFunemizuData*)e2->userdata)->yaw = 90.0f + angle;
    if (dist > 1.0f && gp->retraceCount % 15 == 0) {
        effWaterDamageN64Entry(5, 30, pos.x - 150.0f * sign, pos.y - 50.0f, pos.z, 2.0f);
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(gor_ship_puka_func) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    f32 value = evtGetFloat(event, args[1]);

    evtSetFloat(event, args[2], value * gor_ship_puka_data[index & 0xF]);
    return EVT_RETURN_DONE;
}

static USER_FUNC(scale_func) {
    f32 scale = evtGetFloat(event, event->args[0]);
    MarioWork* mario = marioGetPtr();
    PartyEntry* party0 = partyGetPtr(0);
    PartyEntry* party1 = partyGetPtr(1);
    Vec v0;
    Vec v1;
    Vec v2;

    v0 = vec3_gor_00013c54;
    v0.x = scale;
    v0.y = scale;
    v0.z = scale;
    mario->scale = v0;

    if (party0 != NULL) {
        v1 = vec3_gor_00013c60;
        v1.x = scale;
        v1.y = scale;
        v1.z = scale;
        party0->scale = v1;
    }
    if (party1 != NULL) {
        v2 = vec3_gor_00013c6c;
        v2.x = scale;
        v2.y = scale;
        v2.z = scale;
        party1->scale = v2;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(funeflag_save) {
    MapObject* obj0 = mapGetMapObj("S_fune_b");
    MapObject* obj1 = mapGetMapObj("S_fune_c");

    event->lwData[8] = obj0->flags & 1;
    event->lwData[9] = obj1->flags & 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(funeflag_reset) {
    mapGrpFlagOn("S_fune_b", 1);
    mapGrpFlagOn("S_fune_c", 1);
    return EVT_RETURN_DONE;
}

USER_FUNC(funeflag_load) {
    mapGrpFlagOff("S_fune_b", 1);
    mapGrpFlagOn("S_fune_b", event->lwData[8]);
    mapGrpFlagOff("S_fune_c", 1);
    mapGrpFlagOn("S_fune_c", event->lwData[9]);
    return EVT_RETURN_DONE;
}

USER_FUNC(funechange_func) {
    s32* args = event->args;

    evtSetValue(event, args[0], marioShipChanging());
    return EVT_RETURN_DONE;
}

static USER_FUNC(get_yoshi_color) {
    s32* args = event->args;

    evtSetValue(event, args[0], pouchGetPartyColor(PARTNER_YOSHI));
    return EVT_RETURN_DONE;
}

USER_FUNC(epilogue_npc_clear) {
    NpcWork* work = npcGetWorkPtr();
    s32 i;
    s32 count = work->total;
    NpcEntry* entry = work->entries;

    for (i = 0; i < count; i++, entry++) {
        if (entry->flags & 1) {
            if (entry->moveEvent != NULL) {
                if (entry->regularEvtId != 0) {
                    evtDeleteID(entry->regularEvtId);
                }
                entry->regularEvtId = 0;
                entry->moveEvent = NULL;
            }
            npcDelete(entry);
        }
    }
    return EVT_RETURN_DONE;
}

// clang-format on
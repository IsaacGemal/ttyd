#include "muj_00.h"
#include "eff/eff_mahojin.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
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
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "math_trig.h"
#include "memory.h"
#include "party.h"
#include "party/party_motion.h"
#include "system.h"
#include "math_ext.h"
#include "driver/mapdrv.h"
#include "driver/hitdrv.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern const Vec vec3_muj_0000e4cc;
extern const Vec vec3_muj_0000e4d8;
extern const Vec vec3_muj_0000e4e4;

#pragma pool_data off

#pragma force_active on
extern s32 muj_ship_puka[];
static USER_FUNC(scale_func);
static USER_FUNC(alpha_set_func);
static USER_FUNC(alpha_init_func);
static void alpha_tev(MapTevCallback* param);
s32 koburon_talk[87];
static s32 dai3_shurai_after[68];
s32 dai3_beat_back[194];
s32 muj_gor_ship_start[245];

// One per tobikakari entry, in the Tobidat block tobikakaru_init allocates; entries jump between these spots.
struct TobikakariSpot {
    Vec home;  // 0x0, the entry's starting position
    s32 group; // 0xC, 0 (suifu run anims), 1, or 2 (dai3 run anims); groups 0 and 1 jump at 2, and 2 at 0 or 1
};

// .bss is laid out last-declared-first: the original has Tobidat, _op, _ap and four trailing bytes.
static u32 gap_05_0000000C_bss;
static u8* _ap;
OpField* _op;
TobikakariSpot* Tobidat;

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

const GXColor zero_muj_0000db40 = {0, 0, 0, 0};

// TODO: type
static u32 ext_entry_data[177] = {
    (u32)"c_boro_k", (u32)"S_1", 0x00000000, (u32)"c_boro_k",
    (u32)"S_1", 0x40400000, (u32)"c_boro_k", (u32)"S_1",
    0x40C00000, (u32)"c_boro_k", (u32)"S_1", 0x41100000,
    (u32)"c_boro_k", (u32)"S_1", 0x41400000, (u32)"c_boro_k",
    (u32)"S_1", 0x41700000, (u32)"c_boro_k", (u32)"S_1",
    0x41900000, (u32)"c_boro_k", (u32)"S_1", 0x41A80000,
    (u32)"c_boro_k", (u32)"S_1", 0x41C00000, (u32)"c_boro_k",
    (u32)"S_1", 0x41D80000, (u32)"c_boro_k", (u32)"S_1",
    0x41F00000, (u32)"c_boro_k", (u32)"S_1", 0x42040000,
    (u32)"c_boro_k", (u32)"S_1", 0x42100000, (u32)"c_boro_k",
    (u32)"S_1", 0x421C0000, (u32)"c_boro_k", (u32)"R_1",
    0x00000000, (u32)"c_boro_k", (u32)"R_1", 0x40000000,
    (u32)"c_boro_k", (u32)"R_1", 0x40800000, (u32)"c_boro_k",
    (u32)"R_1", 0x40C00000, (u32)"c_boro_k", (u32)"R_1",
    0x41000000, (u32)"c_boro_k", (u32)"R_1", 0x41200000,
    (u32)"c_boro_k", (u32)"R_1", 0x41400000, (u32)"c_boro_k",
    (u32)"R_1", 0x41600000, (u32)"c_boro_k", (u32)"R_1",
    0x41800000, (u32)"c_boro_k", (u32)"R_1", 0x41900000,
    (u32)"c_elmos", (u32)"S_1", 0x00000000, (u32)"c_elmos",
    (u32)"S_1", 0x40000000, (u32)"c_elmos", (u32)"S_1",
    0x40800000, (u32)"c_elmos", (u32)"S_1", 0x40C00000,
    (u32)"c_elmos", (u32)"S_1", 0x41000000, (u32)"c_elmos",
    (u32)"S_1", 0x41200000, (u32)"c_elmos", (u32)"S_1",
    0x41400000, (u32)"c_elmos", (u32)"S_1", 0x41600000,
    (u32)"c_elmos", (u32)"S_1", 0x41800000, (u32)"c_elmos",
    (u32)"S_1", 0x41900000, (u32)"c_zako_n", (u32)"S_1",
    0x00000000, (u32)"c_zako_n", (u32)"S_1", 0x40400000,
    (u32)"c_zako_n", (u32)"S_1", 0x40C00000, (u32)"c_zako_n",
    (u32)"S_1", 0x41100000, (u32)"c_zako_n", (u32)"S_1",
    0x41400000, (u32)"c_zako_n", (u32)"S_1", 0x41700000,
    (u32)"c_zako_n", (u32)"S_1", 0x41900000, (u32)"c_zako_n",
    (u32)"S_1", 0x41A80000, (u32)"c_zako_n", (u32)"S_1",
    0x41C00000, (u32)"c_zako_n", (u32)"S_1", 0x41D80000,
    (u32)"c_zako_n", (u32)"S_1", 0x41F00000, (u32)"c_zako_n",
    (u32)"S_1", 0x42040000, (u32)"c_zako_n", (u32)"S_1",
    0x42100000, (u32)"c_zako_n", (u32)"S_1", 0x421C0000,
    (u32)"c_zako_n", (u32)"R_1", 0x00000000, (u32)"c_zako_n",
    (u32)"R_1", 0x40000000, (u32)"c_zako_n", (u32)"R_1",
    0x40800000, (u32)"c_zako_n", (u32)"R_1", 0x40C00000,
    (u32)"c_zako_n", (u32)"R_1", 0x41000000, (u32)"c_zako_n",
    (u32)"R_1", 0x41200000, (u32)"c_zako_n", (u32)"R_1",
    0x41400000, (u32)"c_zako_n", (u32)"R_1", 0x41600000,
    (u32)"c_zako_n", (u32)"R_1", 0x41800000, (u32)"c_zako_n",
    (u32)"R_1", 0x41900000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
static u32 suifu_animtbl[21] = {
    0x00000000, 0x00000001, 0x00010001, 0x00020002,
    0x00020003, 0x00030003, 0x00040004, 0x00040005,
    0x00050005, 0x00060006, 0x00060007, 0x00070007,
    0x00080008, 0x00080009, 0x00090009, 0x000A000A,
    0x000A000B, 0x000B000B, 0x000C000C, 0x000C000D,
    0xFFFF0000,
};

// TODO: type
u32 suifu_animtbl_run[11] = {
    0x000E000E, 0x000F000F, 0x00100010, 0x00110011,
    0x00120012, 0x00130013, 0x00140014, 0x00150015,
    0x00160016, 0x00170017, 0xFFFF0000,
};

// TODO: type
static u32 elmos_animtbl[11] = {
    0x00180018, 0x00190019, 0x001A001A, 0x001B001B,
    0x001C001C, 0x001D001D, 0x001E001E, 0x001F001F,
    0x00200020, 0x00210021, 0xFFFF0000,
};

// TODO: type
static u32 dai3_animtbl[21] = {
    0x00220022, 0x00220023, 0x00230023, 0x00240024,
    0x00240025, 0x00250025, 0x00260026, 0x00260027,
    0x00270027, 0x00280028, 0x00280029, 0x00290029,
    0x002A002A, 0x002A002B, 0x002B002B, 0x002C002C,
    0x002C002D, 0x002D002D, 0x002E002E, 0x002E002F,
    0xFFFF0000,
};

// TODO: type
u32 dai3_animtbl_run[11] = {
    0x00300030, 0x00310031, 0x00320032, 0x00330033,
    0x00340034, 0x00350035, 0x00360036, 0x00370037,
    0x00380038, 0x00390039, 0xFFFF0000,
};

static EVT_BEGIN(nannpc_mode_setup)
    CALL(evt_nannpc_init, PTR(ext_entry_data), 0, 0, 0, 305)
    CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
    SET_VALUE(LW(0), 0)
    LOOP(10)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(suifu_animtbl))
        CALL(evt_nannpc_flag_onoff, LW(0), 0, 2)
        ADD(LW(0), 1)
    END_LOOP
    SET_VALUE(LW(0), 10)
    LOOP(95)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(suifu_animtbl))
        ADD(LW(0), 1)
    END_LOOP
    SET_VALUE(LW(0), 105)
    LOOP(100)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(elmos_animtbl))
        ADD(LW(0), 1)
    END_LOOP
    SET_VALUE(LW(0), 205)
    LOOP(100)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(dai3_animtbl))
        ADD(LW(0), 1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(back_dark)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, LW(10))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(back_light)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, LW(10))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(suifu_dark)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, LW(10))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 10)
        LOOP(95)
            CALL(evt_nannpc_set_color, LW(2), LW(0), LW(0), LW(0), 255)
            ADD(LW(2), 1)
        END_LOOP
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(nannpc_light)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, LW(10))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        LOOP(305)
            CALL(evt_nannpc_set_color, LW(2), LW(0), LW(0), LW(0), 255)
            ADD(LW(2), 1)
        END_LOOP
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(garawaru_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(257, 258)
        CALL(evt_npc_set_position, STRING("me"), 1386, 35, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(garawaru_talk)
    CALL(evt_msg_print, 0, STRING("stg5_muj_117_01"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(marco_init)
    RETURN
EVT_END()

static EVT_BEGIN(marco_talk)
    RETURN
EVT_END()

static EVT_BEGIN(odoodo_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(257, 258)
        CALL(evt_npc_set_position, STRING("me"), 1400, 35, -100)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(odoodo_talk)
    CALL(evt_msg_print, 0, STRING("stg5_muj_117_00"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(suifu_a_init)
    RETURN
EVT_END()

static EVT_BEGIN(suifu_a_talk)
    RETURN
EVT_END()

static EVT_BEGIN(suifu_bomb_init)
    RETURN
EVT_END()

static EVT_BEGIN(suifu_bomb_talk)
    RETURN
EVT_END()

static EVT_BEGIN(suifu_c_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(257, 258)
        CALL(evt_npc_set_position, STRING("me"), 1325, 35, -225)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_c_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(256)
        CALL(evt_msg_print, 0, STRING("stg5_muj_94_01"), 0, STRING("me"))
        CASE_BETWEEN(257, 258)
        CALL(evt_msg_print, 0, STRING("stg5_muj_117_04"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_d_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(257, 258)
        CALL(evt_npc_set_position, STRING("me"), 1350, 35, -150)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_d_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(256)
        CALL(evt_msg_print, 0, STRING("stg5_muj_94_02"), 0, STRING("me"))
        CASE_BETWEEN(257, 258)
        CALL(evt_msg_print, 0, STRING("stg5_muj_117_05"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_e_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(257, 258)
        CALL(evt_npc_set_position, STRING("me"), 1250, 35, -175)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_e_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(256)
        CALL(evt_msg_print, 0, STRING("stg5_muj_94_03"), 0, STRING("me"))
        CASE_BETWEEN(257, 258)
        CALL(evt_msg_print, 0, STRING("stg5_muj_117_06"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(korutesu_init)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(267)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(korutesu_talk)
    SWITCH(GSW(0))
        CASE_GREATER_OR_EQUAL(268)
        CALL(evt_msg_print, 0, STRING("hune_00"), 0, STRING("me"))
        CALL(evt_msg_select, 0, STRING("hune_00_yesno"))
        IF_INT_EQUAL(LW(0), 0)
            CALL(evt_msg_print_add, 0, STRING("hune_02"))
            SET_VALUE(GSWF(1178), 1)
            CALL_CHILD(muj_gor_ship_start)
        ELSE
            CALL(evt_msg_print_add, 0, STRING("hune_01"))
        END_IF
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(mony_init)
    RETURN
EVT_END()

static EVT_BEGIN(mony_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(256)
        CALL(evt_msg_print, 0, STRING("stg5_muj_94_04"), 0, STRING("me"))
        CASE_BETWEEN(257, 258)
        CALL(evt_msg_print, 0, STRING("stg5_muj_117_07"), 0, STRING("me"))
        CASE_EQUAL(277)
        CALL(evt_msg_print, 0, STRING("stg5_muj_146_02"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(peton_init)
    RETURN
EVT_END()

static EVT_BEGIN(peton_talk)
    SWITCH(GSW(0))
        CASE_EQUAL(256)
        CALL(evt_msg_print, 0, STRING("stg5_muj_94_05"), 0, STRING("me"))
        CASE_BETWEEN(257, 258)
        CALL(evt_msg_print, 0, STRING("stg5_muj_117_08"), 0, STRING("me"))
        CASE_EQUAL(277)
        CALL(evt_msg_print, 0, STRING("stg5_muj_146_01"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(koburon_init)
    CALL(evt_npc_reaction_flag_onoff, 0, STRING("me"), 15)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_2"))
    CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
    CALL(evt_npc_set_position, STRING("me"), LW(3), -1000, LW(5))
    CALL(evt_npc_change_interrupt, STRING("me"), 6, 0)
    SET_VALUE(LSWF(1), 0)
    RETURN
EVT_END()

EVT_BEGIN(koburon_regp)
    CALL(L_evt_mario_keyon_wait)
    LABEL(0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
    CALL(evt_sub_get_dist, LW(0), LW(2), LW(3), LW(5), LW(1))
    IF_INT_GREATER(LW(1), 100)
        IF_INT_EQUAL(LSWF(1), 1)
            CALL(evt_npc_change_interrupt, STRING("me"), 6, 0)
            CALL(evt_npc_reaction_flag_onoff, 0, STRING("me"), 15)
            CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_KOBURON_IN1"), LW(3), LW(4), LW(5), 0)
            CALL(evt_npc_set_anim, STRING("me"), STRING("A_2"))
            WAIT_MS(300)
            CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
            CALL(evt_npc_set_position, STRING("me"), LW(3), -1000, LW(5))
            SET_VALUE(LSWF(1), 0)
        END_IF
        CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
        CALL(evt_npc_set_position, STRING("me"), LW(3), -1000, LW(5))
    ELSE
        IF_INT_EQUAL(LSWF(1), 0)
            CALL(evt_npc_get_position, STRING("me"), LW(3), LW(4), LW(5))
            CALL(evt_npc_set_position, STRING("me"), LW(3), 35, LW(5))
            CALL(evt_npc_set_anim, STRING("me"), STRING("A_1"))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_KOBURON_OUT1"), LW(3), 35, LW(5), 0)
            SET_VALUE(LSWF(1), 1)
            WAIT_MS(500)
            CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
            CALL(evt_npc_reaction_flag_onoff, 1, STRING("me"), 15)
            CALL(evt_npc_change_interrupt, STRING("me"), 6, PTR(koburon_talk))
        END_IF
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(koburon_talk)
    IF_INT_EQUAL(GSWF(3151), 0)
        CALL(evt_msg_print, 0, STRING("koburon_01"), 0, STRING("me"))
        SET_VALUE(GSWF(3151), 1)
    ELSE
        SWITCH(GSW(721))
            CASE_EQUAL(0)
            CALL(evt_msg_print, 0, STRING("koburon_02"), 0, STRING("me"))
            CASE_EQUAL(1)
            CALL(evt_msg_print, 0, STRING("koburon_03"), 0, STRING("me"))
            CASE_EQUAL(2)
            CALL(evt_msg_print, 0, STRING("koburon_04"), 0, STRING("me"))
            CASE_EQUAL(3)
            CALL(evt_msg_print, 0, STRING("koburon_05"), 0, STRING("me"))
            CASE_EQUAL(4)
            CALL(evt_msg_print, 0, STRING("koburon_06"), 0, STRING("me"))
            CASE_EQUAL(5)
            CALL(evt_msg_print, 0, STRING("koburon_07"), 0, STRING("me"))
            CASE_EQUAL(6)
            CALL(evt_msg_print, 0, STRING("koburon_08"), 0, STRING("me"))
            CASE_EQUAL(7)
            CALL(evt_msg_print, 0, STRING("koburon_09"), 0, STRING("me"))
        END_SWITCH
    END_IF
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    RETURN
EVT_END()

static inline GXColor alphaColor(u8 a) {
    GXColor c = zero_muj_0000db40;

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
    param->tevStage++;
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

USER_FUNC(koburon_get_encount_info) {
    s32* args = event->args;
    s32 id = args[0];
    FieldBattleData* fbat = fbatGetPointer();

    evtSetValue(event, id, fbat->firstAttackKind);
    return EVT_RETURN_DONE;
}

void suifu_kyorokyoro(NanNpcEntry* entry) {
    s32 elapsed;

    switch (entry->state) {
    case 0:
        entry->startTime = gp->renderTime;
        entry->state = 1;
        entry->timer = irand(1000) + 100;
        if (entry->facingAngle == 90.0f) {
            entry->facingAngle = 270.0f;
        } else {
            entry->facingAngle = 90.0f;
        }
        break;
    case 1:
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed > entry->timer) {
            entry->startTime = gp->renderTime;
            entry->state = 2;
            entry->timer = irand(350) + 350;
            if (entry->facingAngle == 90.0f) {
                entry->facingAngle = 270.0f;
            } else {
                entry->facingAngle = 90.0f;
            }
        }
        break;
    case 2:
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed > entry->timer) {
            entry->startTime = gp->renderTime;
            entry->state = 3;
            entry->timer = irand(350) + 350;
            if (entry->facingAngle == 90.0f) {
                entry->facingAngle = 270.0f;
            } else {
                entry->facingAngle = 90.0f;
            }
        }
        break;
    case 3:
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed > entry->timer) {
            entry->callback = NULL;
            if (entry->facingAngle == 90.0f) {
                entry->facingAngle = 270.0f;
            }
        }
        break;
    }
}

void suifu_kyorokyoro_init(NanNpcEntry* entry) {
    entry->state = 0;
    entry->callback = suifu_kyorokyoro;
}

// `delta` is an `int` so the integer conversion stays in one place, and `entry` is declared
// outside the loop and assigned each iteration; both are needed for the original's registers.
USER_FUNC(dai3_butai_main_func) {
    s32* args = event->args;
    f32 cur = (f32)evtGetValue(event, args[0]);
    f32 target = (f32)evtGetValue(event, args[1]);
    f32 centerX = (f32)evtGetValue(event, args[2]);
    f32 centerZ = (f32)evtGetValue(event, args[3]);
    f32 s;
    int delta = (s32)(target - cur);
    s32 i;
    NanNpcEntry* entry;
    NanNpcWork* work = nanNPCWork;

    for (i = 0; i < 100; i++) {
        f32 radius;

        entry = &work->entries[i + 0xCD];
        entry->work[0] += delta;
        radius = (f32)entry->work[1];
        target = 3.141592f * (f32)entry->work[0] / 180.0f;
        cur = sin(target);
        s = cos(target);
        entry->position.x = radius * cur + centerX;
        entry->position.z = radius * s + centerZ;
    }
    return EVT_RETURN_DONE;
}

static inline NanNpcEntry* nanNpcEntryAt(NanNpcWork* arg0, s32 arg1, int arg2) {
    return &arg0->entries[arg1 + arg2];
}

USER_FUNC(kaizoku_main_func) {
    s32* args = event->args;
    f32 v0 = (f32)evtGetValue(event, args[0]);
    f32 v1 = (f32)evtGetValue(event, args[1]);
    f32 v2 = (f32)evtGetValue(event, args[2]);
    f32 v3 = (f32)evtGetValue(event, args[3]);
    s32 delta = (s32)(v1 - v0);
    s32 i;
    f32 s;
    f32 c;
    NanNpcWork* work = nanNPCWork;

    for (i = 0; i < 95; i++) {
        NanNpcEntry* entry = nanNpcEntryAt(work, i, 10);
        entry->work[0] += delta;
        s = (f32)entry->work[1];
        v1 = 3.141592f * (f32)entry->work[0] / 180.0f;
        v0 = sin(v1);
        c = cos(v1);
        entry->position.x = s * v0 + v2;
        entry->position.z = s * c + v3;
    }
    for (i = 0; i < 100; i++) {
        NanNpcEntry* entry = nanNpcEntryAt(work, i, 0x69);
        entry->work[0] += delta;
        s = (f32)entry->work[1];
        v1 = 3.141592f * (f32)entry->work[0] / 180.0f;
        v0 = sin(v1);
        c = cos(v1);
        entry->position.x = s * v0 + v2;
        entry->position.z = s * c + v3;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(mario_main_init_func) {
    _op = _mapAlloc(0x40);
    return EVT_RETURN_DONE;
}

USER_FUNC(mario_main_setup_func) {
    s32* args = event->args;
    f32 x = (f32)evtGetValue(event, args[0]);
    f32 z = (f32)evtGetValue(event, args[1]);
    OpField* op = _op;
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(0);
    NpcEntry* marco = evtNpcNameToPtr(event, "マルコ");
    NpcEntry* cortez = evtNpcNameToPtr(event, "コルテス");
    NpcEntry* monie = evtNpcNameToPtr(event, "モニー");
    NpcEntry* gara = evtNpcNameToPtr(event, "ガラの悪い水夫");
    NpcEntry* piton = evtNpcNameToPtr(event, "ピートン");
    NpcEntry* odoodo = evtNpcNameToPtr(event, "オドオド水夫");

    op[0].dist = distABf(x, -z, x + mario->position.x, -(z + mario->position.z));
    op[0].angle = angleABf(x, -z, x + mario->position.x, -(z + mario->position.z));
    op[1].dist = distABf(x, -z, x + party->position.x, -(z + party->position.z));
    op[1].angle = angleABf(x, -z, x + party->position.x, -(z + party->position.z));
    op[2].dist = distABf(x, -z, x + marco->position.x, -(z + marco->position.z));
    op[2].angle = angleABf(x, -z, x + marco->position.x, -(z + marco->position.z));
    op[3].dist = distABf(x, -z, x + cortez->position.x, -(z + cortez->position.z));
    op[3].angle = angleABf(x, -z, x + cortez->position.x, -(z + cortez->position.z));
    op[4].dist = distABf(x, -z, x + monie->position.x, -(z + monie->position.z));
    op[4].angle = angleABf(x, -z, x + monie->position.x, -(z + monie->position.z));
    op[5].dist = distABf(x, -z, x + gara->position.x, -(z + gara->position.z));
    op[5].angle = angleABf(x, -z, x + gara->position.x, -(z + gara->position.z));
    op[6].dist = distABf(x, -z, x + piton->position.x, -(z + piton->position.z));
    op[6].angle = angleABf(x, -z, x + piton->position.x, -(z + piton->position.z));
    op[7].dist = distABf(x, -z, x + odoodo->position.x, -(z + odoodo->position.z));
    op[7].angle = angleABf(x, -z, x + odoodo->position.x, -(z + odoodo->position.z));
    return EVT_RETURN_DONE;
}


static inline double cosOf(f32 arg0) {
    return cos(arg0);
}

// Written out per object with the locals reused the way the original reuses them: `rad` first
// holds the angle delta, Mario's distance has its own `dist` (declared just before `cur`, which
// gives `cur` the original's f26), Piton's angle goes through `tx` and Odoodo's distance through
// `rad`. The last two objects compute x after the cos call, the first six before it. The cos
// goes through an f32 wrapper so `rad` stays live across the x computation.
USER_FUNC(mario_main_func) {
    s32* args = event->args;
    f32 target;
    f32 centerX;
    f32 centerZ;
    OpField* op;
    MarioWork* mario;
    PartyEntry* party;
    NpcEntry* marco;
    NpcEntry* cortez;
    NpcEntry* monie;
    NpcEntry* gara;
    NpcEntry* piton;
    NpcEntry* odoodo;
    f32 delta;
    f32 rad;
    f32 s;
    f32 c;
    f32 tx;
    f32 dist;
    f32 cur;

    cur = (f32)evtGetValue(event, args[0]);
    target = (f32)evtGetValue(event, args[1]);
    centerX = (f32)evtGetValue(event, args[2]);
    centerZ = (f32)evtGetValue(event, args[3]);
    op = _op;
    mario = marioGetPtr();
    party = partyGetPtr(0);
    marco = evtNpcNameToPtr(event, "マルコ");
    cortez = evtNpcNameToPtr(event, "コルテス");
    monie = evtNpcNameToPtr(event, "モニー");
    gara = evtNpcNameToPtr(event, "ガラの悪い水夫");
    piton = evtNpcNameToPtr(event, "ピートン");
    odoodo = evtNpcNameToPtr(event, "オドオド水夫");
    rad = target - cur;
    delta = rad;

    op[0].angle += delta;
    {
        dist = op[0].dist;

        c = op[0].angle;
        rad = 3.141592f * c / 180.0f;
        s = sin(rad);
        tx = dist * s + centerX;
        c = cosOf(rad);
        mario->position.x = tx;
        mario->position.z = dist * c + centerZ;
    }

    op[1].angle += delta;
    {
        f32 d = op[1].dist;

        c = op[1].angle;
        rad = 3.141592f * c / 180.0f;
        s = sin(rad);
        tx = d * s + centerX;
        c = cosOf(rad);
        party->position.x = tx;
        party->position.z = d * c + centerZ;
    }

    op[2].angle += delta;
    {
        f32 d = op[2].dist;

        c = op[2].angle;
        rad = 3.141592f * c / 180.0f;
        s = sin(rad);
        tx = d * s + centerX;
        c = cosOf(rad);
        marco->position.x = tx;
        marco->position.z = d * c + centerZ;
    }

    op[3].angle += delta;
    {
        f32 d = op[3].dist;

        c = op[3].angle;
        rad = 3.141592f * c / 180.0f;
        s = sin(rad);
        tx = d * s + centerX;
        c = cosOf(rad);
        cortez->position.x = tx;
        cortez->position.z = d * c + centerZ;
    }

    op[4].angle += delta;
    {
        f32 d = op[4].dist;

        c = op[4].angle;
        rad = 3.141592f * c / 180.0f;
        s = sin(rad);
        tx = d * s + centerX;
        c = cosOf(rad);
        monie->position.x = tx;
        monie->position.z = d * c + centerZ;
    }

    op[5].angle += delta;
    {
        f32 d = op[5].dist;

        c = op[5].angle;
        rad = 3.141592f * c / 180.0f;
        s = sin(rad);
        tx = d * s + centerX;
        c = cosOf(rad);
        gara->position.x = tx;
        gara->position.z = d * c + centerZ;
    }

    op[6].angle += delta;
    {
        f32 d = op[6].dist;

        c = op[6].angle;
        tx = 3.141592f * c / 180.0f;
        s = sin(tx);
        c = cosOf(tx);
        piton->position.x = d * s + centerX;
        piton->position.z = d * c + centerZ;
    }

    op[7].angle += delta;
    {
        rad = op[7].dist;

        c = op[7].angle;
        tx = 3.141592f * c / 180.0f;
        s = sin(tx);
        c = cosOf(tx);
        odoodo->position.x = rad * s + centerX;
        odoodo->position.z = rad * c + centerZ;
    }

    marco->prevPosition = marco->position;
    cortez->prevPosition = cortez->position;
    monie->prevPosition = monie->position;
    gara->prevPosition = gara->position;
    piton->prevPosition = piton->position;
    odoodo->prevPosition = odoodo->position;
    return EVT_RETURN_DONE;
}

NanNpcEntry* random_choice_target(s32 type) {
    NanNpcWork* work = nanNPCWork;
    s32 index;

    switch (type) {
    case 0:
    case 1:
        index = irand(100) + 0xCD;
        break;
    case 2:
    default:
        if (irand(2) != 0) {
            index = irand(0x5F) + 0xA;
        } else {
            index = irand(100) + 0x69;
        }
        break;
    }
    return &work->entries[index];
}

void tobikakari_seq00(NanNpcEntry* entry) {
    TobikakariSpot* spot = entry->spot;
    s32 elapsed;

    switch (entry->state) {
    case 0: {
        s32 r = irand(1000);
        if (r < 100) {
            entry->state = 10;
        } else if (r < 200) {
            entry->startTime = (s32)gp->renderTime;
            entry->timer = irand(500) + 1;
            entry->state = 0x15;
        } else {
            entry->startTime = (s32)gp->renderTime;
            entry->timer = irand(2000) + 500;
            entry->state = 0x16;
        }
        break;
    }
    case 10: {
        NanNpcEntry* target;
        entry->startTime = (s32)gp->renderTime;
        entry->hoseiPosition = entry->position;
        target = random_choice_target(spot->group);
        entry->homePosition = target->spot->home;
        entry->homePosition.z += 2.0f;
        entry->jumpQuad = -4.0f * (f32)(irand(30) + 100);
        entry->jumpLinear = entry->homePosition.y - entry->hoseiPosition.y - entry->jumpQuad;
        entry->timer = irand(500) + 400;
        if (entry->hoseiPosition.x < entry->homePosition.x) {
            entry->facingAngle = 90.0f;
        } else {
            entry->facingAngle = 270.0f;
        }
        entry->state = 0x14;
        entry->field_0x70 = 0x15;
        break;
    }
    case 11:
        entry->startTime = (s32)gp->renderTime;
        entry->hoseiPosition = entry->position;
        entry->homePosition = spot->home;
        entry->jumpQuad = -4.0f * (f32)(irand(30) + 100);
        entry->jumpLinear = entry->homePosition.y - entry->hoseiPosition.y - entry->jumpQuad;
        entry->timer = irand(500) + 400;
        entry->state = 0x14;
        entry->field_0x70 = 0x16;
        break;
    case 20:
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed < entry->timer) {
            f32 t = (f32)elapsed / (f32)entry->timer;
            entry->position.x = intplGetValue(0, elapsed, entry->timer, entry->hoseiPosition.x, entry->homePosition.x);
            entry->position.z = intplGetValue(0, elapsed, entry->timer, entry->hoseiPosition.z, entry->homePosition.z);
            entry->position.y = t * (entry->jumpQuad * t + entry->jumpLinear) + entry->hoseiPosition.y;
        } else {
            entry->position = entry->homePosition;
            entry->startTime = (s32)gp->renderTime;
            entry->timer = irand(2000) + 500;
            entry->state = entry->field_0x70;
        }
        break;
    case 21:
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed > entry->timer) {
            entry->state = 11;
        }
        break;
    case 22:
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed > entry->timer) {
            entry->state = 10;
        }
        break;
    }
}

USER_FUNC(tobikakaru_init) {
    NanNpcWork* work = nanNPCWork;
    TobikakariSpot* spot;
    s32 i;
    NanNpcEntry* entry;

    spot = _mapAlloc(0x1310);
    Tobidat = spot;
    for (i = 0; i < 0x5F; i++) {
        entry = &work->entries[i + 10];
        entry->callback = tobikakari_seq00;
        entry->state = 0;
        entry->spot = spot;
        evt_nannpc_set_animtbl_sub(entry, (s16*)suifu_animtbl_run);
        spot->home = entry->position;
        spot->group = 0;
        spot++;
    }
    for (i = 0; i < 100; i++) {
        entry = &work->entries[i + 0x69];
        entry->callback = tobikakari_seq00;
        entry->state = 0;
        entry->spot = spot;
        spot->home = entry->position;
        spot->group = 1;
        spot++;
    }
    for (i = 0; i < 100; i++) {
        entry = &work->entries[i + 0xCD];
        entry->callback = tobikakari_seq00;
        entry->state = 0;
        entry->spot = spot;
        evt_nannpc_set_animtbl_sub(entry, (s16*)dai3_animtbl_run);
        spot->home = entry->position;
        spot->group = 2;
        spot++;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(tobikakaru_end) {
    NanNpcWork* work = nanNPCWork;
    s32 i;

    for (i = 0; i < 0x5F; i++) {
        NanNpcEntry* entry = &work->entries[i + 10];
        entry->callback = NULL;
        evt_nannpc_set_animtbl_sub(entry, (s16*)suifu_animtbl);
    }
    for (i = 0; i < 100; i++) {
        NanNpcEntry* entry = &work->entries[i + 0x69];
        entry->callback = NULL;
    }
    for (i = 0; i < 100; i++) {
        NanNpcEntry* entry = &work->entries[i + 0xCD];
        entry->callback = NULL;
        evt_nannpc_set_animtbl_sub(entry, (s16*)dai3_animtbl);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(ship_rotate) {
    s32* args = event->args;
    f32 a = evtGetFloat(event, args[0]);
    f32 b = evtGetFloat(event, args[1]);
    f32 c = evtGetFloat(event, args[2]);
    f32 d = evtGetFloat(event, args[3]);
    f32 angle = angleABf(a, -b, c, -d);
    evtSetFloat(event, args[4], angle - 90.0f);
    return EVT_RETURN_DONE;
}

USER_FUNC(ship_rotate_rev) {
    s32* args = event->args;
    f32 a = evtGetFloat(event, args[0]);
    f32 b = evtGetFloat(event, args[1]);
    f32 c = evtGetFloat(event, args[2]);
    f32 d = evtGetFloat(event, args[3]);
    f32 angle = angleABf(a, -b, c, -d);
    evtSetFloat(event, args[4], angle - 90.0f + 180.0f);
    return EVT_RETURN_DONE;
}

USER_FUNC(cam_test_init_func) {
    s32* args = event->args;
    Vec v2;
    Vec v1;
    f32 dist;
    f32 angle;
    f32 dx;
    f32 dz;

    v1.x = (f32)evtGetValue(event, args[0]);
    v1.y = 110.0f;
    v1.z = (f32)evtGetValue(event, args[1]);
    v2.x = 70.0f;
    v2.y = 110.0f;
    v2.z = 0.0f;
    dist = PSVECDistance(&v1, &v2);
    dx = v2.x - v1.x;
    dz = v2.z - v1.z;
    angle = 57.29579f * (f32)atan2(-dx, dz);
    evtSetFloat(event, args[2], angle);
    evtSetFloat(event, args[3], dist);
    return EVT_RETURN_DONE;
}

// The output indices are taken as pointers (`xp`, `zp`) and the inputs read through the advanced
// `args`, which keeps the original's second base (`mr r30,r31; addi r31,r31,4`). The circle centre
// z is a local zero so the subtraction stays the original's `fnmsubs` against the pooled 0.0f.
USER_FUNC(cam_test_func) {
    s32* args = event->args;
    s32* xp = args++;
    s32* zp = args++;
    f32 angle = evtGetFloat(event, *args++);
    f32 radius = evtGetFloat(event, *args);
    f32 s;
    f32 c;
    f32 centerZ = 0.0f;
    f32 rad = angle * 6.2831855f / 360.0f;

    s = sin(rad);
    c = cos(rad);
    evtSetValue(event, *xp, (s32)(radius * s + 70.0f));
    evtSetValue(event, *zp, (s32)(centerZ - radius * c));
    return EVT_RETURN_DONE;
}

static USER_FUNC(scale_func) {
    s32* args = event->args;
    f32 scale = evtGetFloat(event, args[0]);
    MarioWork* mario = marioGetPtr();
    PartyEntry* party0 = partyGetPtr(0);
    PartyEntry* party1 = partyGetPtr(1);
    Vec v0;
    Vec v1;
    Vec v2;

    v0 = vec3_muj_0000e4cc;
    v0.x = scale;
    v0.y = scale;
    v0.z = scale;
    mario->scale = v0;
    if (party0 != NULL) {
        v1 = vec3_muj_0000e4d8;
        v1.x = scale;
        v1.y = scale;
        v1.z = scale;
        party0->scale = v1;
    }
    if (party1 != NULL) {
        v2 = vec3_muj_0000e4e4;
        v2.x = scale;
        v2.y = scale;
        v2.z = scale;
        party1->scale = v2;
    }
    return EVT_RETURN_DONE;
}


EVT_BEGIN(koburon_dead)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_BTL_DAMAGED_PLIABLE1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_damage_anim, STRING("me"))
    WAIT_MS(200)
    ADD(LW(1), 30)
    ADD(LW(2), 10)
    CALL(evt_item_entry, STRING("item_99"), 165, LW(0), LW(1), LW(2), 14, -1, 0)
    WAIT_MS(400)
    IF_INT_EQUAL(GSW(721), 7)
        CALL(evt_msg_print, 0, STRING("koburon_14"), 0, STRING("me"))
        GOTO(99)
    END_IF
    CALL(koburon_get_encount_info, LW(0))
    SWITCH(LW(0))
        CASE_OR_EQUAL(131072)
        CASE_OR_EQUAL(262144)
        CASE_OR_EQUAL(524288)
        CALL(evt_msg_print, 0, STRING("koburon_10"), 0, STRING("me"))
        CASE_END
        CASE_OR_EQUAL(1048576)
        CASE_OR_EQUAL(2097152)
        CASE_OR_EQUAL(4194304)
        CALL(evt_msg_print, 0, STRING("koburon_10"), 0, STRING("me"))
        CASE_END
        CASE_OR_EQUAL(8388608)
        CALL(evt_msg_print, 0, STRING("koburon_11"), 0, STRING("me"))
        CASE_END
        CASE_OR_EQUAL(16777216)
        CALL(evt_msg_print, 0, STRING("koburon_13"), 0, STRING("me"))
        CASE_END
    END_SWITCH
    LABEL(99)
    CALL(evt_npc_set_anim, STRING("me"), STRING("S_1"))
    WAIT_MS(100)
    ADD(GSW(721), 1)
    SET_VALUE(LSWF(2), 1)
    CALL(evt_npc_reaction_flag_onoff, 0, STRING("me"), 15)
    CALL(evt_npc_set_anim, STRING("me"), STRING("A_2"))
    WAIT_MS(500)
    CALL(evt_npc_change_interrupt, STRING("me"), 1, 0)
    CALL(evt_npc_set_position, STRING("me"), 0, -1000, 0)
    SET_VALUE(GSWF(3151), 1)
    RETURN
EVT_END()

EVT_BEGIN(koburon_evt)
    IF_INT_EQUAL(GSW(721), 8)
        CALL(evt_npc_delete, STRING("コブロン"))
        RETURN
    END_IF
    CALL(evt_npc_set_position, STRING("コブロン"), 562, 35, -1226)
    CALL(evt_npc_set_width, STRING("コブロン"), 15)
    CALL(evt_npc_set_height, STRING("コブロン"), 22)
    RETURN
EVT_END()

static EVT_BEGIN(kiza_talk)
    CALL(evt_msg_print, 0, STRING("stg5_muj_146_02_01"), 0, STRING("me"))
    RETURN
EVT_END()

static EVT_BEGIN(kiza_entry)
    CALL(evt_npc_entry, STRING("キザ野郎"), STRING("c_kiza"))
    CALL(evt_npc_set_tribe, STRING("キザ野郎"), STRING("キザ野郎"))
    CALL(evt_npc_set_position, STRING("キザ野郎"), 575, 25, -420)
    CALL(evt_npc_set_ry, STRING("キザ野郎"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("キザ野郎"), 1536)
    CALL(evt_npc_change_interrupt, STRING("キザ野郎"), 6, PTR(kiza_talk))
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[15] = {
    {"ガラの悪い水夫", 0x00000600, 0, garawaru_init, NULL, garawaru_talk},
    {"マルコ", 0x00000600, 0, marco_init, NULL, marco_talk},
    {"めがね水夫", 0x00000600, 0, npc_init_evt},
    {"第三勢力幹部", 0x00000600, 0, npc_init_evt},
    {"オドオド水夫", 0x00000600, 0, odoodo_init, NULL, odoodo_talk},
    {"水夫A", 0x00000600, 0, suifu_a_init, NULL, suifu_a_talk},
    {"水夫ボム兵", 0x00000600, 0, suifu_bomb_init, NULL, suifu_bomb_talk},
    {"水夫C", 0x00000600, 0, suifu_c_init, NULL, suifu_c_talk},
    {"水夫D", 0x00000600, 0, suifu_d_init, NULL, suifu_d_talk},
    {"水夫E", 0x00000600, 0, suifu_e_init, NULL, suifu_e_talk},
    {"コルテス", 0x00000600, 0, korutesu_init, NULL, korutesu_talk},
    {"モニー", 0x00000600, 0, mony_init, NULL, mony_talk},
    {"ピートン", 0x00000600, 0, peton_init, NULL, peton_talk},
    {"コブロン", 0x00000200, 0x00000007, koburon_init, koburon_regp, koburon_talk, koburon_dead},
    {NULL},
};

static BeroEntry bero_entry_data[4] = {
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "muj_01", "w_bero", 1, 1},
    {"n_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "muj_10", "e_bero", 1, 1},
    {"n_bero_02", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "dou_11", "n_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(mujinto_hyochaku)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_eff_delete, STRING("sunlight"))
    CALL(evt_map_set_blend, 0, 0, 0, 0, 255)
    WAIT_MS(2000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_muj_00"), 0, 0)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_muj_01"), 0, 0)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_muj_02"), 0, 0)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_muj_03"), 0, 0)
    WAIT_MS(500)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_pos, 1185, -15, 0)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_pos, 0, 1220, -15, 30)
    CALL(evt_party_set_dir, 0, 270, 0)
    CALL(evt_mario_set_pose, STRING("M_W_4B"))
    CALL(evt_cam3d_evt_set, 1250, 141, 535, 1250, 45, -7, 0, 11)
    WAIT_MS(1500)
    SET_VALUE(LW(10), 300)
    CALL(evt_eff64, STRING("sunlight"), STRING("sunlight_n64"), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    SPAWN_EVENT(back_light)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
        CALL(evt_snd_bgm_scope, 0, 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_telop_entry, 9)
    END_INLINE_EVENT
    WAIT_MS(1800)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), 1485, 35, 15)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 1073741856)
    CALL(evt_msg_print, 0, STRING("stg5_muj_03_00"), 0, STRING("ガラの悪い水夫"))
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_npc_move_position, STRING("ガラの悪い水夫"), 1342, 37, 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("stg5_muj_04"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    CALL(evt_npc_move_position, STRING("ガラの悪い水夫"), 1485, 15, 0, FLOAT(60.0f), 0)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), 0, -1000, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 232)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_snd_bgm_scope, 0, 0)
    RETURN
EVT_END()

// TODO: type
u32 suifu_random_setup[380] = {
    0x00000509, 0x00000023, 0xFFFFFE5D, 0x0000010E,
    0x000004BE, 0x00000023, 0xFFFFFEB1, 0x0000005A,
    0x000004F0, 0x00000023, 0xFFFFFE9A, 0x0000010E,
    0x0000051A, 0x00000023, 0xFFFFFE32, 0x0000005A,
    0x000004F9, 0x00000023, 0xFFFFFE51, 0x0000010E,
    0x00000481, 0x00000023, 0xFFFFFE0C, 0x0000005A,
    0x0000052F, 0x00000023, 0xFFFFFE8F, 0x0000010E,
    0x000004B1, 0x00000023, 0xFFFFFE2C, 0x0000005A,
    0x00000554, 0x00000023, 0xFFFFFE12, 0x0000010E,
    0x000004A6, 0x00000023, 0xFFFFFE45, 0x0000005A,
    0x000004FB, 0x00000023, 0xFFFFFE39, 0x0000010E,
    0x000004BF, 0x00000023, 0xFFFFFE57, 0x0000005A,
    0x000004A1, 0x00000023, 0xFFFFFE7C, 0x0000010E,
    0x0000048F, 0x00000023, 0xFFFFFE71, 0x0000005A,
    0x0000048A, 0x00000023, 0xFFFFFE2C, 0x0000010E,
    0x000004B8, 0x00000023, 0xFFFFFE47, 0x0000005A,
    0x000004C3, 0x00000023, 0xFFFFFEA2, 0x0000010E,
    0x000004ED, 0x00000023, 0xFFFFFE61, 0x0000005A,
    0x0000051A, 0x00000023, 0xFFFFFE60, 0x0000010E,
    0x00000488, 0x00000023, 0xFFFFFEAB, 0x0000005A,
    0x000004B9, 0x00000023, 0xFFFFFE1A, 0x0000010E,
    0x00000544, 0x00000023, 0xFFFFFE2C, 0x0000005A,
    0x000004BF, 0x00000023, 0xFFFFFE73, 0x0000010E,
    0x000004B2, 0x00000023, 0xFFFFFE6C, 0x0000005A,
    0x00000477, 0x00000023, 0xFFFFFEA5, 0x0000010E,
    0x000004DE, 0x00000023, 0xFFFFFE4D, 0x0000005A,
    0x0000048B, 0x00000023, 0xFFFFFEBF, 0x0000010E,
    0x000004E3, 0x00000021, 0xFFFFFE36, 0x00000063,
    0x000004B3, 0x00000023, 0xFFFFFEA6, 0x0000010E,
    0x0000055B, 0x00000023, 0xFFFFFE9C, 0x0000005A,
    0x000004CF, 0x00000023, 0xFFFFFE70, 0x0000010E,
    0x00000556, 0x00000023, 0xFFFFFE28, 0x0000005A,
    0x00000505, 0x00000023, 0xFFFFFE9B, 0x0000010E,
    0x00000479, 0x00000023, 0xFFFFFE89, 0x0000005A,
    0x000004D2, 0x00000023, 0xFFFFFE1D, 0x0000010E,
    0x0000046D, 0x00000023, 0xFFFFFE2B, 0x0000005A,
    0x0000052D, 0x00000023, 0xFFFFFE5D, 0x0000010E,
    0x000004CE, 0x00000023, 0xFFFFFE90, 0x0000005A,
    0x000004FF, 0x00000023, 0xFFFFFEAF, 0x0000010E,
    0x0000046D, 0x00000023, 0xFFFFFE3A, 0x0000005A,
    0x000004D0, 0x00000023, 0xFFFFFE58, 0x0000010E,
    0x00000470, 0x00000023, 0xFFFFFE4C, 0x0000005A,
    0x0000046C, 0x00000023, 0xFFFFFE6D, 0x0000010E,
    0x000004B3, 0x00000023, 0xFFFFFE7E, 0x0000005A,
    0x00000535, 0x00000023, 0xFFFFFE78, 0x0000010E,
    0x000004D6, 0x00000023, 0xFFFFFEBD, 0x0000005A,
    0x000004D2, 0x00000023, 0xFFFFFE3C, 0x0000010E,
    0x000004FE, 0x00000023, 0xFFFFFE2A, 0x0000005A,
    0x0000046B, 0x00000023, 0xFFFFFE98, 0x0000010E,
    0x00000514, 0x00000023, 0xFFFFFE4E, 0x0000005A,
    0x00000525, 0x00000023, 0xFFFFFE71, 0x0000010E,
    0x000004C3, 0x00000023, 0xFFFFFE82, 0x0000005A,
    0x00000509, 0x00000023, 0xFFFFFE1E, 0x0000010E,
    0x0000053D, 0x00000023, 0xFFFFFE66, 0x0000005A,
    0x0000054A, 0x00000023, 0xFFFFFE56, 0x0000010E,
    0x0000047E, 0x00000023, 0xFFFFFE73, 0x0000005A,
    0x00000521, 0x00000023, 0xFFFFFEBF, 0x0000010E,
    0x0000046A, 0x00000023, 0xFFFFFE7C, 0x0000005A,
    0x000004F4, 0x00000023, 0xFFFFFE6F, 0x0000010E,
    0x0000049F, 0x00000023, 0xFFFFFEC0, 0x0000005A,
    0x000004B8, 0x00000023, 0xFFFFFE92, 0x0000010E,
    0x000004D5, 0x00000023, 0xFFFFFE7E, 0x0000005A,
    0x00000527, 0x00000023, 0xFFFFFE4F, 0x0000010E,
    0x00000522, 0x00000023, 0xFFFFFE21, 0x0000005A,
    0x0000047B, 0x00000023, 0xFFFFFEB7, 0x0000010E,
    0x00000551, 0x00000023, 0xFFFFFE39, 0x0000005A,
    0x00000478, 0x00000023, 0xFFFFFE19, 0x0000010E,
    0x0000048C, 0x00000023, 0xFFFFFE1C, 0x0000005A,
    0x00000532, 0x00000023, 0xFFFFFE10, 0x0000010E,
    0x00000548, 0x00000023, 0xFFFFFE81, 0x0000005A,
    0x0000050A, 0x00000023, 0xFFFFFE3F, 0x0000010E,
    0x0000045F, 0x00000023, 0xFFFFFE8C, 0x0000005A,
    0x000004DD, 0x00000023, 0xFFFFFE62, 0x0000010E,
    0x0000051A, 0x00000023, 0xFFFFFEA1, 0x0000005A,
    0x00000491, 0x00000023, 0xFFFFFE92, 0x0000010E,
    0x00000521, 0x00000023, 0xFFFFFE86, 0x0000005A,
    0x000004E8, 0x00000023, 0xFFFFFE7F, 0x0000010E,
    0x00000552, 0x00000023, 0xFFFFFE8E, 0x0000005A,
    0x00000535, 0x00000023, 0xFFFFFE20, 0x0000010E,
    0x0000049F, 0x00000023, 0xFFFFFEA6, 0x0000005A,
    0x00000460, 0x00000023, 0xFFFFFE1C, 0x0000010E,
    0x000004A9, 0x00000023, 0xFFFFFE55, 0x0000005A,
    0x000004F5, 0x00000023, 0xFFFFFE10, 0x0000010E,
    0x00000469, 0x00000023, 0xFFFFFEB8, 0x0000005A,
    0x0000055A, 0x00000023, 0xFFFFFE4B, 0x0000010E,
    0x000004E1, 0x00000023, 0xFFFFFEAC, 0x0000005A,
    0x0000052E, 0x00000023, 0xFFFFFEA2, 0x0000010E,
    0x00000512, 0x00000023, 0xFFFFFEB6, 0x0000005A,
    0x000004DE, 0x00000023, 0xFFFFFE8F, 0x0000010E,
    0x0000049A, 0x00000023, 0xFFFFFE27, 0x0000005A,
    0x0000052F, 0x00000023, 0xFFFFFE33, 0x0000010E,
    0x00000537, 0x00000023, 0xFFFFFE41, 0x0000005A,
    0x0000053F, 0x00000023, 0xFFFFFE91, 0x0000010E,
    0x0000050D, 0x00000023, 0xFFFFFE8E, 0x0000005A,
    0x0000054C, 0x00000023, 0xFFFFFEAE, 0x0000010E,
};

EVT_BEGIN(suifu_random_sub)
    CALL(evt_nannpc_set_subfunc, 2, PTR(nanNPCOption))
    SET_VALUE(LW(0), 10)
    SET_INT_READ_ADDR(PTR(suifu_random_setup))
    LOOP(95)
        READ_INT3(LW(1), LW(2), LW(3))
        READ_INT(LW(4))
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_nannpc_set_dir, LW(0), LW(4))
        CALL(evt_nannpc_ppflag_onoff, LW(0), 0, 1)
        CALL(evt_nannpc_flag_onoff, LW(0), 0, 4)
        CALL(evt_nannpc_set_shadow_position, LW(0), 35)
        CALL(evt_nannpc_set_size, LW(0), 30, 15)
        ADD(LW(0), 1)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_nannpc_flag_onoff, 0, 0, 4)
        CALL(evt_nannpc_set_size, 0, 30, 15)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            CALL(evt_nannpc_set_position, 0, LW(0), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_set_position, STRING("水夫C"), 1330, 35, -200)
    CALL(evt_npc_set_position, STRING("水夫D"), 1295, 35, -225)
    CALL(evt_npc_set_position, STRING("水夫E"), 1250, 35, -190)
    CALL(evt_npc_set_position, STRING("モニー"), 1180, 35, -275)
    CALL(evt_npc_set_position, STRING("ピートン"), 1235, 35, -275)
    CALL(evt_npc_set_ry, STRING("水夫E"), 90)
    CALL(evt_npc_set_ry, STRING("モニー"), 90)
    CALL(evt_npc_set_ry, STRING("ピートン"), 90)
    RETURN
EVT_END()

EVT_BEGIN(tama_shoot)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_CANNON_FIRING1"), 0)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), GW(13))
        ADD(LW(0), 320)
        SET_VALUE(LW(1), GW(14))
        ADD(LW(1), 130)
        SET_VALUE(LW(2), GW(15))
        CALL(evt_eff, 0, STRING("kemuri_test"), 16, LW(0), LW(1), LW(2), FLOAT(3.0f), 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.007f), FLOAT(0.0f), 500)
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_STG5_CANNON_SEA1"), 0)
    CALL(evt_eff, 0, STRING("splash"), 0, LW(0), -20, LW(2), FLOAT(3.0f), 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(npc_kyorokyoro)
    CALL(evt_sub_random, 1000, LW(10))
    ADD(LW(10), 100)
    WAIT_MS(LW(10))
    CALL(evt_npc_reverse_ry, LW(0))
    CALL(evt_sub_random, 350, LW(10))
    ADD(LW(10), 350)
    WAIT_MS(LW(10))
    CALL(evt_npc_reverse_ry, LW(0))
    CALL(evt_sub_random, 350, LW(10))
    ADD(LW(10), 350)
    WAIT_MS(LW(10))
    CALL(evt_npc_set_ry, LW(0), 270)
    RETURN
EVT_END()


EVT_BEGIN(dai3_shurai_sub)
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
    CALL(evt_npc_set_position, STRING("マルコ"), 1500, 35, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 1073741856)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), 1500, 35, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 1073741856)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), 1500, 35, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("めがね水夫"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1073741856)
    CALL(evt_cam3d_evt_set, 1335, 145, 312, 1335, 75, -82, 1000, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("マルコ"), 1370, -50, 0, FLOAT(100.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_npc_move_position, STRING("ガラの悪い水夫"), 1385, 0, 0, FLOAT(110.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_npc_move_position, STRING("オドオド水夫"), 1400, -100, 0, FLOAT(80.0f), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 1255, -100, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 1285, -80, FLOAT(120.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_95"), 0, STRING("マルコ"))
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_4"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(500)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_msg_print, 0, STRING("stg5_muj_96"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_set_dir_to_target, STRING("オドオド水夫"), STRING("水夫E"))
    CALL(evt_npc_get_position, STRING("オドオド水夫"), LW(0), LW(1), LW(2))
    ADD(LW(0), -10)
    CALL(evt_npc_move_position, STRING("オドオド水夫"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("オドオド水夫"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 1290, 143, 213, 1290, 90, -82, 500, 11)
    CALL(evt_npc_move_position, STRING("オドオド水夫"), 1325, -150, 0, FLOAT(180.0f), 0)
    WAIT_MS(500)
    CALL(evt_set_dir_to_target, STRING("水夫E"), STRING("オドオド水夫"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_99"), 0, STRING("オドオド水夫"))
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("水夫E"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_100"), 0, STRING("水夫E"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_101"), 0, STRING("オドオド水夫"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_102"), 0, STRING("水夫C"))
    CALL(evt_cam3d_evt_set, 1335, 145, 312, 1335, 75, -82, 500, 11)
    WAIT_MS(1000)
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("モニー"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("ピートン"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫C"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫D"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫E"), 512)
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg5_muj_103"), 0, STRING("マルコ"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg5_muj_103"), 0, STRING("ガラの悪い水夫"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg5_muj_103"), 0, STRING("オドオド水夫"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg5_muj_103"), 0, STRING("ピートン"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("stg5_muj_103"), 0, STRING("水夫E"))
    END_INLINE_EVENT
    WAIT_MS(2000)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫C"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫D"), 512)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫E"), 512)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_CANNON_FIRING1"), 0)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 1000)
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_party_set_dir, 0, 270, 200)
    CALL(evt_npc_set_ry, STRING("ピートン"), 270)
    WAIT_MS(500)
    SET_VALUE(LW(0), 10)
    LOOP(95)
        CALL(evt_nannpc_set_func, LW(0), PTR(suifu_kyorokyoro_init))
        ADD(LW(0), 1)
    END_LOOP
    SET_VALUE(LW(0), STRING("モニー"))
    SPAWN_EVENT(npc_kyorokyoro)
    SET_VALUE(LW(0), STRING("水夫C"))
    SPAWN_EVENT(npc_kyorokyoro)
    SET_VALUE(LW(0), STRING("水夫D"))
    SPAWN_EVENT(npc_kyorokyoro)
    SET_VALUE(LW(0), STRING("水夫E"))
    SPAWN_EVENT(npc_kyorokyoro)
    SET_VALUE(LW(0), STRING("オドオド水夫"))
    SPAWN_EVENT(npc_kyorokyoro)
    SET_VALUE(LW(0), STRING("ガラの悪い水夫"))
    SPAWN_EVENT(npc_kyorokyoro)
    SET_VALUE(LW(0), STRING("マルコ"))
    SPAWN_EVENT(npc_kyorokyoro)
    WAIT_MS(3000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_104"), 0, STRING("オドオド水夫"))
    WAIT_MS(1000)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER4"))
    CALL(evt_cam3d_evt_set, 60, 180, 369, 1289, 10, -184, 0, 11)
    SET_VALUE(LW(0), 741)
    SET_VALUE(LW(1), -60)
    SET_VALUE(LW(2), 48)
    SET_FLOAT_VALUE(LW(3), FLOAT(4.0f))
    CALL_CHILD(tama_shoot)
    WAIT_MS(1000)
    SET_VALUE(LW(0), 872)
    SET_VALUE(LW(1), -60)
    SET_VALUE(LW(2), -155)
    SET_FLOAT_VALUE(LW(3), FLOAT(2.0f))
    CALL_CHILD(tama_shoot)
    WAIT_MS(1000)
    SET_VALUE(LW(0), 991)
    SET_VALUE(LW(1), -60)
    SET_VALUE(LW(2), 77)
    SET_FLOAT_VALUE(LW(3), FLOAT(3.0f))
    CALL_CHILD(tama_shoot)
    WAIT_MS(3000)
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("ガラの悪い水夫"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_124"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_cam3d_evt_set, 350, 150, 654, -865, 200, 82, 1000, 11)
    CALL(evt_mapobj_trans, 1, STRING("S_c"), -1100, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_c"), 0, 0, 0)
    SET_FLOAT_VALUE(GW(13), -1100)
    SET_FLOAT_VALUE(GW(14), 0)
    SET_FLOAT_VALUE(GW(15), 0)
    CALL(evt_hit_bind_update, STRING("A_c"))
    WAIT_MS(2000)
    SET_VALUE(LW(0), 991)
    SET_VALUE(LW(1), -60)
    SET_VALUE(LW(2), 77)
    SET_FLOAT_VALUE(LW(3), FLOAT(3.0f))
    CALL_CHILD(tama_shoot)
    WAIT_MS(2000)
    CALL(evt_npc_set_position, STRING("めがね水夫"), -991, 58, -16)
    CALL(evt_npc_set_ry, STRING("めがね水夫"), 90)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), 0, -100, 0)
    CALL(evt_npc_set_ry, STRING("第三勢力幹部"), 90)
    CALL(evt_msg_print, 0, STRING("stg5_muj_105"), 0, STRING("めがね水夫"))
    WAIT_MS(1000)
    CALL(evt_npc_jump_position_nohit, STRING("めがね水夫"), -802, 162, 0, 500, 120)
    CALL(evt_cam3d_evt_set, -370, 192, 203, -821, 210, -11, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_106"), 0, STRING("めがね水夫"))
    CALL(evt_cam3d_evt_set, -404, 360, 514, 119, 260, 216, 0, 11)
    CALL(evt_npc_set_position, STRING("めがね水夫"), 261, 155, 8)
    CALL(evt_mapobj_trans, 1, STRING("S_c"), 0, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_c"), 0, 0, 0)
    SET_FLOAT_VALUE(GW(13), 0)
    SET_FLOAT_VALUE(GW(14), 0)
    SET_FLOAT_VALUE(GW(15), 0)
    CALL(evt_hit_bind_update, STRING("A_c"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_107"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_108"), 0, STRING("オドオド水夫"))
    CALL(evt_cam3d_evt_set, 700, 145, 178, 202, 210, -46, 0, 11)
    WAIT_MS(1000)
    SET_VALUE(LW(0), 991)
    SET_VALUE(LW(1), -60)
    SET_VALUE(LW(2), 77)
    SET_FLOAT_VALUE(LW(3), FLOAT(3.0f))
    CALL_CHILD(tama_shoot)
    WAIT_MS(2000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_109"), 0, STRING("めがね水夫"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("めがね水夫"), LW(0), LW(1), LW(2))
        WAIT_MS(400)
        CALL(evt_npc_set_position, STRING("めがね水夫"), 0, -1000, 0)
        CALL(evt_npc_set_position, STRING("第三勢力幹部"), LW(0), LW(1), LW(2))
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 0, 0, 720, 800)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        MODULO(LW(0), 360)
        CALL(evt_npc_set_rotate, STRING("めがね水夫"), 0, LW(0), 0)
        CALL(evt_npc_set_rotate, STRING("第三勢力幹部"), 0, LW(0), 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SURPRISED1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_110"), 0, STRING("第三勢力幹部"))
    CALL(evt_npc_set_ry, STRING("マルコ"), 180)
    CALL(evt_cam3d_evt_set, 1195, 50, 405, 200, 180, -43, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_111"), 0, STRING("第三勢力幹部"))
    SET_VALUE(LW(0), 991)
    SET_VALUE(LW(1), -60)
    SET_VALUE(LW(2), 77)
    SET_FLOAT_VALUE(LW(3), FLOAT(3.0f))
    CALL_CHILD(tama_shoot)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 1975, 128, 0, -623, 128, 0, 0, 11)
    CALL(evt_npc_set_ry_lr, STRING("オドオド水夫"), 0)
    CALL(evt_mapobj_trans, 1, STRING("S_c"), -1000, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_c"), 0, 0, 0)
    SET_FLOAT_VALUE(GW(13), -1000)
    SET_FLOAT_VALUE(GW(14), 0)
    SET_FLOAT_VALUE(GW(15), 0)
    CALL(evt_hit_bind_update, STRING("A_c"))
    CALL(evt_mario_dispflag_onoff, 1, 4)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 270, 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 270, 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), -707, 162, 9)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg5_muj_112"), 0, STRING("第三勢力幹部"))
    SPAWN_EVENT_GET_ID(PTR(dai3_shurai_after), GW(8))
    CALL(evt_msg_print, 0, STRING("stg5_muj_113"), 0, STRING("マルコ"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_114"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("マルコ"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_115"), 0, STRING("マルコ"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_116"), 0, STRING("オドオド水夫"))
    CALL(evt_npc_set_ry_lr, STRING("マルコ"), 1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_117"), 0, STRING("マルコ"))
    CALL(evt_npc_set_ry_lr, STRING("マルコ"), 0)
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("マルコ"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_position, STRING("マルコ"), 0, -1000, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_set_party_pos, 1, 9, LW(0), LW(1), LW(2))
    WAIT_MS(1000)
    CALL(evt_party_run, 1)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫C"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫D"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫E"), 1073741856)
    SET_VALUE(GSW(0), 257)
    CALL(evt_mario_adjust_dir)
    CALL(evt_mario_dispflag_onoff, 0, 4)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(dai3_shurai)
    CALL(evt_mapobj_trans, 1, STRING("S_c"), 0, -1000, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_c"), 0, 0, 0)
    SET_FLOAT_VALUE(GW(13), 0)
    SET_FLOAT_VALUE(GW(14), -1000)
    SET_FLOAT_VALUE(GW(15), 0)
    CALL(evt_hit_bind_update, STRING("A_c"))
    LOOP(0)
        CALL(evt_mario_get_name_hitobj_ride, LW(10))
        IF_STR_EQUAL(LW(10), STRING("A_yuka01"))
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(2), -125)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SPAWN_EVENT(dai3_shurai_sub)
    RETURN
EVT_END()

static EVT_BEGIN(dai3_shurai_after)
    LOOP(0)
        CALL(evt_sub_random, 2500, LW(10))
        WAIT_MS(LW(10))
        WAIT_MS(500)
        CALL(evt_sub_random, 2, LW(11))
        SWITCH(LW(11))
            CASE_EQUAL(0)
            SET_VALUE(LW(0), 1086)
            SET_VALUE(LW(1), -60)
            SET_VALUE(LW(2), -247)
            SET_FLOAT_VALUE(LW(3), FLOAT(4.0f))
            CALL_CHILD(tama_shoot)
            CASE_EQUAL(1)
            SET_VALUE(LW(0), 1135)
            SET_VALUE(LW(1), -60)
            SET_VALUE(LW(2), 51)
            SET_FLOAT_VALUE(LW(3), FLOAT(2.0f))
            CALL_CHILD(tama_shoot)
            CASE_EQUAL(2)
            SET_VALUE(LW(0), 1251)
            SET_VALUE(LW(1), -60)
            SET_VALUE(LW(2), 153)
            SET_FLOAT_VALUE(LW(3), FLOAT(3.0f))
            CALL_CHILD(tama_shoot)
        END_SWITCH
    END_LOOP
    RETURN
EVT_END()

// TODO: type
u32 kaizoku_vs_dai3_suifupos[380] = {
    0xFFFFFFA8, 0x00000073, 0xFFFFFFFA, 0x0000010E,
    0xFFFFFFA9, 0x00000073, 0x0000000F, 0x0000010E,
    0xFFFFFFA4, 0x00000073, 0x00000022, 0x0000010E,
    0xFFFFFF98, 0x00000073, 0x0000004C, 0x0000010E,
    0xFFFFFF92, 0x00000073, 0x00000042, 0x0000010E,
    0xFFFFFFAC, 0x00000073, 0x0000004B, 0x0000010E,
    0xFFFFFF92, 0x00000073, 0x00000042, 0x0000010E,
    0xFFFFFF7F, 0x00000073, 0x00000022, 0x0000010E,
    0xFFFFFF7F, 0x00000073, 0x0000000B, 0x0000010E,
    0xFFFFFF75, 0x00000073, 0xFFFFFFF8, 0x0000010E,
    0xFFFFFF67, 0x00000073, 0xFFFFFFFA, 0x0000010E,
    0xFFFFFF85, 0x00000073, 0xFFFFFFEE, 0x0000010E,
    0xFFFFFF98, 0x00000073, 0xFFFFFFE5, 0x0000010E,
    0xFFFFFF64, 0x00000073, 0x0000001A, 0x0000010E,
    0xFFFFFF64, 0x00000073, 0x00000033, 0x0000010E,
    0xFFFFFF67, 0x00000073, 0x00000035, 0x0000010E,
    0xFFFFFF79, 0x00000073, 0x00000035, 0x0000010E,
    0xFFFFFF94, 0x00000073, 0x00000039, 0x0000010E,
    0xFFFFFFA1, 0x00000073, 0x00000043, 0x0000010E,
    0xFFFFFFAC, 0x00000073, 0x00000043, 0x0000010E,
    0xFFFFFF9A, 0x00000073, 0xFFFFFFCE, 0x0000010E,
    0xFFFFFF90, 0x00000073, 0xFFFFFFCF, 0x0000010E,
    0xFFFFFF7C, 0x00000073, 0xFFFFFFD9, 0x0000010E,
    0xFFFFFF6D, 0x00000073, 0xFFFFFFE3, 0x0000010E,
    0xFFFFFF67, 0x00000073, 0xFFFFFFD7, 0x0000010E,
    0xFFFFFF67, 0x00000073, 0xFFFFFFFE, 0x0000010E,
    0xFFFFFF76, 0x00000073, 0x0000000E, 0x0000010E,
    0xFFFFFF87, 0x00000073, 0x0000001F, 0x0000010E,
    0xFFFFFF97, 0x00000073, 0x00000023, 0x0000010E,
    0xFFFFFF97, 0x00000073, 0x00000035, 0x0000010E,
    0xFFFFFF8A, 0x00000073, 0x0000003E, 0x0000010E,
    0xFFFFFF81, 0x00000073, 0x0000002C, 0x0000010E,
    0xFFFFFF72, 0x00000073, 0x00000038, 0x0000010E,
    0xFFFFFF65, 0x00000073, 0x0000001B, 0x0000010E,
    0xFFFFFF77, 0x00000073, 0x00000008, 0x0000010E,
    0xFFFFFF77, 0x00000073, 0xFFFFFFE4, 0x0000010E,
    0xFFFFFF84, 0x00000073, 0xFFFFFFDB, 0x0000010E,
    0xFFFFFF95, 0x00000073, 0xFFFFFFC9, 0x0000010E,
    0xFFFFFFA5, 0x00000073, 0xFFFFFFD1, 0x0000010E,
    0xFFFFFFAA, 0x00000073, 0xFFFFFFDB, 0x0000010E,
    0xFFFFFFAA, 0x00000073, 0xFFFFFFB8, 0x0000010E,
    0xFFFFFF94, 0x00000073, 0xFFFFFFC6, 0x0000010E,
    0xFFFFFF7D, 0x00000073, 0xFFFFFFCB, 0x0000010E,
    0xFFFFFF67, 0x00000073, 0xFFFFFFCC, 0x0000010E,
    0xFFFFFF65, 0x00000073, 0xFFFFFFD4, 0x0000010E,
    0xFFFFFFE0, 0x00000028, 0xFFFFFFBE, 0x0000010E,
    0xFFFFFFC0, 0x00000028, 0xFFFFFFED, 0x0000010E,
    0xFFFFFFCC, 0x00000028, 0xFFFFFFDD, 0x0000010E,
    0xFFFFFFF4, 0x00000028, 0x0000003B, 0x0000010E,
    0xFFFFFFD9, 0x00000028, 0x00000038, 0x0000010E,
    0xFFFFFFCC, 0x00000028, 0x00000033, 0x0000010E,
    0xFFFFFFCA, 0x00000028, 0x0000004C, 0x0000010E,
    0xFFFFFFD5, 0x00000028, 0x00000042, 0x0000010E,
    0xFFFFFFED, 0x00000028, 0x00000035, 0x0000010E,
    0xFFFFFFE0, 0x00000028, 0x00000055, 0x0000010E,
    0xFFFFFFE6, 0x00000028, 0x00000047, 0x0000010E,
    0xFFFFFFD3, 0x00000028, 0x00000028, 0x0000010E,
    0xFFFFFFCC, 0x00000028, 0x00000008, 0x0000010E,
    0xFFFFFFCE, 0x00000028, 0xFFFFFFF7, 0x0000010E,
    0xFFFFFFEB, 0x00000028, 0xFFFFFFCC, 0x0000010E,
    0xFFFFFFE5, 0x00000028, 0xFFFFFFD6, 0x0000010E,
    0xFFFFFFDB, 0x00000028, 0xFFFFFFAF, 0x0000010E,
    0xFFFFFFCC, 0x00000028, 0xFFFFFFB2, 0x0000010E,
    0xFFFFFFDB, 0x00000028, 0xFFFFFFD2, 0x0000010E,
    0xFFFFFFC2, 0x00000028, 0xFFFFFFBA, 0x0000010E,
    0xFFFFFFC2, 0x00000028, 0xFFFFFFC7, 0x0000010E,
    0xFFFFFFC2, 0x00000028, 0x0000000B, 0x0000010E,
    0xFFFFFFC3, 0x00000028, 0xFFFFFFDA, 0x0000010E,
    0xFFFFFFD6, 0x00000028, 0xFFFFFFE5, 0x0000010E,
    0xFFFFFFEA, 0x00000028, 0xFFFFFFE8, 0x0000010E,
    0xFFFFFFEA, 0x00000028, 0xFFFFFFFB, 0x0000010E,
    0xFFFFFFEA, 0x00000028, 0x00000016, 0x0000010E,
    0xFFFFFFD7, 0x00000028, 0x00000016, 0x0000010E,
    0xFFFFFFC3, 0x00000028, 0x00000016, 0x0000010E,
    0xFFFFFFCB, 0x00000028, 0x00000029, 0x0000010E,
    0xFFFFFFC4, 0x00000028, 0x0000001C, 0x0000010E,
    0xFFFFFFCB, 0x00000028, 0xFFFFFFFF, 0x0000010E,
    0xFFFFFFDC, 0x00000028, 0xFFFFFFFD, 0x0000010E,
    0xFFFFFFE0, 0x00000028, 0x0000000D, 0x0000010E,
    0xFFFFFFE5, 0x00000028, 0x0000001F, 0x0000010E,
    0xFFFFFFE5, 0x00000028, 0x0000001F, 0x0000010E,
    0xFFFFFFF6, 0x00000028, 0x00000027, 0x0000010E,
    0xFFFFFFE3, 0x00000028, 0xFFFFFFF4, 0x0000010E,
    0xFFFFFFF2, 0x00000028, 0xFFFFFFDB, 0x0000010E,
    0xFFFFFFF4, 0x00000028, 0xFFFFFFBE, 0x0000010E,
    0xFFFFFFC5, 0x00000028, 0x0000003C, 0x0000010E,
    0xFFFFFFE0, 0x00000028, 0xFFFFFFE5, 0x0000010E,
    0xFFFFFFEF, 0x00000028, 0xFFFFFFAC, 0x0000010E,
    0xFFFFFFE6, 0x00000028, 0xFFFFFFB2, 0x0000010E,
    0xFFFFFFD1, 0x00000028, 0xFFFFFFBF, 0x0000010E,
    0xFFFFFFD1, 0x00000028, 0xFFFFFFD3, 0x0000010E,
    0xFFFFFFC4, 0x00000028, 0xFFFFFFD6, 0x0000010E,
    0xFFFFFFC4, 0x00000028, 0x00000022, 0x0000010E,
    0xFFFFFFC9, 0x00000028, 0x00000051, 0x0000010E,
    0xFFFFFFC3, 0x00000028, 0x00000041, 0x0000010E,
};

// TODO: type
u32 kaizoku_vs_dai3_elmospos[400] = {
    0x00000033, 0x000000A5, 0x00000019, 0x0000010E,
    0x00000022, 0x00000070, 0xFFFFFFC1, 0x0000010E,
    0x00000025, 0x0000009E, 0x00000043, 0x0000010E,
    0x00000018, 0x000000BF, 0x00000045, 0x0000010E,
    0x0000001C, 0x000000E4, 0xFFFFFFCF, 0x0000010E,
    0x00000021, 0x000000C9, 0xFFFFFFE0, 0x0000010E,
    0x0000001E, 0x000000C3, 0xFFFFFFFB, 0x0000010E,
    0x0000001F, 0x000000A6, 0x00000001, 0x0000010E,
    0x00000026, 0x00000090, 0x0000002F, 0x0000010E,
    0x00000024, 0x000000C0, 0x0000002B, 0x0000010E,
    0x00000029, 0x000000A4, 0x00000023, 0x000000F7,
    0x00000000, 0x000000F4, 0x00000011, 0x0000010E,
    0x00000027, 0x000000B9, 0x0000001F, 0x0000010E,
    0x00000021, 0x000000B7, 0xFFFFFF9C, 0x0000010E,
    0x00000007, 0x0000006C, 0xFFFFFFCB, 0x0000010E,
    0x00000024, 0x000000F9, 0x00000014, 0x0000010E,
    0x00000024, 0x000000A4, 0xFFFFFFB7, 0x0000010E,
    0x00000027, 0x00000094, 0x0000005D, 0x0000010E,
    0x00000006, 0x00000066, 0xFFFFFFB4, 0x0000010E,
    0x00000029, 0x0000008A, 0xFFFFFFE1, 0x0000010E,
    0x00000007, 0x000000EB, 0x0000003A, 0x0000010E,
    0x0000001E, 0x000000F1, 0xFFFFFFA3, 0x0000010E,
    0x0000001F, 0x000000D6, 0x0000005B, 0x0000010E,
    0x00000020, 0x00000084, 0x00000006, 0x0000010E,
    0x00000022, 0x000000B9, 0x0000000E, 0x0000010E,
    0x00000029, 0x00000094, 0xFFFFFFF3, 0x0000010E,
    0x00000026, 0x0000008D, 0xFFFFFFB8, 0x0000010E,
    0x00000023, 0x000000D1, 0xFFFFFFCB, 0x0000010E,
    0x00000026, 0x00000098, 0x00000007, 0x0000010E,
    0x00000021, 0x0000006C, 0xFFFFFFEF, 0x0000010E,
    0x0000000E, 0x000000E6, 0xFFFFFFF5, 0x0000010E,
    0x00000023, 0x00000075, 0x00000073, 0x0000010E,
    0x00000009, 0x000000F7, 0x0000001E, 0x0000010E,
    0x00000028, 0x000000A1, 0xFFFFFFC6, 0x0000010E,
    0x0000000A, 0x00000064, 0x00000041, 0x0000010E,
    0x00000025, 0x00000071, 0x00000050, 0x0000010E,
    0x00000007, 0x000000CE, 0x00000077, 0x0000010E,
    0x0000001C, 0x000000F7, 0x0000006B, 0x0000010E,
    0x00000024, 0x000000BC, 0xFFFFFFB8, 0x0000010E,
    0x00000025, 0x000000B0, 0xFFFFFFE8, 0x0000010E,
    0x0000000E, 0x000000B5, 0xFFFFFFB7, 0x0000010E,
    0x00000022, 0x000000B0, 0xFFFFFFCB, 0x0000010E,
    0x00000029, 0x00000085, 0x0000001A, 0x0000010E,
    0x00000017, 0x000000CF, 0x0000003D, 0x0000010E,
    0x0000001E, 0x000000C9, 0x00000019, 0x0000010E,
    0x0000000A, 0x000000ED, 0x00000039, 0x0000010E,
    0x0000001E, 0x000000EF, 0x00000061, 0x0000010E,
    0x00000013, 0x000000F1, 0x0000000D, 0x0000010E,
    0x00000020, 0x000000BB, 0x0000004F, 0x0000010E,
    0x00000020, 0x000000B8, 0x0000007D, 0x0000010E,
    0x00000008, 0x000000C0, 0x00000013, 0x0000010E,
    0x00000019, 0x000000D6, 0x00000026, 0x0000010E,
    0x00000018, 0x00000091, 0x00000025, 0x0000010E,
    0x00000012, 0x000000E8, 0xFFFFFFAA, 0x0000010E,
    0x0000000C, 0x000000DF, 0x00000026, 0x0000010E,
    0x00000015, 0x000000D4, 0x00000010, 0x0000010E,
    0x00000027, 0x00000077, 0x0000002D, 0x0000010E,
    0x00000022, 0x000000C6, 0x0000006B, 0x0000010E,
    0x00000022, 0x0000006D, 0xFFFFFF8F, 0x0000010E,
    0x0000001D, 0x000000CD, 0xFFFFFFAD, 0x0000010E,
    0x00000020, 0x000000D6, 0x00000074, 0x0000010E,
    0x0000000F, 0x000000DF, 0x00000000, 0x0000010E,
    0x00000003, 0x0000007E, 0x0000001F, 0x0000010E,
    0x00000025, 0x000000C7, 0xFFFFFFD3, 0x0000010E,
    0x00000021, 0x000000D9, 0xFFFFFFEE, 0x0000010E,
    0x00000007, 0x00000065, 0xFFFFFFF3, 0x0000010E,
    0x00000024, 0x0000007E, 0xFFFFFFA8, 0x0000010E,
    0x00000015, 0x000000E8, 0xFFFFFF8F, 0x0000010E,
    0x00000024, 0x0000008D, 0x0000007D, 0x0000010E,
    0x00000019, 0x00000064, 0xFFFFFFDF, 0x0000010E,
    0x00000002, 0x000000D2, 0x0000002F, 0x0000010E,
    0x00000029, 0x000000A6, 0x0000007D, 0x0000010E,
    0x00000027, 0x000000B0, 0x00000064, 0x0000010E,
    0x0000002A, 0x0000008A, 0x00000046, 0x000000EE,
    0x00000008, 0x000000E2, 0x00000002, 0x0000010E,
    0x00000028, 0x000000A4, 0x00000055, 0x0000010E,
    0x00000022, 0x000000AC, 0x00000039, 0x0000010E,
    0x0000001C, 0x000000E4, 0x0000004A, 0x0000010E,
    0x0000000F, 0x000000E5, 0xFFFFFFBC, 0x0000010E,
    0x00000026, 0x0000006C, 0x0000006B, 0x0000010E,
    0x00000013, 0x000000EF, 0xFFFFFFDB, 0x0000010E,
    0x00000018, 0x00000073, 0x00000070, 0x0000010E,
    0x00000023, 0x000000D1, 0xFFFFFFA0, 0x0000010E,
    0x00000010, 0x0000007D, 0x00000005, 0x0000010E,
    0x00000007, 0x000000A6, 0x00000074, 0x0000010E,
    0x00000026, 0x00000079, 0x0000005A, 0x0000010E,
    0x00000024, 0x00000096, 0xFFFFFF97, 0x0000010E,
    0x00000024, 0x000000A3, 0xFFFFFF8F, 0x0000010E,
    0x0000001D, 0x000000A7, 0xFFFFFF91, 0x0000010E,
    0x00000023, 0x000000CE, 0xFFFFFF8A, 0x0000010E,
    0x0000000E, 0x000000F7, 0x00000025, 0x0000010E,
    0x00000008, 0x000000F9, 0x00000043, 0x0000010E,
    0x00000003, 0x000000B8, 0xFFFFFFFA, 0x0000010E,
    0x00000007, 0x000000B6, 0x0000007C, 0x0000010E,
    0x00000013, 0x00000073, 0xFFFFFFCE, 0x0000010E,
    0x00000024, 0x000000A0, 0xFFFFFFA6, 0x0000010E,
    0x0000000C, 0x00000067, 0x00000018, 0x0000010E,
    0x00000006, 0x000000A7, 0x00000047, 0x0000010E,
    0x0000001D, 0x000000BA, 0x00000044, 0x0000010E,
    0x00000000, 0x000000B3, 0xFFFFFFDA, 0x0000010E,
};

EVT_BEGIN(iwapera_offscreen)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_yabu"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_iwa_before"), STRING("offscreen"))
    CALL(evt_offscreen_get_boundingbox, STRING("offscreen"), LW(10), LW(11), LW(12), LW(13))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, 0, 1, LW(10), LW(11), LW(12), LW(13))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 2, STRING("offscreen"), 1, LW(10), LW(11), LW(12), LW(13))
    WAIT_MS(1500)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_iwa_before"), 1)
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
    RETURN
EVT_END()

EVT_BEGIN(iwapera_offscreen_end)
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_iwa_before"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_iwa_after"), 1)
    RETURN
EVT_END()

EVT_BEGIN(dai3_kurukuru)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_IMPACT1"), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.02f), FLOAT(0.0f), 500)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), GW(1))
        SET_VALUE(LW(1), LW(0))
        ADD(LW(1), 15)
        CALL(evt_sub_intpl_msec_init, 4, LW(0), LW(1), 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(GW(1), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), GW(2))
        SET_VALUE(LW(1), LW(0))
        ADD(LW(1), -80)
        CALL(evt_sub_intpl_msec_init, 4, LW(0), LW(1), 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(GW(2), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(1000)
    RETURN
EVT_END()

// TODO: type
u32 kaizoku_vs_dai3_dai3pos[300] = {
    0x0000006B, 0x0000009F, 0xFFFFFF88, 0x0000000A,
    0x000000F5, 0xFFFFFFC7, 0x00000008, 0x00000098,
    0x00000028, 0x00000064, 0x000000A4, 0xFFFFFFA9,
    0x00000003, 0x00000079, 0xFFFFFFE4, 0xFFFFFFF3,
    0x00000082, 0xFFFFFFFB, 0x0000006A, 0x000000D4,
    0xFFFFFFC5, 0xFFFFFFF2, 0x000000BA, 0xFFFFFFA9,
    0xFFFFFFFE, 0x000000C4, 0x00000042, 0x0000006D,
    0x000000D6, 0x00000036, 0xFFFFFFF5, 0x0000009C,
    0x0000003B, 0x0000005D, 0x000000E3, 0xFFFFFF93,
    0x00000068, 0x000000E3, 0xFFFFFFAA, 0x00000063,
    0x00000086, 0xFFFFFFE8, 0x00000000, 0x000000C8,
    0xFFFFFF75, 0x00000066, 0x00000069, 0xFFFFFFDB,
    0x0000005B, 0x0000007A, 0xFFFFFFDD, 0x00000065,
    0x0000009F, 0xFFFFFFD6, 0xFFFFFFEE, 0x000000CF,
    0xFFFFFFA6, 0xFFFFFFF1, 0x000000E7, 0xFFFFFF6E,
    0xFFFFFFF0, 0x00000084, 0xFFFFFFD9, 0x0000006A,
    0x0000007B, 0xFFFFFFF8, 0x00000060, 0x0000007B,
    0xFFFFFFA2, 0xFFFFFFED, 0x0000008D, 0x0000005E,
    0xFFFFFFEE, 0x00000105, 0xFFFFFFF8, 0xFFFFFFEE,
    0x00000089, 0x0000003D, 0xFFFFFFFE, 0x0000006D,
    0xFFFFFFFB, 0x0000005E, 0x000000C3, 0xFFFFFF9D,
    0xFFFFFFF3, 0x000000D1, 0xFFFFFF7B, 0xFFFFFFFE,
    0x000000E4, 0xFFFFFFF6, 0x00000061, 0x000000BC,
    0xFFFFFF88, 0xFFFFFFEE, 0x000000D3, 0xFFFFFFE7,
    0xFFFFFFFD, 0x000000E0, 0xFFFFFFBF, 0xFFFFFFFE,
    0x00000099, 0x00000055, 0x0000005F, 0x000000BF,
    0xFFFFFF8B, 0x0000006B, 0x000000DC, 0x00000021,
    0x00000008, 0x00000081, 0xFFFFFFD1, 0x00000002,
    0x00000099, 0x00000084, 0xFFFFFFF3, 0x00000077,
    0x00000021, 0x00000063, 0x0000007F, 0xFFFFFFC1,
    0xFFFFFFF8, 0x000000D1, 0xFFFFFFBD, 0xFFFFFFF7,
    0x000000C3, 0x0000005D, 0xFFFFFFF9, 0x000000C6,
    0x0000007F, 0x00000007, 0x0000007B, 0xFFFFFFA7,
    0x00000055, 0x000000AF, 0x00000065, 0x0000006E,
    0x000000AF, 0xFFFFFF9B, 0x00000001, 0x0000009B,
    0xFFFFFF74, 0x00000007, 0x00000098, 0xFFFFFFC3,
    0x00000066, 0x0000006B, 0xFFFFFFBB, 0x00000062,
    0x00000086, 0x00000060, 0x0000006E, 0x00000088,
    0x00000023, 0x00000004, 0x0000008B, 0x00000031,
    0xFFFFFFFD, 0x00000083, 0x00000028, 0x00000066,
    0x000000B6, 0xFFFFFFD1, 0x00000005, 0x000000F7,
    0x0000005A, 0xFFFFFFFD, 0x000000B3, 0xFFFFFFED,
    0xFFFFFFFD, 0x00000078, 0xFFFFFFA8, 0x00000066,
    0x000000A7, 0xFFFFFFF1, 0x00000006, 0x000000DF,
    0x0000008B, 0x0000005F, 0x000000C3, 0xFFFFFFEC,
    0x0000005B, 0x000000AA, 0x00000006, 0x0000000A,
    0x000000D9, 0x00000029, 0xFFFFFFED, 0x00000091,
    0x00000079, 0xFFFFFFF9, 0x00000085, 0x00000038,
    0x00000005, 0x000000ED, 0xFFFFFFE4, 0xFFFFFFFC,
    0x000000D1, 0xFFFFFFA4, 0x00000060, 0x000000DC,
    0x0000005E, 0x00000063, 0x000000B6, 0x0000004B,
    0x00000065, 0x000000DF, 0x00000055, 0x0000005B,
    0x0000007D, 0xFFFFFFD2, 0x00000006, 0x000000FA,
    0x00000033, 0x00000004, 0x000000D5, 0x0000005A,
    0x00000065, 0x000000B3, 0x00000029, 0xFFFFFFF6,
    0x000000F4, 0x00000010, 0x00000001, 0x00000086,
    0x00000014, 0x00000068, 0x0000008F, 0x0000000E,
    0x00000067, 0x000000CB, 0xFFFFFFB7, 0xFFFFFFFF,
    0x000000F9, 0x00000086, 0x00000007, 0x000000C8,
    0xFFFFFFCF, 0xFFFFFFFE, 0x000000EE, 0xFFFFFFC6,
    0xFFFFFFFA, 0x000000F5, 0xFFFFFF92, 0x00000067,
    0x00000081, 0x00000045, 0x0000005D, 0x000000A2,
    0x00000059, 0x00000000, 0x0000007A, 0xFFFFFFD4,
    0x00000001, 0x00000070, 0x00000047, 0x0000006B,
    0x000000A3, 0x00000037, 0xFFFFFFF1, 0x000000A9,
    0x00000012, 0x00000005, 0x000000B7, 0x0000006B,
    0x00000054, 0x000000C9, 0x0000000A, 0xFFFFFFF7,
    0x0000007C, 0xFFFFFF76, 0x0000006C, 0x000000B1,
    0xFFFFFFB5, 0x00000007, 0x000000EA, 0xFFFFFFE6,
    0x00000002, 0x000000AA, 0x00000037, 0xFFFFFFF4,
    0x0000007C, 0x0000005C, 0x0000006D, 0x000000D0,
    0xFFFFFFB6, 0xFFFFFFF6, 0x00000084, 0xFFFFFFB1,
    0xFFFFFFF1, 0x00000087, 0x0000004F, 0xFFFFFFFE,
    0x00000108, 0x00000070, 0xFFFFFFF3, 0x00000082,
    0x00000002, 0xFFFFFFEE, 0x000000D9, 0xFFFFFFA2,
};

EVT_BEGIN(dai3_butai_setup)
    SET_VALUE(LW(0), 205)
    SET_INT_READ_ADDR(PTR(kaizoku_vs_dai3_dai3pos))
    LOOP(100)
        CALL(evt_nannpc_set_dir, LW(0), 90)
        READ_INT3(LW(1), LW(2), LW(3))
        ADD(LW(1), GW(2))
        ADD(LW(3), GW(3))
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        SET_VALUE(LW(12), 0)
        SUBTRACT(LW(12), GW(3))
        SET_VALUE(LW(13), 0)
        SUBTRACT(LW(13), LW(3))
        CALL(evt_sub_get_dir, GW(2), LW(12), LW(1), LW(13), LW(10))
        CALL(evt_sub_get_dist, GW(2), LW(12), LW(1), LW(13), LW(11))
        CALL(evt_nannpc_set_work, LW(0), 0, LW(10))
        CALL(evt_nannpc_set_work, LW(0), 1, LW(11))
        ADD(LW(0), 1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(dai3_butai_main)
    SET_VALUE(LW(10), GW(1))
    LOOP(0)
        CALL(dai3_butai_main_func, LW(10), GW(1), GW(2), GW(3))
        SET_VALUE(LW(10), GW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dai3_kanbu_setup)
    RETURN
EVT_END()

EVT_BEGIN(dai3_kanbu_main)
    CALL(evt_npc_get_position, STRING("第三勢力幹部"), LW(1), LW(2), LW(3))
    CALL(evt_sub_get_dir, GW(2), GW(3), LW(1), LW(3), GW(10))
    CALL(evt_sub_get_dist, GW(2), GW(3), LW(1), LW(3), GW(11))
    SET_VALUE(LW(10), GW(1))
    LOOP(0)
        SET_VALUE(LW(11), GW(1))
        SUBTRACT(LW(11), LW(10))
        SUBTRACT(GW(10), LW(11))
        SET_VALUE(LW(0), 0)
        SUBTRACT(LW(0), GW(10))
        ADD(LW(0), 180)
        CALL(evt_sub_get_sincos, LW(0), LW(14), LW(15))
        MULTIPLY_FLOAT(LW(14), GW(11))
        MULTIPLY_FLOAT(LW(15), GW(11))
        ADD_FLOAT(LW(14), GW(2))
        ADD_FLOAT(LW(15), GW(3))
        CALL(evt_npc_get_position, STRING("第三勢力幹部"), LW(1), LW(2), LW(3))
        CALL(evt_npc_set_position, STRING("第三勢力幹部"), LW(14), LW(2), LW(15))
        SET_VALUE(LW(10), GW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(kaizoku_setup)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 270, -1)
    CALL(evt_party_set_dir, 1, 270, -1)
    SET_VALUE(LW(0), 10)
    SET_INT_READ_ADDR(PTR(kaizoku_vs_dai3_suifupos))
    LOOP(95)
        READ_INT3(LW(1), LW(2), LW(3))
        ADD(LW(1), GW(5))
        ADD(LW(3), GW(6))
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        READ_INT(LW(4))
        CALL(evt_nannpc_set_dir, LW(0), LW(4))
        SET_VALUE(LW(12), 0)
        SUBTRACT(LW(12), GW(6))
        SET_VALUE(LW(13), 0)
        SUBTRACT(LW(13), LW(3))
        CALL(evt_sub_get_dir, GW(5), LW(12), LW(1), LW(13), LW(10))
        CALL(evt_sub_get_dist, GW(5), LW(12), LW(1), LW(13), LW(11))
        CALL(evt_nannpc_set_work, LW(0), 0, LW(10))
        CALL(evt_nannpc_set_work, LW(0), 1, LW(11))
        ADD(LW(0), 1)
    END_LOOP
    SET_VALUE(LW(0), 105)
    SET_INT_READ_ADDR(PTR(kaizoku_vs_dai3_elmospos))
    LOOP(100)
        READ_INT3(LW(1), LW(2), LW(3))
        ADD(LW(1), GW(5))
        ADD(LW(3), GW(6))
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        READ_INT(LW(4))
        CALL(evt_nannpc_set_dir, LW(0), LW(4))
        SET_VALUE(LW(12), 0)
        SUBTRACT(LW(12), GW(6))
        SET_VALUE(LW(13), 0)
        SUBTRACT(LW(13), LW(3))
        CALL(evt_sub_get_dir, GW(5), LW(12), LW(1), LW(13), LW(10))
        CALL(evt_sub_get_dist, GW(5), LW(12), LW(1), LW(13), LW(11))
        CALL(evt_nannpc_set_work, LW(0), 0, LW(10))
        CALL(evt_nannpc_set_work, LW(0), 1, LW(11))
        CALL(evt_nannpc_ppflag_onoff, LW(0), 0, 4)
        ADD(LW(0), 1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(kaizoku_main)
    SET_VALUE(LW(10), GW(4))
    LOOP(0)
        CALL(kaizoku_main_func, LW(10), GW(4), GW(5), GW(6))
        SET_VALUE(LW(10), GW(4))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(mario_main_init)
    CALL(mario_main_init_func)
    RETURN
EVT_END()


EVT_BEGIN(mario_main)
    CALL(evt_mario_set_pos, 20, 40, 10)
    CALL(evt_party_set_pos, 0, 10, 40, -65)
    CALL(evt_npc_set_position, STRING("マルコ"), 20, 40, -35)
    CALL(evt_npc_set_position, STRING("コルテス"), 126, 75, -30)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), 32, 40, 35)
    CALL(evt_npc_set_position, STRING("モニー"), 10, 40, 36)
    CALL(evt_npc_set_ry, STRING("モニー"), 270)
    CALL(evt_npc_set_position, STRING("ピートン"), 15, 40, 72)
    CALL(evt_npc_set_ry, STRING("ピートン"), 270)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), -6, 40, -88)
    CALL(evt_npc_set_ry, STRING("オドオド水夫"), 270)
    CALL(mario_main_setup_func, GW(5), GW(6))
    SET_VALUE(LW(10), GW(4))
    LOOP(0)
        CALL(mario_main_func, LW(10), GW(4), GW(5), GW(6))
        SET_VALUE(LW(10), GW(4))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mario_main2)
    CALL(evt_mario_set_pos, 20, 40, 10)
    CALL(evt_party_set_pos, 0, 10, 40, -25)
    CALL(evt_npc_set_position, STRING("マルコ"), -3, 40, -69)
    CALL(evt_npc_set_position, STRING("コルテス"), 126, 75, -30)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), 32, 40, 35)
    CALL(evt_npc_set_position, STRING("モニー"), 10, 40, 36)
    CALL(evt_npc_set_ry, STRING("モニー"), 270)
    CALL(evt_npc_set_position, STRING("ピートン"), 15, 40, 72)
    CALL(evt_npc_set_ry, STRING("ピートン"), 270)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), -6, 40, -88)
    CALL(evt_npc_set_ry, STRING("オドオド水夫"), 270)
    CALL(mario_main_setup_func, GW(5), GW(6))
    SET_VALUE(LW(10), GW(4))
    LOOP(0)
        CALL(mario_main_func, LW(10), GW(4), GW(5), GW(6))
        SET_VALUE(LW(10), GW(4))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(kaizoku_move_every)
    LOOP(0)
        CALL(evt_mapobj_trans, 1, STRING("S_b"), GW(5), 0, GW(6))
        CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, GW(4), 0)
        CALL(evt_hit_bind_update, STRING("A_b"))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dai3_move_every)
    LOOP(0)
        CALL(evt_mapobj_trans, 1, STRING("S_c"), GW(2), 0, GW(3))
        CALL(evt_mapobj_rotate, 1, STRING("S_c"), 0, GW(1), 0)
        SET_FLOAT_VALUE(GW(13), GW(2))
        SET_FLOAT_VALUE(GW(14), 0)
        SET_FLOAT_VALUE(GW(15), GW(3))
        CALL(evt_hit_bind_update, STRING("A_c"))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

// TODO: type
u32 dai3_spline_dt[27] = {
    0xC4960000, 0x00000000, 0x80000000, 0xC48991FE,
    0x00000000, 0xC1074163, 0xC392EC96, 0x00000000,
    0x43C79D89, 0x420B1FB1, 0x00000000, 0x43D84891,
    0x43D2A5E1, 0x00000000, 0x43786DB7, 0x43E6D697,
    0x00000000, 0x4325A666, 0x43CF9B4B, 0x00000000,
    0xC3C6A3D5, 0xC3A522A7, 0x00000000, 0xC3F80FC3,
    0xC3B552E6, 0x00000000, 0xC3A60FA7,
};

// TODO: type
u32 kaizoku_spline_dt[30] = {
    0xC20D6AF0, 0x00000000, 0xC4E64A90, 0xC31090B4,
    0x00000000, 0xC4504A56, 0xC381B47C, 0x00000000,
    0xC42FC297, 0xC420CABB, 0x00000000, 0xC38BF6B4,
    0xC3DC9598, 0x00000000, 0x43E7A9D8, 0x42F5F590,
    0x00000000, 0x4429859F, 0x43C71A01, 0x00000000,
    0x4414AF58, 0x44178467, 0x00000000, 0x427D87EC,
    0x4347E362, 0x00000000, 0xC3C9D7A6, 0xC2A091F2,
    0x00000000, 0xC384E476,
};

EVT_BEGIN(kaizoku_move_trace_se)
    CALL(evt_mapobj_get_position, STRING("S_b"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_KOR_SHIP_MOVE1"), LW(10), LW(11), LW(12), LW(13))
    CALL(evt_snd_sfx_dist, LW(13), 30)
    BEGIN_INLINE_EVENT_GET_ID(LW(14))
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("S_b"), LW(10), LW(11), LW(12))
            CALL(evt_snd_sfx_pos, LW(13), LW(10), LW(11), LW(12))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_WHILE(GF(10))
    CALL(evt_snd_sfxoff, LW(13))
    DELETE_EVENT(LW(14))
    RETURN
EVT_END()


EVT_BEGIN(kaizoku_move_trace)
    SET_FLOAT_VALUE(LW(5), GW(5))
    SET_FLOAT_VALUE(LW(6), GW(6))
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), LW(0))
    MULTIPLY(LW(9), 100)
    CALL(evt_sub_spline_init, 1, 0, PTR(kaizoku_spline_dt), 10, 0, LW(0))
    LOOP(0)
        SET_VALUE(LW(10), LW(8))
        DIVIDE(LW(10), 100)
        CALL(evt_sub_spline_get_value_manual, LW(10), LW(0), LW(1), LW(2))
        SWITCH(LW(8))
            CASE_LESS(200000)
            SET_VALUE(LW(7), 600)
            CASE_LESS(300000)
            SET_VALUE(LW(7), 1000)
            CASE_LESS(400000)
            SET_VALUE(LW(7), 1300)
            CASE_LESS(600000)
            SET_VALUE(LW(7), 1600)
            CASE_LESS(800000)
            SET_VALUE(LW(7), 2000)
        END_SWITCH
        ADD(LW(8), LW(7))
        SET_VALUE(LW(3), 1)
        IF_INT_GREATER_OR_EQUAL(LW(8), LW(9))
            SET_VALUE(LW(3), 0)
        END_IF
        SET_FLOAT_VALUE(GW(5), LW(0))
        SET_FLOAT_VALUE(GW(6), LW(2))
        SET_VALUE(LF(0), 1)
        IF_FLOAT_EQUAL(GW(5), LW(5))
            IF_FLOAT_EQUAL(GW(6), LW(6))
                SET_VALUE(LF(0), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 1)
            CALL(ship_rotate, LW(5), LW(6), GW(5), GW(6), GW(4))
        END_IF
        IF_INT_EQUAL(LW(3), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
        SET_FLOAT_VALUE(LW(5), GW(5))
        SET_FLOAT_VALUE(LW(6), GW(6))
    END_LOOP
    CALL(evt_sub_spline_free)
    SET_VALUE(GF(10), 0)
    RETURN
EVT_END()

EVT_BEGIN(dai3_move_trace_se)
    CALL(evt_mapobj_get_position, STRING("S_c"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_3RD_SHIP_MOVE1"), LW(10), LW(11), LW(12), LW(13))
    CALL(evt_snd_sfx_dist, LW(13), 30)
    BEGIN_INLINE_EVENT_GET_ID(LW(14))
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("S_b"), LW(10), LW(11), LW(12))
            CALL(evt_snd_sfx_pos, LW(13), LW(10), LW(11), LW(12))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_WHILE(GF(11))
    CALL(evt_snd_sfxoff, LW(13))
    DELETE_EVENT(LW(14))
    RETURN
EVT_END()

EVT_BEGIN(dai3_move_trace)
    SET_FLOAT_VALUE(LW(5), GW(2))
    SET_FLOAT_VALUE(LW(6), GW(3))
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), LW(0))
    MULTIPLY(LW(9), 100)
    CALL(evt_sub_spline_init, 1, 0, PTR(dai3_spline_dt), 9, 0, LW(0))
    LOOP(0)
        SET_VALUE(LW(10), LW(8))
        DIVIDE(LW(10), 100)
        CALL(evt_sub_spline_get_value_manual, LW(10), LW(0), LW(1), LW(2))
        ADD(LW(8), 1115)
        SET_VALUE(LW(3), 1)
        IF_INT_GREATER_OR_EQUAL(LW(8), LW(9))
            SET_VALUE(LW(3), 0)
        END_IF
        SET_FLOAT_VALUE(GW(2), LW(0))
        SET_FLOAT_VALUE(GW(3), LW(2))
        SET_VALUE(LF(0), 1)
        IF_FLOAT_EQUAL(GW(2), LW(5))
            IF_FLOAT_EQUAL(GW(3), LW(6))
                SET_VALUE(LF(0), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 1)
            CALL(ship_rotate, LW(5), LW(6), GW(2), GW(3), GW(1))
        END_IF
        IF_INT_EQUAL(LW(3), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
        SET_FLOAT_VALUE(LW(5), GW(2))
        SET_FLOAT_VALUE(LW(6), GW(3))
    END_LOOP
    CALL(evt_sub_spline_free)
    RETURN
EVT_END()

static EVT_BEGIN(_kemuri_sub)
    CALL(evt_sub_random, 300, LW(0))
    ADD(LW(0), -400)
    CALL(evt_sub_random, 150, LW(1))
    ADD(LW(1), 150)
    CALL(evt_sub_random, 50, LW(2))
    ADD(LW(2), -300)
    CALL(evt_sub_random, 100, LW(3))
    ADD_FLOAT(LW(3), FLOAT(50.0f))
    DIVIDE_FLOAT(LW(3), FLOAT(100.0f))
    CALL(evt_eff, 0, STRING("kemuri_test"), 2, LW(0), LW(1), LW(2), LW(3), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_3RD_ATTACK3"), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(tobikakari_kemuri)
    LOOP(0)
        SPAWN_EVENT(_kemuri_sub)
        SPAWN_EVENT(_kemuri_sub)
        CALL(evt_sub_random, 200, LW(0))
        ADD(LW(0), 200)
        WAIT_MS(LW(0))
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(cam_dai3_vs_kaizoku)
    SET_VALUE(LW(0), 70)
    SET_VALUE(LW(1), 1400)
    CALL(cam_test_init_func, LW(0), LW(1), LW(2), LW(3))
    ADD_FLOAT(LW(2), FLOAT(-120.0f))
    CALL(evt_cam3d_evt_set, LW(0), 110, LW(1), 70, 90, 0, 0, 11)
    LOOP(0)
        CALL(cam_test_func, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_cam3d_evt_set, LW(0), 110, LW(1), 70, 90, 0, 0, 11)
        ADD_FLOAT(LW(2), FLOAT(0.27f))
        IF_INT_GREATER_OR_EQUAL(LW(2), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(2), FLOAT(360.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(kaizoku_vs_dai3)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_clear_party, 1)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("コルテス"), 20, 40, -1800)
    CALL(evt_npc_set_ry, STRING("コルテス"), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 1024)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), -945, 150, -5)
    CALL(evt_npc_set_ry, STRING("第三勢力幹部"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("第三勢力幹部"), 1073774624)
    CALL(evt_npc_flag_onoff, 0, STRING("第三勢力幹部"), 1024)
    CALL(evt_npc_flag_onoff, 0, STRING("モニー"), 1024)
    CALL(evt_npc_flag_onoff, 0, STRING("ピートン"), 1024)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 1024)
    SET_VALUE(GW(1), 0)
    SET_VALUE(GW(2), -1200)
    SET_VALUE(GW(3), 0)
    SPAWN_EVENT_GET_ID(PTR(dai3_move_every), LW(10))
    SET_VALUE(GW(4), 0)
    SET_VALUE(GW(5), 0)
    SET_VALUE(GW(6), -2000)
    SPAWN_EVENT_GET_ID(PTR(kaizoku_move_every), LW(11))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai"), 1)
    CALL(evt_cam3d_evt_set, -847, 334, 977, -847, 78, -475, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_msg_print, 0, STRING("stg5_muj_118"), 0, STRING("第三勢力幹部"))
    DELETE_EVENT(GW(8))
    CALL(evt_cam3d_evt_set, 0, 470, -91, 0, 400, -383, 500, 11)
    WAIT_MS(600)
    CALL(evt_npc_flag_onoff, 0, STRING("第三勢力幹部"), 32768)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), -1105, 60, 30)
    SPAWN_EVENT(dai3_butai_setup)
    WAIT_FRAMES(2)
    SPAWN_EVENT_GET_ID(PTR(dai3_butai_main), LW(12))
    SPAWN_EVENT_GET_ID(PTR(dai3_kanbu_main), LW(13))
    SPAWN_EVENT(kaizoku_setup)
    WAIT_FRAMES(2)
    SPAWN_EVENT_GET_ID(PTR(kaizoku_main), LW(14))
    SPAWN_EVENT_GET_ID(PTR(mario_main), LW(15))
    WAIT_MS(400)
    CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 33554432)
    CALL(evt_npc_pera_onoff, STRING("コルテス"), 0)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_119"), 0, STRING("コルテス"))
    CALL(evt_msg_continue)
    SET_VALUE(GF(0), 1)
    SET_VALUE(GF(1), 1)
    BEGIN_INLINE_EVENT
        SET_VALUE(GF(10), 1)
        SPAWN_EVENT(kaizoku_move_trace_se)
        SET_VALUE(LW(0), 20000)
        CALL_CHILD(kaizoku_move_trace)
        SET_VALUE(GF(10), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SET_VALUE(GF(11), 1)
        SPAWN_EVENT(dai3_move_trace_se)
        SET_VALUE(LW(0), 15000)
        CALL_CHILD(dai3_move_trace)
        SET_VALUE(GF(11), 0)
        CALL_CHILD(dai3_kurukuru)
        SET_VALUE(GF(1), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_msg_print, 0, STRING("stg5_muj_120"), 0, STRING("第三勢力幹部"))
        CALL(evt_msg_continue)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL_CHILD(iwapera_offscreen)
        CALL(evt_mapobj_get_position, STRING("S_iwa_before"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon, STRING("SFX_OFF5_SHIP_APPEAR1"), 0)
        CALL(evt_snd_bgmon, 512, STRING("BGM_BOSS_STG5_KORTESS1"))
        WAIT_MS(3500)
        CALL(evt_cam3d_evt_set, 0, 2650, 2025, 0, 250, 225, 500, 11)
        CALL_CHILD(iwapera_offscreen_end)
        WAIT_MS(4000)
        SPAWN_EVENT_GET_ID(PTR(cam_dai3_vs_kaizoku), LW(10))
        WAIT_MS(9500)
        DELETE_EVENT(LW(10))
        CALL(evt_cam3d_evt_set, -205, 528, 672, -205, 108, -278, 0, 11)
        WAIT_MS(1400)
        CALL(evt_cam3d_evt_set, -700, 290, 74, -374, 80, -316, 0, 11)
        WAIT_MS(200)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(0))
    WAIT_WHILE(GF(1))
    DELETE_EVENT(LW(10))
    DELETE_EVENT(LW(11))
    DELETE_EVENT(LW(12))
    DELETE_EVENT(LW(13))
    DELETE_EVENT(LW(14))
    DELETE_EVENT(LW(15))
    CALL(evt_msg_print, 0, STRING("stg5_muj_121"), 0, STRING("第三勢力幹部"))
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -120, 320, 259, -163, 115, -249, 0, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_122"), 0, STRING("コルテス"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -436, 595, 725, -226, 120, -460, 500, 11)
    WAIT_MS(1000)
    CALL(tobikakaru_init)
    SPAWN_EVENT_GET_ID(PTR(tobikakari_kemuri), LW(15))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELM_VS_3RD1"), -226, 120, -460, LW(9))
    CALL(evt_snd_sfx_dist, LW(9), 30)
    WAIT_MS(2500)
    CALL(evt_cam3d_evt_set, -369, 245, -37, 0, 67, -303, 0, 11)
    CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
    CALL(evt_mario_set_dir, 90, 200, 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_123"), 0, STRING("マルコ"))
    CALL(evt_cam3d_evt_set, -436, 595, 725, -226, 120, -460, 0, 11)
    CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
    WAIT_MS(500)
    CALL(evt_party_run, 0)
    BEGIN_INLINE_EVENT_GET_ID(LW(3))
        CALL(evt_party_set_dir, 0, 270, 200)
        CALL(evt_party_jump_pos, 0, -405, 60, -237, 1000, 0, 80)
    END_INLINE_EVENT
    CALL(evt_mario_set_dir, 270, 200, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_jump_pos, -430, 60, -212, 1000, 0, 80)
    CALL(evt_snd_sfxoff, LW(9))
    LOOP(0)
        IS_EVENT_RUNNING(LW(3), LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_npc_release_fieldnpc, 1)
    CALL(evt_npc_set_battle_info, STRING("第三勢力幹部"), 5)
    CALL(evt_npc_battle_start, STRING("第三勢力幹部"))
    CALL(evt_npc_wait_battle_end)
    DELETE_EVENT(LW(15))
    CALL(tobikakaru_end)
    SET_VALUE(GSW(0), 260)
    SET_VALUE(LW(0), 0)
    CALL_CHILD(dai3_beat_back)
    RETURN
EVT_END()

EVT_BEGIN(senin_kansei)
    LOOP(0)
        WAIT_MS(400)
        SET_VALUE(LW(0), GW(5))
        SET_VALUE(LW(2), GW(6))
        BEGIN_INLINE_EVENT
            ADD(LW(0), -30)
            CALL(evt_msg_toge, 2, LW(0), 200, LW(2))
            CALL(evt_msg_print, 0, STRING("stg5_muj_127"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(300)
        BEGIN_INLINE_EVENT
            ADD(LW(0), -32)
            CALL(evt_msg_toge, 2, LW(0), 100, LW(2))
            CALL(evt_msg_print, 0, STRING("stg5_muj_128"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(200)
        BEGIN_INLINE_EVENT
            ADD(LW(0), 100)
            CALL(evt_msg_toge, 2, LW(0), 155, LW(2))
            CALL(evt_msg_print, 0, STRING("stg5_muj_129"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(300)
        BEGIN_INLINE_EVENT
            ADD(LW(0), -40)
            CALL(evt_msg_toge, 2, LW(0), 150, LW(2))
            CALL(evt_msg_print, 0, STRING("stg5_muj_130"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(400)
        BEGIN_INLINE_EVENT
            ADD(LW(0), 80)
            CALL(evt_msg_toge, 2, LW(0), 107, LW(2))
            CALL(evt_msg_print, 0, STRING("stg5_muj_131"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(200)
        BEGIN_INLINE_EVENT
            ADD(LW(0), 65)
            CALL(evt_msg_toge, 2, LW(0), 205, LW(2))
            CALL(evt_msg_print, 0, STRING("stg5_muj_132"), 0, 0)
        END_INLINE_EVENT
    END_LOOP
    RETURN
EVT_END()

// TODO: type
u32 dai3_taisan_spline_dt00[9] = {
    0xC3D00000, 0x00000000, 0xC3A70000, 0xC3CED68B,
    0x00000000, 0xC3B6BB8B, 0xC3CDD0DB, 0x00000000,
    0xC3CA6743,
};

// TODO: type
u32 dai3_taisan_spline_dt01[9] = {
    0xC3D00000, 0x00000000, 0xC3A70000, 0xC451C073,
    0x00000000, 0xC3A9EEC6, 0xC47EBF3F, 0x00000000,
    0xC3AD2B3C,
};

EVT_BEGIN(nameraka_rotate)
    LOOP(0)
        IF_FLOAT_NOT_EQUAL(GW(1), GW(7))
            IF_FLOAT_GREATER_OR_EQUAL(GW(1), GW(7))
                SET_FLOAT_VALUE(LW(0), GW(1))
                SUBTRACT_FLOAT(LW(0), FLOAT(1.0f))
                SET_FLOAT_VALUE(GW(1), LW(0))
                IF_FLOAT_LESS_OR_EQUAL(GW(1), GW(7))
                    SET_FLOAT_VALUE(GW(1), GW(7))
                END_IF
            ELSE
                SET_FLOAT_VALUE(LW(0), GW(1))
                ADD_FLOAT(LW(0), FLOAT(1.0f))
                SET_FLOAT_VALUE(GW(1), LW(0))
                IF_FLOAT_GREATER_OR_EQUAL(GW(1), GW(7))
                    SET_FLOAT_VALUE(GW(1), GW(7))
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(dai3_taisan_move_trace00)
    SET_FLOAT_VALUE(LW(5), GW(2))
    SET_FLOAT_VALUE(LW(6), GW(3))
    CALL(evt_sub_spline_init, 1, 0, PTR(dai3_taisan_spline_dt00), 3, 0, LW(0))
    LOOP(0)
        CALL(evt_sub_spline_get_value, LW(0), LW(1), LW(2), LW(3))
        SET_FLOAT_VALUE(GW(2), LW(0))
        SET_FLOAT_VALUE(GW(3), LW(2))
        SET_VALUE(LF(0), 1)
        IF_FLOAT_EQUAL(GW(2), LW(5))
            IF_FLOAT_EQUAL(GW(3), LW(6))
                SET_VALUE(LF(0), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 1)
            CALL(ship_rotate_rev, LW(5), LW(6), GW(2), GW(3), GW(7))
        END_IF
        SET_FLOAT_VALUE(LW(5), GW(2))
        SET_FLOAT_VALUE(LW(6), GW(3))
        IF_INT_EQUAL(LW(3), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dai3_taisan_move_trace01)
    SET_FLOAT_VALUE(LW(5), GW(2))
    SET_FLOAT_VALUE(LW(6), GW(3))
    CALL(evt_sub_spline_init, 1, 0, PTR(dai3_taisan_spline_dt01), 3, 0, LW(0))
    LOOP(0)
        CALL(evt_sub_spline_get_value, LW(0), LW(1), LW(2), LW(3))
        SET_FLOAT_VALUE(GW(2), LW(0))
        SET_FLOAT_VALUE(GW(3), LW(2))
        SET_VALUE(LF(0), 1)
        IF_FLOAT_EQUAL(GW(2), LW(5))
            IF_FLOAT_EQUAL(GW(3), LW(6))
                SET_VALUE(LF(0), 0)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 1)
            CALL(ship_rotate, LW(5), LW(6), GW(2), GW(3), GW(7))
        END_IF
        SET_FLOAT_VALUE(LW(5), GW(2))
        SET_FLOAT_VALUE(LW(6), GW(3))
        IF_INT_EQUAL(LW(3), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dai3_beat_back)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_party_stop, 0)
    CALL(evt_party_set_pos, 0, -71, 45, -240)
    CALL(evt_npc_set_position, STRING("第三勢力幹部"), -448, 60, -236)
    CALL(evt_cam3d_evt_set, -436, 595, 725, -226, 120, -460, 0, 11)
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_VALUE(GW(2), -416)
    SET_VALUE(GW(3), -334)
    SET_FLOAT_VALUE(GW(4), FLOAT(0.0f))
    SET_VALUE(GW(5), -78)
    SET_VALUE(GW(6), -267)
    SPAWN_EVENT_GET_ID(PTR(dai3_move_every), LW(10))
    SPAWN_EVENT_GET_ID(PTR(kaizoku_move_every), LW(11))
    SPAWN_EVENT(dai3_butai_setup)
    SPAWN_EVENT(kaizoku_setup)
    WAIT_FRAMES(1)
    SPAWN_EVENT_GET_ID(PTR(dai3_butai_main), LW(12))
    SPAWN_EVENT_GET_ID(PTR(dai3_kanbu_main), LW(13))
    SPAWN_EVENT_GET_ID(PTR(kaizoku_main), LW(14))
    SPAWN_EVENT_GET_ID(PTR(mario_main2), LW(15))
    WAIT_FRAMES(2)
    SET_VALUE(GW(1), 273)
    SET_VALUE(GW(4), 215)
    IF_INT_EQUAL(LW(0), 0)
    ELSE
        CALL(evt_seq_wait, 2)
    END_IF
    SET_FLOAT_VALUE(GW(7), GW(1))
    SPAWN_EVENT_GET_ID(PTR(nameraka_rotate), LW(9))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_125"), 0, STRING("第三勢力幹部"))
    SET_VALUE(GF(11), 1)
    SPAWN_EVENT(dai3_move_trace_se)
    SET_VALUE(LW(0), 3500)
    SPAWN_EVENT(dai3_taisan_move_trace01)
    WAIT_MS(3500)
    SET_VALUE(GF(11), 0)
    DELETE_EVENT(LW(9))
    CALL(evt_cam3d_evt_set, -141, 262, 147, -45, 43, -397, 500, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    CALL(evt_msg_print_party, STRING("stg5_muj_126"))
    SPAWN_EVENT_GET_ID(PTR(senin_kansei), LW(10))
    WAIT_MS(3000)
    SET_VALUE(GSW(0), 261)
    DELETE_EVENT(LW(10))
    CALL(evt_fade_entry, 6, 1000, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1500)
    SET_VALUE(LF(10), 0)
    SET_VALUE(LW(0), 24)
    CALL_CHILD(evt_memcard_save)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_envoff_f, 512, 2000)
    CALL(evt_snd_sfx_all_stop)
    CALL(evt_fade_set_mapchange_type, 1, 55, 0, 54, 0)
    CALL(evt_bero_mapchange, STRING("aji_10"), STRING(""))
    RETURN
EVT_END()

// TODO: type
u32 leave_island_suifupos[380] = {
    0x00000235, 0x0000004D, 0xFFFFFDA2, 0x0000005A,
    0x0000023E, 0x0000004D, 0xFFFFFD9C, 0x0000005A,
    0x0000024E, 0x0000004D, 0xFFFFFD96, 0x0000005A,
    0x0000024E, 0x0000004D, 0xFFFFFDA3, 0x0000005A,
    0x00000250, 0x0000004D, 0xFFFFFDB7, 0x0000005A,
    0x0000025F, 0x0000004D, 0xFFFFFDB8, 0x0000005A,
    0x0000026F, 0x0000004D, 0xFFFFFDBC, 0x0000005A,
    0x00000278, 0x0000004D, 0xFFFFFDC8, 0x0000005A,
    0x0000028F, 0x0000004D, 0xFFFFFDCC, 0x0000005A,
    0x0000028F, 0x0000004D, 0xFFFFFDBF, 0x0000005A,
    0x00000283, 0x0000004D, 0xFFFFFDA8, 0x0000005A,
    0x00000271, 0x0000004D, 0xFFFFFDA3, 0x0000005A,
    0x00000278, 0x0000004D, 0xFFFFFD92, 0x0000005A,
    0x00000288, 0x0000004D, 0xFFFFFD8D, 0x0000005A,
    0x00000293, 0x0000004D, 0xFFFFFD83, 0x0000005A,
    0x00000298, 0x0000004D, 0xFFFFFD97, 0x0000005A,
    0x00000286, 0x0000004D, 0xFFFFFDD6, 0x0000005A,
    0x00000264, 0x0000004D, 0xFFFFFD99, 0x0000005A,
    0x00000267, 0x0000004D, 0xFFFFFDAE, 0x0000005A,
    0x00000293, 0x0000004D, 0xFFFFFDAB, 0x0000005A,
    0x00000265, 0x0000004D, 0xFFFFFD84, 0x0000005A,
    0x0000027A, 0x0000004D, 0xFFFFFD75, 0x0000005A,
    0x00000298, 0x0000004D, 0xFFFFFD67, 0x0000005A,
    0x00000295, 0x0000004D, 0xFFFFFD75, 0x0000005A,
    0x0000027F, 0x0000004D, 0xFFFFFDB8, 0x0000005A,
    0x000002AC, 0x00000041, 0xFFFFFDC9, 0x0000005A,
    0x0000029C, 0x00000041, 0xFFFFFDC6, 0x0000005A,
    0x000002A3, 0x00000041, 0xFFFFFD7F, 0x0000005A,
    0x000002A6, 0x00000041, 0xFFFFFDB9, 0x0000005A,
    0x000002A9, 0x00000041, 0xFFFFFDAB, 0x0000005A,
    0x000002A0, 0x00000041, 0xFFFFFDD7, 0x0000005A,
    0x000002D7, 0x00000028, 0xFFFFFD95, 0x0000005A,
    0x000002B5, 0x0000002A, 0xFFFFFD80, 0x0000005A,
    0x000002B0, 0x0000002A, 0xFFFFFD8E, 0x0000005A,
    0x000002B7, 0x0000002A, 0xFFFFFD9A, 0x0000005A,
    0x000002BD, 0x0000002A, 0xFFFFFD5B, 0x0000005A,
    0x000002BD, 0x0000002A, 0xFFFFFD67, 0x0000005A,
    0x000002D5, 0x0000002A, 0xFFFFFD67, 0x0000010E,
    0x000002EA, 0x0000002A, 0xFFFFFD67, 0x0000010E,
    0x00000332, 0x0000002A, 0xFFFFFD53, 0x0000010E,
    0x000002B7, 0x0000002A, 0xFFFFFD72, 0x0000010E,
    0x000002C6, 0x0000002A, 0xFFFFFDA2, 0x0000010E,
    0x000002C3, 0x0000002A, 0xFFFFFD8B, 0x0000010E,
    0x00000332, 0x0000002A, 0xFFFFFD89, 0x0000010E,
    0x000002CA, 0x0000002A, 0xFFFFFD7C, 0x0000010E,
    0x000002DC, 0x0000002A, 0xFFFFFD78, 0x0000010E,
    0x000002F2, 0x0000002A, 0xFFFFFD7A, 0x0000010E,
    0x00000301, 0x0000002A, 0xFFFFFD76, 0x0000010E,
    0x00000310, 0x0000002A, 0xFFFFFD78, 0x0000010E,
    0x00000327, 0x0000002A, 0xFFFFFD78, 0x0000010E,
    0x00000334, 0x0000002A, 0xFFFFFD73, 0x0000010E,
    0x000002E2, 0x0000002A, 0xFFFFFD6C, 0x0000010E,
    0x00000319, 0x0000002A, 0xFFFFFD6B, 0x0000010E,
    0x00000310, 0x0000002A, 0xFFFFFD49, 0x0000010E,
    0x00000312, 0x0000002A, 0xFFFFFD44, 0x0000010E,
    0x00000320, 0x0000002A, 0xFFFFFD55, 0x0000010E,
    0x000002EA, 0x0000002A, 0xFFFFFD4E, 0x0000010E,
    0x00000309, 0x0000002A, 0xFFFFFD5E, 0x0000010E,
    0x000002F6, 0x0000002A, 0xFFFFFD5D, 0x0000010E,
    0x000002E1, 0x0000002A, 0xFFFFFD58, 0x0000010E,
    0x000002D0, 0x0000002A, 0xFFFFFD57, 0x0000010E,
    0x00000302, 0x0000002A, 0xFFFFFD4D, 0x0000010E,
    0x0000034E, 0x00000073, 0xFFFFFD60, 0x0000010E,
    0x0000034E, 0x00000073, 0xFFFFFD7A, 0x0000010E,
    0x0000035D, 0x00000073, 0xFFFFFD7A, 0x0000010E,
    0x00000365, 0x00000073, 0xFFFFFD79, 0x0000010E,
    0x00000376, 0x00000073, 0xFFFFFD81, 0x0000010E,
    0x00000377, 0x00000073, 0xFFFFFD96, 0x0000010E,
    0x00000382, 0x00000073, 0xFFFFFD77, 0x0000010E,
    0x0000038B, 0x00000073, 0xFFFFFD89, 0x0000010E,
    0x00000369, 0x00000073, 0xFFFFFD8E, 0x0000010E,
    0x0000038D, 0x00000073, 0xFFFFFD9E, 0x0000010E,
    0x0000038D, 0x00000073, 0xFFFFFDA9, 0x0000010E,
    0x0000036A, 0x00000073, 0xFFFFFDBA, 0x0000010E,
    0x0000035D, 0x00000073, 0xFFFFFDB2, 0x0000010E,
    0x00000354, 0x00000073, 0xFFFFFDAC, 0x0000010E,
    0x00000354, 0x00000073, 0xFFFFFD88, 0x0000010E,
    0x0000034B, 0x00000073, 0xFFFFFD89, 0x0000010E,
    0x0000034B, 0x00000073, 0xFFFFFDA5, 0x0000010E,
    0x00000357, 0x00000073, 0xFFFFFDB4, 0x0000010E,
    0x00000354, 0x00000073, 0xFFFFFDBE, 0x0000010E,
    0x00000362, 0x00000073, 0xFFFFFDCF, 0x0000010E,
    0x0000038E, 0x00000073, 0xFFFFFDBA, 0x0000010E,
    0x00000358, 0x00000073, 0xFFFFFDD0, 0x0000010E,
    0x00000358, 0x00000073, 0xFFFFFDE0, 0x0000010E,
    0x00000350, 0x00000073, 0xFFFFFDE3, 0x0000010E,
    0x00000365, 0x00000073, 0xFFFFFDE4, 0x0000010E,
    0x00000377, 0x00000073, 0xFFFFFDD1, 0x0000010E,
    0x0000037C, 0x00000073, 0xFFFFFDE0, 0x0000010E,
    0x00000387, 0x00000073, 0xFFFFFDDC, 0x0000010E,
    0x00000393, 0x00000073, 0xFFFFFDD9, 0x0000010E,
    0x00000393, 0x00000073, 0xFFFFFDC9, 0x0000010E,
    0x00000383, 0x00000073, 0xFFFFFDC4, 0x0000010E,
    0x00000374, 0x00000073, 0xFFFFFDBA, 0x0000010E,
    0x00000380, 0x00000073, 0xFFFFFDB0, 0x0000010E,
};

EVT_BEGIN(move_ship_se00)
    LOOP(5)
        WAIT_MS(2000)
        CALL(evt_mapobj_get_position, STRING("S_b"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_SHIP_KOL_MOVE1"), LW(0), LW(1), LW(2), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(move_ship_se01)
    LOOP(7)
        WAIT_MS(1400)
        CALL(evt_mapobj_get_position, STRING("S_b"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_SHIP_KOL_MAST1"), LW(0), LW(1), LW(2), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(option_ship_npc)
    SET_FLOAT_VALUE(LW(10), GW(0))
    SET_FLOAT_VALUE(LW(11), GW(1))
    LOOP(0)
        SET_FLOAT_VALUE(LW(4), GW(0))
        SUBTRACT_FLOAT(LW(4), LW(10))
        SET_FLOAT_VALUE(LW(5), GW(1))
        SUBTRACT_FLOAT(LW(5), LW(11))
        SET_VALUE(LW(0), 10)
        LOOP(95)
            CALL(evt_nannpc_get_position, LW(0), LW(1), LW(2), LW(3))
            ADD_FLOAT(LW(1), LW(4))
            ADD_FLOAT(LW(2), LW(5))
            CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
            ADD(LW(0), 1)
        END_LOOP
        CALL(evt_npc_get_position, STRING("水夫A"), LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_npc_set_position, STRING("水夫A"), LW(1), LW(2), LW(3))
        CALL(evt_npc_get_position, STRING("水夫ボム兵"), LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_npc_set_position, STRING("水夫ボム兵"), LW(1), LW(2), LW(3))
        CALL(evt_npc_get_position, STRING("水夫C"), LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_npc_set_position, STRING("水夫C"), LW(1), LW(2), LW(3))
        CALL(evt_npc_get_position, STRING("オドオド水夫"), LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_npc_set_position, STRING("オドオド水夫"), LW(1), LW(2), LW(3))
        SET_FLOAT_VALUE(LW(10), GW(0))
        SET_FLOAT_VALUE(LW(11), GW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(option_ship_korutesu)
    SET_FLOAT_VALUE(LW(10), GW(0))
    SET_FLOAT_VALUE(LW(11), GW(1))
    LOOP(0)
        SET_FLOAT_VALUE(LW(4), GW(0))
        SUBTRACT_FLOAT(LW(4), LW(10))
        SET_FLOAT_VALUE(LW(5), GW(1))
        SUBTRACT_FLOAT(LW(5), LW(11))
        CALL(evt_npc_get_position, STRING("コルテス"), LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        CALL(evt_npc_set_position, STRING("コルテス"), LW(1), LW(2), LW(3))
        SET_FLOAT_VALUE(LW(10), GW(0))
        SET_FLOAT_VALUE(LW(11), GW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(option_ship_marco)
    SET_FLOAT_VALUE(LW(10), GW(0))
    SET_FLOAT_VALUE(LW(11), GW(1))
    LOOP(0)
        SET_FLOAT_VALUE(LW(4), GW(0))
        SUBTRACT_FLOAT(LW(4), LW(10))
        SET_FLOAT_VALUE(LW(5), GW(1))
        SUBTRACT_FLOAT(LW(5), LW(11))
        CALL(evt_npc_get_position, STRING("マルコ"), LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_npc_set_position, STRING("マルコ"), LW(1), LW(2), LW(3))
        SET_FLOAT_VALUE(LW(10), GW(0))
        SET_FLOAT_VALUE(LW(11), GW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(option_ship_mario)
    SET_FLOAT_VALUE(LW(10), GW(0))
    SET_FLOAT_VALUE(LW(11), GW(1))
    LOOP(0)
        SET_FLOAT_VALUE(LW(4), GW(0))
        SUBTRACT_FLOAT(LW(4), LW(10))
        SET_FLOAT_VALUE(LW(5), GW(1))
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
        SET_FLOAT_VALUE(LW(10), GW(0))
        SET_FLOAT_VALUE(LW(11), GW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(option_ship_main)
    LOOP(0)
        SET_VALUE(LW(0), GW(0))
        SET_VALUE(LW(1), GW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_b"), LW(0), LW(1), -605)
        CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 180, 0)
        CALL(evt_hit_bind_update, STRING("A_b"))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(leave_island)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("コルテス"), 640, 125, -603)
    CALL(evt_npc_set_ry, STRING("コルテス"), 90)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), 706, 40, -566)
    CALL(evt_npc_set_ry, STRING("オドオド水夫"), 90)
    CALL(evt_npc_set_position, STRING("水夫A"), 720, 40, -589)
    CALL(evt_npc_set_ry, STRING("水夫A"), 90)
    CALL(evt_npc_set_position, STRING("水夫C"), 779, 40, -548)
    CALL(evt_npc_set_ry, STRING("水夫C"), 270)
    CALL(evt_npc_set_position, STRING("水夫ボム兵"), 751, 40, -590)
    CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 1073742368)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫A"), 1073742368)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫C"), 1073742368)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫ボム兵"), 1073742368)
    SET_VALUE(LW(0), 10)
    SET_INT_READ_ADDR(PTR(leave_island_suifupos))
    LOOP(95)
        READ_INT3(LW(1), LW(2), LW(3))
        READ_INT(LW(4))
        ADD(LW(2), 2)
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_nannpc_set_dir, LW(0), LW(4))
        ADD(LW(0), 1)
    END_LOOP
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), 920, 25, -423)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    CALL(evt_npc_set_position, STRING("モニー"), 950, 25, -383)
    CALL(evt_npc_set_ry, STRING("モニー"), 270)
    CALL(evt_npc_set_position, STRING("ピートン"), 980, 25, -423)
    CALL(evt_npc_set_ry, STRING("ピートン"), 270)
    CALL(evt_npc_set_position, STRING("水夫D"), 1010, 25, -383)
    CALL(evt_npc_set_ry, STRING("水夫D"), 270)
    CALL(evt_npc_set_position, STRING("ボロ水夫A"), 1040, 25, -423)
    CALL(evt_npc_set_ry, STRING("ボロ水夫A"), 270)
    CALL(evt_npc_set_position, STRING("ボロ水夫B"), 1060, 25, -383)
    CALL(evt_npc_set_ry, STRING("ボロ水夫B"), 270)
    CALL(evt_npc_set_position, STRING("店員"), 1090, 35, -423)
    CALL(evt_npc_set_ry, STRING("店員"), 270)
    CALL(evt_npc_set_position, STRING("宿店員"), 1110, 35, -383)
    CALL(evt_npc_set_ry, STRING("宿店員"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("ボロ水夫A"), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("ボロ水夫B"), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("店員"), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("宿店員"), 1)
    SET_VALUE(GW(0), 770)
    SET_VALUE(GW(1), 0)
    SET_RAW_VALUE(LW(0), GW(1))
    SET_FLOAT_VALUE(LW(1), FLOAT(1.5f))
    SPAWN_EVENT(option_ship_npc)
    SPAWN_EVENT(option_ship_korutesu)
    SPAWN_EVENT(option_ship_main)
    SPAWN_EVENT(muj_ship_puka)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mario_set_pos, 800, 25, -415)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_pos, 0, 835, 25, -425)
    CALL(evt_party_set_dir, 0, 270, 0)
    CALL(evt_npc_set_position, STRING("マルコ"), 750, 25, -415)
    CALL(evt_npc_set_ry, STRING("マルコ"), 270)
    CALL(evt_cam3d_evt_set, 750, 438, 999, 750, 225, -233, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_fade_end_wait)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, 660, 267, -170, 660, 190, -603, 0, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_133"), 0, STRING("コルテス"))
    CALL(evt_cam3d_evt_set, 775, 133, -36, 775, 45, -550, 0, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("マルコ"), 90)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_134"), 0, STRING("マルコ"))
    CALL(evt_cam3d_evt_set, 750, 151, -218, 750, 55, -759, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_135"), 0, STRING("水夫A"))
    CALL(evt_cam3d_evt_set, 787, 151, -218, 787, 55, -759, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_136"), 0, STRING("水夫C"))
    CALL(evt_cam3d_evt_set, 775, 133, -36, 775, 45, -550, 0, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_137"), 0, STRING("マルコ"))
    CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 745, 40, -555, 500, 30)
    CALL(evt_npc_set_ry, STRING("マルコ"), 90)
    SPAWN_EVENT(option_ship_marco)
    CALL(evt_msg_print, 0, STRING("stg5_muj_138"), 0, STRING("モニー"))
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_set_dir, 90, 200, 0)
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, 950, 133, -36, 950, 45, -550, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_139"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_140"), 0, STRING("ピートン"))
    CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2))
    ADD(LW(0), -10)
    CALL(evt_npc_move_position, STRING("ガラの悪い水夫"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_msg_print, 0, STRING("stg5_muj_141"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_cam3d_evt_set, 820, 133, -36, 820, 45, -550, 500, 11)
    CALL(evt_party_set_dir, 0, 270, 200)
    WAIT_MS(200)
    CALL(evt_msg_print_party, STRING("stg5_muj_142"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NOD1_2"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 0, 270, 200)
        CALL(evt_party_jump_pos, 0, 765, 40, -535, 500, 0, -1000)
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_mario_set_dir, 90, 200, 0)
    WAIT_MS(200)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FLD_JUMP5"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_jump_pos, 730, 40, -535, 500, 0, -1000)
    CALL(evt_mario_set_dir, 90, 200, 0)
    SPAWN_EVENT(option_ship_mario)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 640, 270, -110, 640, 185, -603, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_143"), 0, STRING("コルテス"))
    CALL(evt_npc_set_ry, STRING("コルテス"), 270)
    SET_VALUE(LW(0), 10)
    LOOP(95)
        CALL(evt_nannpc_get_position, LW(0), LW(10), LW(11), LW(12))
        IF_FLOAT_GREATER_OR_EQUAL(LW(11), 60)
            IF_FLOAT_LESS_OR_EQUAL(LW(11), 80)
                BEGIN_INLINE_EVENT
                    CALL(evt_sub_random, 300, LW(13))
                    ADD(LW(13), 100)
                    WAIT_MS(LW(13))
                    CALL(evt_nannpc_set_dir, LW(0), 270)
                END_INLINE_EVENT
            END_IF
        END_IF
        ADD(LW(0), 1)
    END_LOOP
    CALL(evt_cam3d_evt_set, 770, 317, 411, 770, 149, -559, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_mario_cont_onoff, 0)
    SPAWN_EVENT(move_ship_se00)
    SPAWN_EVENT(move_ship_se01)
    CALL(evt_mapobj_get_position, STRING("S_b"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_WAVE1"), LW(0), LW(1), LW(2), LW(9))
    CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_STARTING1_2"), 0)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST_SHIP1"))
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_cam3d_evt_set, 944, 164, -240, 33, 155, -611, 13000, 11)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 1, 770, -770, 18000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(GW(0), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(17000)
    CALL(evt_snd_sfxoff, LW(9))
    CALL(evt_nannpcwork_flag_onoff, 0, 1)
    SET_VALUE(LW(0), 10)
    LOOP(95)
        CALL(evt_nannpc_ppflag_onoff, LW(0), 0, 4)
        ADD(LW(0), 1)
    END_LOOP
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_eff_softdelete, STRING("sunlight"))
    SET_VALUE(LW(10), 600)
    SPAWN_EVENT(back_dark)
    CALL_CHILD(suifu_dark)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_muj_144"), 0, 0)
    WAIT_MS(2000)
    SET_VALUE(GSW(0), 268)
    CALL(evt_bero_mapchange, STRING("gor_00"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(muj_gor_ship_sub)
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), GW(0))
        SET_FLOAT_VALUE(LW(1), GW(1))
        SET_FLOAT_VALUE(LW(2), GW(2))
        CALL(evt_mapobj_trans, 1, STRING("S_b"), LW(0), LW(1), -605)
        CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, LW(2), 0)
        CALL(evt_hit_bind_update, STRING("A_b"))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()


static EVT_BEGIN(muj_gor_ship)
    CALL(evt_npc_set_position, STRING("コルテス"), 655, 40, -542)
    CALL(evt_npc_set_ry, STRING("コルテス"), 90)
    CALL(alpha_init_func)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_mizu_06"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_mizu_06"), 256)
    CALL(alpha_set_func, 0)
    SET_FLOAT_VALUE(GW(0), FLOAT(685.0f))
    SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
    SET_VALUE(GW(2), 0)
    SPAWN_EVENT(muj_gor_ship_sub)
    SET_RAW_VALUE(LW(0), GW(1))
    SET_FLOAT_VALUE(LW(1), FLOAT(1.5f))
    SPAWN_EVENT_GET_ID(PTR(muj_ship_puka), GW(10))
    RETURN
EVT_END()

static EVT_BEGIN(ship_pera)
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_kaiten"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_offscreen_set_texture_fmt, STRING("offscreen"), 6, 6)
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_b"), STRING("offscreen"))
    CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_alloc_capture, STRING("capture_img"), 1, STRING("offscreen"), 0, 0, 0, 608, 480)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_set_virtual_point, STRING("capture_img"), 685, 0, -605)
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("Z_1"))
    CALL(evt_img_onoff, STRING("capture_img"), 1)
    CALL(evt_snd_sfxon, STRING("SFX_OFF_SHIP_REVERSE1"), 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_b"), 1)
    CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
    CALL(evt_img_wait_animend, STRING("capture_img"))
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_b"), 1)
    SET_VALUE(GW(2), 180)
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_b"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    CALL(evt_paper_delete, STRING("OFF_d_kaiten"))
    RETURN
EVT_END()

const Vec vec3_muj_0000e4cc = {0.0f, 0.0f, 0.0f};
const Vec vec3_muj_0000e4d8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_muj_0000e4e4 = {0.0f, 0.0f, 0.0f};

EVT_BEGIN(muj_gor_ship_start)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST_SHIP1"))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 100, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(scale_func, LW(0))
            CALL(evt_npc_set_scale, STRING("コルテス"), LW(0), LW(0), LW(0))
            CALL(evt_npc_set_scale, STRING("モニー"), LW(0), LW(0), LW(0))
            CALL(evt_npc_set_scale, STRING("ピートン"), LW(0), LW(0), LW(0))
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
    CALL(evt_cam3d_evt_set, 685, 278, 455, 685, 158, -537, 500, 11)
    WAIT_MS(1000)
    DELETE_EVENT(GW(10))
    SET_VALUE(GF(10), 1)
    CALL(evt_npc_set_position, STRING("船ダミー"), 0, GW(1), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL_CHILD(ship_pera)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("船ダミー"), LW(0), LW(1), LW(2))
            SET_FLOAT_VALUE(GW(1), LW(1))
            IF_INT_EQUAL(GF(10), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("S_b"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_JUMP1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_jump_position_nohit, STRING("船ダミー"), 0, GW(1), 0, 1800, 150)
    CALL(evt_mapobj_get_position, STRING("S_b"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG0_SHIP_LANDING1"), LW(0), LW(1), LW(2), 0)
    SET_VALUE(GF(10), 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mizu_06"), 1)
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
    SET_RAW_VALUE(LW(0), GW(1))
    SET_FLOAT_VALUE(LW(1), FLOAT(1.5f))
    SPAWN_EVENT_GET_ID(PTR(muj_ship_puka), GW(10))
    CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_STARTING1_4"), 0)
    SET_FLOAT_VALUE(LW(0), GW(0))
    SET_FLOAT_VALUE(LW(1), GW(0))
    ADD_FLOAT(LW(1), FLOAT(-800.0f))
    CALL(evt_sub_intpl_msec_init, 1, LW(0), LW(1), 10000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        SET_FLOAT_VALUE(GW(0), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_bero_mapchange, STRING("muj_20"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(into_muj_ship)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    ADD_FLOAT(GW(0), FLOAT(-600.0f))
    SET_FLOAT_VALUE(LW(10), FLOAT(-600.0f))
    SET_FLOAT_VALUE(LW(0), FLOAT(730.0f))
    ADD_FLOAT(LW(0), LW(10))
    SET_FLOAT_VALUE(LW(1), FLOAT(705.0f))
    ADD_FLOAT(LW(1), LW(10))
    SET_FLOAT_VALUE(LW(2), FLOAT(710.0f))
    ADD_FLOAT(LW(2), LW(10))
    SET_FLOAT_VALUE(LW(3), FLOAT(655.0f))
    ADD_FLOAT(LW(3), LW(10))
    CALL(evt_mario_set_pos, LW(0), 40, -550)
    CALL(evt_party_set_pos, 0, LW(1), 40, -570)
    CALL(evt_party_set_pos, 1, LW(2), 40, -590)
    CALL(evt_npc_set_position, STRING("コルテス"), LW(3), 40, -542)
    CALL(evt_cam3d_evt_set, 682, 199, 75, 682, 78, -614, 0, 11)
    SPAWN_EVENT_GET_ID(PTR(option_ship_korutesu), LW(8))
    SPAWN_EVENT_GET_ID(PTR(option_ship_mario), LW(9))
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_ARRIVAL1_4"), 0)
    SET_FLOAT_VALUE(LW(0), GW(0))
    SET_FLOAT_VALUE(LW(1), GW(0))
    ADD_FLOAT(LW(1), FLOAT(600.0f))
    CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 6000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        SET_FLOAT_VALUE(GW(0), LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_cont_onoff, 1, 1)
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    DELETE_EVENT(LW(8))
    DELETE_EVENT(LW(9))
    CALL(evt_cam3d_evt_off, 500, 11)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    IF_INT_EQUAL(GSW(0), 273)
        SET_VALUE(GSW(0), 274)
    END_IF
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
    RETURN
EVT_END()

EVT_BEGIN(mony_peton_boarding_sub)
    SET_FLOAT_VALUE(LW(10), GW(0))
    SET_FLOAT_VALUE(LW(11), GW(1))
    LOOP(0)
        SET_FLOAT_VALUE(LW(4), GW(0))
        SUBTRACT_FLOAT(LW(4), LW(10))
        SET_FLOAT_VALUE(LW(5), GW(1))
        SUBTRACT_FLOAT(LW(5), LW(11))
        CALL(evt_npc_get_position, STRING("モニー"), LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_npc_set_position, STRING("モニー"), LW(1), LW(2), LW(3))
        CALL(evt_npc_get_position, STRING("ピートン"), LW(1), LW(2), LW(3))
        ADD_FLOAT(LW(1), LW(4))
        ADD_FLOAT(LW(2), LW(5))
        CALL(evt_npc_set_position, STRING("ピートン"), LW(1), LW(2), LW(3))
        SET_FLOAT_VALUE(LW(10), GW(0))
        SET_FLOAT_VALUE(LW(11), GW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mony_peton_boarding)
    SET_VALUE(LW(0), 0)
    IF_INT_EQUAL(GSW(0), 276)
        SET_VALUE(LW(0), 1)
    END_IF
    IF_INT_EQUAL(GSW(0), 277)
        SET_VALUE(LW(0), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_npc_set_position, STRING("モニー"), 710, 40, -590)
    CALL(evt_npc_set_ry, STRING("モニー"), 90)
    CALL(evt_npc_set_position, STRING("ピートン"), 745, 40, -575)
    CALL(evt_npc_set_ry, STRING("ピートン"), 90)
    SPAWN_EVENT(mony_peton_boarding_sub)
    IF_INT_EQUAL(GSW(0), 277)
        RETURN
    END_IF
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_mario_get_name_hitobj_ride, LW(10))
        IF_INT_LESS_OR_EQUAL(LW(0), 790)
            IF_STR_EQUAL(LW(10), STRING("A_hasi_hon"))
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_cam3d_evt_set, 727, 170, -204, 727, 127, -415, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_145"), 0, STRING("ピートン"))
    CALL(evt_msg_print, 0, STRING("stg5_muj_146"), 0, STRING("モニー"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSW(0), 277)
    RETURN
EVT_END()

static HitDamageReturn hitDmgRetPoints[8] = {
    { "A_yuka01", { 1083.0f, 35.0f, -738.0f } },
    { "A_yuka06", { 582.0f, 35.0f, -1173.0f } },
    { "A_yuka03", { 1083.0f, 35.0f, -738.0f } },
    { "A_yuka04", { 1083.0f, 35.0f, -738.0f } },
    { "A_yuka02", { 1240.0f, 14.0f, 40.0f } },
    { "A_yuka05", { 1240.0f, 14.0f, 40.0f } },
    { "A_hasi_hon", { 1048.0f, 24.0f, -423.0f } },
    { NULL, { 0.0f, 0.0f, 0.0f } },
};

EVT_BEGIN(muj_00_init_evt)
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("pointLight1"), STRING("S_b"), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("pointLight2"), STRING("S_b"), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("pointLight3"), STRING("S_b"), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("pointLight4"), STRING("S_b"), 0)
    END_INLINE_EVENT
    IF_INT_GREATER_OR_EQUAL(GSW(0), 268)
        IF_INT_EQUAL(GSWF(3125), 1)
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST_SHIP1"))
        ELSE
            CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
        END_IF
    ELSE
        IF_INT_NOT_EQUAL(GSW(0), 231)
            IF_INT_NOT_EQUAL(GSW(0), 267)
                IF_INT_EQUAL(GSW(0), 260)
                    CALL(evt_snd_bgmon, 512, STRING("BGM_BOSS_STG5_KORTESS1"))
                ELSE
                    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
                END_IF
            END_IF
        END_IF
    END_IF
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ2"))
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_npc_set_tribe, STRING("モニー"), STRING("モニー（指輪）"))
    CALL(evt_npc_set_tribe, STRING("ピートン"), STRING("ピートン（指輪）"))
    CALL(evt_npc_sound_data_set, STRING("マルコ"), STRING("SFX_STG5_MARCO_MOVE1L"), STRING("SFX_STG5_MARCO_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 382)
        CALL_CHILD(kiza_entry)
    END_IF
    CALL_CHILD(koburon_evt)
    CALL(evt_npc_entry, STRING("船ダミー"), STRING("c_suifu_b"))
    CALL(evt_npc_flag_onoff, 1, STRING("船ダミー"), 1073741984)
    CALL(evt_eff64, STRING("sunlight"), STRING("sunlight_n64"), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_mobj_kururing_floor, STRING("kururin1"), 1375, 35, -430, STRING("S_ground"), 125, GSWF(5638))
    CALL(evt_paper_entry, STRING("OFF_d_yabu"))
    CALL(evt_paper_entry, STRING("OFF_d_kaiten"))
    CALL(evt_map_playanim, STRING("S_mizu"), 1, 0)
    CALL(evt_map_playanim, STRING("fune_c_01"), 1, 0)
    CALL(evt_map_playanim, STRING("fune_c_02"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_b"), 1)
    CALL(evt_hitobj_onoff, STRING("A_b"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_c"), 1)
    CALL(evt_hitobj_onoff, STRING("A_c"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hasi"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_mizu_01"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_mizu_04"), 1)
    CALL(evt_hitobj_onoff, STRING("A_hasi1"), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_iwa_before"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_iwa_after"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_iwa_s011"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_after"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before"), 1)
    IF_INT_LESS_OR_EQUAL(GSW(0), 255)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_before"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_iwa_s011"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_after"), 1)
        CALL(evt_hitobj_onoff, STRING("A_iwa_09"), 1, 0)
    END_IF
    IF_INT_LESS(GSW(0), 260)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_iwa_before"), 1)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_iwa_after"), 1)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 257)
        IF_INT_LESS(GSW(0), 258)
            SPAWN_EVENT_GET_ID(PTR(dai3_shurai_after), GW(8))
        END_IF
    END_IF
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    SWITCH(GSW(0))
        CASE_EQUAL(231)
        SPAWN_EVENT(mujinto_hyochaku)
        CASE_EQUAL(256)
        CALL_CHILD(nannpc_mode_setup)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_c"), 1)
        CALL(evt_hitobj_onoff, STRING("A_c"), 1, 1)
        CALL(evt_hit_bind_mapobj, STRING("A_c"), STRING("S_c"))
        SPAWN_EVENT(dai3_shurai)
        SPAWN_EVENT(suifu_random_sub)
        CASE_EQUAL(257)
        CALL_CHILD(nannpc_mode_setup)
        SPAWN_EVENT(suifu_random_sub)
        CASE_EQUAL(259)
        CALL_CHILD(nannpc_mode_setup)
        CALL_CHILD(mario_main_init)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_b"), 1)
        CALL(evt_hitobj_onoff, STRING("A_b"), 1, 1)
        CALL(evt_hit_bind_mapobj, STRING("A_b"), STRING("S_b"))
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_c"), 1)
        CALL(evt_hitobj_onoff, STRING("A_c"), 1, 1)
        CALL(evt_hit_bind_mapobj, STRING("A_c"), STRING("S_c"))
        SPAWN_EVENT(kaizoku_vs_dai3)
        CASE_EQUAL(260)
        CALL_CHILD(nannpc_mode_setup)
        CALL_CHILD(mario_main_init)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_b"), 1)
        CALL(evt_hitobj_onoff, STRING("A_b"), 1, 1)
        CALL(evt_hit_bind_mapobj, STRING("A_b"), STRING("S_b"))
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_c"), 1)
        CALL(evt_hitobj_onoff, STRING("A_c"), 1, 1)
        CALL(evt_hit_bind_mapobj, STRING("A_c"), STRING("S_c"))
        SET_VALUE(LW(0), 1)
        SPAWN_EVENT(dai3_beat_back)
        CASE_EQUAL(267)
        CALL(evt_mario_get_party, LW(15))
        IF_INT_EQUAL(LW(15), 0)
            CALL(evt_mario_set_party, 0, 1)
            CALL(evt_party_run, 0)
            CALL(evt_pouch_mario_recovery)
            CALL(evt_pouch_all_party_recovery)
        END_IF
        CALL_CHILD(nannpc_mode_setup)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_b"), 1)
        CALL(evt_hitobj_onoff, STRING("A_b"), 1, 1)
        CALL(evt_hit_bind_mapobj, STRING("A_b"), STRING("S_b"))
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hasi"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mizu_01"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mizu_04"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_mizu_00"), 1)
        CALL(evt_hitobj_onoff, STRING("A_hasi1"), 1, 1)
        SPAWN_EVENT(leave_island)
        CASE_GREATER_OR_EQUAL(268)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_b"), 1)
        CALL(evt_hitobj_onoff, STRING("A_b"), 1, 1)
        CALL(evt_hit_bind_mapobj, STRING("A_b"), STRING("S_b"))
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hasi"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mizu_01"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_mizu_04"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_mizu_00"), 1)
        CALL(evt_hitobj_onoff, STRING("A_hasi1"), 1, 1)
        CALL_CHILD(muj_gor_ship)
        IF_INT_EQUAL(GSWF(3125), 1)
            SET_VALUE(GSWF(3125), 0)
            SET_VALUE(GSWF(39), 1)
            SPAWN_EVENT(into_muj_ship)
        END_IF
        SPAWN_EVENT(mony_peton_boarding)
    END_SWITCH
    CALL(evt_hitobj_onoff, STRING("A_b_bero01"), 1, 0)
    RETURN
EVT_END()


// clang-format on

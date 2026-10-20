#pragma once

#include "evt/evt_cmd.h"
#include "memory.h"
#include <dolphin/os.h>
#include <dolphin/types.h>
#include <dolphin/mtx.h>

typedef struct NanNpcEntry NanNpcEntry;

typedef struct NanNpcWork {
    NanNpcEntry* entries;   // 0x0
    u8 pad_0x04[0x0C - 0x04]; // 0x4
    s32 count;              // 0xC
} NanNpcWork;

struct NanNpcEntry {
    u8 pad_0x00[0x18];              // 0x0
    Vec position;                   // 0x18
    Vec hoseiPosition;              // 0x24
    Vec basePos;                    // 0x30, origin of the current move
    u8 pad_0x3C[0x50 - 0x3C];       // 0x3C
    OSTime startTime;               // 0x50, renderTime when the current state began
    u8 pad_0x58[0x60 - 0x58];       // 0x58
    f32 facingAngle;                // 0x60, 90 or 270
    void (*callback)(NanNpcEntry*); // 0x64
    s32 state;                      // 0x68, switch selector of the seq functions
    s32 timer;                      // 0x6C, wait in ms, or the step count of a move
    s32 field_0x70;                 // 0x70
    s32 index;                      // 0x74, the entry number from gundan1000tai_init
    f32 field_0x78;                 // 0x78
    f32 field_0x7C;                 // 0x7C
    f32 stepX;                      // 0x80, +1 or -1 per frame in the sway state
    u8 pad_0x84[0x88 - 0x84];       // 0x84
    Vec targetPos;                  // 0x88
    u8 pad_0x94[0xA4 - 0x94];       // 0x94
    s32 moveDone;                   // 0xA4, 1 once a move has reached its target
    s32 endFlag;                    // 0xA8, set by gundan1000tai_totsuend; nonzero moves to state 100
    u8 pad_0xAC[0xC0 - 0xAC];       // 0xAC
};

extern NanNpcWork* nanNPCWork;

USER_FUNC(unk_gor_000006d8);
USER_FUNC(funechange_func);
USER_FUNC(funeflag_load);
USER_FUNC(funeflag_reset);
USER_FUNC(funeflag_save);
USER_FUNC(gor_ship_puka_func);
void gor_stg4_018_cb(s32 type, smartEntry* smart);
void suifu100taikikan_seq01(NanNpcEntry* entry);
void suifu100taikikan_seq00(NanNpcEntry* entry);
USER_FUNC(suifu100taikikan_away);
USER_FUNC(suifu100taikikan_init);
USER_FUNC(hosei_party_pos);
USER_FUNC(hosei_mario_pos);
void gundan1000tai_seq01(NanNpcEntry* entry);
void gundan1000tai_seq00(NanNpcEntry* entry);
USER_FUNC(gundan1000tai_totsuend);
USER_FUNC(gundan1000tai_areacheck);
USER_FUNC(gundan1000tai_totsugeki);
USER_FUNC(gundan1000tai_init);
USER_FUNC(gundan1000tai_circle_pos_func);
void geundan1000tai_circle_pos(Vec* out);
USER_FUNC(fune_yure_func);
USER_FUNC(fune_yure_func_init);
f32 lineardec_type(s32 target, s32 count, f32 start, f32 end);
EVT_DEF(no00_custom);
EVT_DEF(unk_evt_gor_0001f4b4);
EVT_DEF(mony_peton_talk);
EVT_DEF(unk_evt_gor_0001fa08);
EVT_DEF(unk_evt_gor_0001fb1c);
EVT_DEF(garawaru_urouro2);
EVT_DEF(garawaru_regl);
EVT_DEF(megane_regl);
EVT_DEF(odoodo_regl);
EVT_DEF(odoodo_ani_urouro);
EVT_DEF(odoodo_ani_init);
EVT_DEF(odoodo_ani_regl);
EVT_DEF(odoodo_ani_talk);
EVT_DEF(suifu_a_regl);
EVT_DEF(suifu_b_init);
EVT_DEF(suifu_b_regl);
EVT_DEF(suifu_b_talk);
EVT_DEF(suifubomb_a_urouro2);
EVT_DEF(suifubomb_a_init);
EVT_DEF(suifubomb_a_regl);
EVT_DEF(suifubomb_a_talk);
EVT_DEF(suifubomb_b_urouro2);
EVT_DEF(suifubomb_b_init);
EVT_DEF(suifubomb_b_regl);
EVT_DEF(suifubomb_b_talk);
EVT_DEF(suifubomb_c_init);
EVT_DEF(suifubomb_c_talk);
EVT_DEF(tentyou_init);
EVT_DEF(tentyou_talk);
EVT_DEF(tenin_init);
EVT_DEF(tenin_talk);
EVT_DEF(mokorim_init);
EVT_DEF(mokorim_talk);
EVT_DEF(rakgan_init);
EVT_DEF(rakgan_talk);
EVT_DEF(ship_a_kaji_animation);
EVT_DEF(ship_a_wakka_animation);
EVT_DEF(ship_a_entotsu_animation);
EVT_DEF(ship_a_neji_animation);
EVT_DEF(ship_a_yure_animation);
EVT_DEF(ship_a_animation);
EVT_DEF(enter_gorotsuki_town_init);
EVT_DEF(christine_osoware_setup);
EVT_DEF(enter_gorotsuki_town);
EVT_DEF(christine_osoware);
EVT_DEF(gundan1000tai_beforebattle);
EVT_DEF(gundan1000tai_kemuri);
EVT_DEF(gundan1000tai_zaza_se);
EVT_DEF(gundan1000tai_jump_se);
EVT_DEF(gundan1000tai_yure);
EVT_DEF(gundan1000tai_afterbattle);
EVT_DEF(mony_peton_after);
EVT_DEF(shukko_event_move_ship_se00);
EVT_DEF(shukko_event_move_ship_se01);
EVT_DEF(shukko_event_move_ship);
EVT_DEF(shukko_event);
EVT_DEF(unk_evt_gor_000277f4);
EVT_DEF(gorotsuki_kikan);
EVT_DEF(into_gor_ship);
EVT_DEF(unk_evt_gor_00028704);
EVT_DEF(unk_evt_gor_000288b4);
EVT_DEF(unk_evt_gor_00028a14);
EVT_DEF(gor_00_koopa_evt);
EVT_DEF(gor_ship_puka);
EVT_DEF(gor_muj_ship_sub);
EVT_DEF(gor_muj_ship_init);
EVT_DEF(gor_muj_ship_pera);
EVT_DEF(gor_muj_ship_start);
EVT_DEF(tsumini);
EVT_DEF(punio_kinoko);
EVT_DEF(puniko_kinoko);
EVT_DEF(epilogue_npc_entry2);
EVT_DEF(return_gorotsuki_town_init);
EVT_DEF(return_gorotsuki_town);
EVT_DEF(gor_00_init_evt);

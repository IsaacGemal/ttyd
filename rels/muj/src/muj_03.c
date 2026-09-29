#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_item.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
s32 nigeru_suifu01[271];

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

EVT_BEGIN(muj_03_event_01)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 234)
        IF_INT_EQUAL(GSWF(3131), 0)
            SPAWN_EVENT(nigeru_suifu01)
            SET_VALUE(GSWF(3131), 1)
            SET_VALUE(LW(0), 1)
            RETURN
        END_IF
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "muj_02", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, muj_03_event_01, -1, NULL, "muj_04", "w_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(nigeru_suifu01)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), 570, 150, -50)
    CALL(evt_npc_set_ry, STRING("オドオド水夫"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 1073776160)
    CALL(evt_npc_set_position, STRING("めがね水夫"), 530, 150, -50)
    CALL(evt_npc_set_ry, STRING("めがね水夫"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("めがね水夫"), 1073776160)
    CALL(evt_npc_sound_data_set, STRING("めがね水夫"), STRING("SFX_STG5_KANBU_GLASSES_MOVE1L"), STRING("SFX_STG5_KANBU_GLASSES_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_pos, 610, 150, 0)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_pos, 0, 630, 150, 25)
    CALL(evt_party_set_dir, 0, 270, 0)
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("めがね水夫"), 32768)
    CALL(evt_msg_print, 0, STRING("stg5_muj_90_22"), 0, STRING("オドオド水夫"))
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("オドオド水夫"), 270)
        WAIT_MS(200)
        CALL(evt_npc_jump_position_nohit, STRING("オドオド水夫"), 464, 180, -50, 500, 50)
        WAIT_MS(300)
        CALL(evt_npc_jump_position_nohit, STRING("オドオド水夫"), 382, 180, -50, 500, 50)
        CALL(evt_npc_set_position, STRING("オドオド水夫"), 0, -1000, 0)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_WHILE(GF(0))
    CALL(evt_npc_set_ry, STRING("めがね水夫"), 270)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_muj_90_23"), 0, STRING("めがね水夫"))
    WAIT_MS(200)
    CALL(evt_npc_jump_position_nohit, STRING("めがね水夫"), 464, 180, -50, 500, 50)
    WAIT_MS(300)
    CALL(evt_npc_jump_position_nohit, STRING("めがね水夫"), 382, 180, -50, 500, 50)
    WAIT_MS(800)
    CALL(evt_npc_set_ry, STRING("めがね水夫"), 90)
    WAIT_MS(200)
    CALL(evt_npc_jump_position_nohit, STRING("めがね水夫"), 465, 180, -50, 500, 50)
    CALL(evt_npc_set_ry, STRING("めがね水夫"), 90)
    WAIT_MS(200)
    CALL(evt_cam3d_evt_set, 465, 288, 263, 465, 240, -8, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("めがね水夫"), 270)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_muj_90_24"), 0, STRING("めがね水夫"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        WAIT_MS(300)
        CALL(evt_npc_jump_position_nohit, STRING("めがね水夫"), 382, 180, -50, 500, 50)
        CALL(evt_npc_set_position, STRING("めがね水夫"), 0, -1000, 0)
    END_INLINE_EVENT
    CALL(evt_party_set_dir, 0, 270, 200)
    WAIT_MS(300)
    CALL(evt_party_set_dir, 0, 90, 200)
    WAIT_MS(300)
    CALL(evt_party_set_dir, 0, 270, 200)
    WAIT_MS(300)
    CALL(evt_party_set_dir, 0, 90, 200)
    WAIT_MS(300)
    CALL(evt_party_set_dir, 0, 270, 200)
    CALL(evt_msg_print_party, STRING("stg5_muj_90_25"))
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(evt_kusa_5)
    CALL(evt_item_delete_check, STRING("item_kusa_5"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_5"), 146, -250, 10, 95, 4, GSWF(3155), 0)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(evt_kusa_7)
    CALL(evt_item_delete_check, STRING("item_kusa_7"), LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_item_entry, STRING("item_kusa_7"), 121, 175, 210, -50, 4, GSWF(3156), 0)
    END_IF
    RETURN
EVT_END()

// TODO: type
static u32 tree_tbl[30] = {
    0x00000005, (u32)"A_treeA_01", (u32)"treeA_01_m", (u32)"treeA_01_ha",
    0x00000000, 0x00000000, 0x00000005, (u32)"A_treeA_02",
    (u32)"treeA_02_m", (u32)"treeA_02_ha", 0x00000000, 0x00000000,
    0x00000005, (u32)"A_treeA_03", (u32)"treeA_03_m", (u32)"treeA_03_ha",
    0x00000000, 0x00000000, 0x00000005, (u32)"A_treeA_04",
    (u32)"treeA_04_m", (u32)"treeA_04_ha", 0x00000000, 0x00000000,
    0x00000007, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 kusa_tbl[36] = {
    0x00000001, (u32)"A_kuwa_move_01", (u32)"S_kuwa_move_01", 0x00000000,
    0x00000001, (u32)"A_kuwa_move_02", (u32)"S_kuwa_move_02", 0x00000000,
    0x00000001, (u32)"A_kuwa_move_03", (u32)"S_kuwa_move_03", 0x00000000,
    0x00000001, (u32)"A_kuwa_move_04", (u32)"S_kuwa_move_04", 0x00000000,
    0x00000001, (u32)"A_kuwa_move_05", (u32)"S_kuwa_move_05", (u32)&evt_kusa_5,
    0x00000001, (u32)"A_kuwa_move_06", (u32)"S_kuwa_move_06", 0x00000000,
    0x00000001, (u32)"A_kuwa_move_07", (u32)"S_kuwa_move_07", (u32)&evt_kusa_7,
    0x00000001, (u32)"A_kuwa_move_08", (u32)"S_kuwa_move_08", 0x00000000,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(muj_03_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ5"))
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mobj_badgeblk, STRING("mobj01"), -50, 9, 0, 121, 0, GSWF(3133), 0)
    CALL(evt_mobj_brick, STRING("mobj02"), -50, 94, 0, 0, 13, 0, GSWF(3134))
    CALL(evt_mobj_brick, STRING("bblk00"), -470, 155, -50, 0, 0, 0, GSWF(3145))
    CALL(evt_mobj_powerupblk, STRING("pblk01"), -470, 240, -50, 0, GSWF(5552), 0)
    CALL(evt_mobj_badgeblk, STRING("mobj03"), -50, 270, -100, 129, 0, GSWF(3147), 0)
    CALL(evt_mobj_brick, STRING("mobj_hidden2"), -50, 209, 0, 156, 11, 0, GSWF(3153))
    CALL(evt_item_entry, STRING("item_00"), 125, 275, 10, 100, 16, GSWF(5642), 0)
    CALL(evt_eff64, STRING("sunlight"), STRING("sunlight_n64"), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    SET_VALUE(LW(0), PTR(tree_tbl))
    CALL_CHILD(evt_sub_tree_access_entry)
    SET_VALUE(LW(0), PTR(kusa_tbl))
    CALL_CHILD(evt_sub_kusa_access_entry)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

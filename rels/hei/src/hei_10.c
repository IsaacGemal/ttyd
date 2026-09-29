#include "hei_10.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 hei_test_water_tev[];
static USER_FUNC(npc_jump);
static void ext_dispent(void);  // TODO: real signature
static void ext_main(void);  // TODO: real signature
static void ext_shadow_disp(void);  // TODO: real signature
static void ext_disp(void);  // TODO: real signature
static void ext_init(void);  // TODO: real signature
static void randf(void);  // TODO: real signature

// clang-format off
extend_data(44,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(45,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[4] = {
    {"w_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hei_09", "e_bero", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "hei_11", "w_bero", 1, 1},
    {"dokan", 2, 0, 0x06, 100000, 0, 0, -1, NULL, 10, NULL, "hei_07", "dokan", 1, 1},
    {NULL},
};

// TODO: type
static u32 ext_data[63] = {
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x00000000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x40000000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x40800000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x40C00000,
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x41000000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x41200000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x41400000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x41600000,
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x41800000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x41900000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x41A00000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x41B00000,
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x41C00000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x41D00000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x41E00000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x41F00000,
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x42000000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x42080000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x42100000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x42180000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: mt$559 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 mt$559[12] = {
//     0x41000000, 0x00000000, 0x00000000, 0x00000000,
//     0x00000000, 0x41000000, 0x00000000, 0x3F000000,
//     0x00000000, 0x00000000, 0x41000000, 0x00000000,
// };

// TODO: write wait_anim_end (global, 0x88 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

// TODO: write wait_anim_frame (global, 0x68 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(golden_se)
    LOOP(0)
        CALL(wait_anim_frame, 35)
        CALL(evt_npc_get_position, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_MOVE1"), LW(0), LW(1), LW(2), 0)
        CALL(wait_anim_frame, 50)
        CALL(evt_npc_get_position, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_G_WAIT2"), LW(0), LW(1), LW(2), 0)
        CALL(wait_anim_frame, 95)
        CALL(evt_npc_get_position, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_G_WAIT1"), LW(0), LW(1), LW(2), 0)
        CALL(wait_anim_end)
    END_LOOP
    RETURN
EVT_END()

// TODO: write npc_jump (static, 0x484 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_chorobon_wait (global, 0x64 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_chorobon_escape (global, 0x60 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_dispent (static, 0x40 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_main (static, 0xc3c bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_init (static, 0x1f4 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_golden)
    CALL(evt_npc_entry, STRING("dummy"), STRING("c_chorobon"))
    CALL(evt_npc_set_position, STRING("dummy"), 0, -1000, 0)
    SPAWN_EVENT_GET_ID(PTR(golden_se), LW(15))
    WAIT_FRAMES(10)
    CALL(evt_mario_wait_movable)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("eff0"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, STRING("eff1"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 58, 227, 0, 5, -68, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg1_hei_42"), 0, STRING("ゴールドチョロボン"))
    WAIT_MS(300)
    CALL(evt_cam3d_event_from_road, 0, -15, 0, 0, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
    CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 500, 11)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 1000, 11)
    DELETE_EVENT(LW(15))
    CALL(evt_npc_set_anim, STRING("ゴールドチョロボン"), STRING("CBN_W_1"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    DIVIDE_FLOAT(LW(0), FLOAT(4.0f))
    SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
    CALL(evt_npc_flag_onoff, 1, STRING("ゴールドチョロボン"), 131088)
    LOOP(3)
        ADD_FLOAT(LW(3), LW(0))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_MOVE1"), LW(3), LW(1), LW(2), 0)
        CALL(evt_npc_jump_position, STRING("ゴールドチョロボン"), LW(3), LW(1), LW(2), 300, 0, FLOAT(30.0f), 0)
    END_LOOP
    CALL(evt_npc_flag_onoff, 1, STRING("ゴールドチョロボン"), 1073741856)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("ゴールドチョロボン"), LW(3), LW(4), LW(5))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_MOVE1"), LW(3), LW(4), LW(5), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("ゴールドチョロボン"), 131088)
    CALL(npc_jump, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(2), FLOAT(3.5f), FLOAT(2.0f), 0)
    CALL(evt_npc_battle_start, STRING("ゴールドチョロボン"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_TORIDE1"))
    CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -15, 0)
    CALL(evt_ext_entry, 500, PTR(ext_data), PTR(ext_init), PTR(ext_main), PTR(ext_dispent))
    CALL(evt_msg_print, 0, STRING("stg1_hei_44"), 0, STRING("ゴールドチョロボン"))
    WAIT_MS(300)
    CALL(evt_chorobon_escape)
    CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -1000, 0)
    SET_VALUE(GSW(0), 32)
    CALL(evt_mario_key_onoff, 1)
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(2), FLOAT(500.0f))
        DIVIDE_FLOAT(LW(2), FLOAT(40.0f))
        SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
        CALL(evt_npc_flag_onoff, 0, STRING("ゴールドチョロボン"), 131088)
        LOOP(40)
            ADD_FLOAT(LW(3), LW(2))
            CALL(npc_jump, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(3), FLOAT(0.8f), FLOAT(1.0f), 0)
        END_LOOP
        CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -1000, 0)
    END_INLINE_EVENT
    CALL(evt_chorobon_wait)
    RETURN
EVT_END()

EVT_BEGIN(hei_10_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_TORIDE1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_HEI3"))
    CALL(evt_snd_set_rev_mode, 1)
    CALL(evt_mobj_badgeblk, STRING("m"), 100, 46, -75, 241, 0, GSWF(1796), 1)
    CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -1000, 0)
    IF_INT_EQUAL(GSWF(1774), 1)
        IF_INT_EQUAL(GSWF(1775), 1)
            IF_INT_LESS(GSW(0), 32)
                CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -15, 0)
                CALL(evt_npc_change_fbat_mode, 1)
                SPAWN_EVENT(evt_golden)
            END_IF
        END_IF
    END_IF
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_map_set_tevcallback, 1, PTR(hei_test_water_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("mizu"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("mizu"), 256)
    RETURN
EVT_END()

// TODO: type
static u32 wp[1] = {
    0x00000000,
};

// TODO: write chb_jump (global, 0x150 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_shadow_disp (static, 0x38 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_disp (static, 0x34 bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it

// TODO: write randf (static, 0x7c bytes) here:
// its asm is in build/G8MJ01/hei/asm/hei_10.s; this is where the original has it, just before the first script that uses it
// clang-format on

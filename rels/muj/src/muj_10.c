#include "muj_10.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_nannpc.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "driver/effdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "system.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 korutesu_apper[];
extern s32 muj_ship_puka[];

static s32 tornade_alpha_func(EventEntry* event, BOOL isFirstCall);

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(megane_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(251, 252)
        CALL(evt_npc_set_position, STRING("me"), -455, 25, -40)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(megane_talk)
    CALL(evt_msg_print, 0, STRING("stg5_dou_19_00"), 0, STRING("me"))
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[2] = {
    {
        "めがね水夫", 0x00000600, 0x00000000, megane_init, 0x00000000, megane_talk,
    },
};

static EVT_BEGIN(e_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_b_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("S_b_doa"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door_custom)
    CALL(evt_bero_get_info_anime, LW(1), LW(10), LW(11), LW(12), LW(13))
    IF_INT_NOT_EQUAL(LW(12), 0)
        SET_FLOAT_VALUE(LW(0), FLOAT(180.0f))
        CALL_CHILD(LW(12))
        CALL(evt_bero_mario_go)
        WAIT_MS(1500)
        SPAWN_EVENT(bero_close_door_play_se)
        SET_VALUE(LW(9), LW(12))
        CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(10))
            IF_INT_EQUAL(LW(10), 0)
                SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
                CALL_CHILD(LW(9))
                BREAK_LOOP
            ELSE
                CALL_CHILD(LW(9))
            END_IF
        END_LOOP
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(bero_in_event)
    IF_INT_EQUAL(GSW(0), 258)
        SPAWN_EVENT(door_custom)
        CALL(evt_party_stop, 0)
        CALL(evt_party_stop, 1)
        BEGIN_INLINE_EVENT
            CALL(evt_mario_set_pos, -46, 45, -287)
            CALL(evt_mario_set_dir, 270, 0, 0)
            CALL(evt_mario_mov_pos2, -95, -151, FLOAT(120.0f))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(200)
            CALL(evt_party_set_pos, 0, -46, 45, -287)
            CALL(evt_party_set_dir, 0, 270, 0)
            CALL(evt_party_move_pos2, 0, -79, -190, FLOAT(120.0f))
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(400)
            CALL(evt_party_set_pos, 1, -46, 45, -287)
            CALL(evt_party_set_dir, 1, 270, 0)
            CALL(evt_party_move_pos2, 1, -95, -227, FLOAT(120.0f))
        END_INLINE_EVENT
        SET_VALUE(LW(0), 1)
        RETURN
    END_IF
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[3] = {
    {
        "w_bero", 0x0000, 0x0000, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
        -1, bero_in_event, -1, 0x00000000, "dou_11", "e_bero",
        0x0001, 0x0001, 0x00000000, 0x00000000,
    },
    {
        "A_b_bero03", 0x0001, 0x0002, 0x00000002, 0x000186A0, 0x00000000, 0x00000000,
        -1, bero_in_event, -1, 0x00000000, "muj_11", "w_bero",
        0x0005, 0x0005, e_close, e_open,
    },
};

EVT_BEGIN(nazo_event)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_mario_get_name_hitobj_ride, LW(3))
        CALL(evt_mario_get_motion, LW(4))
        IF_INT_GREATER_OR_EQUAL(LW(0), -250)
            IF_STR_EQUAL(LW(3), STRING("A_hasi_ita"))
                IF_INT_NOT_EQUAL(LW(4), 31)
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_stop, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("めがね水夫"), 1536)
    CALL(evt_npc_flag_onoff, 1, STRING("めがね水夫"), 65536)
    CALL(evt_npc_set_position, STRING("めがね水夫"), -540, 25, 0)
    CALL(evt_npc_set_ry, STRING("めがね水夫"), 90)
    CALL(evt_cam3d_evt_set, FLOAT(-382.7f), FLOAT(158.0f), FLOAT(527.8f), FLOAT(-382.7f), FLOAT(62.5f), FLOAT(-13.8f), 500, 11)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_2"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
        CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 60)
        WAIT_MS(1000)
        CALL(evt_mario_set_dir, 270, 200, 0)
        CALL(evt_party_set_dir, 0, 270, 200)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg5_dou_19_00_00"), 0, STRING("めがね水夫"))
    WAIT_MS(1000)
    CALL(evt_npc_move_position, STRING("めがね水夫"), -454, 0, 0, FLOAT(120.0f), 0)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_dou_19"), 0, STRING("めがね水夫"))
    CALL(evt_npc_get_position, STRING("めがね水夫"), LW(0), LW(1), LW(2))
    CALL(evt_npc_move_position, STRING("めがね水夫"), LW(0), -40, 0, FLOAT(120.0f), 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 251)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(oidasare_event)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 30, 50, -278)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_pos, 0, 30, 50, -278)
    CALL(evt_party_set_dir, 0, 90, 0)
    CALL(evt_cam3d_evt_set, 220, 200, 480, 220, 70, -258, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD1"), 19, 118, -236, 0)
    WAIT_MS(1000)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_party_jump_pos, 0, 190, 50, -250, 600, 0, 30)
        CALL(evt_party_jump_pos, 0, 210, 50, -250, 300, 0, -1000)
    END_INLINE_EVENT
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_A_1A"))
    CALL(evt_mario_jump_pos, 230, 50, -250, 500, 2, 30)
    CALL(evt_mario_set_pose, STRING("M_D_7B"))
    CALL(evt_mario_jump_pos, 250, 50, -250, 250, 2, -1000)
    CALL(evt_mario_set_pos, 250, 50, -250)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_LAND_DAMAGE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_FALL_CRUSH1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_D_8"))
    WAIT_MS(1000)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_cam3d_evt_off, 500, 11)
    SET_VALUE(GSW(0), 254)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: type
u32 test_ext_entry_data[93] = {
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
    (u32)"c_elmos", (u32)"S_1", 0x00000000, (u32)"c_elmos",
    (u32)"S_1", 0x40000000, (u32)"c_elmos", (u32)"S_1",
    0x40800000, (u32)"c_elmos", (u32)"S_1", 0x40C00000,
    (u32)"c_elmos", (u32)"S_1", 0x41000000, (u32)"c_elmos",
    (u32)"S_1", 0x41200000, (u32)"c_elmos", (u32)"S_1",
    0x41400000, (u32)"c_elmos", (u32)"S_1", 0x41600000,
    (u32)"c_elmos", (u32)"S_1", 0x41800000, (u32)"c_elmos",
    (u32)"S_1", 0x41900000, 0x00000000, 0x00000000,
    0x00000000,
};

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
static u32 elmos_animtbl[11] = {
    0x00140014, 0x00150015, 0x00160016, 0x00170017,
    0x00180018, 0x00190019, 0x001A001A, 0x001B001B,
    0x001C001C, 0x001D001D, 0xFFFF0000,
};

// TODO: type
u32 senin_event_suifupos[380] = {
    0x00000008, 0x00000078, 0xFFFFFF32, 0x0000010D,
    0xFFFFFFEF, 0x00000078, 0xFFFFFEE5, 0x0000010E,
    0x00000015, 0x00000078, 0xFFFFFECC, 0x0000010E,
    0x00000021, 0x00000078, 0xFFFFFECC, 0x0000010E,
    0xFFFFFFEF, 0x00000078, 0xFFFFFED9, 0x0000010E,
    0xFFFFFFFA, 0x00000078, 0xFFFFFF24, 0x0000010E,
    0xFFFFFFFC, 0x00000078, 0xFFFFFF33, 0x0000010E,
    0x00000000, 0x00000078, 0xFFFFFF3F, 0x0000010E,
    0x00000017, 0x00000078, 0xFFFFFF06, 0x0000010E,
    0x00000007, 0x00000078, 0xFFFFFF41, 0x0000010E,
    0x0000002B, 0x00000078, 0xFFFFFF29, 0x0000010E,
    0x00000036, 0x00000078, 0xFFFFFF37, 0x0000010E,
    0x00000017, 0x00000078, 0xFFFFFF4F, 0x0000010E,
    0x0000000B, 0x00000078, 0xFFFFFF57, 0x0000010E,
    0x00000006, 0x00000078, 0xFFFFFF66, 0x0000010E,
    0x00000016, 0x00000078, 0xFFFFFF61, 0x0000010E,
    0xFFFFFFF9, 0x00000078, 0xFFFFFEFA, 0x0000010E,
    0x00000028, 0x00000078, 0xFFFFFF35, 0x0000010E,
    0xFFFFFFFC, 0x00000078, 0xFFFFFF47, 0x0000010E,
    0x00000000, 0x00000078, 0xFFFFFEED, 0x0000010E,
    0x0000001A, 0x00000078, 0xFFFFFEF8, 0x0000010E,
    0x00000028, 0x00000078, 0xFFFFFF21, 0x0000010E,
    0x00000022, 0x00000078, 0xFFFFFF57, 0x0000010E,
    0x00000021, 0x00000078, 0xFFFFFF46, 0x0000010E,
    0x0000001E, 0x00000078, 0xFFFFFF28, 0x0000010E,
    0x00000009, 0x00000078, 0xFFFFFF17, 0x0000010E,
    0x0000000E, 0x00000078, 0xFFFFFF25, 0x0000010E,
    0x00000019, 0x00000078, 0xFFFFFF38, 0x0000010E,
    0x00000032, 0x00000078, 0xFFFFFF2D, 0x0000010E,
    0x00000021, 0x00000078, 0xFFFFFEFD, 0x0000010E,
    0x0000000A, 0x00000078, 0xFFFFFED9, 0x0000010E,
    0x0000000A, 0x00000078, 0xFFFFFEE4, 0x0000010E,
    0xFFFFFFFF, 0x00000078, 0xFFFFFECD, 0x0000010E,
    0x00000004, 0x00000078, 0xFFFFFED5, 0x0000010E,
    0x0000000D, 0x00000078, 0xFFFFFF03, 0x0000010E,
    0x00000014, 0x00000078, 0xFFFFFEDC, 0x0000010E,
    0x0000000D, 0x00000078, 0xFFFFFF03, 0x0000010E,
    0x00000022, 0x00000078, 0xFFFFFF14, 0x0000010D,
    0x00000023, 0x00000078, 0xFFFFFF09, 0x0000010E,
    0x0000001F, 0x00000078, 0xFFFFFF0F, 0x0000010E,
    0x00000031, 0x00000078, 0xFFFFFF3F, 0x0000010E,
    0x00000003, 0x00000078, 0xFFFFFEFE, 0x0000010E,
    0x0000000B, 0x00000078, 0xFFFFFF10, 0x0000010E,
    0x00000001, 0x00000078, 0xFFFFFEF8, 0x0000010E,
    0x00000027, 0x00000078, 0xFFFFFF18, 0x0000010E,
    0x0000000F, 0x00000078, 0xFFFFFEF6, 0x0000010E,
    0x00000019, 0x00000078, 0xFFFFFEE2, 0x0000010E,
    0x00000028, 0x00000078, 0xFFFFFF04, 0x0000010E,
    0x0000000E, 0x00000078, 0xFFFFFEF0, 0x0000010E,
    0x00000016, 0x00000078, 0xFFFFFEE8, 0x0000010F,
    0xFFFFFECF, 0x0000004F, 0xFFFFFF84, 0x0000005A,
    0xFFFFFEE2, 0x0000004F, 0xFFFFFF8C, 0x0000005A,
    0xFFFFFEDE, 0x0000004F, 0xFFFFFF75, 0x0000005A,
    0xFFFFFEEF, 0x0000004F, 0xFFFFFF84, 0x0000005A,
    0xFFFFFEFD, 0x0000004F, 0xFFFFFF90, 0x0000005A,
    0xFFFFFF17, 0x0000004F, 0xFFFFFF8F, 0x0000005A,
    0xFFFFFF34, 0x0000004F, 0xFFFFFF86, 0x0000005A,
    0xFFFFFF09, 0x0000004F, 0xFFFFFF80, 0x0000005A,
    0xFFFFFF19, 0x0000004F, 0xFFFFFF7E, 0x0000005A,
    0xFFFFFF13, 0x0000004F, 0xFFFFFF35, 0x0000005A,
    0xFFFFFF07, 0x0000004F, 0xFFFFFF40, 0x0000005A,
    0xFFFFFEFB, 0x0000004F, 0xFFFFFF48, 0x0000005A,
    0xFFFFFEF0, 0x0000004F, 0xFFFFFF52, 0x0000005A,
    0xFFFFFEE1, 0x0000004F, 0xFFFFFF66, 0x0000005A,
    0xFFFFFEEE, 0x0000004F, 0xFFFFFF69, 0x0000005A,
    0xFFFFFEF2, 0x0000004F, 0xFFFFFF5D, 0x0000005A,
    0xFFFFFEEE, 0x0000004F, 0xFFFFFF79, 0x0000005A,
    0xFFFFFEFE, 0x0000004F, 0xFFFFFF80, 0x0000005A,
    0xFFFFFF05, 0x0000004F, 0xFFFFFF50, 0x0000005A,
    0xFFFFFF15, 0x0000004F, 0xFFFFFF9A, 0x0000005A,
    0xFFFFFF12, 0x0000004F, 0xFFFFFF75, 0x0000005A,
    0xFFFFFF06, 0x0000004F, 0xFFFFFF67, 0x0000005A,
    0xFFFFFF09, 0x0000004F, 0xFFFFFF70, 0x0000005A,
    0xFFFFFF02, 0x0000004F, 0xFFFFFF6F, 0x0000005A,
    0xFFFFFF02, 0x0000004F, 0xFFFFFF5D, 0x0000005A,
    0xFFFFFF13, 0x0000004F, 0xFFFFFF5D, 0x0000005A,
    0xFFFFFF33, 0x00000045, 0xFFFFFF97, 0x0000005A,
    0xFFFFFF3F, 0x00000045, 0xFFFFFF9C, 0x0000005A,
    0xFFFFFF45, 0x00000045, 0xFFFFFF8D, 0x0000005A,
    0xFFFFFF31, 0x00000045, 0xFFFFFF73, 0x0000005A,
    0xFFFFFF27, 0x00000045, 0xFFFFFF4C, 0x0000005A,
    0xFFFFFF39, 0x00000045, 0xFFFFFF58, 0x0000005A,
    0xFFFFFF2F, 0x00000045, 0xFFFFFF3D, 0x0000005A,
    0xFFFFFF2A, 0x00000045, 0xFFFFFF29, 0x0000005A,
    0xFFFFFF1D, 0x00000045, 0xFFFFFF24, 0x0000005A,
    0xFFFFFF1A, 0x00000045, 0xFFFFFF40, 0x0000005A,
    0xFFFFFF2B, 0x00000045, 0xFFFFFF72, 0x0000005A,
    0xFFFFFF2D, 0x00000045, 0xFFFFFF62, 0x0000005A,
    0xFFFFFF3D, 0x00000045, 0xFFFFFF68, 0x0000005A,
    0xFFFFFF46, 0x00000045, 0xFFFFFF77, 0x0000005A,
    0xFFFFFF26, 0x00000045, 0xFFFFFF59, 0x0000005A,
    0xFFFFFF31, 0x00000045, 0xFFFFFF52, 0x0000005A,
    0xFFFFFF20, 0x00000045, 0xFFFFFF37, 0x0000005A,
    0xFFFFFF47, 0x00000045, 0xFFFFFF83, 0x0000005A,
    0xFFFFFF26, 0x00000045, 0xFFFFFF68, 0x0000005A,
};

// TODO: type
u32 senin_event_erumospos[180] = {
    0xFFFFFF80, 0x000000A7, 0xFFFFFF7C, 0xFFFFFF87,
    0x000000BF, 0xFFFFFF7B, 0xFFFFFF8B, 0x0000009D,
    0xFFFFFFA3, 0xFFFFFF90, 0x00000091, 0xFFFFFFB2,
    0xFFFFFF8E, 0x000000A7, 0xFFFFFFB1, 0xFFFFFF71,
    0x00000091, 0xFFFFFF60, 0xFFFFFF67, 0x00000090,
    0xFFFFFF40, 0xFFFFFF4A, 0x00000091, 0xFFFFFF1A,
    0xFFFFFF42, 0x00000091, 0xFFFFFF02, 0xFFFFFF37,
    0x00000091, 0xFFFFFEDE, 0xFFFFFF6E, 0x000000AF,
    0xFFFFFF45, 0xFFFFFF41, 0x000000B3, 0xFFFFFEDD,
    0xFFFFFF50, 0x000000AF, 0xFFFFFF0F, 0xFFFFFF49,
    0x000000DA, 0xFFFFFEF9, 0xFFFFFF2C, 0x000000DB,
    0xFFFFFEED, 0xFFFFFF63, 0x000000DB, 0xFFFFFF12,
    0xFFFFFF6A, 0x000000DA, 0xFFFFFF3F, 0xFFFFFF87,
    0x000000D9, 0xFFFFFF75, 0xFFFFFF84, 0x000000DB,
    0xFFFFFFA9, 0xFFFFFF88, 0x000000DA, 0xFFFFFF93,
    0xFFFFFF8E, 0x0000011B, 0xFFFFFFC1, 0xFFFFFF87,
    0x0000011B, 0xFFFFFF99, 0xFFFFFF69, 0x000000F0,
    0xFFFFFF54, 0xFFFFFF7E, 0x0000011B, 0xFFFFFF7C,
    0xFFFFFF62, 0x0000011B, 0xFFFFFF23, 0xFFFFFF4E,
    0x0000011B, 0xFFFFFF2F, 0xFFFFFF55, 0x0000011B,
    0xFFFFFEFB, 0xFFFFFF4B, 0x0000011B, 0xFFFFFEE0,
    0xFFFFFF4E, 0x000000F9, 0xFFFFFEE6, 0xFFFFFF5A,
    0x000000C6, 0xFFFFFF02, 0xFFFFFF55, 0x000000FC,
    0xFFFFFEF9, 0xFFFFFF4C, 0x000000FD, 0xFFFFFF24,
    0xFFFFFF49, 0x000000C4, 0xFFFFFF24, 0xFFFFFF68,
    0x000000C4, 0xFFFFFF2B, 0xFFFFFF5E, 0x000000C4,
    0xFFFFFF3E, 0xFFFFFF73, 0x000000D5, 0xFFFFFF59,
    0xFFFFFF68, 0x000000D5, 0xFFFFFF76, 0xFFFFFF6E,
    0x000000E9, 0xFFFFFF5B, 0xFFFFFF78, 0x000000E9,
    0xFFFFFF90, 0xFFFFFF87, 0x000000D0, 0xFFFFFF9C,
    0xFFFFFF8C, 0x000000D1, 0xFFFFFF82, 0xFFFFFF70,
    0x000000B8, 0xFFFFFF4D, 0xFFFFFF61, 0x000000B7,
    0xFFFFFF13, 0xFFFFFF4E, 0x000000B8, 0xFFFFFEE2,
    0xFFFFFF3F, 0x00000108, 0xFFFFFEDD, 0xFFFFFF58,
    0x000000F5, 0xFFFFFF09, 0xFFFFFF6D, 0x000000EE,
    0xFFFFFF32, 0xFFFFFF83, 0x000000F3, 0xFFFFFF66,
    0xFFFFFF93, 0x000000F3, 0xFFFFFFA9, 0xFFFFFFA8,
    0x000000F4, 0xFFFFFFB5, 0xFFFFFFB2, 0x000000D1,
    0xFFFFFFA2, 0xFFFFFFA9, 0x000000FB, 0xFFFFFF77,
    0xFFFFFFA9, 0x0000010A, 0xFFFFFF46, 0xFFFFFF8A,
    0x0000010A, 0xFFFFFF28, 0xFFFFFF84, 0x000000FD,
    0xFFFFFEF1, 0xFFFFFF84, 0x000000FD, 0xFFFFFEF1,
    0xFFFFFF7C, 0x000000FC, 0xFFFFFEED, 0xFFFFFF6D,
    0x000000FC, 0xFFFFFEE8, 0xFFFFFF6B, 0x000000FA,
    0xFFFFFECF, 0xFFFFFF82, 0x000000FC, 0xFFFFFF0E,
};

void elmosalpha_func(NanNpcEntry* entry) {
    s32 elapsed;

    switch (entry->field_0x68) {
    case 0:
        entry->field_0x50 = gp->renderTime;
        entry->field_0x6C = irand(1000);
        entry->field_0x68 = 1;
        // fallthrough
    case 1:
        elapsed = ((s32)gp->renderTime - (s32)entry->field_0x50) / (OS_TIMER_CLOCK / 1000);
        if (elapsed >= entry->field_0x6C) {
            entry->field_0x50 = gp->renderTime;
            entry->field_0x6C = 500;
            entry->field_0x68 = 2;
        }
        break;
    case 2:
        elapsed = ((s32)gp->renderTime - (s32)entry->field_0x50) / (OS_TIMER_CLOCK / 1000);
        if (elapsed < entry->field_0x6C) {
            entry->color.a = (u8)intplGetValue(0, elapsed, entry->field_0x6C, 0.0f, 255.0f);
        } else {
            entry->field_0x68 = 3;
        }
        break;
    case 3:
        break;
    }
}

USER_FUNC(suifu_option_init_func) {
    NanNpcWork* work = nanNPCWork;
    s32 i;

    for (i = 0; i < 95; i++) {
        work->entries[i].homePosition = work->entries[i].position;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(suifu_option_func) {
    f32 offset = evtGetFloat(event, event->args[0]);
    NanNpcWork* work = nanNPCWork;
    s32 i;

    for (i = 0; i < 95; i++) {
        f32 homeY = work->entries[i].homePosition.y;

        work->entries[i].position.y = homeY + offset;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(senin_event)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_nannpc_init, PTR(test_ext_entry_data), 0, 0, 0, 155)
    CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
    CALL(evt_nannpcwork_flag_onoff, 0, 1)
    SET_INT_READ_ADDR(PTR(senin_event_suifupos))
    SET_VALUE(LW(0), 0)
    LOOP(95)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(suifu_animtbl))
        READ_INT4(LW(1), LW(2), LW(3), LW(4))
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_nannpc_set_dir, LW(0), LW(4))
        CALL(evt_nannpc_ppflag_onoff, LW(0), 0, 4)
        ADD(LW(0), 1)
    END_LOOP
    SET_INT_READ_ADDR(PTR(senin_event_erumospos))
    SET_VALUE(LW(0), 95)
    LOOP(60)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(elmos_animtbl))
        READ_INT3(LW(1), LW(2), LW(3))
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_nannpc_set_dir, LW(0), 90)
        CALL(evt_nannpc_set_color, LW(0), 255, 255, 255, 0)
        CALL(evt_nannpc_ppflag_onoff, LW(0), 0, 4)
        ADD(LW(0), 1)
    END_LOOP
    SET_RAW_VALUE(LW(0), GW(0))
    SET_FLOAT_VALUE(LW(1), FLOAT(1.5f))
    SPAWN_EVENT(muj_ship_puka)
    CALL(evt_hit_bind_mapobj, STRING("A_b"), STRING("S_b"))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mapobj_trans, 1, STRING("S_b"), 0, GW(0), 0)
            CALL(evt_hit_bind_update, STRING("A_b"))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(suifu_option_init_func)
        LOOP(0)
            CALL(suifu_option_func, GW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_npc_entry, STRING("オドオド水夫"), STRING("c_odo_k"))
    CALL(evt_npc_set_tribe, STRING("オドオド水夫"), STRING("オドオド水夫"))
    CALL(evt_npc_entry, STRING("ピートン"), STRING("c_m_waka"))
    CALL(evt_npc_set_tribe, STRING("ピートン"), STRING("ピートン（指輪）"))
    CALL(evt_npc_set_tribe, STRING("モニー"), STRING("モニー（指輪）"))
    CALL(evt_npc_set_position, STRING("コルテス"), 0, -1000, 0)
    CALL(evt_npc_set_ry, STRING("コルテス"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 819200)
    CALL(evt_npc_set_position, STRING("水夫D"), -158, 44, -160)
    CALL(evt_npc_set_ry, STRING("水夫D"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫D"), 688656)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫D"), 1024)
    CALL(evt_npc_set_position, STRING("モニー"), -159, 44, -257)
    CALL(evt_npc_set_ry, STRING("モニー"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 688656)
    CALL(evt_npc_flag_onoff, 0, STRING("モニー"), 1024)
    CALL(evt_npc_set_position, STRING("ピートン"), -195, 44, -264)
    CALL(evt_npc_set_ry, STRING("ピートン"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 688656)
    CALL(evt_npc_flag_onoff, 0, STRING("ピートン"), 1024)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -45, 120, -289)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 688656)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 1024)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), -18, 120, -179)
    CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 688656)
    CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 1024)
    CALL(evt_cam3d_evt_set, -71, 178, 259, -71, 83, -282, 0, 11)
    CALL(evt_bero_exec_wait, 65536)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_dou_19_01"), 0, STRING("水夫D"))
    CALL(evt_msg_print, 0, STRING("stg5_dou_19_02"), 0, STRING("モニー"))
    CALL(evt_msg_print, 0, STRING("stg5_dou_19_03"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_cam3d_evt_set, -150, 300, 682, -150, 150, -175, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_position, STRING("コルテス"), -215, 120, -160)
    CALL_CHILD(korutesu_apper)
    CALL(evt_msg_print, 0, STRING("stg5_dou_19_04"), 0, STRING("コルテス"))
    CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_APPEAR3"), 0)
    SET_VALUE(LW(0), 95)
    LOOP(60)
        CALL(evt_nannpc_set_func, LW(0), PTR(elmosalpha_func))
        ADD(LW(0), 1)
    END_LOOP
    WAIT_MS(3000)
    SET_VALUE(GSW(0), 259)
    CALL(evt_fade_set_mapchange_type, 0, 2, 500, 1, 500)
    CALL(evt_bero_mapchange, STRING("muj_00"), STRING(""))
    RETURN
EVT_END()

static EVT_BEGIN(dai3_shurai_after)
    LOOP(0)
        CALL(evt_sub_random, 2500, LW(10))
        WAIT_MS(LW(10))
        WAIT_MS(1500)
        BEGIN_INLINE_EVENT
            CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 500)
        END_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(1), 250)
        CALL(evt_eff64, STRING(""), STRING("hokori_n64"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    END_LOOP
    RETURN
EVT_END()

static HitDamageReturn hitDmgRetPoints[7] = {
    { "A_yuka01", { 0.0f, -1000.0f, 0.0f } },
    { "A_yuka02", { 0.0f, -1000.0f, 0.0f } },
    { "A_yuka03", { 0.0f, -1000.0f, 0.0f } },
    { "A_yuka04", { 0.0f, -1000.0f, 0.0f } },
    { "A_hasi_ita", { 0.0f, -1000.0f, 0.0f } },
    { "ship_change_", { 0.0f, -1000.0f, 0.0f } },
    { NULL, { 0.0f, 0.0f, 0.0f } },
};

EVT_BEGIN(muj_10_init_evt)
    IF_INT_LESS(GSW(0), 253)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ8"))
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ8"))
    END_IF
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_cam_road_reset)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_npc_sound_data_set, STRING("めがね水夫"), STRING("SFX_STG5_KANBU_GLASSES_MOVE1L"), STRING("SFX_STG5_KANBU_GLASSES_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    IF_INT_EQUAL(GSW(0), 250)
        SPAWN_EVENT(nazo_event)
    END_IF
    IF_INT_EQUAL(GSW(0), 253)
        SPAWN_EVENT(oidasare_event)
    END_IF
    IF_INT_EQUAL(GSW(0), 258)
        SPAWN_EVENT(senin_event)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 260)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_b"), 1)
    END_IF
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_map_playanim, STRING("noname"), 1, 0)
    IF_INT_LESS(GSW(0), 260)
        IF_INT_NOT_EQUAL(GSW(0), 258)
            CALL(evt_map_playanim, STRING("fune"), 1, 0)
        END_IF
        BEGIN_INLINE_EVENT
            LOOP(0)
                CALL(evt_mapobj_get_position, STRING("S_b"), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_SHIP_KOR_SHAKE1"), LW(0), LW(1), LW(2), 0)
                WAIT_MS(2000)
            END_LOOP
        END_INLINE_EVENT
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 260)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hasi_rope01"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hasi_rope02"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hasi_m"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_10_sui02_01"), 1)
        CALL(evt_hitobj_onoff, STRING("A_hasi_ita"), 1, 0)
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hasi_zan"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_10_sui02_02"), 1)
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 257)
        IF_INT_LESS(GSW(0), 258)
            SPAWN_EVENT(dai3_shurai_after)
        END_IF
    END_IF
    CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    CALL_CHILD(evt_gazigazi_entry)
    CALL(evt_hitobj_onoff, STRING("A_b_bero02"), 1, 0)
    IF_INT_LESS(GSW(0), 258)
        CALL(evt_mobj_recovery_blk, STRING("r_blk"), 12, -260, 135, -145, 0, 0)
        CALL(evt_mobj_save_blk, STRING("s_blk"), -45, 83, 0, 0, 0)
    END_IF
    RETURN
EVT_END()

static s32 tornade_alpha_func(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);
    f32 alpha = evtGetFloat(event, args[1]);
    EffectEntry* effect = effNameToPtr((const char*)value);

    *(f32*)((u8*)effect->userdata + 0x148) = alpha;
    return EVT_RETURN_DONE;
}
// clang-format on

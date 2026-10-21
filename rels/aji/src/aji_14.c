#include "aji_14.h"
#include "driver/animdrv.h"
#include "driver/npcdrv.h"
#include "memory.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_emergency[];

// clang-format off
extend_data(56,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(57,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(w_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(w_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.25f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_1"), 0, 0, LW(0))
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 0, STRING("S_door1_2"), 0, 0, LW(0))
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"w_bero", 1, 10, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "aji_13", "e_bero", 5, 5, &w_door_close, &w_door_open},
    {NULL},
};

// groupWorldPos holds one Vec per group; groupWorldPosCount is the count of entries.
s32 waku(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc;
    AnimationPose* pose;
    Vec* clear;
    s32 idx;

    npc = npcNameToPtr_NoAssert("ロボット");
    if (npc != NULL) {
        pose = animPoseGetAnimPosePtr(npc->poseId);
        animPoseWorldPositionEvalOn(npc->poseId);
        if (pose->groupWorldPos != NULL) {
            s32 i;
            Vec* positions = (Vec*)pose->groupWorldPos->address;
            idx = animPoseGetGroupIdx(npc->poseId, "locator17");
            if (npc->position.y >= 0.0f) {
                npcNameToPtr("枠")->position = positions[idx];
            } else {
                npcNameToPtr("枠")->position = (Vec){0.0f, -1000.0f, 0.0f};
            }
            // The original reloads the base before the loop instead of reusing `positions`.
            clear = (Vec*)pose->groupWorldPos->address;
            for (i = 0; i < pose->groupWorldPosCount; i++) {
                clear[i] = (Vec){0.0f, -1000.0f, 0.0f};
            }
        }
    }

    npc = npcNameToPtr_NoAssert("ロボット２");
    if (npc != NULL) {
        pose = animPoseGetAnimPosePtr(npc->poseId);
        animPoseWorldPositionEvalOn(npc->poseId);
        if (pose->groupWorldPos != NULL) {
            s32 i;
            Vec* positions = (Vec*)pose->groupWorldPos->address;
            // A base of its own, as in the first block (sharing `clear` changes the registers).
            Vec* clear2;

            idx = animPoseGetGroupIdx(npc->poseId, "locator17");
            if (npc->position.y >= 0.0f) {
                npcNameToPtr("枠２")->position = positions[idx];
            } else {
                npcNameToPtr("枠２")->position = (Vec){0.0f, -1000.0f, 0.0f};
            }
            clear2 = (Vec*)pose->groupWorldPos->address;
            for (i = 0; i < pose->groupWorldPosCount; i++) {
                clear2[i] = (Vec){0.0f, -1000.0f, 0.0f};
            }
        }
    }
    return 0;
}

EVT_BEGIN(evt_robot_waku)
    CALL(evt_npc_entry, STRING("枠"), STRING("c_mb_robo_2a"))
    CALL(evt_npc_flag_onoff, 1, STRING("枠"), 786432)
    CALL(evt_npc_set_anim, STRING("枠"), STRING("L_1"))
    CALL(evt_npc_entry, STRING("枠２"), STRING("c_mb_robo_2a"))
    CALL(evt_npc_flag_onoff, 1, STRING("枠２"), 786432)
    CALL(evt_npc_set_anim, STRING("枠２"), STRING("L_1"))
    BEGIN_INLINE_EVENT
        SET_TYPE(0)
        CALL(waku)
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(evt_boss)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_set_position, STRING("幹部"), 150, 60, 0)
    CALL(evt_npc_set_position, STRING("ロボット"), 260, 60, 0)
    CALL(evt_npc_set_ry, STRING("幹部"), 270)
    CALL(evt_npc_set_ry, STRING("ロボット"), 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ロボット"), 33554432)
    CALL(evt_npc_flag_onoff, 0, STRING("ロボット"), 1536)
    CALL(evt_npc_pera_onoff, STRING("ロボット"), 0)
    CALL(evt_npc_set_anim, STRING("ロボット"), STRING("Z_1"))
    CALL(evt_cam3d_evt_set, 375, 420, 935, -225, -30, 0, 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, 375, 500, 935, -225, 50, 0, 4000, 11)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_mario_mov_pos2, -420, 0, FLOAT(80.0f))
    WAIT_MS(1000)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_MAGNUM_APP1"))
    CALL(evt_msg_print, 0, STRING("stg7_aji_36"), 0, STRING("幹部"))
    WAIT_MS(300)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1300)
    CALL(evt_cam3d_evt_set, 10, 190, 275, 175, 60, -142, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_flag_onoff, 1, STRING("幹部"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("ロボット"), 32768)
    CALL(evt_msg_print, 0, STRING("stg7_aji_37"), 0, STRING("幹部"))
    WAIT_MS(300)
    CALL(evt_party_stop, 0)
    CALL(evt_party_set_pos, 0, -460, 0, 15)
    CALL(evt_cam3d_evt_set, -435, 40, 287, -435, 40, -13, 0, 11)
    WAIT_FRAMES(1)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg7_aji_37_01"))
    WAIT_MS(300)
    CALL(evt_cam3d_evt_set, -211, 169, 250, 275, 80, -90, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg7_aji_37_02"), 0, STRING("幹部"))
    WAIT_MS(300)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -435, 40, 287, -435, 40, -13, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg7_aji_38"))
    WAIT_MS(300)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 140, 100, 295, 140, 110, -105, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg7_aji_39"), 0, STRING("幹部"))
    WAIT_MS(300)
    WAIT_MS(500)
    CALL(evt_npc_flag_onoff, 0, STRING("幹部"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("ロボット"), 32768)
    CALL(evt_cam3d_evt_set, 250, 124, 545, 250, 140, -105, 500, 11)
    WAIT_MS(500)
    CALL(evt_set_dir_to_target, STRING("幹部"), STRING("ロボット"))
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("幹部"), 260, -50, 0, FLOAT(80.0f), 1)
    CALL(evt_npc_get_position, STRING("ロボット"), LW(0), LW(1), LW(2))
    CALL(evt_npc_set_ry, STRING("幹部"), 270)
    WAIT_MS(200)
    ADD(LW(1), 50)
    SUBTRACT(LW(2), 5)
    CALL(evt_npc_jump_position_nohit, STRING("幹部"), LW(0), LW(1), LW(2), 300, FLOAT(50.0f))
    CALL(evt_npc_set_position, STRING("幹部"), 0, -1000, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(966)
        CALL(evt_npc_get_position, STRING("ロボット"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_BOSS_ACTIVE1"), LW(0), LW(1), LW(2), LW(9))
        WAIT_MS(2533)
        CALL(evt_snd_sfxoff, LW(9))
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("ロボット"), STRING("R_1"))
    CALL(evt_npc_wait_anim, STRING("ロボット"), FLOAT(1.0f))
    CALL(evt_npc_set_anim, STRING("ロボット"), STRING("S_1"))
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(20)
            ADD_FLOAT(LW(0), FLOAT(-2.25f))
            CALL(evt_npc_set_rotate, STRING("ロボット"), 0, LW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 165, 183, 285, 281, 193, -98, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_flag_onoff, 1, STRING("ロボット"), 1536)
    CALL(evt_msg_print, 0, STRING("stg7_aji_41"), 0, STRING("ロボット"))
    CALL(evt_npc_flag_onoff, 0, STRING("ロボット"), 1536)
    CALL(evt_cam3d_evt_set, 250, 124, 545, 250, 140, -105, 300, 11)
    WAIT_MS(300)
    WAIT_MS(2000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_anim, STRING("ロボット"), STRING("W_1"))
        CALL(evt_npc_move_position, STRING("ロボット"), 130, 0, 0, FLOAT(80.0f), 1)
        CALL(evt_npc_set_anim, STRING("ロボット"), STRING("S_1"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_npc_get_position, STRING("ロボット"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_BOSS_MOVE1L"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -475, 12, 300, 140, 90, -123, 0, 11)
    WAIT_FRAMES(1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, -100, 0, FLOAT(100.0f))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -140, 15, FLOAT(100.0f))
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("ロボット"), STRING("J_1"))
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_npc_set_anim, STRING("ロボット"), STRING("J_2"))
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("ロボット"), 0, 0, 0, 500, FLOAT(80.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 50)
    END_INLINE_EVENT
    CALL(evt_npc_set_anim, STRING("ロボット"), STRING("J_3"))
    WAIT_MS(200)
    CALL(evt_npc_set_anim, STRING("ロボット"), STRING("S_1"))
    WAIT_MS(500)
    WAIT_MS(2000)
    CALL(evt_cam3d_evt_set, -50, 475, 1104, -50, 35, -13, 1000, 11)
    WAIT_MS(300)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envoff, 512)
    CALL(evt_npc_set_battle_info, STRING("ロボット"), 5)
    CALL(evt_npc_battle_start, STRING("ロボット"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_pos, -100, 0, 0)
    CALL(evt_party_set_pos, 0, -140, 0, 15)
    CALL(evt_npc_set_position, STRING("ロボット"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ロボット２"), 0, 0, 0)
    CALL(evt_cam3d_evt_set, 0, 200, 675, 0, 75, -13, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_npc_get_position, STRING("ロボット２"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_3RD_EXPLSION1"), LW(0), LW(1), LW(2), 0)
    ADD(LW(1), 80)
    CALL(evt_eff64, STRING("eff1"), STRING("akari_charge_n64"), 1, LW(0), LW(1), LW(2), FLOAT(3.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_3RD_EXPLSION2"), LW(0), LW(1), LW(2), LW(10))
    CALL(evt_eff64, STRING("eff2"), STRING("radiation_n64"), 2, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_eff_softdelete, STRING("eff1"))
    ADD(LW(2), 100)
    CALL(evt_eff, 0, STRING("kemuri_test"), 8, LW(0), LW(1), LW(2), FLOAT(1.75f), 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_sub_random, 300, LW(0))
            CALL(evt_sub_random, 200, LW(1))
            CALL(evt_sub_random, 200, LW(2))
            SUBTRACT(LW(0), 150)
            CALL(evt_npc_get_position, STRING("ロボット２"), LW(10), LW(11), LW(12))
            ADD(LW(0), LW(10))
            ADD(LW(1), LW(11))
            ADD(LW(2), LW(12))
            CALL(evt_eff, 0, STRING("kemuri_test"), 8, LW(0), LW(1), LW(2), FLOAT(1.75f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_MS(300)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("ロボット２"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_3RD_EXPLSION3"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(1000)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(3000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 0, STRING("幹部"), 1536)
        CALL(evt_npc_set_anim, STRING("幹部"), STRING("D_1"))
        CALL(evt_npc_set_position, STRING("幹部"), 0, 70, 0)
        CALL(evt_npc_jump_position_nohit, STRING("幹部"), -200, 200, 100, 1000, FLOAT(80.0f))
    END_INLINE_EVENT
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg7_aji_42"), 0, STRING("ロボット"))
    CALL(evt_fade_set_mapchange_type, 1, 17, 900, 16, 900)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_bero_mapchange, STRING("moo_04"), STRING("evt_aji_14"))
    RETURN
EVT_END()

EVT_BEGIN(evt_boss2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_pos, -100, 0, 0)
    CALL(evt_party_set_pos, 0, -140, 0, 15)
    CALL(evt_seq_wait, 2)
    WAIT_MS(1000)
    CALL(evt_item_entry, STRING("dummy"), 120, 0, 1000, 0, 17, -1, 0)
    CALL(evt_sub_intpl_msec_init, 11, 1000, 25, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_item_set_position, STRING("dummy"), 0, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    BEGIN_INLINE_EVENT_GET_ID(LW(10))
        SET_VALUE(LW(10), 0)
        CALL(evt_item_get_position, STRING("dummy"), LW(0), LW(1), LW(2))
        LOOP(0)
            CALL(evt_sub_get_sincos, LW(10), LW(11), LW(12))
            MULTIPLY_FLOAT(LW(11), FLOAT(10.0f))
            SET_FLOAT_VALUE(LW(12), LW(1))
            ADD_FLOAT(LW(12), LW(11))
            ADD(LW(10), 5)
            MODULO(LW(10), 360)
            CALL(evt_item_set_position, STRING("dummy"), LW(0), LW(12), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("stg7_aji_42_01"))
    CALL(evt_mario_set_dir, 270, 200, 0)
    WAIT_MS(200)
    CALL(evt_mario_set_pose, STRING("M_N_2"))
    WAIT_MS(500)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -40, 15, FLOAT(80.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 0, 0, FLOAT(80.0f))
    SET_VALUE(GSW(0), 373)
    DELETE_EVENT(LW(10))
    SET_VALUE(LW(0), 7)
    SET_VALUE(LW(1), STRING("dummy"))
    CALL_CHILD(evt_sub_starstone)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[] = {
    {"幹部", 0x40000600},
    {"ロボット", 0x40000600},
    {NULL},
};

EVT_BEGIN(aji_14_init_evt)
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_NOT_EQUAL(LW(0), STRING("tuzuki"))
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI2"))
    ELSE
        CALL(evt_snd_bgmoff, 512)
        CALL(evt_snd_envoff, 512)
    END_IF
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("tuzuki"))
        SPAWN_EVENT(evt_boss2)
        RETURN
    END_IF
    IF_INT_LESS(GSW(0), 373)
        CALL(evt_npc_entry, STRING("ロボット２"), STRING("c_mb_robo_2b"))
        CALL(evt_npc_set_anim, STRING("ロボット２"), STRING("MGN_Q_1"))
        CALL(evt_npc_set_position, STRING("ロボット２"), 0, -1000, 0)
        CALL_CHILD(evt_robot_waku)
        SPAWN_EVENT(evt_boss)
    END_IF
    SPAWN_EVENT(evt_emergency)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

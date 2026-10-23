#include "gon_12.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "party/party_motion.h"
#include "system.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(evt_chuchu_back_loop);

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door03"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 1, STRING("door03"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[2] = {
    {"e_bero", 1, 12, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "gon_08", "w_bero", 5, 5, door_close, door_open},
    {NULL},
};

static USER_FUNC(evt_chuchu_back_loop) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "チュチュリーナ");
    s32 value = evtGetValue(event, args[0]);
    OSTime now = gp->inBattle ? gp->renderTime : gp->renderFieldTime;
    s32 elapsed;

    if (isFirstCall) {
        event->userdata[0] = ((u64)now >> 32) & 0xFFFFFFFF;
        event->userdata[1] = now & 0xFFFFFFFF;
    }

    elapsed = (s32)((u64)((s32)now - event->userdata[1]) / (OS_TIMER_CLOCK / 1000));
    if (elapsed < value) {
        npc->rotation.z = intplGetValue(0xB, elapsed, value, 0.0f, 360.0f);
        return EVT_RETURN_BLOCK;
    }
    npc->rotation.z = 0.0f;
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(chuchurina_sogu)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 50)
        CALL(evt_npc_set_position, STRING("チュチュリーナ"), 0, -1000, 0)
        RETURN
    END_IF
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envoff, 512)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_set_pos, 200, 0, 0)
    CALL(evt_party_set_pos, 0, 230, 0, 15)
    CALL(evt_bero_mario_go_init)
    CALL(evt_seq_wait, 2)
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT(bero_play_enter_anime)
    CALL(evt_bero_mario_go_wait)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, 160, 15, FLOAT(150.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, 130, 0, FLOAT(150.0f))
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_party_stop, 0)
    SET_VALUE(GSW(0), 50)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("s_heya"), 2)
    CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 1073741856)
    CALL(evt_npc_flag_onoff, 0, STRING("チュチュリーナ"), 1024)
    WAIT_MS(750)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 96)
    CALL(evt_eff_fukidashi, 3, STRING(""), 0, 0, 0, 0, 0, 0, 0, 0, 96)
    WAIT_MS(800)
    CALL(evt_cam3d_evt_set, -78, 152, 528, -78, 57, -13, 1000, 11)
    WAIT_MS(1000)
    CALL(evt_npc_reverse_ry, STRING("チュチュリーナ"))
    WAIT_MS(500)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_CYUCYURINA1"))
    CALL(evt_msg_print, 0, STRING("stg1_gon_18_01"), 0, STRING("チュチュリーナ"))
    CALL(evt_cam3d_evt_set, 141, 91, 263, 141, 45, -2, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg1_gon_18_02"))
    CALL(evt_cam3d_evt_set, -110, 75, 225, -110, 35, 0, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg1_gon_18_04"), 0, STRING("チュチュリーナ"))
    CALL(evt_cam3d_evt_set, -3, 88, 170, 134, 35, -7, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg1_gon_18_05"))
    CALL(evt_npc_flag_onoff, 1, STRING("チュチュリーナ"), 32768)
    CALL(evt_cam3d_evt_set, 58, 13, 151, -138, 38, -31, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg1_gon_18_07"), 0, STRING("チュチュリーナ"))
    CALL(evt_cam3d_evt_set, 141, 91, 263, 141, 45, -2, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg1_gon_18_08"))
    CALL(evt_cam3d_evt_set, -110, 75, 225, -110, 35, 0, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg1_gon_18_10"), 0, STRING("チュチュリーナ"))
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_W_1"))
    CALL(evt_npc_move_position, STRING("チュチュリーナ"), 0, 0, 0, FLOAT(60.0f), 0)
    CALL(evt_cam3d_evt_set, 115, 91, 263, 115, 45, -2, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_npc_set_position, STRING("チュチュリーナ"), 0, 0, 0)
    WAIT_MS(200)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), -25)
    CALL(evt_npc_move_position, STRING("チュチュリーナ"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_S_1"))
    CALL(evt_msg_print_party, STRING("stg1_gon_18_11"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(0), -15)
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_W_1"))
    CALL(evt_npc_move_position, STRING("チュチュリーナ"), LW(0), LW(2), 0, FLOAT(60.0f), 0)
    CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    CALL(evt_eff, 0, STRING("kiss"), 0, LW(0), LW(1), LW(2), -45, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHURINA_KISS1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHURINA_KISS2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_K_1"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_O"))
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_S_1"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_R_1"))
    CALL(evt_npc_move_position, STRING("チュチュリーナ"), 85, 0, 0, FLOAT(80.0f), 0)
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_S_1"))
    CALL(evt_msg_print, 0, STRING("stg1_gon_18_13"), 0, STRING("チュチュリーナ"))
    CALL(evt_mario_set_pose, STRING("M_Z_1"))
    CALL(evt_cam3d_evt_set, 217, 14, 228, 101, 52, -35, 400, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_rotate_offset, STRING("チュチュリーナ"), 0, 15, 0)
        CALL(evt_chuchu_back_loop, 450)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHURINA_JUMP1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("チュチュリーナ"), 0, 67, -125, 500, 30)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg1_gon_18_14"))
    CALL(evt_cam3d_evt_set, 91, 66, 99, 43, 82, -10, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print, 0, STRING("stg1_gon_18_16"), 0, STRING("チュチュリーナ"))
    CALL(evt_cam3d_evt_set, 179, 36, 299, 43, 82, -10, 300, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_rotate_offset, STRING("チュチュリーナ"), 0, 0, 0)
    CALL(evt_npc_set_rotate, STRING("チュチュリーナ"), 0, 0, 355)
    WAIT_MS(50)
    CALL(evt_npc_set_anim, STRING("チュチュリーナ"), STRING("PCH_W_1"))
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("チュチュリーナ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHURINA_ESCAPE1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("チュチュリーナ"), 35, 0, -145, 300, 20)
    WAIT_MS(700)
    CALL(evt_cam3d_evt_set, 141, 91, 263, 141, 45, -2, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_msg_print_party, STRING("stg1_gon_18_17"))
    CALL(evt_mario_set_dir, 90, 200, 0)
    CALL(evt_msg_continue)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_GON1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON1"))
    SPAWN_EVENT(bero_case_entry)
    CALL(evt_cam3d_evt_off, 500, 4)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    CALL(evt_mario_get_party, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        SET_VALUE(GSWF(1511), 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(chuchurina_talk)
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[2] = {
    {"チュチュリーナ", 0, 0, npc_init_evt, NULL, chuchurina_talk},
    {NULL},
};

static EVT_BEGIN(item_get00)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box00"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item00"), 15, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item00"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box00"))
    CALL(evt_item_get_item, STRING("item00"))
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSW(0), 51)
    RETURN
EVT_END()

EVT_BEGIN(item_get01)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box01"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item01"), 148, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item01"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box01"))
    CALL(evt_item_get_item, STRING("item01"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(item_get02)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mobj_get_position, STRING("mobj_box02"), LW(0), LW(1), LW(2))
    CALL(evt_item_entry, STRING("item02"), 154, LW(0), LW(1), LW(2), 16, -1, 0)
    CALL(evt_item_flag_on, STRING("item02"), 2)
    CALL(evt_mobj_wait_animation_end, STRING("mobj_box02"))
    CALL(evt_item_get_item, STRING("item02"))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(gon_12_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 50)
        CALL_CHILD(evt_bero_info_run)
    END_IF
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_GON1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_GON1"))
    CALL(evt_snd_set_rev_mode, 2)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_npc_change_fbat_mode, 1)
    SPAWN_EVENT(chuchurina_sogu)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 51)
        SET_VALUE(GSWF(1507), 1)
    END_IF
    CALL(evt_mobj_itembox, STRING("mobj_box00"), -117, 0, -75, 0, 0, PTR(item_get00), GSWF(1507))
    CALL(evt_mobj_itembox, STRING("mobj_box01"), -56, 0, -25, 0, 0, PTR(item_get01), GSWF(1508))
    CALL(evt_mobj_itembox, STRING("mobj_box02"), -175, 0, 50, 0, 0, PTR(item_get02), GSWF(1509))
    CALL(evt_mobj_itembox, STRING("mobj_box03"), -150, 0, 20, 0, 0, 0, 1)
    CALL(evt_mobj_powerupblk, STRING("pblk00"), -150, 60, -25, 0, GSWF(5540), 0)
    CALL(evt_map_playanim, STRING("hokori"), 1, 0)
    RETURN
EVT_END()
// clang-format on

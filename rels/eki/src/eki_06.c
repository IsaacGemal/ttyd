#include "eki_06.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on

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

static EVT_BEGIN(e_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door"), 0, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(e_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.44434f))
    CALL(evt_mapobj_rotate, 0, STRING("door"), 0, LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_data[3] = {
    {"e_bero_1", 1, 3, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_05", "w_bero_2", 5, 5, e_close, e_open},
    {"e_bero_2", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "eki_05", "w_bero_1", 1, 1},
    {NULL},
};

// TODO: write sw_entry (global, 0x160 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_06.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(evt_box_open)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 317)
        RETURN
    END_IF
    LABEL(0)
    CALL(evt_npc_get_ReactionOfLivingBody, 0, LW(0))
    IF_INT_GREATER(LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(0)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_cam3d_evt_set, -140, 152, 528, -140, 57, -12, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GW(0), 0)
    SET_VALUE(GW(1), 0)
    SET_VALUE(GW(2), 0)
    CALL(sw_entry, STRING("MOBJ_eki_06_su3"), -240, 0, 40)
    CALL(sw_entry, STRING("MOBJ_eki_06_su2"), -140, 0, 40)
    CALL(sw_entry, STRING("MOBJ_eki_06_su1"), -40, 0, 40)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_BOX_OPEN1"), -240, 0, 40, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_BOX_OPEN1"), -140, 0, 40, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_BOX_OPEN1"), -40, 0, 40, 0)
    CALL(evt_map_playanim, STRING("anm_hako"), 0, 0)
    CALL(evt_map_checkanim, STRING("anm_hako"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_hitobj_onoff, STRING("a_hako"), 1, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 317)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_sw_complete)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, -140, 152, 528, -140, 57, -12, 500, 11)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_mobj_set_anim, STRING("MOBJ_eki_06_su1"), STRING("ANIM_6"))
        CALL(evt_mobj_get_position, STRING("MOBJ_eki_06_su1"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_SMALL1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(600)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_JUMP1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_mobj_set_anim, STRING("MOBJ_eki_06_su2"), STRING("ANIM_8"))
        CALL(evt_mobj_get_position, STRING("MOBJ_eki_06_su2"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_SMALL1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(600)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_JUMP1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(800)
        CALL(evt_mobj_set_anim, STRING("MOBJ_eki_06_su3"), STRING("ANIM_8"))
        CALL(evt_mobj_get_position, STRING("MOBJ_eki_06_su3"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_SMALL1"), LW(0), LW(1), LW(2), 0)
        WAIT_MS(600)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_OBJ_SWITCH_JUMP1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_mobj_wait_animation_end, STRING("MOBJ_eki_06_su1"))
    CALL(evt_mobj_wait_animation_end, STRING("MOBJ_eki_06_su2"))
    CALL(evt_mobj_wait_animation_end, STRING("MOBJ_eki_06_su3"))
    CALL(evt_mobj_delete, STRING("MOBJ_eki_06_su1"))
    CALL(evt_mobj_delete, STRING("MOBJ_eki_06_su2"))
    CALL(evt_mobj_delete, STRING("MOBJ_eki_06_su3"))
    CALL(evt_cam3d_evt_set, -80, 202, 448, -80, 107, -92, 500, 11)
    WAIT_MS(500)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("origami"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_origami"), 1)
    CALL(evt_hitobj_onoff, STRING("a_ori"), 1, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, STRING("origami"), LW(0), LW(1), LW(2))
        LOOP(3)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_PERA_KAI_OPEN1"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(300)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_PERA_KAI_OPEN1_2"), LW(0), LW(1), LW(2), 0)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS(LW(2), -60)
            CALL(evt_mario_mov_pos2, LW(0), -25, FLOAT(80.0f))
        END_IF
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("anm_kaidan167"), 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(900)
        CALL(evt_mapobj_get_position, STRING("origami"), LW(0), LW(1), LW(2))
        LOOP(6)
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG6_PERA_KAI_OPEN2"), LW(0), LW(1), LW(2), 0)
            WAIT_MS(300)
        END_LOOP
    END_INLINE_EVENT
    WAIT_FRAMES(100)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_1"), 1)
    WAIT_FRAMES(32)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_2"), 1)
    WAIT_FRAMES(34)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_3"), 1)
    CALL(evt_map_checkanim, STRING("anm_kaidan167"), LW(0), LW(1))
    WAIT_MS(LW(1))
    CALL(evt_snd_bgmon, 512, STRING("BGM_FF_ENV_SUCCESS1"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 318)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(eki_06_init_evt)
    SET_VALUE(LW(0), PTR(bero_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_envon, 272, STRING("ENV_STG6_EKI2"))
    CALL(evt_npc_change_fbat_mode, 1)
    SPAWN_EVENT(evt_box_open)
    SWITCH(GSW(0))
        CASE_LESS(317)
        CALL(evt_hitobj_onoff, STRING("a_ori"), 1, 0)
        CASE_EQUAL(317)
        SET_VALUE(GW(0), 0)
        SET_VALUE(GW(1), 0)
        SET_VALUE(GW(2), 0)
        CALL(sw_entry, STRING("MOBJ_eki_06_su3"), -240, 0, 40)
        CALL(sw_entry, STRING("MOBJ_eki_06_su2"), -140, 0, 40)
        CALL(sw_entry, STRING("MOBJ_eki_06_su1"), -40, 0, 40)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("origami"), 1)
        CALL(evt_hitobj_onoff, STRING("a_ori"), 1, 0)
        CASE_DEFAULT
        CALL(evt_map_playanim, STRING("anm_kaidan167"), 2, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n_origami"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_2"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("oku_3"), 1)
    END_SWITCH
    IF_INT_LESS(GSW(0), 317)
    ELSE
        CALL(evt_map_playanim, STRING("anm_hako"), 2, 0)
        CALL(evt_hitobj_onoff, STRING("a_hako"), 1, 0)
    END_IF
    CALL(evt_mapobj_trans, 1, STRING("HAKO"), 0, FLOAT(0.1f), 0)
    CALL(evt_mobj_powerupblk, STRING("pblk01"), 95, 295, -80, 0, GSWF(5562), 0)
    RETURN
EVT_END()

// TODO: type
u32 lock[1] = {
    0x00000000,
};

// TODO: write sw_main (global, 0x1004 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_06.s; this is where the original has it, just before the first script that uses it

// TODO: write sw_reset (global, 0x2ac bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_06.s; this is where the original has it, just before the first script that uses it

// TODO: write sw_cnt (global, 0xb0 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_06.s; this is where the original has it, just before the first script that uses it

// TODO: write sw_cntup (global, 0xf0 bytes) here:
// its asm is in build/G8MJ01/eki/asm/eki_06.s; this is where the original has it, just before the first script that uses it
// clang-format on

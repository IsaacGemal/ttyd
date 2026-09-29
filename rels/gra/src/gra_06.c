#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_telop.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 evt_kagemario_init_11_text_804[];
extern s32 evt_usu_kagemario_party_kill_11_data_4F98[];

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

EVT_BEGIN(ne_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(-0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("door_L"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("door_R"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(ne_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.389f))
    CALL(evt_mapobj_rotate, 0, STRING("door_L"), 0, LW(0), 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_rotate, 0, STRING("door_R"), 0, LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(gra_06_ido)
    SET_VALUE(GSWF(2079), 1)
    CALL(evt_bero_mapchange, STRING("jin_09"), 0)
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(gra_06_ido_in_sub)
    CALL(evt_seq_wait, 2)
    CALL(evt_mario_set_pos, 210, -100, 318)
    CALL(evt_party_set_pos, 0, 210, -100, 318)
    CALL(evt_party_set_pos, 1, 210, -100, 318)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_party_set_dir, 0, 270, -1)
    CALL(evt_party_set_dir, 1, 270, -1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_jump_pos, 165, 0, 374, 800, 0, -1000)
        CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_party_jump_pos, 0, 165, 0, 374, 800, 0, -1000)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(800)
        CALL(evt_party_jump_pos, 1, 165, 0, 374, 800, 0, -1000)
    END_INLINE_EVENT
    WAIT_MS(1500)
    RETURN
EVT_END()

EVT_BEGIN(gra_06_ido_in)
    SPAWN_EVENT(gra_06_ido_in_sub)
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

// TODO: type
static u32 bero_entry_data[60] = {
    (u32)"n_bero", 0x00010006, 0x00004E20, 0x000186A0,
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)"jin_00", (u32)"w_bero",
    0x00050005, (u32)&ne_close, (u32)&ne_open, (u32)"sw_bero",
    0x00000000, 0x00004E20, 0x000186A0, 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)"gra_05", (u32)"e_bero", 0x00010001,
    0x00000000, 0x00000000, (u32)"ana", 0x0F000000,
    0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
    0xFFFFFFFF, (u32)&gra_06_ido_in, 0x00000000, (u32)&gra_06_ido,
    (u32)"jin_09", 0x00000000, 0x00040004, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

EVT_BEGIN(gra_06_saku_open)
    CALL(evt_mapobj_get_position, STRING("kusari"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_GATE_CLOSED1"), LW(10), LW(11), LW(12), 0)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    LOOP(2)
        LOOP(5)
            ADD(LW(0), 1)
            ADD(LW(1), 1)
            ADD(LW(2), -2)
            CALL(evt_mapobj_rotate, 0, STRING("mon_L"), 0, LW(0), 0)
            CALL(evt_mapobj_rotate, 0, STRING("mon_R"), 0, LW(1), 0)
            CALL(evt_mapobj_rotate, 0, STRING("kusari"), 0, LW(2), 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(10)
            ADD(LW(0), -1)
            ADD(LW(1), -1)
            ADD(LW(2), 2)
            CALL(evt_mapobj_rotate, 0, STRING("mon_L"), 0, LW(0), 0)
            CALL(evt_mapobj_rotate, 0, STRING("mon_R"), 0, LW(1), 0)
            CALL(evt_mapobj_rotate, 0, STRING("kusari"), 0, LW(2), 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(5)
            ADD(LW(0), 1)
            ADD(LW(1), 1)
            ADD(LW(2), -2)
            CALL(evt_mapobj_rotate, 0, STRING("mon_L"), 0, LW(0), 0)
            CALL(evt_mapobj_rotate, 0, STRING("mon_R"), 0, LW(1), 0)
            CALL(evt_mapobj_rotate, 0, STRING("kusari"), 0, LW(2), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_GREATER(LW(2), 160)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_mario_key_onoff, 0)
            CALL(evt_msg_print_party, STRING("stg4_gra_20"))
            CALL(evt_mario_key_onoff, 1)
        END_IF
        RETURN
    END_IF
    CALL(evt_mapobj_get_position, STRING("kusari"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_GATE_OPEN1"), LW(0), LW(1), LW(2), 0)
    BEGIN_INLINE_EVENT
        CALL(evt_hitobj_onoff, STRING("A_Mon"), 1, 1)
        CALL(evt_hit_bind_mapobj, STRING("A_mon_L"), STRING("mon_L"))
        CALL(evt_hit_bind_mapobj, STRING("A_mon_R"), STRING("mon_R"))
        CALL(evt_sub_intpl_msec_init, 11, 0, 900, 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
            CALL(evt_mapobj_rotate, 0, STRING("mon_L"), 0, LW(0), 0)
            MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
            CALL(evt_mapobj_rotate, 0, STRING("mon_R"), 0, LW(0), 0)
            CALL(evt_hit_bind_update, STRING("A_mon_L"))
            CALL(evt_hit_bind_update, STRING("A_mon_R"))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(15)
        CALL(evt_mapobj_get_position, STRING("kusari"), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG4_GATE_LOCK_FALL1"), LW(10), 0, LW(12), 0)
    END_INLINE_EVENT
    SET_FLOAT_VALUE(LW(2), 0)
    SET_FLOAT_VALUE(LW(3), FLOAT(-1.0f))
    LOOP(30)
        ADD_FLOAT(LW(2), FLOAT(1.5f))
        MULTIPLY_FLOAT(LW(3), FLOAT(1.3f))
        CALL(evt_mapobj_trans, 1, STRING("kusari"), 0, LW(3), LW(2))
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(100)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kusari"), 1)
    SET_VALUE(GSWF(2078), 1)
    CALL(evt_hitobj_onoff, STRING("a_saku01"), 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(first_gra06jin)
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_GRA1"))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam3d_evt_set, -84, 562, 619, -431, 402, 73, 0, 11)
    CALL(evt_mario_set_pos, -320, 0, 365)
    CALL(evt_party_set_pos, 0, -350, 0, 365)
    CALL(evt_seq_wait, 2)
    CALL(evt_mario_normalize)
    BEGIN_INLINE_EVENT
        WAIT_MS(1500)
        CALL(evt_telop_entry, 8)
    END_INLINE_EVENT
    CALL(evt_cam3d_evt_set, 305, 562, 372, -42, 402, -174, 5000, 0)
    WAIT_MS(6000)
    CALL(evt_cam3d_evt_set, 1050, 219, 1169, 88, 250, 153, 2000, 0)
    CALL(evt_snd_bgmoff_f, 512, 3000)
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -110, 350, FLOAT(100.0f))
    END_INLINE_EVENT
    CALL(evt_mario_mov_pos2, -65, 310, FLOAT(100.0f))
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 120, 92, 530, -83, 40, 315, 0, 0)
    CALL(evt_mario_mov_pos2, -67, 312, FLOAT(100.0f))
    WAIT_FRAMES(1)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG4_GRA1"))
    CALL(evt_party_stop, 0)
    CALL(evt_party_set_dir, 0, 90, 200)
    WAIT_MS(300)
    CALL(evt_msg_print_party, STRING("stg4_gra_19"))
    CALL(evt_party_run, 0)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 193)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(teresa_oidasare_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_mario_set_dir, 270, 0, 0)
    CALL(evt_mario_adjust_dir)
    WAIT_FRAMES(1)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 65, 360, -115)
    CALL(evt_party_set_pos, 0, 72, 370, -155)
    CALL(evt_cam3d_event_from_road, 65, 60, -115, 0, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
    CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 0, 11)
    CALL(evt_seq_wait, 2)
    CALL(evt_mario_set_pose, STRING("M_D_1"))
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_DEFAULT
        BEGIN_INLINE_EVENT
            WAIT_MS(300)
            CALL(evt_sub_intpl_msec_init, 2, 370, 70, 800)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(evt_party_set_pos, 0, 72, LW(0), -155)
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
            CALL(evt_mario_get_party, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(1)
                CALL(evt_party_set_pose, 0, STRING("PKR_D_1"))
                CASE_EQUAL(2)
                CALL(evt_party_set_pose, 0, STRING("PNK_D_1"))
                CASE_EQUAL(4)
                CALL(evt_party_set_pose, 0, STRING("PYS_D_1"))
                CASE_EQUAL(5)
                CALL(evt_party_set_pose, 0, STRING("PWD_D_1"))
            END_SWITCH
            CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 50)
        END_INLINE_EVENT
    END_SWITCH
    CALL(evt_sub_intpl_msec_init, 2, 360, 60, 800)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_mario_set_pos, 65, LW(0), -115)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_LAND_DAMAGE2"), LW(0), LW(1), LW(2), 0)
    CALL(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos)
    CALL(evt_mario_set_pose, STRING("M_D_2"))
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 50)
    WAIT_MS(700)
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_DEFAULT
        BEGIN_INLINE_EVENT
            CALL(evt_party_get_pos, 0, LW(7), LW(8), LW(9))
            CALL(evt_party_jump_pos, 0, LW(7), LW(8), LW(9), 300, 0, 10)
        END_INLINE_EVENT
    END_SWITCH
    CALL(evt_mario_set_pose, STRING("M_U_3"))
    WAIT_MS(300)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_mario_get_party, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_party_set_pose, 0, STRING("PKR_S_1"))
        CASE_EQUAL(2)
        CALL(evt_party_set_pose, 0, STRING("PNK_S_1"))
        CASE_EQUAL(4)
        CALL(evt_party_set_pose, 0, STRING("PYS_S_1"))
        CASE_EQUAL(5)
        CALL(evt_party_set_pose, 0, STRING("PWD_S_1"))
    END_SWITCH
    CALL(evt_cam3d_evt_off, 0, 11)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    SET_VALUE(GSWF(2227), 0)
    SET_VALUE(LW(0), STRING("n_bero"))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

EVT_BEGIN(gra_06_init_evt)
    CALL(evt_item_entry, STRING("item00"), 125, -130, 0, 150, 16, GSWF(5633), 0)
    CALL(evt_kagemario_init_11_text_804)
    IF_INT_EQUAL(GSWF(2227), 1)
        SET_VALUE(LW(0), PTR(bero_entry_data))
        CALL(evt_bero_get_info)
        SPAWN_EVENT(bero_case_entry)
        SET_VALUE(LW(0), STRING("n_bero"))
        CALL_CHILD(bero_case_switch_off)
        SPAWN_EVENT(teresa_oidasare_evt)
    ELSE
        SET_VALUE(LW(0), PTR(bero_entry_data))
        CALL(evt_bero_get_info)
        IF_INT_LESS(GSW(0), 193)
            CALL_CHILD(bero_case_entry)
        ELSE
            CALL_CHILD(evt_bero_info_run)
        END_IF
    END_IF
    CALL(evt_mobj_save_blk, STRING("mobj_save"), -125, 60, 235, 0, 0)
    CALL(evt_mobj_recovery_blk, STRING("mobj_recover"), 10, -270, 60, 270, 0, 0)
    CALL(evt_mobj_brick, STRING("mobj_hidden1"), 67, 90, -13, 121, 11, 0, GSWF(2090))
    CALL_CHILD(evt_usu_kagemario_party_kill_11_data_4F98)
    IF_INT_EQUAL(GSWF(2078), 0)
        CALL(evt_hitobj_onoff, STRING("A_Mon"), 1, 0)
        CALL(evt_run_case_evt, 9, 1, STRING("a_saku01"), 0, PTR(gra_06_saku_open), 0)
    ELSE
        CALL(evt_hit_bind_mapobj, STRING("A_mon_L"), STRING("mon_L"))
        CALL(evt_hit_bind_mapobj, STRING("A_mon_R"), STRING("mon_R"))
        CALL(evt_mapobj_rotate, 0, STRING("mon_L"), 0, 90, 0)
        CALL(evt_mapobj_rotate, 0, STRING("mon_R"), 0, -90, 0)
        CALL(evt_hit_bind_update, STRING("A_mon_L"))
        CALL(evt_hit_bind_update, STRING("A_mon_R"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("kusari"), 1)
        CALL(evt_hitobj_onoff, STRING("a_saku01"), 1, 0)
    END_IF
    IF_INT_LESS(GSW(0), 193)
        SPAWN_EVENT(first_gra06jin)
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG4_JIN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG4_GRA1"))
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

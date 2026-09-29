#include "gor.h"
#include "evt/evt_cmd.h"
#include "evt/evt_johoya.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_urouro.h"
#include "evt/evt_window.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

EVT_BEGIN(gor_villager_urouro)
    CALL(evt_npc_get_position, STRING("me"), LW(0), LW(1), LW(2))
    CALL(urouro_init_func, STRING("me"), LW(0), LW(2), FLOAT(100.0f), FLOAT(30.0f), 0)
    CALL(urouro_main_func, STRING("me"))
    RETURN
EVT_END()

// TODO: write unk_gor_00000104 (global, 0x34 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(inemuri_snd)
    CALL(unk_gor_00000104, -1)
    LABEL(0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SLEEP1"), LW(0), LW(1), LW(2), LW(3))
    CALL(unk_gor_00000104, LW(3))
    WAIT_FRAMES(140)
    GOTO(0)
    RETURN
EVT_END()

// TODO: write unk_gor_000000cc (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(mario_and_party_sleep)
    SET_VALUE(LW(1), 1200)
    SET_VALUE(LW(2), 900)
    SET_VALUE(LW(3), 600)
    SET_VALUE(LF(0), 0)
    SET_VALUE(LW(15), -1)
    LOOP(0)
        IF_INT_EQUAL(LW(1), 0)
            CALL(evt_mario_set_pose, STRING("M_N_5A"))
            SPAWN_EVENT_GET_ID(PTR(inemuri_snd), LW(15))
        END_IF
        IF_INT_EQUAL(LW(2), 0)
            CALL(evt_mario_get_party, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                SWITCH(LW(0))
                    CASE_EQUAL(1)
                    CALL(evt_party_set_pose, 0, STRING("PKR_N_1"))
                    CASE_EQUAL(2)
                    CALL(evt_party_set_pose, 0, STRING("PNK_N_1"))
                    CASE_EQUAL(3)
                    CALL(evt_party_set_pose, 0, STRING("PBM_N_1"))
                    CASE_EQUAL(4)
                    CALL(evt_party_set_pose, 0, STRING("PYS_N_1"))
                    CASE_EQUAL(5)
                    CALL(evt_party_set_pose, 0, STRING("PWD_N_1"))
                    CASE_EQUAL(6)
                    CALL(evt_party_set_pose, 0, STRING("PTR_N_1"))
                    CASE_EQUAL(7)
                    CALL(evt_party_set_pose, 0, STRING("PCH_N_1"))
                END_SWITCH
            END_IF
        END_IF
        IF_INT_EQUAL(LW(3), 0)
            IF_INT_EQUAL(LF(0), 0)
                SET_VALUE(LF(0), 1)
                SET_VALUE(LW(3), 51)
                CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
                CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_YAWN1"), LW(4), LW(5), LW(6), 0)
                CALL(evt_mario_set_pose, STRING("M_I_5"))
            ELSE
                SET_VALUE(LF(0), 0)
                CALL(evt_mario_set_pose, STRING("M_S_1"))
            END_IF
        END_IF
        WAIT_FRAMES(1)
        IF_INT_NOT_EQUAL(GF(0), 0)
            BREAK_LOOP
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(1), 0)
            SUBTRACT(LW(1), 1)
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(2), 0)
            SUBTRACT(LW(2), 1)
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(3), 0)
            SUBTRACT(LW(3), 1)
        END_IF
    END_LOOP
    IF_INT_NOT_EQUAL(LW(15), -1)
        DELETE_EVENT(LW(15))
        CALL(unk_gor_000000cc)
    END_IF
    IF_INT_LESS_OR_EQUAL(LW(2), 0)
        CALL(evt_mario_get_party, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SWITCH(LW(0))
                CASE_EQUAL(1)
                CALL(evt_party_set_pose, 0, STRING("PKR_S_1"))
                CASE_EQUAL(2)
                CALL(evt_party_set_pose, 0, STRING("PNK_S_1"))
                CASE_EQUAL(3)
                CALL(evt_party_set_pose, 0, STRING("PBM_S_1"))
                CASE_EQUAL(4)
                CALL(evt_party_set_pose, 0, STRING("PYS_S_1"))
                CASE_EQUAL(5)
                CALL(evt_party_set_pose, 0, STRING("PWD_S_1"))
                CASE_EQUAL(6)
                CALL(evt_party_set_pose, 0, STRING("PTR_S_1"))
                CASE_EQUAL(7)
                CALL(evt_party_set_pose, 0, STRING("PCH_S_1"))
            END_SWITCH
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        IF_INT_NOT_EQUAL(LW(3), 0)
            SUBTRACT(LW(3), 1)
        END_IF
        WAIT_FRAMES(LW(3))
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_IF
    IF_INT_LESS_OR_EQUAL(LW(1), 0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_AWAKE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_mario_set_pose, STRING("M_N_5B"))
        WAIT_MS(550)
        CALL(evt_mario_set_pose, STRING("M_S_1"))
    END_IF
    SET_VALUE(GF(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(luigi_gor_first_talk)
    CALL(evt_party_stop, 0)
    IF_INT_EQUAL(GSWF(1175), 0)
        CALL(evt_msg_print, 0, STRING("gor_luigi_00"), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_I_2"))
        WAIT_MS(500)
        IF_INT_LESS(GSW(0), 401)
            SET_VALUE(LW(2), STRING("gor_luigi_01"))
        ELSE
            SET_VALUE(LW(2), STRING("gor_luigi_01_1"))
        END_IF
        CALL(evt_msg_print, 0, LW(2), 0, STRING("me"))
        CALL(evt_mario_set_pose, STRING("M_N_5B"))
        WAIT_MS(500)
        SET_VALUE(GSWF(1175), 1)
    END_IF
    SWITCH(GSW(0))
        CASE_BETWEEN(56, 111)
        SET_VALUE(LW(2), STRING("gor_luigi_02"))
        CASE_BETWEEN(112, 163)
        SET_VALUE(LW(2), STRING("gor_luigi_03"))
        CASE_BETWEEN(164, 214)
        SET_VALUE(LW(2), STRING("gor_luigi_04"))
        CASE_BETWEEN(215, 260)
        SET_VALUE(LW(2), STRING("gor_luigi_05"))
        CASE_BETWEEN(261, 338)
        SET_VALUE(LW(2), STRING("gor_luigi_06"))
        CASE_BETWEEN(339, 372)
        SET_VALUE(LW(2), STRING("gor_luigi_07"))
        CASE_BETWEEN(373, 400)
        SET_VALUE(LW(2), STRING("gor_luigi_08"))
        CASE_GREATER_OR_EQUAL(401)
        SET_VALUE(LW(2), STRING("gor_luigi_09"))
    END_SWITCH
    CALL(evt_msg_print, 0, LW(2), 0, STRING("me"))
    CALL(evt_msg_select, 0, STRING("gor_luigi_yesno"))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("gor_luigi_12"))
        GOTO(1)
    END_IF
    CALL(evt_msg_print_add, 0, STRING("gor_luigi_11"))
    CALL(johoya_data_alloc, STRING("gor_luigi_talk"))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    CALL(evt_win_other_select, 18)
    IF_INT_EQUAL(LW(0), 0)
        CALL(johoya_data_free)
        CALL(evt_msg_print, 0, STRING("gor_luigi_13"), 0, STRING("me"))
        GOTO(1)
    END_IF
    IF_INT_EQUAL(LW(10), 0)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon_f, 257, STRING("BGM_EVT_RUIJI1"), 500)
        CALL(evt_snd_envoff, 1024)
        SET_VALUE(LW(10), 1)
    END_IF
    SET_VALUE(GF(0), 0)
    SPAWN_EVENT(mario_and_party_sleep)
    CALL(evt_msg_print, 0, LW(1), 0, STRING("me"))
    SET_VALUE(GF(0), 1)
    WAIT_WHILE(GF(0))
    CALL(johoya_luigi_newjoho_setreadflag)
    GOTO(0)
    LABEL(1)
    IF_INT_EQUAL(LW(10), 1)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        CALL(evt_snd_envoff, 513)
        CALL(evt_snd_envon, 288, 0)
    END_IF
    CALL(evt_party_run, 0)
    RETURN
EVT_END()

// TODO: write evt_goki_sub_intpl_get_value (global, 0xf4 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_goki_sub_intpl_init (global, 0xa8 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_gor_villager_urouro_check_func (global, 0x148 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_gor_0000041c (global, 0xf0 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_gor_0000050c (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x70 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0x108 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor.s; this is where the original has it, just before the first script that uses it
// clang-format on

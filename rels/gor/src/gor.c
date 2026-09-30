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
#include "data/mapdata.h"
#include "driver/msgdrv.h"
#include "gor_00.h"
#include "gor_01.h"
#include "gor_02.h"
#include "gor_03.h"
#include "gor_04.h"
#include "gor_10.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

const Vec vec3_gor_00012450 = {0.0f, 0.0f, 0.0f};
const Vec vec3_gor_0001245c = {0.0f, 0.0f, 0.0f};

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

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("gor_00", gor_00_init_evt);
    relSetEvtAddr("gor_01", gor_01_init_evt);
    relSetEvtAddr("gor_02", gor_02_init_evt);
    relSetEvtAddr("gor_03", gor_03_init_evt);
    relSetEvtAddr("gor_04", gor_04_init_evt);
    relSetEvtAddr("gor_10", gor_10_init_evt);
    relSetEvtAddr("gor_11", gor_11_init_evt);
    relSetEvtAddr("gor_12", gor_12_init_evt);
    relSetBtlAddr("gor", btlsetup_gor_tbl, btl_gor_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    if (strcmp(gp->currentMapName, "gor_10") == 0) {
        tegami_delete();
    }
    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}

s32 unk_gor_0000050c(EventEntry* event, BOOL isFirstCall) {
    if (unk_gor_bss00000000[0] == NULL) {
        return EVT_RETURN_DONE;
    }
    _mapFree(unk_gor_bss00000000[0]);
    unk_gor_bss00000000[0] = NULL;
    return EVT_RETURN_DONE;
}

s32 unk_gor_0000041c(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    char* v0 = (char*)evtGetValue(event, args[0]);
    const char* msg = msgSearch((const char*)evtGetValue(event, args[1]));
    char* v2 = (char*)evtGetValue(event, args[2]);
    s32 len = strlen(v0) + strlen(msg) + strlen(v2) + 1;
    void* buf = _mapAlloc(len);

    unk_gor_bss00000000[0] = buf;
    memset(buf, 0, len);
    sprintf((char*)unk_gor_bss00000000[0], "%s%s%s", v0, msg, v2);
    evtSetValue(event, args[3], (s32)unk_gor_bss00000000[0]);
    return EVT_RETURN_DONE;
}

s32 evt_gor_villager_urouro_check_func(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 idx0 = args[0];
    s32 idx1 = args[1];
    f32 f29 = evtGetFloat(event, args[2]);
    f32 f28 = evtGetFloat(event, args[3]);
    f32 f30 = evtGetFloat(event, args[4]);
    f32 f31 = evtGetFloat(event, idx0);
    f32 f1 = evtGetFloat(event, idx1);
    Vec pos1;
    Vec pos2;
    Vec tmp1;
    Vec tmp2;

    tmp1 = vec3_gor_00012450;
    tmp1.x = f31;
    tmp1.z = f1;
    pos1 = tmp1;
    tmp2 = vec3_gor_0001245c;
    tmp2.x = f29;
    tmp2.z = f28;
    pos2 = tmp2;
    if (PSVECDistance(&pos2, &pos1) <= f30) {
        return EVT_RETURN_DONE;
    }
    evtSetFloat(event, idx0, f29);
    evtSetFloat(event, idx1, f28);
    return EVT_RETURN_DONE;
}

s32 evt_goki_sub_intpl_init(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 v0 = evtGetValue(event, args[0]);
    s32 v1 = evtGetValue(event, args[1]);
    s32 v2 = evtGetValue(event, args[2]);
    s32 v3 = evtGetValue(event, args[3]);
    s32 v4 = evtGetValue(event, args[4]);
    s32 v5 = evtGetValue(event, args[5]);

    event->lwData[9] = v0;
    event->lwData[10] = v1;
    event->lwData[11] = v2;
    event->lwData[12] = v3;
    event->lwData[13] = v4;
    event->lwData[14] = 0;
    event->lwData[15] = v5;
    return EVT_RETURN_DONE;
}

s32 evt_goki_sub_intpl_get_value(EventEntry* event, BOOL isFirstCall) {
    event->lwData[0] = (s32)intplGetValue(event->lwData[9], event->lwData[14], event->lwData[15],
                                         (f32)event->lwData[10], (f32)event->lwData[11]);
    event->lwData[1] = (s32)intplGetValue(event->lwData[9], event->lwData[14], event->lwData[15],
                                         (f32)event->lwData[12], (f32)event->lwData[13]);
    event->lwData[14]++;
    return EVT_RETURN_DONE;
}

s32 unk_gor_00000104(EventEntry* event, BOOL isFirstCall) {
    unk_gor_bss00000004[0] = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

s32 unk_gor_000000cc(EventEntry* event, BOOL isFirstCall) {
    if (unk_gor_bss00000004[0] != -1) {
        psndSFXOff(unk_gor_bss00000004[0]);
    }
    return EVT_RETURN_DONE;
}
// clang-format on

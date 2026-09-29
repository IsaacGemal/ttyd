#include "aji_08.h"
#include "driver/camdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "party/party_motion.h"

#pragma pool_data off

#pragma force_active on
extern s32 evt_emergency[];

// clang-format off
extend_data(32,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(33,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(s_door_open)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.16602f))
    CALL(evt_mapobj_trans, 1, STRING("S_door1_2"), LW(0), 0, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("S_door1_1"), LW(0), 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(s_door_close)
    MULTIPLY_FLOAT(LW(0), FLOAT(0.16602f))
    CALL(evt_mapobj_trans, 1, STRING("S_door1_2"), LW(0), 0, 0)
    MULTIPLY_FLOAT(LW(0), FLOAT(-1.0f))
    CALL(evt_mapobj_trans, 1, STRING("S_door1_1"), LW(0), 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_slit0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_bero_mapchange, STRING("aji_06"), STRING("tenjo"))
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_slit1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_bero_mapchange, STRING("aji_07"), STRING("tenjo"))
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_slit2)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_bero_mapchange, STRING("aji_09"), STRING("tenjo"))
    SET_VALUE(LW(0), 1)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"s_bero_1", 0x0001, 0x000A, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "aji_01", "n_bero_3", 5, 5, s_door_close, s_door_open},
    {"s_bero_2", 0x0F00, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, 0, evt_slit0, "aji_06", "tenjo", 1, 1},
    {"s_bero_3", 0x0F00, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, 0, evt_slit1, "aji_07", "tenjo", 1, 1},
    {"s_bero_4", 0x0F00, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, 0, evt_slit2, "aji_09", "tenjo", 1, 1},
    {NULL},
};

USER_FUNC(camAtY) {
    s32* args = event->args;
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    evtSetValue(event, args[0], (s32)cam->cameraAt.y);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(heya_tenura_onoff)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_heya"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_tenura"), 1)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(camAtY, LW(1))
        IF_INT_GREATER_OR_EQUAL(LW(1), 180)
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_heya"), 1)
                CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_tenura"), 1)
                SET_VALUE(LW(0), 1)
            END_IF
        ELSE
            IF_INT_EQUAL(LW(0), 1)
                CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_heya"), 1)
                CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_tenura"), 1)
                SET_VALUE(LW(0), 0)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

USER_FUNC(slit_chk) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        if (mario->position.x < -200.0f || mario->position.x > 300.0f) {
            if (mario->position.y < 169.0f && mario->position.y > 160.0f) {
                camCtrlOff(CAMERA_3D);
                evtStopID(event->lwData[0]);
                return EVT_RETURN_DONE;
            }
        }
    }
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(aji_08_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG7_DUN2"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG7_AJI2"))
    CALL(evt_snd_set_rev_mode, 1)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    SPAWN_EVENT_GET_ID(PTR(heya_tenura_onoff), LW(0))
    SPAWN_EVENT(evt_emergency)
    BEGIN_INLINE_EVENT
        CALL(slit_chk)
    END_INLINE_EVENT
    CALL(evt_map_playanim, STRING("bari"), 1, 0)
    CALL(evt_map_playanim, STRING("awa1"), 1, 0)
    CALL(evt_map_playanim, STRING("awa2"), 1, 0)
    CALL(evt_item_entry, STRING("item00"), 125, -975, 170, 75, 16, GSWF(5666), 0)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mapobj_get_position, STRING("S_ita"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_AMB_CEILING_MOVE1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_sub_intpl_msec_init, 11, -100, 100, 1000)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_ita"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(0)
            END_IF
            CALL(evt_mapobj_get_position, STRING("S_ita"), LW(0), LW(1), LW(2))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG7_AMB_CEILING_MOVE1"), LW(0), LW(1), LW(2), 0)
            CALL(evt_sub_intpl_msec_init, 11, 100, -100, 1000)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_ita"), 0, 0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(1)
            END_IF
        END_LOOP
    END_INLINE_EVENT
    IF_INT_GREATER_OR_EQUAL(GSW(0), 373)
        CALL(evt_npc_check_delete, STRING("軍団ザコ魔法使い"))
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

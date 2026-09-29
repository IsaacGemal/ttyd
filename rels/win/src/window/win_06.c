#include "win_06.h"
#include "driver/camdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_telop.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "party/party_motion.h"
#include <dolphin/mtx.h>
#include <string.h>

static CamKaiten* ckp0;
static CamKaiten* ckp1;
static CamKaiten* ckpw;

#pragma pool_data off

#pragma force_active on
static USER_FUNC(cam_kaiten_set_para);
static USER_FUNC(cam_kaiten_get_para);
static USER_FUNC(cam_kaiten_set_direct);
static USER_FUNC(cam_kaiten_init);

// clang-format off
extend_data(48,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(49,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static EVT_BEGIN(first_evt_dokan)
    IF_INT_NOT_EQUAL(GSW(0), 70)
        SET_VALUE(LW(0), 0)
        RETURN
    END_IF
    SET_VALUE(GF(0), 1)
    WAIT_WHILE(GF(0))
    SET_VALUE(LW(0), 0)
    RETURN
EVT_END()

static BeroEntry bero_entry_data[] = {
    {"dokan1", 2, 0, 9, 100000, 0, 0, -1, first_evt_dokan, 6, NULL, "tik_03", "dokan", 1, 1},
    {"e_bero", 0, 0, 0x4E20, 100000, 0, 0, -1, NULL, -1, NULL, "win_00", "w_bero", 1, 1},
    {NULL},
};

EVT_BEGIN(welcome_wonderforest_init)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    RETURN
EVT_END()

USER_FUNC(camps30) {
    camGetPtr(CAMERA_3D)->fovY = 30.0f;
    return EVT_RETURN_DONE;
}

USER_FUNC(camps25) {
    camGetPtr(CAMERA_3D)->fovY = 25.0f;
    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_init) {
    s32* args = event->args;
    Vec a;
    Vec p;
    f32 dx;
    f32 dz;
    f32 dist;

    ckp0 = _mapAlloc(0x20);
    memset(ckp0, 0, 0x20);
    ckp1 = _mapAlloc(0x20);
    memset(ckp1, 0, 0x20);
    ckpw = _mapAlloc(0x20);
    memset(ckp1, 0, 0x20);

    ckp0->pos.x = evtGetFloat(event, args[0]);
    ckp0->pos.y = evtGetFloat(event, args[1]);
    ckp0->pos.z = evtGetFloat(event, args[2]);
    ckp0->at.x = evtGetFloat(event, args[3]);
    ckp0->at.y = evtGetFloat(event, args[4]);
    ckp0->at.z = evtGetFloat(event, args[5]);
    ckp1->pos.x = evtGetFloat(event, args[6]);
    ckp1->pos.y = evtGetFloat(event, args[7]);
    ckp1->pos.z = evtGetFloat(event, args[8]);
    ckp1->at.x = evtGetFloat(event, args[9]);
    ckp1->at.y = evtGetFloat(event, args[10]);
    ckp1->at.z = evtGetFloat(event, args[11]);

    p = ckp0->pos;
    a = ckp0->at;
    a.y = 0.0f;
    p.y = 0.0f;
    dist = PSVECDistance(&p, &a);
    dx = a.x - p.x;
    dz = a.z - p.z;
    ckp0->angle = 57.29579f * (f32)atan2(-dx, dz);
    ckp0->dist = dist;

    p = ckp1->pos;
    a = ckp1->at;
    a.y = 0.0f;
    p.y = 0.0f;
    dist = PSVECDistance(&p, &a);
    dx = a.x - p.x;
    dz = a.z - p.z;
    ckp1->angle = 57.29579f * (f32)atan2(-dx, dz);
    ckp1->dist = dist;

    ckpw->pos = ckp0->pos;
    ckpw->at = ckp0->at;
    ckpw->angle = ckp0->angle;
    ckpw->dist = ckp0->dist;

    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_set_direct) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 s;
    f32 c;

    s = (f32)sin(6.2831855f * ckpw->angle / 360.0f);
    c = (f32)cos(6.2831855f * ckpw->angle / 360.0f);
    ckpw->pos.x = ckpw->at.x + ckpw->dist * s;
    ckpw->pos.z = ckpw->at.z - ckpw->dist * c;

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = ckpw->pos;
    cam->storedAt = ckpw->at;
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 11;
    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_get_para) {
    s32* args = event->args;
    CamKaiten* kp;

    switch (evtGetValue(event, args[0])) {
        case 0:
            kp = ckp0;
            break;
        case 1:
            kp = ckp1;
            break;
        case 2:
        default:
            kp = ckpw;
            break;
    }

    evtSetFloat(event, args[1], kp->pos.x);
    evtSetFloat(event, args[2], kp->pos.y);
    evtSetFloat(event, args[3], kp->pos.z);
    evtSetFloat(event, args[4], kp->at.x);
    evtSetFloat(event, args[5], kp->at.y);
    evtSetFloat(event, args[6], kp->at.z);
    evtSetFloat(event, args[7], kp->angle);
    evtSetFloat(event, args[8], kp->dist);
    return EVT_RETURN_DONE;
}

static USER_FUNC(cam_kaiten_set_para) {
    s32* args = event->args;
    CamKaiten* kp;

    switch (evtGetValue(event, args[0])) {
        case 0:
            kp = ckp0;
            break;
        case 1:
            kp = ckp1;
            break;
        case 2:
        default:
            kp = ckpw;
            break;
    }

    kp->pos.x = evtGetFloat(event, args[1]);
    kp->pos.y = evtGetFloat(event, args[2]);
    kp->pos.z = evtGetFloat(event, args[3]);
    kp->at.x = evtGetFloat(event, args[4]);
    kp->at.y = evtGetFloat(event, args[5]);
    kp->at.z = evtGetFloat(event, args[6]);
    kp->angle = evtGetFloat(event, args[7]);
    kp->dist = evtGetFloat(event, args[8]);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(welcome_wonderforest)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_snd_bgm_scope, 0, 1)
    BEGIN_INLINE_EVENT
        WAIT_MS(5000)
        CALL(evt_telop_entry, 4)
    END_INLINE_EVENT
    CALL(camps30)
    CALL(cam_kaiten_init, -350, 16, 276, 0, 75, 0, 255, 16, 366, 0, 75, 0)
    BEGIN_INLINE_EVENT_GET_ID(LW(7))
        LOOP(0)
            CALL(cam_kaiten_set_direct)
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(cam_kaiten_get_para, 0, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LW(0), LF(7))
        ADD_FLOAT(LW(0), FLOAT(360.0f))
        CALL(cam_kaiten_get_para, 1, LF(0), LF(1), LF(2), LF(3), LF(4), LF(5), LW(1), LF(7))
        MULTIPLY_FLOAT(LW(0), FLOAT(10.0f))
        MULTIPLY_FLOAT(LW(1), FLOAT(10.0f))
        CALL(evt_sub_intpl_msec_init, 11, LW(0), LW(1), 7500)
        LOOP(0)
            CALL(cam_kaiten_get_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            CALL(evt_sub_intpl_msec_get_value_para, LW(6), LW(9))
            DIVIDE_FLOAT(LW(6), FLOAT(10.0f))
            CALL(cam_kaiten_set_para, 2, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7))
            IF_INT_EQUAL(LW(9), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(8000)
    DELETE_EVENT(LW(7))
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 1)
    CALL(evt_party_cont_onoff, 1, 1)
    SET_VALUE(GF(0), 0)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    WAIT_MS(500)
    BEGIN_INLINE_EVENT
        CALL(evt_party_set_dir, 1, 90, 200)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_mov_pos2, 20, 50, 125)
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_move_pos2, 0, -20, 40, 125)
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(camps25)
    CALL(evt_cam3d_evt_set, 0, FLOAT(92.5f), FLOAT(336.7f), 0, FLOAT(32.5f), -8, 0, 0)
    WAIT_MS(1000)
    CALL(evt_msg_print_party, STRING("stg2_win_e01_00_00"))
    WAIT_MS(300)
    CALL(evt_party_get_pos, 1, LW(0), LW(1), LW(2))
    ADD(LW(0), 1)
    CALL(evt_party_move_pos2, 1, LW(0), 85, 125)
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg2_win_e00_00"), 0, STRING("extparty"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSW(0), 71)
    CALL(evt_snd_bgm_scope, 0, 0)
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(win_06_init_evt)
    IF_INT_LESS(GSW(0), 71)
        BEGIN_INLINE_EVENT
            CALL(evt_snd_bgmoff, 512)
            CALL(evt_snd_envon, 272, STRING("ENV_STG2_WIN1"))
            WAIT_MS(9000)
            CALL(evt_snd_bgmon_f, 768, STRING("BGM_STG2_WIN1"), 2000)
            CALL(evt_snd_bgm_scope, 0, 1)
        END_INLINE_EVENT
    ELSE
        CALL(evt_snd_bgmon, 512, STRING("BGM_STG2_WIN1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG2_WIN1"))
    END_IF
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_tree_mki"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_kusa"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("group22"), 2)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_oku_01H081"), 2)
    IF_INT_EQUAL(GSW(0), 70)
        CALL_CHILD(welcome_wonderforest_init)
        SPAWN_EVENT(welcome_wonderforest)
    END_IF
    CALL(evt_mobj_save_blk, STRING("mobj_save"), 275, 60, -50, 0, 0)
    RETURN
EVT_END()


// clang-format on

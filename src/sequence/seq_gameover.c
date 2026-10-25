#include "driver/seqdrv.h"
#include "driver/fadedrv.h"
#include "driver/camdrv.h"
#include "driver/mapdrv.h"
#include "driver/npcdrv.h"
#include "manager/evtmgr.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "sound.h"
#include "system.h"
#include "statuswindow.h"
#include "countdown.h"
#include "nameent.h"
#include "cam_road.h"
#include "cam_shift.h"
#include "seq/seq_mapchange.h"
#include "seq/seq_gameover.h"
#include <string.h>
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"

// object-local data
static EventEntry* work;
static EventEntry** wp = &work;

// clang-format off
// TODO: type
s32 fall_data[26] = {
    0x000000C8, 0x000000AA, 0x0000008C, 0x0000006E,
    0x00000050, 0x00000032, 0x00000014, 0x0000000A,
    0x0000001A, 0x0000001D, 0x0000001E, 0x0000001D,
    0x0000001A, 0x0000000A, 0x00000000, 0x0000000C,
    0x0000000F, 0x00000010, 0x0000000F, 0x0000000B,
    0x00000006, 0x00000000, 0x00000004, 0x00000003,
    0x00000001, 0x00000000,
};

// TODO: type
s32 fall_data2[19] = {
    0x000000C8, 0x000000AA, 0x0000008C, 0x0000006E,
    0x00000050, 0x00000032, 0x00000014, 0x00000000,
    0x0000000C, 0x0000000F, 0x00000010, 0x0000000F,
    0x0000000B, 0x00000006, 0x00000000, 0x00000004,
    0x00000003, 0x00000001, 0x00000000,
};

EVT_BEGIN(_fall_char)
    SET_INT_READ_ADDR(PTR(fall_data))
    LOOP(26)
        READ_INT(LW(1))
        CALL(evt_mapobj_trans, 1, LW(0), 0, LW(1), 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(_fall_char_last)
    SET_INT_READ_ADDR(PTR(fall_data2))
    LOOP(19)
        READ_INT(LW(1))
        CALL(evt_mapobj_trans, 1, LW(0), 0, LW(1), 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(fall_gameover)
    SET_VALUE(LW(0), STRING("g"))
    SPAWN_EVENT(_fall_char)
    WAIT_FRAMES(10)
    SET_VALUE(LW(0), STRING("a"))
    SPAWN_EVENT(_fall_char)
    WAIT_FRAMES(20)
    SET_VALUE(LW(0), STRING("m"))
    SPAWN_EVENT(_fall_char)
    WAIT_FRAMES(10)
    SET_VALUE(LW(0), STRING("e"))
    SPAWN_EVENT(_fall_char)
    WAIT_FRAMES(10)
    SET_VALUE(LW(0), STRING("o"))
    SPAWN_EVENT(_fall_char)
    WAIT_FRAMES(10)
    SET_VALUE(LW(0), STRING("v"))
    SPAWN_EVENT(_fall_char)
    WAIT_FRAMES(10)
    SET_VALUE(LW(0), STRING("e2"))
    SPAWN_EVENT(_fall_char)
    WAIT_FRAMES(30)
    SET_VALUE(LW(0), STRING("r"))
    SPAWN_EVENT_GET_ID(PTR(_fall_char_last), LW(10))
    LOOP(0)
        IS_EVENT_RUNNING(LW(10), LW(11))
        IF_INT_EQUAL(LW(11), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(60)
    RETURN
EVT_END()

EVT_BEGIN(evt_gameover)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, 0, 0, 0)
    CALL(evt_cam_road_reset)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_mario_kill_party, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_mario_clear_party, 1)
    WAIT_FRAMES(1)
    CALL(N_evt_snd_bgm_unpause)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_D_2"))
    CALL(evt_mario_set_dir, 180, 0, 0)
    CALL(evt_mario_adjust_dir)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("f"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("i"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("n"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("d"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("e3"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("l"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("j"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("u"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("e4"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("g1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("o289"), 1)
    CALL(evt_mapobj_trans, 1, STRING("g"), 0, 200, 0)
    CALL(evt_mapobj_trans, 1, STRING("a"), 0, 200, 0)
    CALL(evt_mapobj_trans, 1, STRING("m"), 0, 200, 0)
    CALL(evt_mapobj_trans, 1, STRING("e"), 0, 200, 0)
    CALL(evt_mapobj_trans, 1, STRING("o"), 0, 200, 0)
    CALL(evt_mapobj_trans, 1, STRING("v"), 0, 200, 0)
    CALL(evt_mapobj_trans, 1, STRING("e2"), 0, 200, 0)
    CALL(evt_mapobj_trans, 1, STRING("r"), 0, 200, 0)
    CALL(evt_cam_ctrl_onoff, 4, 1)
    CALL(evt_cam3d_evt_set, 0, 208, 653, 0, 91, -30, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_map_playanim, STRING("anm_hokori"), 1, 0)
    CALL_CHILD(fall_gameover)
    RETURN
EVT_END()
// clang-format on

void seq_gameOverMain(SequenceWork* work) {
    CameraEntry* cam;
    MarioWork* mario;

    switch (work->state) {
    case 0:
        if (work->mapName == NULL) {
            fadeEntry(2, 1000, (GXColor){ 0, 0, 0, 0xFF });
            gp->areaFadeInType = 10;
            gp->areaFadeInDuration = 300;
        }
        mapPreLoad("sys_00");
        work->state++;
        break;

    case 1:
        if (fadeIsFinish()) {
            _relUnLoad();
            _unload(gp->currentMapName, NULL, NULL);
            strcpy(gp->lastMapName, gp->currentMapName);
            statusWinForceOff();
            statusWinDispOff();
            cam = camGetPtr(CAMERA_3D);
            cam->viewport[0] = 0.0f;
            cam->viewport[1] = 0.0f;
            cam->viewport[2] = gp->fbWidth;
            cam->viewport[3] = gp->efbHeight;
            cam = camGetPtr(CAMERA_2D);
            cam->viewport[0] = 0.0f;
            cam->viewport[1] = 0.0f;
            cam->viewport[2] = gp->fbWidth;
            cam->viewport[3] = gp->efbHeight;
            camGetPtr(CAMERA_2D)->flags &= ~0x100;
            camGetPtr(CAMERA_2D)->flags |= 0x400;
            camGetPtr(CAMERA_2D)->flags |= 0x200;
            fadeTecOff();
            fadeSoftFocusOff();
            npcClearDeadInfo();
            countDownEnd();
            nameEntReInit();
            work->state++;
        }
        break;

    case 2:
        if (mapPreLoad("sys_00")) {
            mapLoad("sys_00");
            cam = camGetPtr(CAMERA_3D);
            mario = marioGetPtr();
            cam->marioPos.x = 0.0f;
            cam->marioPos.y = 0.01f;
            cam->marioPos.z = 0.0f;
            cam->marioAt.x = cam->marioPos.x;
            cam->marioAt.y = cam->marioPos.y;
            cam->marioAt.z = cam->marioPos.z;
            mario->position = (Vec){ 0.0f, 0.0f, 0.0f };
            mario->camFollowPos = mario->position;
            camRoadReset();
            camShiftReset();
            work->state++;
        }
        break;

    case 3:
        fadeEntry(0x1F, 200, (GXColor){ 0, 0, 0, 0xFF });
        work->state++;
        break;

    case 4:
        if (fadeIsFinish()) {
            fadeEntry(9, 1000, (GXColor){ 0, 0, 0, 0xFF });
            marioReset();
            marioEntry();
            psndBGMOn(0x200, (char*)0x93);
            if (psbgm[0].streamId != 0xFFFFFFFF) {
                sound.streams[psbgm[0].streamId].flags |= 0x8000;
            }
            *wp = evtEntryType(evt_gameover, 0, 0, 0);
            work->state++;
        }
        break;

    case 5:
        if (evtCheckID((*wp)->eventId) == 0 || (keyGetButtonTrg(0) & 0x1100)) {
            fadeEntry(0x24, 0, (GXColor){ 0, 0, 0, 0xFF });
            psndBGMOff_f_d(0x200, 0xFA0, 0);
            work->state++;
        }
        break;

    case 6:
        if (fadeIsFinish()) {
            _relUnLoad();
            _unload("sys_00", NULL, NULL);
            strcpy(gp->beroEnterName, "");
            strcpy(gp->currentMapName, "");
            strcpy(gp->currentAreaName, "");
            statusWinDispOn();
            camGetPtr(CAMERA_2D)->flags &= ~0x200;
            if (gp->flags & 0x1000) {
                seqSetSeq(SEQ_E3, NULL, NULL);
            } else {
                seqSetSeq(SEQ_TITLE, NULL, NULL);
            }
        }
        break;
    }
}


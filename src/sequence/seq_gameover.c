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

// object-local data
static const char str_802C4960[] = "sys_00";
static EventEntry* work;
static EventEntry** wp = &work;
static const u32 zero_8041a3c0 = 0;

// mapdrv.h declares mapPreLoad(void); the original calls it with a map-name string, so reach it through a
// matching function pointer. Report as a header bug.
#define mapPreLoad(map) ((BOOL (*)(const char*))mapPreLoad)(map)

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
        mapPreLoad(str_802C4960);
        work->state++;
        break;

    case 1:
        if (fadeIsFinish()) {
            _relUnLoad();
            _unload(gp->currentMapName, NULL, NULL);
            strcpy(&gp->currentAreaName[0x10], gp->currentMapName);
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
        if (mapPreLoad(str_802C4960)) {
            mapLoad(str_802C4960);
            cam = camGetPtr(CAMERA_3D);
            mario = marioGetPtr();
            cam->marioPos.x = 0.0f;
            cam->marioPos.y = 0.01f;
            cam->marioPos.z = 0.0f;
            cam->marioAt.x = cam->marioPos.x;
            cam->marioAt.y = cam->marioPos.y;
            cam->marioAt.z = cam->marioPos.z;
            mario->position = (Vec){ 0.0f, 0.0f, 0.0f };
            mario->field_EC = mario->position;
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
            _unload(str_802C4960, NULL, NULL);
            strcpy(gp->beroEnterName, (char*)&zero_8041a3c0);
            strcpy(gp->currentMapName, (char*)&zero_8041a3c0);
            strcpy(gp->currentAreaName, (char*)&zero_8041a3c0);
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

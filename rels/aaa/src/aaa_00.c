#include "aaa_00.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "manager/filemgr.h"
#include "memory.h"
#include "system.h"
#include "win/win_main.h"
#include <dolphin/gx.h>
#include <string.h>

#pragma pool_data off

typedef struct AaaWork {
    FileEntry* tpl; // 0x0
    s32 alpha;      // 0x4
} AaaWork;

static AaaWork* wp;


static void draw(CameraId cameraId, void* param) {
    CameraEntry camBackup;
    f32 viewport[6];
    f32 projection[7];
    CameraEntry* camera;

    GXGetViewportv(viewport);
    GXGetProjectionv(projection);
    camera = camGetPtr(CAMERA_2D);
    GXSetProjection(camGetPtr(CAMERA_2D)->projection, camera->projType);
    camBackup = *camGetCurPtr();
    camera = camGetPtr(CAMERA_2D);
    *camGetCurPtr() = *camera;

    winTexInit(*wp->tpl->data);
    winTexSet(3, (Vec){0.0f, 0.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, wp->alpha});

    *camGetCurPtr() = camBackup;
    GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
    GXSetProjectionv(projection);
}

USER_FUNC(mapdraw) {
    if (isFirstCall) {
        wp = _mapAlloc(sizeof(AaaWork));
        memset(wp, 0, sizeof(AaaWork));
        wp->tpl = fileAllocf(4, "%s/mariost.tpl", getMarioStDvdRoot());
        wp->alpha = 0;
    }

    wp->alpha += 2;
    if (wp->alpha > 255) {
        wp->alpha = 255;
        evtSetValue(event, GF(0), TRUE);
    }
    dispEntry(CAMERA_3D, 1000.0f, 7, draw, wp);
    return EVT_RETURN_BLOCK;
}

void mapdelete(void) {
    if (wp != NULL && wp->tpl != NULL) {
        fileFree(wp->tpl);
    }
}

// effitemget_busaiku. A lecture helper: see rel/common/evt_lect.h.
#include "driver/camdrv.h"
#include "evt/evt_cmd.h"
#include "mario/mariost.h"

static USER_FUNC(effitemget_busaiku) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec offset;

    if (flag) {
        offset = (Vec){0.0f, 2016.0f, 0.0f};
    } else {
        offset = (Vec){0.0f, -2016.0f, 0.0f};
    }
    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    PSVECAdd(&cam->cameraPos, &offset, &cam->storedPos);
    PSVECAdd(&cam->cameraAt, &offset, &cam->storedAt);
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 0xB;
    return EVT_RETURN_DONE;
}

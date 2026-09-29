#include "evt/evt_damage.h"

#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "mario/mario.h"
#include "system.h"

USER_FUNC(camdir) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 yaw = cam->viewYaw;
    HitEntry* hit = *(HitEntry**)((u8*)marioGetPtr() + 0x204);
    s32 dir = 1;

    if (hit != NULL) {
        Vec* pos = hitGetDamageReturnPos(hit);
        f32 angle = reviseAngle(yaw - angleABf((f32)event->lwData[0], (f32)event->lwData[2], pos->x, pos->z));

        if (angle > 180.0f) {
            yaw = reviseAngle(180.0f + yaw);
            dir = -1;
        }
    }

    event->lwData[3] = (s32)(1024.0f * -yaw) + 0xF24A7A80;
    event->lwData[4] = ((yaw > 90.0f && yaw < 270.0f) ? -dir : dir) * 1024 + 0xF24A7A80;

    return EVT_RETURN_DONE;
}

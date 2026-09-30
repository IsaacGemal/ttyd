#include "evt/evt_damage.h"

#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
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

USER_FUNC(evt_gazigazi_move_position) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 startX = (f32)evtGetValue(event, args[1]);
    f32 startY = (f32)evtGetValue(event, args[2]);
    f32 startZ = (f32)evtGetValue(event, args[3]);
    f32 endX = (f32)evtGetValue(event, args[4]);
    f32 endY = (f32)evtGetValue(event, args[5]);
    f32 endZ = (f32)evtGetValue(event, args[6]);
    s32 steps = evtGetValue(event, args[7]);
    s32 mode = evtGetValue(event, args[8]);
    NpcEntry* npc = evtNpcNameToPtr(event, (char*)name);
    s32 curStep;
    Vec pos;

    if (isFirstCall) {
        event->userdata[0] = (s32)gp->renderTime;
    }
#ifdef MUST_MATCH
    // MUST_MATCH: evaluating the step expression in the condition and assigning curStep inside the
    // first call keeps the divwu result in r4 and its callee-saved copy in r26; the natural s32
    // form homes the step in r26 throughout.
    if ((s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->userdata[0]) < steps) {
        startX = intplGetValue(mode, curStep = (u32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->userdata[0]), steps, startX, endX);
#else
    curStep = (s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->userdata[0]);
    if (curStep < steps) {
        startX = intplGetValue(mode, curStep, steps, startX, endX);
#endif
        startY = intplGetValue(mode, curStep, steps, startY, endY);
        startZ = intplGetValue(mode, curStep, steps, startZ, endZ);
        pos = (Vec){ startX, startY, startZ };
        npc->prevPosition = pos;
        npc->position = npc->prevPosition;
        return EVT_RETURN_BLOCK;
    }
    pos = (Vec){ endX, endY, endZ };
    npc->prevPosition = pos;
    npc->position = npc->prevPosition;
    return EVT_RETURN_DONE;
}

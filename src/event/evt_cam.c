#include "evt/evt_cam.h"

#include <math.h>

#include "driver/camdrv.h"
#include "evt/evt_npc.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "pmario_sound.h"
#include "system.h"
#include "cam_road.h"
#include "cam_shift.h"



USER_FUNC(evt_cam_ctrl_onoff) {
    s32* args = event->args;
    CameraId camId = evtGetValue(event, args[0]);
    s32 on = evtGetValue(event, args[1]);

    if (on) {
        camCtrlOn(camId);
    } else {
        camCtrlOff(camId);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_get_pos) {
    s32* args = event->args;
    CameraEntry* cam = camGetPtr(evtGetValue(event, args[0]));

    evtSetFloat(event, args[1], cam->cameraPos.x);
    evtSetFloat(event, args[2], cam->cameraPos.y);
    evtSetFloat(event, args[3], cam->cameraPos.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_get_at) {
    s32* args = event->args;
    CameraEntry* cam = camGetPtr(evtGetValue(event, args[0]));

    evtSetFloat(event, args[1], cam->cameraAt.x);
    evtSetFloat(event, args[2], cam->cameraAt.y);
    evtSetFloat(event, args[3], cam->cameraAt.z);
    return EVT_RETURN_DONE;
}

// NONMATCHING (71.36%): 11 instructions differ, 47 missing, 14 extra, plus register differences.
USER_FUNC(evt_cam_shake) {
    s32* args = event->args;
    CameraId camId = evtGetValue(event, args[0]);
    f32 ampX = evtGetFloat(event, args[1]);
    f32 ampY = evtGetFloat(event, args[2]);
    s32 duration = evtGetValue(event, args[3]);
    OSTime start = event->lifetime;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = start;
        event->userdata[2] = start >> 32;
        event->field_84 = 0;
        event->wInterpRelatedTime = start;
    }
    event->userdata[0]++;
    event->userdata[0] = event->userdata[0] % 4;
    if (event->userdata[0] < 2) {
        PSMTX44Trans(camGetPtr(camId)->projAdjust, ampX, ampY, 0.0f);
    } else {
        PSMTX44Trans(camGetPtr(camId)->projAdjust, -ampX, -ampY, 0.0f);
    }
    camGetPtr(camId)->flags |= 8;
    if (duration != 0) {
        if (OSTicksToMilliseconds(start - *(OSTime*)&event->userdata[1]) >= duration) {
            camGetPtr(camId)->flags &= ~8;
            padRumbleOff(0);
            return EVT_RETURN_DONE;
        }
    }
    {
        f32 mag = sqrtf(ampX * ampX + ampY * ampY);
        s32 onTime = (s32)(13000.0f * mag);
        s32 offTime = (s32)(50.0f * (1.0f - mag));

        if (onTime != 0 && offTime != 0) {
            if (event->field_84 == 0) {
                padRumbleOn(0);
                if (OSTicksToMilliseconds(start - event->wInterpRelatedTime) >= onTime) {
                    event->field_84 = 1 - event->field_84;
                    event->wInterpRelatedTime = start;
                }
            } else {
                padRumbleOff(0);
                if (OSTicksToMilliseconds(start - event->wInterpRelatedTime) >= offTime) {
                    event->field_84 = 1 - event->field_84;
                    event->wInterpRelatedTime = start;
                }
            }
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_cam3d_evt_zoom_in) {
    s32* args = event->args;
    s32 scale = evtGetValue(event, args[0]);
    s32 time = evtGetValue(event, args[1]);
    s32 zoom = evtGetValue(event, args[2]);
    s32 curve = evtGetValue(event, args[3]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec dir;

    PSVECSubtract(&cam->cameraAt, &cam->cameraPos, &dir);
    PSVECScale(&dir, &dir, (f32)scale / 100.0f);
    PSVECAdd(&dir, &cam->cameraPos, &dir);

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = dir;
    cam->storedAt = cam->cameraAt;
    cam->storedPos.x += (f32)zoom * (f32)cos(cam->viewYaw);
    cam->storedPos.z += (f32)zoom * (f32)sin(cam->viewYaw);
    cam->storedAt.x += (f32)zoom * (f32)cos(cam->viewYaw);
    cam->storedAt.z += (f32)zoom * (f32)sin(cam->viewYaw);
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 3;
    cam->curveA = curve;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_set_at) {
    s32* args = event->args;
    s32 x = evtGetValue(event, args[0]);
    s32 y = evtGetValue(event, args[1]);
    s32 z = evtGetValue(event, args[2]);
    s32 dolly = evtGetValue(event, args[3]);
    s32 time = evtGetValue(event, args[4]);
    s32 curve = evtGetValue(event, args[5]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec at = makeVecByValue((f32)x, (f32)y, (f32)z);
    Vec pos;

    PSVECSubtract(&at, &cam->cameraPos, &pos);
    PSVECScale(&pos, &pos, (f32)dolly / 100.0f);
    PSVECAdd(&pos, &cam->cameraPos, &pos);

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = pos;
    cam->storedAt = at;
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 3;
    cam->curveA = curve;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_set) {
    s32* args = event->args;
    f32 x = evtGetValue(event, args[0]);
    f32 y = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);
    f32 ax = evtGetValue(event, args[3]);
    f32 ay = evtGetValue(event, args[4]);
    f32 az = evtGetValue(event, args[5]);
    s32 time = evtGetValue(event, args[6]);
    s32 curve = evtGetValue(event, args[7]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec sp = makeVecByValue(x, y, z);
    Vec sa = makeVecByValue2(ax, ay, az);

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = sp;
    cam->storedAt = sa;
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 3;
    cam->curveA = curve;
    return EVT_RETURN_DONE;
}

// NONMATCHING (80.37%): 4 instructions differ, 16 missing, 16 extra, plus register differences.
USER_FUNC(evt_cam3d_evt_set_rel) {
    s32* args = event->args;
    f32 x = evtGetValue(event, args[0]);
    f32 y = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);
    f32 ax = evtGetValue(event, args[3]);
    f32 ay = evtGetValue(event, args[4]);
    f32 az = evtGetValue(event, args[5]);
    s32 time = evtGetValue(event, args[6]);
    s32 curve = evtGetValue(event, args[7]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    MarioWork* mario = marioGetPtr();

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    {
        Vec sa;
        Vec sp;

        sa.x = mario->position.x + ax;
        sa.y = mario->position.y + ay;
        sa.z = mario->position.z + az;
        sp.x = sa.x + x;
        sp.y = sa.y + y;
        sp.z = sa.z + z;
        cam->storedPos = sp;
        cam->storedAt = sa;
    }
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 3;
    cam->curveA = curve;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_set_npc_rel) {
    s32* args = event->args;
    const char* npcName = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetValue(event, args[1]);
    f32 y = evtGetValue(event, args[2]);
    f32 z = evtGetValue(event, args[3]);
    f32 ax = evtGetValue(event, args[4]);
    f32 ay = evtGetValue(event, args[5]);
    f32 az = evtGetValue(event, args[6]);
    s32 time = evtGetValue(event, args[7]);
    s32 curve = evtGetValue(event, args[8]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    NpcEntry* npc = evtNpcNameToPtr(event, npcName);
    Vec stPos;
    Vec stAt;
    Vec pos = makeVecByValue(x, y, z);
    Vec at = makeVecByValue2(ax, ay, az);

    PSVECAdd(&npc->position, &at, &stAt);
    PSVECAdd(&stAt, &pos, &stPos);
    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = stPos;
    cam->storedAt = stAt;
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 3;
    cam->curveA = curve;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_set_rel_dir) {
    s32* args = event->args;
    f32 x = evtGetValue(event, args[0]);
    f32 y = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);
    f32 ax = evtGetValue(event, args[3]);
    f32 ay = evtGetValue(event, args[4]);
    f32 az = evtGetValue(event, args[5]);
    s32 time = evtGetValue(event, args[6]);
    s32 curve = evtGetValue(event, args[7]);
    f32 yaw = -camGetPtr(CAMERA_3D)->viewYaw;
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    MarioWork* mario = marioGetPtr();
    Vec stPos;
    Vec stAt;
    Vec pos = makeVecByValue(x, y, z);
    Vec at = makeVecByValue2(ax, ay, az);
    Mtx rot;

    PSMTXRotRad(rot, 'Y', MTXDegToRad(yaw));
    PSMTXMultVec(rot, &pos, &pos);
    PSMTXMultVec(rot, &at, &at);
    PSVECAdd(&mario->position, &at, &stAt);
    PSVECAdd(&stAt, &pos, &stPos);

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = stPos;
    cam->storedAt = stAt;
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 3;
    cam->curveA = curve;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_off) {
    s32* args = event->args;
    s32 time = evtGetValue(event, args[0]);
    s32 curve = evtGetValue(event, args[1]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 1;
    cam->curveA = curve;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_road_shift_onoff) {
    s32* args = event->args;
    s32 on = evtGetValue(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    if (on) {
        cam->flags &= ~0x20;
    } else {
        cam->flags |= 0x20;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_set_xyz) {
    s32* args = event->args;
    s32 x = evtGetValue(event, args[0]);
    s32 y = evtGetValue(event, args[1]);
    s32 z = evtGetValue(event, args[2]);
    s32 dolly = evtGetValue(event, args[3]);
    s32 time = evtGetValue(event, args[4]);
    s32 curve = evtGetValue(event, args[5]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    {
        Vec prev = (Vec){ 0.0f, 0.0f, 0.0f };
        prev.x = cam->blendPos.x;
        prev.y = cam->blendPos.y;
        prev.z = cam->blendPos.z;
        cam->anchorPrev = prev;
    }
    {
        Vec anchor = (Vec){ 0.0f, 0.0f, 0.0f };
        anchor.x = (f32)x;
        anchor.y = (f32)y;
        anchor.z = (f32)z;
        cam->anchor = anchor;
    }
    cam->dollyStartAlt = cam->dollyPercent;
    cam->dollyEnd = (f32)dolly;
    cam->startTimeB = gp->renderTime;
    cam->durationB = OSMillisecondsToTicks(time);
    cam->stateB = 3;
    cam->curveB = curve;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_xyz_off) {
    s32* args = event->args;
    s32 time = evtGetValue(event, args[0]);
    s32 curve = evtGetValue(event, args[1]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    cam->anchorPrev = makeVecByValue(cam->blendPos.x, cam->blendPos.y, cam->blendPos.z);
    cam->dollyStartAlt = cam->dollyPercent;
    cam->startTimeB = gp->renderTime;
    cam->durationB = OSMillisecondsToTicks(time);
    cam->stateB = 1;
    cam->curveB = curve;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_get_shift) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    f32 oldX = mario->position.x;
    f32 oldY = mario->position.y;
    f32 oldZ = mario->position.z;
    f32 out;

    mario->position.x = x;
    mario->position.y = y;
    mario->position.z = z;
    camShiftMain(cam, mario, &out);
    mario->position.x = oldX;
    mario->position.y = oldY;
    mario->position.z = oldZ;
    evtSetFloat(event, args[3], out);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_evt_set_now) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    cam->storedPos = cam->cameraPos;
    cam->presetPos = cam->storedPos;
    cam->storedAt = cam->cameraAt;
    cam->presetAt = cam->storedAt;
    cam->startTimeA = gp->renderTime;
    cam->durationA = 0;
    cam->stateA = 3;
    cam->curveA = 0xB;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam3d_event_from_road) {
    s32* args = event->args;
    CameraParams work;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    f32 bank = evtGetFloat(event, args[3]);

    camRoadMain(&work.fovY, x, y, z, x, y, z, bank);
    evtSetFloat(event, args[4], work.pos.x);
    evtSetFloat(event, args[5], work.pos.y);
    evtSetFloat(event, args[6], work.pos.z);
    evtSetFloat(event, args[7], work.at.x);
    evtSetFloat(event, args[8], work.at.y);
    evtSetFloat(event, args[9], work.at.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_road_reset) {
    MarioWork* mario = marioGetPtr();
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    cam->marioPos.x = mario->position.x;
    cam->marioPos.y = mario->position.y + 0.01f;
    cam->marioPos.z = mario->position.z;
    cam->marioAt.x = cam->marioPos.x;
    cam->marioAt.y = cam->marioPos.y;
    cam->marioAt.z = cam->marioPos.z;

    mario->field_EC = mario->position;
    mario->field_F8 = mario->position;

    camRoadReset();
    camShiftReset();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_road_reset2) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    MarioWork* mario = marioGetPtr();
    CameraEntry* cam = camGetPtr(CAMERA_3D);

    cam->marioPos.x = x;
    cam->marioPos.y = y + 0.01f;
    cam->marioPos.z = z;
    cam->dollyStart = 0.0f;
    cam->marioAt.x = cam->marioPos.x;
    cam->marioAt.y = cam->marioPos.y;
    cam->marioAt.z = cam->marioPos.z;

    {
        Vec ec = (Vec){ 0.0f, 0.0f, 0.0f };
        ec.x = x;
        ec.y = y;
        ec.z = z;
        mario->field_EC = ec;
    }
    {
        Vec f8 = (Vec){ 0.0f, 0.0f, 0.0f };
        f8.x = x;
        f8.y = y;
        f8.z = z;
        mario->field_F8 = f8;
    }

    camRoadReset();
    camShiftReset();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_shift_reset) {
    marioGetPtr();
    camGetPtr(CAMERA_3D);
    camShiftReset();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_type_change) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    CameraId camId = evtGetValue(event, args[1]);

    if (type == 0) {
        camSetTypePersp(camId);
    } else {
        camSetTypeOrtho(camId);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_letter_box_onoff) {
    s32* args = event->args;
    s32 on = evtGetValue(event, args[0]);
    s32 enable = evtGetValue(event, args[1]);
    CameraEntry* cam = camGetPtr(CAMERA_2D);

    if (on) {
        cam->flags |= 0x100;
        psndSetFlag(0x20);
        cam->letterBoxAlpha = 0xFF;
        cam->letterBoxTop = 0;
        cam->letterBoxBottom = 0;
        cam->letterBoxCamId = CAMERA_2D;
    } else {
        cam->flags &= ~0x100;
        psndClearFlag(0x20);
    }
    if (enable) {
        cam->flags |= 0x400;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_letter_box_disable) {
    s32* args = event->args;
    s32 disable = evtGetValue(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_2D);

    if (disable) {
        cam->flags |= 0x200;
        psndSetFlag(0x40);
    } else {
        cam->flags &= ~0x200;
        psndClearFlag(0x40);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_cam_letter_box_wait) {
    CameraEntry* cam = camGetPtr(CAMERA_2D);

    if (cam->flags & 0x100) {
        if (cam->letterBoxTop == 0x31 && cam->letterBoxBottom == 0x2C) {
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_cam_letter_box_camid) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);

    camGetPtr(CAMERA_2D)->letterBoxCamId = id;
    return EVT_RETURN_DONE;
}

inline Vec makeVecByValue(f32 x, f32 y, f32 z) {
    Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

inline Vec makeVecByValue2(f32 x, f32 y, f32 z) {
    Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

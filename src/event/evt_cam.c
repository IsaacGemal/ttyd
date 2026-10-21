#include "evt/evt_cam.h"

#include "math_ext.h"

#include "driver/camdrv.h"
#include "evt/evt_npc.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "parse.h"
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

int __float_nan[];
#define CAM_NAN (*(f32*)__float_nan)

#define CAM_FP_NAN 1
#define CAM_FP_INFINITE 2
#define CAM_FP_ZERO 3
#define CAM_FP_NORMAL 4
#define CAM_FP_SUBNORMAL 5

// Float classification the original's inline sqrtf uses (same shape as math.h's __fpclassifyf).
static inline s32 camShakeFpClassify(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return CAM_FP_NAN;
            }
            return CAM_FP_INFINITE;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return CAM_FP_SUBNORMAL;
            }
            return CAM_FP_ZERO;
        default:
            return CAM_FP_NORMAL;
    }
}

// sqrtf as the original inlines it: Newton-Raphson refinement plus the NaN classification.
// The local consts are what the original's inline pools as double_0p5/double_3/double_0.
static inline f32 camShakeSqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return CAM_NAN;
    } else if (camShakeFpClassify(x) == CAM_FP_NAN) {
        return CAM_NAN;
    }
    return x;
}

// The original treats 0x88 as the two words of userdataEx, not as wInterpRelatedTime: it stores
// the halves separately (the u64 store schedules the two words in the opposite order) and reads
// them back joined into a u64 (a plain u64 load colours start, event and camId differently).
USER_FUNC(evt_cam_shake) {
    s32* args = event->args;
    CameraId camId = evtGetValue(event, args[0]);
    f32 ampX = evtGetFloat(event, args[1]);
    f32 ampY = evtGetFloat(event, args[2]);
    s32 duration = evtGetValue(event, args[3]);
    u64 start = event->lifetime;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = (start >> 32) & 0xFFFFFFFF;
        event->userdata[2] = start & 0xFFFFFFFF;
        event->field_84 = 0;
        event->userdataEx[0] = (start >> 32) & 0xFFFFFFFF;
        event->userdataEx[1] = start & 0xFFFFFFFF;
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
        if (OSTicksToMilliseconds(start - *(u64*)&event->userdata[1]) > duration) {
            camGetPtr(camId)->flags &= ~8;
            padRumbleOff(0);
            return EVT_RETURN_DONE;
        }
    }
    {
        f32 mag = camShakeSqrtf(ampX * ampX + ampY * ampY);
        s32 onTime = (s32)(13000.0f * mag);
        s32 offTime = (s32)(50.0f * (1.0f - mag));
        u64 wInterp = ((u64)(u32)event->userdataEx[0] << 32) |
                      (u32)event->userdataEx[1];

        if (onTime != 0 && offTime != 0) {
            if ((s32)event->field_84 == 0) {
                padRumbleOn(0);
                if (OSTicksToMilliseconds(start - wInterp) > onTime) {
                    event->field_84 = 1 - event->field_84;
                    event->userdataEx[0] = (start >> 32) & 0xFFFFFFFF;
                    event->userdataEx[1] = start & 0xFFFFFFFF;
                }
            } else {
                padRumbleOff(0);
                if (OSTicksToMilliseconds(start - wInterp) > offTime) {
                    event->field_84 = 1 - event->field_84;
                    event->userdataEx[0] = (start >> 32) & 0xFFFFFFFF;
                    event->userdataEx[1] = start & 0xFFFFFFFF;
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
    Vec at = (Vec){ (f32)x, (f32)y, (f32)z };
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

/// @todo reconstructed: retail's evt_cam .rodata has a zero Vec template at 0x0C (vec3_802c5f44) that no retail
/// function references, between evt_cam3d_evt_set_at's (0x00) and evt_cam3d_evt_set's (0x18, 0x24); templates pool
/// in source order, so a function with one compound-literal Vec sat here. Invented: the name, the arguments and the
/// body (a Mario-relative evt_cam3d_evt_set_at).
#ifdef MUST_MATCH
USER_FUNC(evt_cam3d_evt_set_at_rel) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    s32 time = evtGetValue(event, args[3]);
    s32 curve = evtGetValue(event, args[4]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    MarioWork* mario = marioGetPtr();
    Vec at = (Vec){ mario->position.x + x, mario->position.y + y, mario->position.z + z };

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = cam->cameraPos;
    cam->storedAt = at;
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 3;
    cam->curveA = curve;
    return EVT_RETURN_DONE;
}
#endif

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
    Vec sp = (Vec){ x, y, z };
    Vec sa = (Vec){ ax, ay, az };

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
    Vec sp;
    Vec sa;
    f32 sx;
    f32 sy;
    f32 sz;

    sx = mario->position.x + ax;
    sy = mario->position.y + ay;
    sz = mario->position.z + az;
    sa.x = sx;
    sa.y = sy;
    sa.z = sz;
    sp.x = x + sx;
    sp.y = y + sy;
    sp.z = z + sz;
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

/// @todo reconstructed: retail's .rodata has two unreferenced zero Vec templates at 0x30/0x3C (vec3_802c5f68/74),
/// between evt_cam3d_evt_set's and evt_cam3d_evt_set_npc_rel's, and an unreferenced "%f%f%f%f%f%f%d%s"
/// (str_fmt_ffffffds) after the conversion double. Invented: the name and the body (an evt_cam3d_evt_set_npc_rel that
/// sscanfs its offsets, time and NPC name from one string).
#ifdef MUST_MATCH
USER_FUNC(evt_cam3d_evt_set_npc_rel_str) {
    s32* args = event->args;
    const char* str = (const char*)evtGetValue(event, args[0]);
    s32 curve = evtGetValue(event, args[1]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec pos = (Vec){ 0.0f, 0.0f, 0.0f };
    Vec at = (Vec){ 0.0f, 0.0f, 0.0f };
    s32 time;
    char npcName[32];
    NpcEntry* npc;
    Vec stPos;
    Vec stAt;

    sscanf(str, "%f%f%f%f%f%f%d%s", &pos.x, &pos.y, &pos.z, &at.x, &at.y, &at.z, &time, npcName);
    npc = evtNpcNameToPtr(event, npcName);
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
#endif

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
    Vec pos = (Vec){ x, y, z };
    Vec at = (Vec){ ax, ay, az };

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
    Vec pos = (Vec){ x, y, z };
    Vec at = (Vec){ ax, ay, az };
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

/// @todo reconstructed: retail's .sdata2 has 100.0f (float_100_8041af30) ahead of evt_cam3d_evt_set_rel_dir's
/// deg2rad, while its live users (evt_cam3d_evt_set_at, evt_cam3d_evt_zoom_in) are compiled after set_rel_dir, so a
/// 100.0f user sat between set_rel_dir and evt_cam_road_reset2. Invented: the name and the body
/// (evt_cam3d_evt_zoom_in's percent dolly without the sideways shift).
#ifdef MUST_MATCH
USER_FUNC(evt_cam3d_evt_dolly) {
    s32* args = event->args;
    s32 dolly = evtGetValue(event, args[0]);
    s32 time = evtGetValue(event, args[1]);
    s32 curve = evtGetValue(event, args[2]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec pos;

    PSVECSubtract(&cam->cameraAt, &cam->cameraPos, &pos);
    PSVECScale(&pos, &pos, (f32)dolly / 100.0f);
    PSVECAdd(&pos, &cam->cameraPos, &pos);

    cam->presetPos = cam->cameraPos;
    cam->presetAt = cam->cameraAt;
    cam->storedPos = pos;
    cam->storedAt = cam->cameraAt;
    cam->startTimeA = gp->renderTime;
    cam->durationA = OSMillisecondsToTicks(time);
    cam->stateA = 3;
    cam->curveA = curve;
    return EVT_RETURN_DONE;
}
#endif

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

    {
        Vec prev = (Vec){ cam->blendPos.x, cam->blendPos.y, cam->blendPos.z };
        cam->anchorPrev = prev;
    }
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

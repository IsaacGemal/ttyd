#include <dolphin/types.h>

#include "driver/animdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "evt/evt_urouro.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "math_ext.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"

// LOCAL_PROTO(psndSFXOnEx_3D): retail event callers narrow volume and pan to u8; the implementation uses s32 and sharing that signature regresses matched event code.
s32 psndSFXOnEx_3D(const char* name, Vec* position, u8 vol, u8 pan, u16 pitch, u16 flags);

// Work structure allocated by urouro_init_func, held in event->lwData[10].
// Not declared in a header yet.
typedef struct UrouroWork {
    /* 0x00 */ s32 flags;
    /* 0x04 */ s32 mode;
    /* 0x08 */ s32 timer;
    /* 0x0C */ s32 rotateCount;
    /* 0x10 */ s32 walkTime;   // mode 0: walk for walkTime/2 + rand(walkTime/2)
    /* 0x14 */ s32 waitTime;   // mode 2: stand for waitTime/2 + rand(waitTime/2)
    /* 0x18 */ s32 turnTime;   // mode 3: time between the about-turns while standing
    /* 0x1C */ Vec center;     // x/z: the point the npc walks back to when farther than radius
    /* 0x28 */ f32 radius;
} UrouroWork; // 0x2C

USER_FUNC(urouro_init_func) {
    s32* args = event->args;
    NpcEntry* npc;
    UrouroWork* work;
    s32 name;
    f32 centerX;
    f32 centerZ;
    f32 radius;
    f32 moveSpeed;
    s32 flags;

    name = evtGetValue(event, args[0]);
    npc = evtNpcNameToPtr(event, (const char*)name);
    centerX = evtGetFloat(event, args[1]);
    centerZ = evtGetFloat(event, args[2]);
    radius = evtGetFloat(event, args[3]);
    moveSpeed = evtGetFloat(event, args[4]);
    flags = evtGetValue(event, args[5]);

    event->lwData[10] = (s32)_mapAlloc(sizeof(UrouroWork));
    work = (UrouroWork*)event->lwData[10];
    work->flags = flags;
    work->mode = 0;
    work->timer = 0;
    work->rotateCount = 0;
    work->walkTime = sysMsec2Frame(6000);
    work->waitTime = sysMsec2Frame(9000);
    work->turnTime = sysMsec2Frame(1500);
    work->center.x = centerX;
    work->center.y = npc->position.y;
    work->center.z = centerZ;
    work->radius = radius;
    npc->moveSpeed = moveSpeed;
    npc->flags |= 0x400000;
    return EVT_RETURN_DONE;
}

USER_FUNC(urouro_main_func) {
    s32* args = event->args;
    NpcEntry* npc;
    MarioWork* mario;
    UrouroWork* work;
    s32 name;
    f32 dist;
    f32 step;
    f32 maxStep;
    s32 sfxFlag;

    name = evtGetValue(event, args[0]);
    npc = evtNpcNameToPtr(event, (const char*)name);
    mario = marioGetPtr();
    work = (UrouroWork*)event->lwData[10];

    switch (work->mode) {
        case 0:
            work->timer = work->walkTime / 2 + irand(work->walkTime / 2 + 1);
            work->timer -= work->timer % 20;
            work->rotateCount = 0;
            npc->direction = reviseAngle(npc->direction + (f32)irand(60) - 30.0f);
            npc->moveDir = npc->direction;
            work->mode = 1;
            break;
        case 1:
            dist = distABf(npc->position.x, npc->position.z, work->center.x, work->center.z);
            if (animPoseGetPeraEnd(npc->poseId)) {
                if (dist > work->radius) {
                    if (!(work->flags & 1)) {
                        work->flags |= 3;
                    }
                    if (work->flags & 2) {
                        npc->direction = reviseAngle(angleABf(npc->position.x, npc->position.z, work->center.x, work->center.z));
                        npc->moveDir = npc->direction;
                        work->flags &= ~2;
                    }
                } else if (work->flags & 1) {
                    work->flags &= ~3;
                }
            }
            if (animPoseGetPeraEnd(npc->poseId) && npc == marioGetTouchNpcPtr()
                && __fabsf(compAngle(npc->moveDir, mario->directionView)) < 90.0f
                && __fabsf(compAngle(angleABf(npc->position.x, npc->position.z, mario->position.x, mario->position.z), mario->directionView)) < 90.0f
                && dist < work->radius) {
                npc->direction = reviseAngle(npc->moveDir - 180.0f);
                npc->moveDir = npc->direction;
                work->flags &= ~3;
            }
            if (animPoseGetPeraEnd(npc->poseId) && npc->wallHit != NULL) {
                npc->direction = reviseAngle(angleABf(npc->position.x, npc->position.z, work->center.x, work->center.z));
                npc->moveDir = npc->direction;
                work->flags &= ~3;
            }
            work->timer--;
            if (work->timer == 0) {
                work->mode = 2;
                break;
            }
            if (animPoseGetPeraEnd(npc->poseId)) {
                maxStep = 20.0f * npc->moveSpeed / 1000.0f;
                step = npc->moveSpeed * npc->moveDt;
                if (step >= maxStep) {
                    step = maxStep;
                }
                npc->moveOffset.x = step * (f32)sin(npc->moveDir * 6.2831855f / 360.0f);
                npc->moveOffset.z = step * -(f32)cos(npc->moveDir * 6.2831855f / 360.0f);
                sfxFlag = 0;
                if (psndGetFlag() & 0x100) {
                    if (!(animPoseGetMaterialFlag(npc->poseId) & 0x1800)) {
                        sfxFlag = 0x10000;
                    }
                }
                if (npc->footstepAccum > (f32)npc->footstepStride / npc->moveSpeed) {
                    if (npc->footstepToggle != 0) {
                        if (npc->footstepSfxA != NULL) {
                            psndSFXOnEx_3D((const char*)(sfxFlag | searchPSSFXList(npc->footstepSfxA)), &npc->position, 0xff, 0xff, 0, 8);
                        }
                    } else {
                        if (npc->footstepSfxB != NULL) {
                            psndSFXOnEx_3D((const char*)(sfxFlag | searchPSSFXList(npc->footstepSfxB)), &npc->position, 0xff, 0xff, 0, 8);
                        }
                    }
                    npc->footstepAccum = 0.0f;
                    npc->footstepToggle = (s8)(1 - npc->footstepToggle);
                }
                if (npc->footstepSfxB != NULL || npc->footstepToggle != 0) {
                    npc->footstepAccum += 1000.0f * npc->moveDt;
                }
            }
            break;
        case 2:
            work->timer = work->waitTime / 2 + irand(work->waitTime / 2 + 1);
            npc->direction = reviseAngle(npc->direction + (f32)irand(60) - 30.0f);
            npc->moveDir = npc->direction;
            work->rotateCount = 0;
            if (work->flags & 0x10000) {
                work->rotateCount = irand(1000) % 3 + 2;
            }
            work->mode = 3;
            break;
        case 3:
            work->timer--;
            if (work->timer == 0) {
                work->timer = work->turnTime / 2 + irand(work->turnTime / 2 + 1);
                if (work->rotateCount == 0) {
                    work->mode = 0;
                } else {
                    work->rotateCount--;
                    npc->direction = reviseAngle(npc->direction + 180.0f);
                }
            }
            break;
    }
    return EVT_RETURN_BLOCK;
}

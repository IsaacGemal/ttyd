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
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ s32 field_0x14;
    /* 0x18 */ s32 field_0x18;
    /* 0x1C */ f32 field_0x1C;
    /* 0x20 */ f32 field_0x20;
    /* 0x24 */ f32 field_0x24;
    /* 0x28 */ f32 field_0x28;
} UrouroWork; // 0x2C

USER_FUNC(urouro_init_func) {
    s32* args = event->args;
    NpcEntry* npc;
    UrouroWork* work;
    s32 name;
    f32 a;
    f32 b;
    f32 c;
    f32 d;
    s32 flags;

    name = evtGetValue(event, args[0]);
    npc = evtNpcNameToPtr(event, (const char*)name);
    a = evtGetFloat(event, args[1]);
    b = evtGetFloat(event, args[2]);
    c = evtGetFloat(event, args[3]);
    d = evtGetFloat(event, args[4]);
    flags = evtGetValue(event, args[5]);

    event->lwData[10] = (s32)_mapAlloc(sizeof(UrouroWork));
    work = (UrouroWork*)event->lwData[10];
    work->flags = flags;
    work->mode = 0;
    work->timer = 0;
    work->rotateCount = 0;
    work->field_0x10 = sysMsec2Frame(6000);
    work->field_0x14 = sysMsec2Frame(9000);
    work->field_0x18 = sysMsec2Frame(1500);
    work->field_0x1C = a;
    work->field_0x20 = npc->position.y;
    work->field_0x24 = b;
    work->field_0x28 = c;
    npc->moveSpeed = d;
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
            work->timer = work->field_0x10 / 2 + irand(work->field_0x10 / 2 + 1);
            work->timer -= work->timer % 20;
            work->rotateCount = 0;
            npc->direction = reviseAngle(npc->direction + (f32)irand(60) - 30.0f);
            npc->moveDir = npc->direction;
            work->mode = 1;
            break;
        case 1:
            dist = distABf(npc->position.x, npc->position.z, work->field_0x1C, work->field_0x24);
            if (animPoseGetPeraEnd(npc->poseId)) {
                if (dist > work->field_0x28) {
                    if (!(work->flags & 1)) {
                        work->flags |= 3;
                    }
                    if (work->flags & 2) {
                        npc->direction = reviseAngle(angleABf(npc->position.x, npc->position.z, work->field_0x1C, work->field_0x24));
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
                && dist < work->field_0x28) {
                npc->direction = reviseAngle(npc->moveDir - 180.0f);
                npc->moveDir = npc->direction;
                work->flags &= ~3;
            }
            if (animPoseGetPeraEnd(npc->poseId) && npc->wallHit != NULL) {
                npc->direction = reviseAngle(angleABf(npc->position.x, npc->position.z, work->field_0x1C, work->field_0x24));
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
            work->timer = work->field_0x14 / 2 + irand(work->field_0x14 / 2 + 1);
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
                work->timer = work->field_0x18 / 2 + irand(work->field_0x18 / 2 + 1);
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

#include "motion/mot_ship.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "driver/hitdrv.h"
#include "driver/effdrv.h"
#include "pmario_sound.h"
#include "system.h"
#include "memory.h"
#include <string.h>
#include "party.h"
#include "eff/eff_funemizu.h"
#include "math_ext.h"

// Local declarations for symbols that have no home header yet (see REPORT.md).

// Motion data allocated as mario->motStruct (size 0x48).
typedef struct MotionShipData {
    u32 flags;        // 0x00
    f32 unk04;        // 0x04
    f32 unk08;        // 0x08
    f32 unk0C;        // 0x0C
    f32 unk10;        // 0x10
    f32 velX;         // 0x14
    f32 velY;         // 0x18
    f32 unk1C;        // 0x1C
    f32 unk20;        // 0x20
    EffectEntry* eff; // 0x24
    f32 unk28;        // 0x28
    f32 unk2C;        // 0x2C
    s32 swirlTimer;   // 0x30
    f32 unk34;        // 0x34
    u32 sfxId;        // 0x38
    Vec unk3C;        // 0x3C
} MotionShipData;     // size 0x48

const char* paper_ship[4] = {
    "p_hune",
    "p_hune_g",
    "p_hune_y",
    "p_hune_v",
};

// Maps hitGetAttr's bits 0x4000/0x8000/0x10000/0x20000 to the harbor type. Keeping the
// mapping in a single-exit inline (its locals `type`/`attr`, returning `type`) is what gives
// the original's callee-saved allocation (hit r28 / type r29) in all three copies; inlining
// the same if-else chain directly into checkHarbor swapped them from the middle copy on.

static inline s32 checkHarborType(HitEntry* hit) {
    s32 type = 0;
    s32 attr = hitGetAttr(hit);
    if (attr & 0x4000) {
        type = 1;
    } else if (attr & 0x8000) {
        type = 2;
    } else if (attr & 0x10000) {
        type = 3;
    } else if (attr & 0x20000) {
        type = 4;
    }
    return type;
}

s32 checkHarbor(Vec* out) {
    MarioWork* mario = marioGetPtr();
    HitEntry* hit;
    f32 sp28;
    f32 sp24;
    f32 sp20;
    f32 sp1C;
    f32 sp18;
    f32 sp14;
    f32 sp10;
    f32 sp0C;
    f32 sp08;
    f32 rx;
    f32 ry;
    f32 rz;
    Vec pos1;
    Vec pos2;
    Vec pos3;
    s32 type;
    f32 x;
    f32 y;
    f32 z;

    sincosf(&sp28, &sp24, toMovedir(mario->unk1A8));
    x = mario->position.x + 40.0f * sp28;
    z = mario->position.z + 40.0f * sp24;
    y = mario->position.y + 20.0f;
    sp14 = 40.0f;
    hit = hitCheckFilter(NULL, &sp20, &sp1C, &sp18, &sp14, &sp10, &sp0C, &sp08, x, y, z, 0.0f, -1.0f,
                        0.0f);
    if (hit != NULL) {
        type = checkHarborType(hit);
        if (type != 0) {
            hitObjGetPos(hitGetName(hit), &pos1);
            rx = (f32)(s32)((100.0f * pos1.x + 0.5f) / 100.0f);
            ry = (f32)(s32)((100.0f * pos1.y + 0.5f) / 100.0f);
            rz = (f32)(s32)((100.0f * pos1.z + 0.5f) / 100.0f);
            out->x = rx;
            out->y = ry;
            out->z = rz;
            return 1;
        }
    }
    sincosf(&sp28, &sp24, toMovedir(mario->unk1A8 + 90.0f));
    x = mario->position.x + 40.0f * sp28;
    z = mario->position.z + 40.0f * sp24;
    y = mario->position.y + 20.0f;
    sp14 = 40.0f;
    hit = hitCheckFilter(NULL, &sp20, &sp1C, &sp18, &sp14, &sp10, &sp0C, &sp08, x, y, z, 0.0f, -1.0f,
                        0.0f);
    if (hit != NULL) {
        type = checkHarborType(hit);
        if (type != 0) {
            hitObjGetPos(hitGetName(hit), &pos2);
            rx = (f32)(s32)((100.0f * pos2.x + 0.5f) / 100.0f);
            ry = (f32)(s32)((100.0f * pos2.y + 0.5f) / 100.0f);
            rz = (f32)(s32)((100.0f * pos2.z + 0.5f) / 100.0f);
            out->x = rx;
            out->y = ry;
            out->z = rz;
            return 1;
        }
    }
    sincosf(&sp28, &sp24, toMovedir(mario->unk1A8 - 90.0f));
    x = mario->position.x + 40.0f * sp28;
    z = mario->position.z + 40.0f * sp24;
    y = mario->position.y + 20.0f;
    sp14 = 40.0f;
    hit = hitCheckFilter(NULL, &sp20, &sp1C, &sp18, &sp14, &sp10, &sp0C, &sp08, x, y, z, 0.0f, -1.0f,
                        0.0f);
    if (hit != NULL) {
        type = checkHarborType(hit);
        if (type != 0) {
            hitObjGetPos(hitGetName(hit), &pos3);
            rx = (f32)(s32)((100.0f * pos3.x + 0.5f) / 100.0f);
            ry = (f32)(s32)((100.0f * pos3.y + 0.5f) / 100.0f);
            rz = (f32)(s32)((100.0f * pos3.z + 0.5f) / 100.0f);
            out->x = rx;
            out->y = ry;
            out->z = rz;
            return 1;
        }
    }
    return 0;
}

#define SHIP_DATA ((MotionShipData*)marioGetPtr()->motStruct)

// Constants table used by shipMove (defined in this unit's .sdata).
f32 shipData[6] = {-0.05f, -0.7f, 0.02f, 0.02f, 2.0f, 0.01f};

// A local temp assigned before each `> shipData[4]` clamp makes MWCC hoist the constant load
// ahead of the `marioGetPtr` call, as the original does; without it the compare operand order
// and the temp register differ. The natural inline form is under #else.
void shipMove(void) {
    MarioWork* mario = marioGetPtr();
#ifdef MUST_MATCH
    f32 ftmp;
#endif
    f32 sp0C;
    f32 sp08;
    f32 f31;
    f32 f30 = mario->controlStickSensitivity;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f26;

    sp0C = 0.0f;
    sp0C = 0.0f;
    if (SHIP_DATA->swirlTimer != 0) {
        SHIP_DATA->swirlTimer--;
        movePos(&sp0C, &sp08, SHIP_DATA->unk2C, SHIP_DATA->unk28);
        SHIP_DATA->velX += sp0C;
        SHIP_DATA->velY += sp08;
#ifdef MUST_MATCH
        ftmp = shipData[4];
        if (SHIP_DATA->velX > ftmp) {
            SHIP_DATA->velX = shipData[4];
        }
        if (SHIP_DATA->velX < -shipData[4]) {
            SHIP_DATA->velX = -shipData[4];
        }
        ftmp = shipData[4];
        if (SHIP_DATA->velY > ftmp) {
            SHIP_DATA->velY = shipData[4];
        }
        if (SHIP_DATA->velY < -shipData[4]) {
            SHIP_DATA->velY = -shipData[4];
        }
#else
        if (SHIP_DATA->velX > shipData[4]) {
            SHIP_DATA->velX = shipData[4];
        }
        if (SHIP_DATA->velX < -shipData[4]) {
            SHIP_DATA->velX = -shipData[4];
        }
        if (SHIP_DATA->velY > shipData[4]) {
            SHIP_DATA->velY = shipData[4];
        }
        if (SHIP_DATA->velY < -shipData[4]) {
            SHIP_DATA->velY = -shipData[4];
        }
#endif
    }
    f28 = SHIP_DATA->velX;
    f27 = SHIP_DATA->velY;
    if (mario->controlStickSensitivity) {
        f26 = f30 * (f32)sin((3.1415927f * mario->controlStickAngle) / 180.0f);
        f29 = f30 * -(f32)cos((3.1415927f * mario->controlStickAngle) / 180.0f);
    } else {
        f26 = f28;
        f29 = f27;
    }
    f31 = shipData[4];
    if (f30 || SHIP_DATA->swirlTimer != 0) {
        if (f26 > f28) {
#ifdef MUST_MATCH
            // Named temp: the original computes the sum in f1 and the coefficient in f0.
            { f32 t = 1.0f + f26 - f28; f28 = t * shipData[3] + f28; }
#else
            f28 = (1.0f + f26 - f28) * shipData[3] + f28;
#endif
            if (f28 >= f31) {
                f28 = f31;
            }
        } else {
#ifdef MUST_MATCH
            { f32 t = f26 - 1.0f - f28; f28 = t * shipData[3] + f28; }
#else
            f28 = (f26 - 1.0f - f28) * shipData[3] + f28;
#endif
            if (f28 <= -f31) {
                f28 = -f31;
            }
        }
        if (f29 > f27) {
#ifdef MUST_MATCH
            { f32 t = 1.0f + f29 - f27; f27 = t * shipData[3] + f27; }
#else
            f27 = (1.0f + f29 - f27) * shipData[3] + f27;
#endif
            if (f27 >= f31) {
                f27 = f31;
            }
        } else {
#ifdef MUST_MATCH
            { f32 t = f29 - 1.0f - f27; f27 = t * shipData[3] + f27; }
#else
            f27 = (f29 - 1.0f - f27) * shipData[3] + f27;
#endif
            if (f27 <= -f31) {
                f27 = -f31;
            }
        }
    }
    if (!f30) {
        f32 f3 = shipData[0];
        if (*(void**)((u8*)mario + 0x1F0) != NULL) {
            f3 = shipData[1];
        }
        f3 = __fabsf(f3);
        f28 = -f28 * f3 + f28;
        f27 = -f27 * f3 + f27;
        if (__fabsf(f28) < 0.01f) {
            f28 = 0.0f;
        }
        if (__fabsf(f27) < 0.01f) {
            f27 = 0.0f;
        }
    }
    SHIP_DATA->velX = f28;
    SHIP_DATA->velY = f27;
    f29 = distABf(0.0f, 0.0f, f28, f27);
    f28 = 0.0f;
    if (f30) {
        if (f29 >= shipData[4]) {
            f29 = shipData[4];
        }
    } else {
        if (f29 < shipData[5]) {
            f29 = 0.0f;
            SHIP_DATA->velX = 0.0f;
            SHIP_DATA->velY = 0.0f;
        }
    }
    mario->baseSpeed = f29;
    if (f29) {
        mario->directionView = angleABf(0.0f, 0.0f, SHIP_DATA->velX, SHIP_DATA->velY);
    }
    if (f29 >= 1.0f) {
        MarioWork* m = marioGetPtr();
        if (marioGetPtr()->motStruct != NULL) {
            if (SHIP_DATA->sfxId == 0xFFFFFFFFu) {
                SHIP_DATA->sfxId = psndSFXOn_3D((char*)0x190, &m->position);
            }
        }
    } else if (f29 <= 0.2f) {
        marioGetPtr();
        if (marioGetPtr()->motStruct != NULL) {
            if (SHIP_DATA->sfxId != 0xFFFFFFFFu) {
                psndSFXOff(SHIP_DATA->sfxId);
                SHIP_DATA->sfxId = 0xFFFFFFFFu;
            }
        }
    }
    if (SHIP_DATA->sfxId != 0xFFFFFFFFu) {
        psndSFX_pos(SHIP_DATA->sfxId, &mario->position);
    }
    *(f32*)((u8*)mario + 0x18C) = f29;
}

s32 marioChkShipMoveMode(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_SHIP) {
        return 0;
    }
    if ((s32)mario->currSubMotionId >= 0x32 && (s32)mario->currSubMotionId <= 0x34) {
        return 1;
    }
    return 0;
}

void marioShipChgFall(void) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->flags & 0x20000)) {
        mario->flags |= 0x20000;
        *(f32*)((u8*)mario + 0x7C) = -0.1f;
        *(f32*)((u8*)mario + 0x80) = -0.2f;
        *(f32*)((u8*)mario + 0x84) = 0.024f;
        *(f32*)((u8*)mario + 0x88) = -0.004f;
    }
}

void mot_ship_post(void) {
    MarioWork* mario = marioGetPtr();

    allPartyRideOff();
    if (mario->dispFlags & 0x40000000) {
        marioPaperOff();
    }
    mario->wPlayerCollisionBox.x = 20.0f;
    mario->wPlayerCollisionBox.y = 37.0f;
    marioGetPtr();
    if (marioGetPtr()->motStruct != NULL) {
        if (((MotionShipData*)marioGetPtr()->motStruct)->sfxId != 0xFFFFFFFFu) {
            psndSFXOff(((MotionShipData*)marioGetPtr()->motStruct)->sfxId);
            ((MotionShipData*)marioGetPtr()->motStruct)->sfxId = 0xFFFFFFFFu;
        }
    }
    mario->dispFlags &= ~0x4;
    mario->dispFlags &= ~0x100;
    mario->dispFlags &= ~0x8;
    marioOfsRotReset();
    if (marioGetPtr()->motStruct != NULL) {
        __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
        marioGetPtr()->motStruct = NULL;
    }
}

void marioReInit_ship(void) {
    MarioWork* mario = marioGetPtr();
    MotionShipData* data;

    mario->trigFlags &= ~1;
    data = __memAlloc(HEAP_DEFAULT, 0x48);
    marioGetPtr()->motStruct = data;
    memset(marioGetPtr()->motStruct, 0, 0x48);
    mario->currMotionId = MARIO_MOTION_SHIP;
    *(f32*)((u8*)mario + 0x9C) = -15.0f;
    mario->currSubMotionId = 0x32;
    mario->wPlayerCollisionBox.x = 40.0f;
    mario->wPlayerCollisionBox.y = 40.0f;
    marioPaperOn(paper_ship[marioGetColor()]);
    marioChgPaper("PM_H_1A");
    marioChgPose("M_Z_1");
    mario->dispFlags |= 0x8;
    *(s32*)((u8*)mario + 0x2A4) = 0x58;
    // Header declares marioSetPaperAnimeLocalTime(s16); the original caller passes the full word
    // without sign-extending, so call through an s32 prototype to reproduce the codegen (see REPORT.md).
    ((void (*)(s32))marioSetPaperAnimeLocalTime)(*(s32*)((u8*)mario + 0x2A4));
    ((MotionShipData*)marioGetPtr()->motStruct)->sfxId = 0xFFFFFFFFu;
    ((MotionShipData*)marioGetPtr()->motStruct)->flags = 0;
    ((MotionShipData*)marioGetPtr()->motStruct)->unk04 = 0.0f;
    ((MotionShipData*)marioGetPtr()->motStruct)->velX = 0.0f;
    ((MotionShipData*)marioGetPtr()->motStruct)->velY = 0.0f;
    ((MotionShipData*)marioGetPtr()->motStruct)->unk1C = 0.0f;
    ((MotionShipData*)marioGetPtr()->motStruct)->unk20 = 0.0f;
    ((MotionShipData*)marioGetPtr()->motStruct)->unk08 = 0.0f;
    ((MotionShipData*)marioGetPtr()->motStruct)->unk0C = 4.25f;
    ((MotionShipData*)marioGetPtr()->motStruct)->eff =
        effFunemizuEntry(0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    *(u32*)((u8*)((MotionShipData*)marioGetPtr()->motStruct)->eff->userdata + 0x48) = mario->camId;
    allPartyForceRideOn();
}

void marioForceShipAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn(paper_ship[marioGetColor()]);
    marioChgPaper("PM_H_1B");
    marioChgPose("M_Z_1");
    mario->dispFlags |= 0x8;
}

s32 marioChkShipPlace(HitEntry* hit) {
    return (hitGetAttr(hit) & 0x3C000) != 0;
}

void marioShipSetDispDir(void) {
    marioGetPtr();
    ((MotionShipData*)marioGetPtr()->motStruct)->velX = 0.0f;
    ((MotionShipData*)marioGetPtr()->motStruct)->velY = 0.0f;
}

void marioShipForceStop(void) {
    marioGetPtr();
    ((MotionShipData*)marioGetPtr()->motStruct)->velX = 0.0f;
    ((MotionShipData*)marioGetPtr()->motStruct)->velY = 0.0f;
}

s32 marioChkSwirlPower(void) {
    marioGetPtr();
    return ((MotionShipData*)marioGetPtr()->motStruct)->swirlTimer != 0;
}

void marioSetSwirlPower(s32 msec, f32 a, f32 b) {
    marioGetPtr();
    ((MotionShipData*)marioGetPtr()->motStruct)->swirlTimer = sysMsec2Frame(msec);
    ((MotionShipData*)marioGetPtr()->motStruct)->unk2C = a;
    ((MotionShipData*)marioGetPtr()->motStruct)->unk28 = b;
}

s32 marioShipChanging(void) {
    MarioWork* mario = marioGetPtr();
    s32 id;

    if (mario->currMotionId != MARIO_MOTION_SHIP) {
        return 0;
    }
    id = (s32)mario->currSubMotionId;
    if (id == 0xA) {
        return 2;
    }
    if ((id >= 0 && id <= 0xC) || (id >= 0x1E && id <= 0x30)) {
        return 1;
    }
    if (id == 0x32 || id == 0x34) {
        return 3;
    }
    return 0;
}

s32 N_marioShipGetDir(void) {
    marioGetPtr();
    if (marioGetPtr()->motStruct != NULL) {
        return ((MotionShipData*)marioGetPtr()->motStruct)->flags & 2 ? 1 : 0;
    }
    return 0;
}

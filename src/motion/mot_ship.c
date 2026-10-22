#include "motion/mot_ship.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "mario/mario_cam.h"
#include "mario/mario_hit.h"
#include "mario/mario_motion.h"
#include "motion/mot_jump.h"
#include "mario/mariost.h"
#include "driver/hitdrv.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "manager/evtmgr.h"
#include "evt/evt_cmd.h"
#include "evt/evt_snd.h"
#include "pmario_sound.h"
#include "system.h"
#include "memory.h"
#include <string.h>
#include "party.h"
#include "eff/eff_funemizu.h"
#include "math_ext.h"

static EVT_BEGIN(sound_evt)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1_1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1_1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1_1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP1_1"), LW(0), LW(1), LW(2), 0)
RETURN
EVT_END()

EVT_BEGIN(sound_evt2)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP2"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP2"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP2"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_SHIP2"), LW(0), LW(1), LW(2), 0)
RETURN
EVT_END()

// Local declarations for symbols that have no home header yet

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

// Constants table used by shipMove (defined in this unit's .sdata).
f32 shipData[6] = {-0.05f, -0.7f, 0.02f, 0.02f, 2.0f, 0.01f};

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
s32 N_marioShipGetDir(void) {
    marioGetPtr();
    if (marioGetPtr()->motStruct != NULL) {
        return marioGetPtr()->motionShipData->flags & 2 ? 1 : 0;
    }
    return 0;
}

s32 marioShipChanging(void) {
    MarioWork* mario = marioGetPtr();
    s32 id;

    if (mario->currMotionId != MARIO_MOTION_SHIP) {
        return 0;
    }
    id = mario->currSubMotionId;
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
void marioSetSwirlPower(s32 msec, f32 a, f32 b) {
    marioGetPtr();
    marioGetPtr()->motionShipData->swirlTimer = sysMsec2Frame(msec);
    marioGetPtr()->motionShipData->unk2C = a;
    marioGetPtr()->motionShipData->unk28 = b;
}
s32 marioChkSwirlPower(void) {
    marioGetPtr();
    return marioGetPtr()->motionShipData->swirlTimer != 0;
}
void marioShipForceStop(void) {
    marioGetPtr();
    marioGetPtr()->motionShipData->velX = 0.0f;
    marioGetPtr()->motionShipData->velY = 0.0f;
}
void marioShipSetDispDir(void) {
    marioGetPtr();
    marioGetPtr()->motionShipData->velX = 0.0f;
    marioGetPtr()->motionShipData->velY = 0.0f;
}
s32 marioChkShipPlace(HitEntry* hit) {
    return (hitGetAttr(hit) & 0x3C000) != 0;
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

void marioReInit_ship(void) {
    MarioWork* mario = marioGetPtr();
    MotionShipData* data;

    mario->trigFlags &= ~1;
    data = __memAlloc(HEAP_DEFAULT, 0x48);
    marioGetPtr()->motStruct = data;
    memset(marioGetPtr()->motStruct, 0, 0x48);
    mario->currMotionId = MARIO_MOTION_SHIP;
    mario->dispOfs.y = -15.0f;
    mario->currSubMotionId = 0x32;
    mario->wPlayerCollisionBox.x = 40.0f;
    mario->wPlayerCollisionBox.y = 40.0f;
    marioPaperOn(paper_ship[marioGetColor()]);
    marioChgPaper("PM_H_1A");
    marioChgPose("M_Z_1");
    mario->dispFlags |= 0x8;
    mario->paperAnimeTime = 0x58;
    marioSetPaperAnimeLocalTime(mario->paperAnimeTime);
    marioGetPtr()->motionShipData->sfxId = 0xFFFFFFFFu;
    marioGetPtr()->motionShipData->flags = 0;
    marioGetPtr()->motionShipData->unk04 = 0.0f;
    marioGetPtr()->motionShipData->velX = 0.0f;
    marioGetPtr()->motionShipData->velY = 0.0f;
    marioGetPtr()->motionShipData->unk1C = 0.0f;
    marioGetPtr()->motionShipData->unk20 = 0.0f;
    marioGetPtr()->motionShipData->unk08 = 0.0f;
    marioGetPtr()->motionShipData->unk0C = 4.25f;
    marioGetPtr()->motionShipData->eff =
        effFunemizuEntry(0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    ((EffFunemizuData*)marioGetPtr()->motionShipData->eff->userdata)->camId = mario->camId;
    allPartyForceRideOn();
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
        if (marioGetPtr()->motionShipData->sfxId != 0xFFFFFFFFu) {
            psndSFXOff(marioGetPtr()->motionShipData->sfxId);
            marioGetPtr()->motionShipData->sfxId = 0xFFFFFFFFu;
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
// Moves the wake effect along with the ship. A stopped ship eases the wake strength toward
// a resting value that depends on its bob phase.
static inline void shipWakeUpdate(f32 speed) {
    MarioWork* m = marioGetPtr();

    if (!speed) {
        marioGetPtr();
        { s32 b; if (marioGetPtr()->motionShipData->unk08 > 180.0f) b = 0; else b = 1; speed = b ? 0.3f : 0.6f; }
        marioGetPtr()->motionShipData->unk10 += 0.1f * (speed - marioGetPtr()->motionShipData->unk10);
        speed = marioGetPtr()->motionShipData->unk10;
    }
    ((EffFunemizuData*)marioGetPtr()->motionShipData->eff->userdata)->x = m->position.x;
#ifdef MUST_MATCH
    {
        // Assign zero in the right operand to keep the original f0/f1 loads and fadds.
        f32 zero;
        ((EffFunemizuData*)marioGetPtr()->motionShipData->eff->userdata)->y = m->position.y + (zero = 0.0f);
    }
#else
    ((EffFunemizuData*)marioGetPtr()->motionShipData->eff->userdata)->y = m->position.y + 0.0f;
#endif
    ((EffFunemizuData*)marioGetPtr()->motionShipData->eff->userdata)->z = m->position.z;
    ((EffFunemizuData*)marioGetPtr()->motionShipData->eff->userdata)->speed = speed;
    ((EffFunemizuData*)marioGetPtr()->motionShipData->eff->userdata)->yaw = m->dispDir;
}

// Defined after shipMove, where the original's copies sit: MWCC inlines them into mot_ship
// (-inline auto,deferred) and still emits their bodies, which pool 15, 50, 30.30303 and 180
// between checkHarbor's and shipMove's constants. The linker strips the unreferenced bodies.
static s32 shipJumpDir(HitEntry* hit);
static void shipHarborOffset(Vec* out, s32 dir, f32 x, f32 z);
static s32 shipHarborTarget(HitEntry* hit, Vec* out);

void mot_ship(void) {
    MarioWork* mario = marioGetPtr();
    MotionShipData* data;
    s32 dir;
    f32 f30;
    f32 f31;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        data = __memAlloc(HEAP_DEFAULT, 0x48);
        marioGetPtr()->motStruct = data;
        memset(marioGetPtr()->motStruct, 0, 0x48);
        marioGetPtr()->motionShipData->sfxId = 0xFFFFFFFF;
        marioGetPtr()->motionShipData->unk20 = 0.0f;
        marioGetPtr()->motionShipData->unk08 = 0.0f;
        marioGetPtr()->motionShipData->unk0C = 4.25f;
        mario->currSubMotionId = 0;
        if (!(mario->trigFlags & 4)) {
            camGetPtr(CAMERA_2D)->flags |= 0x200;
        }
    }

    switch (mario->currSubMotionId) {
    case 0: {
        psndSFXOn_3D((char*)0x18D, &mario->position);
        psndSFXOn_3D((char*)0xAF, &mario->position);
        marioCamZoomUp();
        marioPreJump();
        marioChgPose("M_J_1B");
        camFollowYOff();
        mario->dispFlags |= 0x100;
        mario->multiTimer = 0;
        mario->currSubMotionId = 1;
        mario->airTimer = 0;
        mario->airTimerPeak = 0;
        mario->baseSpeed = 0.0f;
        marioGetPtr()->motionShipData->unk1C = 0.0f;
        N_marioSetBottomlessRespawnPosOnBeroEntry(mario->position.x, mario->position.y, mario->position.z);
        dir = shipJumpDir(mario->hitobjJumpFrom);
        marioGetPtr()->motionShipData->flags = dir;
    }
    /* fall through */
    case 1: {
        s32 wrap;

        marioMakeJumpPara();
        if (marioGetPtr()->motionShipData->flags & 1) {
            f31 = 30.0f;
        } else {
            f31 = -30.0f;
        }
        marioGetPtr()->motionShipData->unk1C += 0.125f * (f31 - marioGetPtr()->motionShipData->unk1C);
        if (marioGetPtr()->motionShipData->flags & 1) {
            if (marioGetPtr()->motionShipData->unk1C >= 30.0f) {
                marioGetPtr()->motionShipData->unk1C = 30.0f;
            }
        } else {
            if (marioGetPtr()->motionShipData->unk1C <= -30.0f) {
                marioGetPtr()->motionShipData->unk1C = -30.0f;
            }
        }
        mario->dispDir += marioGetPtr()->motionShipData->unk1C;
        wrap = 0;
        if (marioGetPtr()->motionShipData->flags & 1) {
            if (mario->dispDir >= 270.0f) {
                wrap = 1;
            }
        } else {
            if (mario->dispDir <= -270.0f) {
                wrap = 1;
            }
        }
        if (wrap) {
            mario->currSubMotionId = 2;
            mario->multiTimer = 0x14;
            marioPaperOn(paper_ship[marioGetColor()]);
            marioChgPaper("PM_H_1A");
            marioChgPose("M_Z_1");
            mario->dispFlags |= 8;
            marioGetPtr()->motionShipData->unk20 = 15.0f;
            {
                EventEntry* evt = evtEntry(sound_evt, 0, 0);
                evt->lwData[0] = (s32)(1024.0f * mario->position.x) - 0x0DB58580;
                evt->lwData[1] = (s32)(1024.0f * mario->position.y) - 0x0DB58580;
                evt->lwData[2] = (s32)(1024.0f * mario->position.z) - 0x0DB58580;
            }
        }
        if (mario->jumpVelY > 0.0f) {
            mario->position.y += mario->jumpVelY;
            mario->flags &= ~0x000F0000;
        }
        break;
    }
    case 2: {
        s32 wrap;

        f31 = marioGetPtr()->motionShipData->unk20 * ((marioGetPtr()->motionShipData->flags & 1) ? 1.0f : -1.0f);
        marioGetPtr()->motionShipData->unk1C += 0.125f * (f31 - marioGetPtr()->motionShipData->unk1C);
        mario->dispDir += marioGetPtr()->motionShipData->unk1C;
        wrap = 0;
        if (marioGetPtr()->motionShipData->flags & 1) {
            if (mario->dispDir >= 1260.0f) {
                wrap = 1;
            }
        } else {
            if (mario->dispDir <= -1080.0f) {
                wrap = 1;
            }
        }
        if (wrap) {
            mario->multiTimer = 0x28;
            mario->currSubMotionId = 3;
        }
        break;
    }
    case 3:
        if (mario->multiTimer == 0xE) {
            allPartyRideShip();
        }
        if (mario->multiTimer == 0x14) {
            marioCamZoomOffReq2(0x384);
            camFollowYOn();
        }
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 4;
        }
        if (mario->dispOfs.y > -15.0f) {
            mario->dispOfs.y -= 1.0f;
            if (mario->dispOfs.y <= -15.0f) {
                mario->dispOfs.y = -15.0f;
            }
        }
        /* fall through */
    case 4: {
        s32 wrap;

        f31 = marioGetPtr()->motionShipData->unk20 * ((marioGetPtr()->motionShipData->flags & 1) ? 1.0f : -1.0f);
        marioGetPtr()->motionShipData->unk1C += 0.125f * (f31 - marioGetPtr()->motionShipData->unk1C);
        mario->dispDir += marioGetPtr()->motionShipData->unk1C;
        marioGetPtr()->motionShipData->unk20 -= 0.1f;
        if (marioGetPtr()->motionShipData->unk20 <= 1.0f) {
            marioGetPtr()->motionShipData->unk20 = 1.0f;
        }
        wrap = 0;
        if (marioGetPtr()->motionShipData->flags & 1) {
            if (mario->dispDir >= 1620.0f) {
                mario->dispDir = 1620.0f;
                wrap = 1;
            }
        } else {
            if (mario->dispDir <= -1440.0f) {
                mario->dispDir = -1440.0f;
                wrap = 1;
            }
        }
        if (wrap && mario->currSubMotionId == 4) {
            mario->currSubMotionId = 0xA;
        }
        break;
    }
    case 10: {
        mario->dispFlags &= ~0x100;
        mario->dispDir = revise360(mario->dispDir);
        mario->wPlayerCollisionBox.x = 40.0f;
        mario->wPlayerCollisionBox.y = 40.0f;
        dir = shipHarborTarget(mario->hitobjJumpFrom, &marioGetPtr()->motionShipData->unk3C);
        {
            f30 = 0.1f * distABf(marioGetPtr()->motionShipData->unk3C.x, marioGetPtr()->motionShipData->unk3C.z, mario->position.x, mario->position.z);
            switch (dir) {
            case 1:
                marioGetPtr()->motionShipData->unk3C.z -= f30;
                break;
            case 2:
                marioGetPtr()->motionShipData->unk3C.z += f30;
                break;
            case 3:
                marioGetPtr()->motionShipData->unk3C.x -= f30;
                break;
            case 4:
                marioGetPtr()->motionShipData->unk3C.x += f30;
                break;
            default:
                break;
            }
        }
        {
            f32 dy;
            mario->baseSpeed = distABf(mario->position.x, mario->position.z, marioGetPtr()->motionShipData->unk3C.x, marioGetPtr()->motionShipData->unk3C.z) / 36.0f;
            mario->directionView = angleABf(mario->position.x, mario->position.z, marioGetPtr()->motionShipData->unk3C.x, marioGetPtr()->motionShipData->unk3C.z);
            mario->moveDir = mario->directionView;
            f30 = marioGetPtr()->motionShipData->unk3C.y;
            marioGetPtr()->motionShipData->unk04 = f30;
            // The distance is doubled as `d + d`, not `2.0f * d`.
            dy = marioGetPtr()->motionShipData->unk3C.y - mario->position.y;
            mario->jumpVelY = 0.0f;
            mario->jumpAccelY = (dy + dy) / 306.0f;
            mario->jumpJerkY = 0.0f;
            mario->jumpJerkStepY = 0.0f;
            mario->multiTimer = 0x24;
            mario->currSubMotionId = 0xB;
        }
        break;
    }
    case 11: {
        s32 landed;
        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        mario->position.y += mario->jumpVelY;
        marioMakeJumpPara();
        f30 = mario->jumpVelY;
        {
            MarioWork* m = marioGetPtr();
            f32 sp0C;
            void* hit;
            f32 dist;
            f32 height;
            m->wPlayerCollisionBox.x = 20.0f;
            hit = marioChkLandon(&sp0C, f30);
            m->wPlayerCollisionBox.x = 40.0f;
            if (hit != NULL && ((height = m->position.y - sp0C) <= 0.0f || height + f30 <= 0.0f)) {
                m->hitobjStandOn = hit;
                m->position.y = sp0C;
                m->flags &= ~0x000F0000;
                dist = distABf(m->position.x, m->position.z, m->field_0x11C.x, m->field_0x11C.z);
                m->field_0x128 = __fabsf(dist);
                m->flags &= ~0x80;
                m->airTimer = 0;
                m->airTimerPeak = 0;
                camFollowYOn();
                landed = 1;
            } else {
                landed = 0;
            }
        }
        if (landed) {
            mario->jumpVelY = 0.0f;
            mario->jumpAccelY = 0.0f;
            mario->jumpJerkY = 0.0f;
            mario->jumpJerkStepY = 0.0f;
            mario->baseSpeed = 0.0f;
            marioGetPtr()->motionShipData->unk1C = 0.0f;
            mario->flags &= ~0x000F0000;
            mario->dispDir = revise360(mario->dispDir);
            mario->dispReqDir = mario->dispDir;
            mario->directionView = toMovedir(mario->dispDir);
            mario->moveDir = mario->directionView;
            mario->dispFlags &= ~0x100;
            mario->dispFlags |= 0x1000;
            psndSFXOn_3D((char*)0x18F, &mario->position);
            marioGetPtr()->motionShipData->eff = effFunemizuEntry(0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            ((EffFunemizuData*)marioGetPtr()->motionShipData->eff->userdata)->camId = mario->camId;
            marioGetPtr()->motionShipData->unk08 = 0.0f;
            marioGetPtr()->motionShipData->unk0C = 4.25f;
            mario->currSubMotionId = 0x32;
            if (!(mario->trigFlags & 4)) {
                camGetPtr(CAMERA_2D)->flags &= ~0x200;
            }
        }
        break;
    }
    case 30:
        if (!(mario->trigFlags & 4)) {
            camGetPtr(CAMERA_2D)->flags |= 0x200;
        }
        mario->currSubMotionId = 0x1F;
        /* fall through */
    case 31:
        mario->dispOfs.y += -0.4f;
        if (mario->dispOfs.y <= -18.0f) {
            mario->multiTimer = 4;
            mario->currSubMotionId = 0x20;
        }
        shipWakeUpdate(1.6f);
        break;
    case 32:
        if (--mario->multiTimer > 0) {
            break;
        }
        mario->currSubMotionId = 0x28;
        /* fall through */
    case 40: {
        marioGetPtr();
        if (marioGetPtr()->motStruct != NULL && marioGetPtr()->motionShipData->sfxId != 0xFFFFFFFF) {
            psndSFXOff(marioGetPtr()->motionShipData->sfxId);
            marioGetPtr()->motionShipData->sfxId = 0xFFFFFFFF;
        }
        camFollowYOff();
        mario->dispFlags &= ~0x1000;
        {
            marioGetPtr()->motionShipData->unk04 = marioGetPtr()->motionShipData->unk3C.y;
            mario->baseSpeed = 2.8f;
            {
                // Keeping the rounding factor first in the expression tree pools 10 before 2.
                s32 n = (s32)((10.0f * ((2.0f * distABf(mario->position.x, mario->position.z,
                    marioGetPtr()->motionShipData->unk3C.x, marioGetPtr()->motionShipData->unk3C.z)) / mario->baseSpeed) + 0.5f) / 10.0f);
                f32 f6;
                mario->directionView = angleABf(mario->position.x, mario->position.z, marioGetPtr()->motionShipData->unk3C.x, marioGetPtr()->motionShipData->unk3C.z);
                mario->moveDir = mario->directionView;
                marioGetPtr()->motionShipData->flags = ((mario->trigFlags & 0x200) ? 1 : 0) + 1;
                f6 = 0.5f * (f32)n;
                f30 = 80.0f / (f6 * f6 - f6);
                mario->jumpVelY = f30 * f6;
                mario->jumpAccelY = -f30;
                mario->jumpJerkY = 0.0f;
                mario->jumpJerkStepY = 0.0f;
                mario->multiTimer = (s16)(s32)(0.5f + f6);
                mario->currSubMotionId = 0x29;
                mario->field_0x2B0 = __fabsf(mario->dispOfs.y / (f32)(mario->multiTimer + 0x58));
                effDelete(marioGetPtr()->motionShipData->eff);
            }
        }
        break;
    }
    case 41: {
        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        mario->position.y += mario->jumpVelY;
        marioMakeJumpPara();
        mario->dispOfs.y += mario->field_0x2B0;
        if (mario->dispOfs.y > 0.0f) {
            mario->dispOfs.y = 0.0f;
        }
        if (--mario->multiTimer <= 0) {
            marioChgPaper("PM_H_1A");
            marioChgPose("M_Z_1");
            mario->paperAnimeTime = 0x58;
            marioSetPaperAnimeLocalTime(mario->paperAnimeTime);
            marioGetPtr()->motionShipData->unk1C = 0.0f;
            mario->dispFlags |= 0x100;
            marioAdjustMoveDir();
            mario->currSubMotionId = 0x2B;
            psndSFXOn_3D((char*)0x192, &mario->position);
            {
                EventEntry* evt = evtEntry(sound_evt2, 0, 0);
                evt->lwData[0] = (s32)(1024.0f * mario->position.x) - 0x0DB58580;
                evt->lwData[1] = (s32)(1024.0f * mario->position.y) - 0x0DB58580;
                evt->lwData[2] = (s32)(1024.0f * mario->position.z) - 0x0DB58580;
            }
        }
        break;
    }
    case 43: {
        mario->dispOfs.y += mario->field_0x2B0;
        if (mario->dispOfs.y > 0.0f) {
            mario->dispOfs.y = 0.0f;
        }
        marioSetPaperAnimeLocalTime(mario->paperAnimeTime);
        if (--mario->paperAnimeTime >= 0x2C) {
            f30 = (marioGetPtr()->motionShipData->flags & 1) ? 20.0f : -20.0f;
            marioGetPtr()->motionShipData->unk1C += 0.125f * (f30 - marioGetPtr()->motionShipData->unk1C);
            if (marioGetPtr()->motionShipData->flags & 1) {
                if (marioGetPtr()->motionShipData->unk1C >= 20.0f) {
                    marioGetPtr()->motionShipData->unk1C = 20.0f;
                }
            } else {
                if (marioGetPtr()->motionShipData->unk1C <= -20.0f) {
                    marioGetPtr()->motionShipData->unk1C = -20.0f;
                }
            }
            mario->dispDir += marioGetPtr()->motionShipData->unk1C;
            if (mario->paperAnimeTime == 0x2C) {
                mario->currSubMotionId = 0x2C;
            }
        }
        break;
    }
    case 44:
        mario->dispOfs.y += mario->field_0x2B0;
        if (mario->dispOfs.y > 0.0f) {
            mario->dispOfs.y = 0.0f;
        }
        marioSetPaperAnimeLocalTime(mario->paperAnimeTime);
        if (--mario->paperAnimeTime <= 0) {
            mario->paperAnimeTime = 0;
            mario->currSubMotionId = 0x2D;
            mario->multiTimer = 6;
            mario->dispDir = revise360(mario->dispDir);
            if (marioGetPtr()->motionShipData->flags & 2) {
                mario->dispDir -= 360.0f;
            }
        }
        mario->dispDir += marioGetPtr()->motionShipData->unk1C;
        break;
    case 45:
        marioSetPaperAnimeLocalTime(mario->paperAnimeTime);
        f30 = marioGetPtr()->motionShipData->unk20 * ((marioGetPtr()->motionShipData->flags & 1) ? 1.0f : -1.0f);
        {
            marioGetPtr()->motionShipData->unk1C += 0.05f * (f30 - marioGetPtr()->motionShipData->unk1C);
        }
        mario->dispDir += marioGetPtr()->motionShipData->unk1C;
        marioGetPtr()->motionShipData->unk20 += -0.005f;
        if (marioGetPtr()->motionShipData->unk20 <= 10.0f) {
            marioGetPtr()->motionShipData->unk20 = 10.0f;
        }
        if (--mario->multiTimer > 0) {
            break;
        }
        /* fall through */
    case 46: {
        s32 r27;
        f30 = marioGetPtr()->motionShipData->unk20 * ((marioGetPtr()->motionShipData->flags & 1) ? 1.0f : -1.0f);
        {
            marioGetPtr()->motionShipData->unk1C += 0.05f * (f30 - marioGetPtr()->motionShipData->unk1C);
        }
        mario->dispDir += marioGetPtr()->motionShipData->unk1C;
        marioGetPtr()->motionShipData->unk20 += -0.005f;
        if (marioGetPtr()->motionShipData->unk20 <= 10.0f) {
            marioGetPtr()->motionShipData->unk20 = 10.0f;
        }
        r27 = 0;
        if (marioGetPtr()->motionShipData->flags & 1) {
            if (mario->dispDir >= 360.0f) {
                mario->dispDir = revise360(mario->dispDir);
                r27 = 1;
            }
        } else {
            if (mario->dispDir <= -360.0f) {
                mario->dispDir = revise360(mario->dispDir);
                r27 = 1;
            }
        }
        if (!r27) {
            break;
        }
        mario->dispDir = revise360(mario->dispDir);
        if (marioGetPtr()->motionShipData->flags & 2) {
            mario->dispDir -= 360.0f;
        }
        marioPaperOff();
        mario->dispFlags &= ~8;
        marioChgPose("M_J_1B");
        mario->currSubMotionId = 0x2F;
        marioGetPtr()->motionShipData->unk20 = 10.0f;
        /* fall through */
    }
    case 47: {
        s32 r27 = 0;
        marioGetPtr();
        marioGetPtr();
        f30 = (marioGetPtr()->motionShipData->flags & 1) ? 4.0f : -4.0f;
        {
            marioGetPtr()->motionShipData->unk1C += 0.05f * (f30 - marioGetPtr()->motionShipData->unk1C);
        }
        mario->dispDir += marioGetPtr()->motionShipData->unk1C;
        if (marioGetPtr()->motionShipData->flags & 1) {
            if (mario->dispDir >= 540.0f) {
                mario->dispDir = 540.0f;
                r27 = 1;
            }
        } else {
            if (mario->dispDir <= -360.0f) {
                mario->dispDir = -360.0f;
                r27 = 1;
            }
        }
        if (mario->currSubMotionId == 0x2F && r27) {
            mario->dispDir = revise360(mario->dispDir);
            mario->dispReqDir = mario->dispDir;
            mario->moveDir = toMovedir(mario->dispDir);
            mario->directionView = mario->moveDir;
            mario->dispFlags &= ~0x100;
            mario->wPlayerCollisionBox.x = 20.0f;
            mario->wPlayerCollisionBox.y = 37.0f;
            mario->jumpVelY = 0.0f;
            mario->jumpAccelY = 0.0f;
            mario->jumpJerkY = 0.0f;
            mario->jumpJerkStepY = 0.0f;
            mario->baseSpeed = 0.0f;
            mario->flags &= ~0x000F0000;
            mario->dispOfs.y = 0.0f;
            marioChgMot(0xA);
            marioSetFallPara();
            allPartyRideOff();
            N_marioSetBottomlessRespawnPosOnBeroEntry(mario->position.x, mario->position.y, mario->position.z);
            N_marioReloadMapOnBottomlessOff();
            if (marioGetPtr()->motStruct != NULL) {
                __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
                marioGetPtr()->motStruct = NULL;
            }
            if (!(mario->trigFlags & 4)) {
                camGetPtr(CAMERA_2D)->flags &= ~0x200;
            }
        }
        break;
    }
    case 50:
        mario->currSubMotionId = 0x34;
        /* fall through */
    case 52: {
        if (mario->hitobjStandOn != NULL) {
            f31 = revise360(marioGetPtr()->motionShipData->unk08 - marioGetPtr()->motionShipData->unk0C);
            marioGetPtr()->motionShipData->unk08 = f31;
            mario->dispOfs.y = 3.0f * (f32)sin(3.1415927f * marioGetPtr()->motionShipData->unk08 / 180.0f) + -13.0f;
        }
        if (mario->flags & 0x20) {
            shipWakeUpdate(mario->baseSpeed);
        } else {
        if (mario->hitobjStandOn == NULL) {
            mario->wStickDir1 = 0;
            mario->wStickDir2 = 0;
        }
        shipMove();
        if (mario->flags & 0x20000) {
            marioMakeJumpPara();
            if (mario->jumpVelY <= -7.0f) {
                mario->jumpVelY = -7.0f;
            }
            f30 = mario->jumpVelY;
            {
                MarioWork* m = marioGetPtr();
                f32 sp08;
                s32 landed;
                void* hit;
                f32 dist;
                f32 height;
                m->wPlayerCollisionBox.x = 20.0f;
                hit = marioChkLandon(&sp08, f30);
                m->wPlayerCollisionBox.x = 40.0f;
                if (hit != NULL && ((height = m->position.y - sp08) <= 0.0f || height + f30 <= 0.0f)) {
                    m->hitobjStandOn = hit;
                    m->position.y = sp08;
                    m->flags &= ~0x000F0000;
                    dist = distABf(m->position.x, m->position.z, m->field_0x11C.x, m->field_0x11C.z);
                    m->field_0x128 = __fabsf(dist);
                    m->flags &= ~0x80;
                    m->airTimer = 0;
                    m->airTimerPeak = 0;
                    camFollowYOn();
                    landed = 1;
                } else {
                    landed = 0;
                }
                if (!landed) {
                    mario->position.y += mario->jumpVelY;
                }
            }
        } else {
            marioClearJumpPara();
        }
        if (strcmp(gp->currentMapName, "dou_03") == 0 && mario->position.y <= 260.0f && mario->position.y >= 50.0f) {
            if ((f32)(s32)(10.0f * mario->slopeAngle + 0.5f) / 10.0f != 0.0f) {
                mario->position.y += 2.0f * (f32)sin(3.1415927f * mario->slopeAngle / 180.0f);
                { f30 = (f32)cos(3.1415927f * mario->slopeAngle / 180.0f); mario->position.z += 2.0f * -f30; }
            }
        }
        mario->hitobjFront = NULL;
        mario->actionHitObj = NULL;
        mario->hitobjPush = NULL;
        mario->hitobjWall = NULL;
        marioCheckWallShip(mario->baseSpeed, mario->directionView);
        if (mario->baseSpeed < 0.5f) {
            mario->baseSpeed = 0.0f;
            mario->currSubMotionId = 0x32;
        }
        shipWakeUpdate(mario->baseSpeed);
        if ((mario->buttonNew & 0xA00) != 0) {
            if (checkHarbor(&marioGetPtr()->motionShipData->unk3C)) {
                mario->currSubMotionId = 0x1E;
            }
        }
        }
        break;
    }
    default:
        break;
    }
}

void marioShipChgFall(void) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->flags & 0x20000)) {
        mario->flags |= 0x20000;
        mario->jumpVelY = -0.1f;
        mario->jumpAccelY = -0.2f;
        mario->jumpJerkY = 0.024f;
        mario->jumpJerkStepY = -0.004f;
    }
}
s32 marioChkShipMoveMode(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_SHIP) {
        return 0;
    }
    if (mario->currSubMotionId >= 0x32 && mario->currSubMotionId <= 0x34) {
        return 1;
    }
    return 0;
}
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
    if (marioGetPtr()->motionShipData->swirlTimer != 0) {
        marioGetPtr()->motionShipData->swirlTimer--;
        movePos(&sp0C, &sp08, marioGetPtr()->motionShipData->unk2C, marioGetPtr()->motionShipData->unk28);
        marioGetPtr()->motionShipData->velX += sp0C;
        marioGetPtr()->motionShipData->velY += sp08;
#ifdef MUST_MATCH
        ftmp = shipData[4];
        if (marioGetPtr()->motionShipData->velX > ftmp) {
            marioGetPtr()->motionShipData->velX = shipData[4];
        }
        if (marioGetPtr()->motionShipData->velX < -shipData[4]) {
            marioGetPtr()->motionShipData->velX = -shipData[4];
        }
        ftmp = shipData[4];
        if (marioGetPtr()->motionShipData->velY > ftmp) {
            marioGetPtr()->motionShipData->velY = shipData[4];
        }
        if (marioGetPtr()->motionShipData->velY < -shipData[4]) {
            marioGetPtr()->motionShipData->velY = -shipData[4];
        }
#else
        if (marioGetPtr()->motionShipData->velX > shipData[4]) {
            marioGetPtr()->motionShipData->velX = shipData[4];
        }
        if (marioGetPtr()->motionShipData->velX < -shipData[4]) {
            marioGetPtr()->motionShipData->velX = -shipData[4];
        }
        if (marioGetPtr()->motionShipData->velY > shipData[4]) {
            marioGetPtr()->motionShipData->velY = shipData[4];
        }
        if (marioGetPtr()->motionShipData->velY < -shipData[4]) {
            marioGetPtr()->motionShipData->velY = -shipData[4];
        }
#endif
    }
    f28 = marioGetPtr()->motionShipData->velX;
    f27 = marioGetPtr()->motionShipData->velY;
    if (mario->controlStickSensitivity) {
        f26 = f30 * (f32)sin((3.1415927f * mario->controlStickAngle) / 180.0f);
        f29 = f30 * -(f32)cos((3.1415927f * mario->controlStickAngle) / 180.0f);
    } else {
        f26 = f28;
        f29 = f27;
    }
    f31 = shipData[4];
    if (f30 || marioGetPtr()->motionShipData->swirlTimer != 0) {
        if (f26 > f28) {
            // The sum goes in f1 and the coefficient in f0.
            { f32 t = 1.0f + f26 - f28; f28 = t * shipData[3] + f28; }
            if (f28 >= f31) {
                f28 = f31;
            }
        } else {
            { f32 t = f26 - 1.0f - f28; f28 = t * shipData[3] + f28; }
            if (f28 <= -f31) {
                f28 = -f31;
            }
        }
        if (f29 > f27) {
            { f32 t = 1.0f + f29 - f27; f27 = t * shipData[3] + f27; }
            if (f27 >= f31) {
                f27 = f31;
            }
        } else {
            { f32 t = f29 - 1.0f - f27; f27 = t * shipData[3] + f27; }
            if (f27 <= -f31) {
                f27 = -f31;
            }
        }
    }
    if (!f30) {
        f32 f3 = shipData[0];
        if (mario->hitobjWall != NULL) {
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
    marioGetPtr()->motionShipData->velX = f28;
    marioGetPtr()->motionShipData->velY = f27;
    f29 = distABf(0.0f, 0.0f, f28, f27);
    f28 = 0.0f;
    if (f30) {
        if (f29 >= shipData[4]) {
            f29 = shipData[4];
        }
    } else {
        if (f29 < shipData[5]) {
            f29 = 0.0f;
            marioGetPtr()->motionShipData->velX = 0.0f;
            marioGetPtr()->motionShipData->velY = 0.0f;
        }
    }
    mario->baseSpeed = f29;
    if (f29) {
        mario->directionView = angleABf(0.0f, 0.0f, marioGetPtr()->motionShipData->velX, marioGetPtr()->motionShipData->velY);
    }
    if (f29 >= 1.0f) {
        MarioWork* m = marioGetPtr();
        if (marioGetPtr()->motStruct != NULL) {
            if (marioGetPtr()->motionShipData->sfxId == 0xFFFFFFFFu) {
                marioGetPtr()->motionShipData->sfxId = psndSFXOn_3D((char*)0x190, &m->position);
            }
        }
    } else if (f29 <= 0.2f) {
        marioGetPtr();
        if (marioGetPtr()->motStruct != NULL) {
            if (marioGetPtr()->motionShipData->sfxId != 0xFFFFFFFFu) {
                psndSFXOff(marioGetPtr()->motionShipData->sfxId);
                marioGetPtr()->motionShipData->sfxId = 0xFFFFFFFFu;
            }
        }
    }
    if (marioGetPtr()->motionShipData->sfxId != 0xFFFFFFFFu) {
        psndSFX_pos(marioGetPtr()->motionShipData->sfxId, &mario->position);
    }
    mario->field_0x18C = f29;
}

// Turn direction for boarding the ship: from Mario's angle to the harbor object for the first
// two harbor attributes, fixed for the other two.
static s32 shipJumpDir(HitEntry* hit) {
    MarioWork* m = marioGetPtr();
    s32 attr = hitGetAttr(hit);
    Vec pos2;
    Vec pos1;
    s32 dir = 0;
    if (attr & 0x4000) {
        f32 a;
        hitObjGetPos(hitGetName(m->hitobjJumpFrom), &pos2);
        a = angleABf(m->position.x, m->position.z, pos2.x, pos2.z);
        a = revise360(a - m->targetYaw);
        dir = (a >= 180.0f) ? 2 : 1;
    } else if (attr & 0x8000) {
        f32 a;
        hitObjGetPos(hitGetName(m->hitobjJumpFrom), &pos1);
        a = angleABf(m->position.x, m->position.z, pos1.x, pos1.z);
        a = revise360(a - m->targetYaw);
        dir = (a >= 180.0f) ? 2 : 1;
    } else if (attr & 0x10000) {
        dir = 2;
    } else if (attr & 0x20000) {
        dir = 1;
    }
    return dir;
}

// Moves the docking point out in front of the harbor on its open side.
static void shipHarborOffset(Vec* out, s32 dir, f32 x, f32 z) {
    if (dir == 1) {
        out->x = x;
        out->z = z - 50.0f - 30.30303f;
    } else if (dir == 2) {
        out->x = x;
        out->z = 30.30303f + (50.0f + z);
    } else if (dir == 3) {
        out->x = (x - 50.0f) - 30.30303f;
        out->z = z;
    } else if (dir == 4) {
        out->x = 30.30303f + (50.0f + x);
        out->z = z;
    } else {
        out->x = x;
        out->z = z;
    }
}

// Docking point for the harbor Mario jumped from (rounded to 0.01) and the harbor side.
static s32 shipHarborTarget(HitEntry* hit, Vec* out) {
    MarioWork* m = marioGetPtr();
    Vec pos;
    s32 attr;
    s32 dir;
    f32 fx;
    f32 fz;

    hitObjGetPos(hitGetName(hit), &pos);
    dir = 0;
    fx = (f32)(s32)((100.0f * pos.x + 0.5f) / 100.0f);
    fz = (f32)(s32)((100.0f * pos.z + 0.5f) / 100.0f);
    out->y = m->position.y - 15.0f;
    attr = hitGetAttr(m->hitobjJumpFrom);
    if (attr & 0x4000) {
        dir = 1;
    } else if (attr & 0x8000) {
        dir = 2;
    } else if (attr & 0x10000) {
        dir = 3;
    } else if (attr & 0x20000) {
        dir = 4;
    }
    shipHarborOffset(out, dir, fx, fz);
    return dir;
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

    sincosf(&sp28, &sp24, toMovedir(mario->dispDir));
    x = mario->position.x + 40.0f * sp28;
    z = mario->position.z + 40.0f * sp24;
    y = mario->position.y + 20.0f;
    sp14 = 40.0f;
    hit = hitCheckFilter(NULL, x, y, z, 0.0f, -1.0f, 0.0f, &sp20, &sp1C, &sp18, &sp14, &sp10, &sp0C,
                        &sp08);
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
    sincosf(&sp28, &sp24, toMovedir(mario->dispDir + 90.0f));
    x = mario->position.x + 40.0f * sp28;
    z = mario->position.z + 40.0f * sp24;
    y = mario->position.y + 20.0f;
    sp14 = 40.0f;
    hit = hitCheckFilter(NULL, x, y, z, 0.0f, -1.0f, 0.0f, &sp20, &sp1C, &sp18, &sp14, &sp10, &sp0C,
                        &sp08);
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
    sincosf(&sp28, &sp24, toMovedir(mario->dispDir - 90.0f));
    x = mario->position.x + 40.0f * sp28;
    z = mario->position.z + 40.0f * sp24;
    y = mario->position.y + 20.0f;
    sp14 = 40.0f;
    hit = hitCheckFilter(NULL, x, y, z, 0.0f, -1.0f, 0.0f, &sp20, &sp1C, &sp18, &sp14, &sp10, &sp0C,
                        &sp08);
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


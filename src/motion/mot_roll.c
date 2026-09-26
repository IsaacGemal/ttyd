#include "dolphin/types.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_sbr.h"
#include "pmario_sound.h"
#include "memory.h"
#include "driver/camdrv.h"
#include <string.h>
#include <math.h>

// locals for symbols with no home header (see REPORT.md)
void* marioChkLandon(f32* pos, f32 spd);
f32 marioGetFallSpd(void);
void marioLandOn(void);
void marioClearJumpPara(void);
void marioGetRubInit(void);
BOOL marioSlitChkWallAround(void);
void marioChgMotSub(s32 motion, s32 sub);
void allPartyRideShip(void);
void allPartyRideOff(void);
void allPartyForceRideOn(void);
void camFollowYOff(void);
extern s32 roll_flyout_flag2;
extern f32 rollData;

void mot_fall_roll(void) {
    MarioWork* mario = marioGetPtr();
    f32 spd = marioGetFallSpd();
    f32 y;
    void* hit;

    if (spd < -10.0f) {
        spd = -10.0f;
    }
    hit = marioChkLandon(&y, spd);
    if (hit != NULL) {
        mario->position.y = y;
        mario->hitobjStandOn = hit;
        mario->hitobjJumpFrom = NULL;
    } else {
        mario->position.y += spd;
    }
    if (mario->hitobjStandOn != NULL) {
        marioLandOn();
        roll_flyout_flag2 = 0;
    }
}

void roll_upstairs(void) {
    MarioWork* mario = marioGetPtr();

    mario->position.y += *(f32*)((u8*)mario + 0x7C);
    if ((s32)--mario->multiTimer <= 0) {
        mario->flags &= ~0x80;
        mario->flags &= ~0x40;
        mario->flags &= ~0x40000;
    }
}

void roll_upstairs_on(void) {
    MarioWork* mario = marioGetPtr();

    mario->flags |= 0x40000;
    mario->flags |= 0x80;
    marioClearJumpPara();
    mario->multiTimer = 3;
    {
        u32 t = mario->multiTimer;
        *(f32*)((u8*)mario + 0x7C) =
            (*(f32*)((u8*)mario + 0x1C4) - mario->position.y) / (f32)(s32)t;
    }
    *(f32*)((u8*)mario + 0x7C) = 4.0f;
    *(f32*)((u8*)mario + 0x80) = -0.42f;
    *(f32*)((u8*)mario + 0x84) = 0.04f;
    *(f32*)((u8*)mario + 0x88) = -0.01f;
}
void marioReInit_roll(void) {
    MarioWork* mario = marioGetPtr();
    void* mem;

    mem = __memAlloc(HEAP_DEFAULT, 0x14);
    marioGetPtr()->motStruct = mem;
    memset(marioGetPtr()->motStruct, 0, 0x14);
    {
        MarioWork* m = marioGetPtr();
        *(s32*)((u8*)m + 0x2A8) = -1;
    }
    mario->trigFlags &= ~1;
    mario->currMotionId = 0x16;
    mario->flags |= 0x01000000;
    mario->field_0x8 = 0x10;
    marioPaperOn("p_roll");
    marioChgPose("M_I_U");
    marioChgPaper("PM_R_1B");
    mario->dispFlags |= 0x8;
    mario->dispFlags |= 0x100;
    mario->currSubMotionId = 0x1E;
    mario = marioGetPtr();
    mario->wPlayerCollisionBox.x = 14.8f;
    mario->wPlayerCollisionBox.y = 14.8f;
    mario->mBaseDashSpeed = rollData;
    *(f32*)((u8*)marioGetPtr()->motStruct + 0x0) = 0.0f;
    *(f32*)((u8*)marioGetPtr()->motStruct + 0x4) = 0.0f;
    *(f32*)((u8*)mario + 0xB0) = -6.0f;
    *(f32*)((u8*)mario + 0xB4) = 6.0f;
    *(f32*)((u8*)mario + 0x98) = 6.0f;
    *(f32*)((u8*)mario + 0x9C) = 0.0f;
    *(f32*)((u8*)mario + 0xC8) = 1.0f;
    *(f32*)((u8*)mario + 0xCC) = 1.0f;
    *(f32*)((u8*)mario + 0xD0) = 1.0f;
    *(f32*)((u8*)mario + 0x130) = 1.0f;
    allPartyForceRideOn();
}

void rollEvtJumpSetup(void) {
    MarioWork* mario = marioGetPtr();

    camFollowYOff();
    mario->flags |= 0x10000;
    mario->airTimer = 1;
}

// NONMATCHING (part of marioSetRollDispDir, 94%): helper inlined by MWCC; the constants
// (135.0f/90.0f) are CSE'd across the inlined copies here, whereas the original reloads them.
static s32 rollDispAngleInRange(f32 angle, f32 ref, f32 range) {
    f32 a = (f32)(s32)(10.0f * angle) / 10.0f;
    f32 lo = a - range;
    f32 hi = a + range;
    s32 kind = 0;
    s32 res = 0;

    if (lo < 0.0f) {
        lo += 360.0f;
        kind = 1;
    }
    if (hi > 360.0f) {
        hi -= 360.0f;
        kind = 2;
    }
    if (kind == 1) {
        if (hi < ref && ref < lo) {
            res = 1;
        }
    } else if (kind == 2) {
        if (hi < ref && ref < lo) {
            res = 1;
        }
    } else if (kind == 0) {
        if (ref < lo || hi < ref) {
            res = 1;
        }
    }
    return res;
}

// NONMATCHING (part of marioSetRollDispDir, 94%): helper inlined by MWCC.
static f32 rollDispCalcTarget(MarioWork* mario, f32 angle) {
    f32 res = 270.0f + mario->targetYaw - angle;
    f32 off;

    if (*(s8*)((u8*)mario + 0x43) != 0) {
        off = -180.0f;
    } else {
        off = 0.0f;
    }
    res += off;
    return revise360(res);
}

// NONMATCHING (94.25%): logic transcribed from asm. Differences: `fabsf(f31-f30)` is commuted to
// `f30-f31` by MWCC, and the 135.0f/90.0f constants are CSE'd across inlined copies (original
// reloads them from sda21). Two `bne`/`beq` inversions remain.
void marioSetRollDispDir(void) {
    MarioWork* mario = marioGetPtr();
    f32 f30 = mario->directionView;
    f32 f31 = *(f32*)((u8*)mario + 0x19C);
    MarioWork* mario2 = marioGetPtr();

    if (rollDispAngleInRange(f31, f30, 135.0f)) {
        f31 = revise360(180.0f + f31);
    }
    {
        s32 flg;
        f32 diff;

        if (revise360((f32)(s32)(10.0f * f31) / 10.0f -
                      (f32)(s32)(10.0f * mario2->targetYaw) / 10.0f) >= 180.0f) {
            flg = 1;
        } else {
            flg = 0;
        }
        diff = f31 - f30;
        if (flg != 0) {
            if (fabsf(diff) > 90.0f && f30 < f31) {
                f30 += 360.0f;
            }
        } else {
            if (fabsf(diff) > 90.0f && f30 > f31) {
                f31 += 360.0f;
            }
        }
    }
    if (!rollDispAngleInRange(f31, f30, 90.0f)) {
        if (fabsf(f31 - f30) > 180.0f) {
            if (f31 > f30) {
                f31 -= 360.0f;
            } else {
                f30 -= 360.0f;
            }
        }
    }
    f31 = revise360(1.2f * (f30 - f31) + f31);
    {
        f32 a = toMovedir(*(f32*)((u8*)mario + 0x1A8));

        if (*(s8*)((u8*)mario + 0x43) != 0) {
            a = revise360(180.0f + a);
        }
        if (rollDispAngleInRange(f31, a, 135.0f)) {
            *(s8*)((u8*)mario + 0x43) =
                (*(s8*)((u8*)mario + 0x43) + 1) & 1;
        }
    }
    *(f32*)((u8*)mario + 0x1A8) = rollDispCalcTarget(mario, f31);
    if (*(f32*)((u8*)mario + 0x1A8) >= 200.0f) {
        *(f32*)((u8*)mario + 0x1A8) = rollDispCalcTarget(mario, f31);
    }
    if (*(f32*)((u8*)mario + 0x1A8) < 170.0f) {
        *(f32*)((u8*)mario + 0x1A8) = rollDispCalcTarget(mario, f31);
    }
}

void psndSFXOff(s32 id);
extern const Vec vec3_802c5d40[];
extern f32 rollData;

// NONMATCHING (91.99%): only the three Vec copies differ - the original groups the loads
// (`lwz r3; lwz r0`) before the stores and keeps the 0x8=0 store ahead of them, while MWCC
// interleaves load/store here and swaps r0/r3. No expression shape tried changes the schedule.
void marioRollForceCancel(void) {
    const Vec* vec = vec3_802c5d40;
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId == 0x16) {
        MarioWork* m2 = marioGetPtr();

        if (*(u32*)((u8*)m2 + 0x2A8) != 0xFFFFFFFF) {
            psndSFXOff(*(s32*)((u8*)m2 + 0x2A8));
            *(s32*)((u8*)m2 + 0x2A8) = -1;
        }
        {
            MarioWork* m3 = marioGetPtr();
            m3->unk148 = 0.0f;
            m3->unk150.z = 0.004f;
        }
        if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
            mario->unk1A8 = 0.0f;
            *(f32*)((u8*)mario + 0x1AC) = 0.0f;
            *(f32*)((u8*)mario + 0x19C) = 270.0f;
            mario->directionView = 270.0f;
        } else {
            mario->unk1A8 = 180.0f;
            *(f32*)((u8*)mario + 0x1AC) = 180.0f;
            *(f32*)((u8*)mario + 0x19C) = 90.0f;
            mario->directionView = 90.0f;
        }
        marioSetSpec();
        mario->field_0x8 = 0;
        *(Vec*)((u8*)mario + 0xB0) = vec[4];
        *(Vec*)((u8*)mario + 0x98) = vec[5];
        *(Vec*)((u8*)mario + 0xBC) = vec[6];
        mario->dispFlags &= ~0x100;
        mario->dispFlags &= ~0x8;
        mario->flags &= ~0x01000000;
        marioPaperOff();
        marioAdjustMoveDir();
        marioChgPose("M_S_1");
        if (marioGetPtr()->motStruct != NULL) {
            __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
            marioGetPtr()->motStruct = NULL;
        }
        allPartyRideOff();
        marioChgMotSub(0, 0);
    }
}

// NONMATCHING (91.50%): same Vec-copy scheduling/register diff as marioRollForceCancel.
void mot_roll_post(void) {
    const Vec* vec = vec3_802c5d40;
    MarioWork* mario = marioGetPtr();
    MarioWork* m2 = marioGetPtr();

    if (*(u32*)((u8*)m2 + 0x2A8) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)m2 + 0x2A8));
        *(s32*)((u8*)m2 + 0x2A8) = -1;
    }
    {
        MarioWork* m3 = marioGetPtr();
        m3->unk148 = 0.0f;
        m3->unk150.z = 0.004f;
    }
    if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
        mario->unk1A8 = 0.0f;
        *(f32*)((u8*)mario + 0x1AC) = 0.0f;
        *(f32*)((u8*)mario + 0x19C) = 270.0f;
        mario->directionView = 270.0f;
    } else {
        mario->unk1A8 = 180.0f;
        *(f32*)((u8*)mario + 0x1AC) = 180.0f;
        *(f32*)((u8*)mario + 0x19C) = 90.0f;
        mario->directionView = 90.0f;
    }
    marioSetSpec();
    mario->field_0x8 = 0;
    {
        Vec* dst = (Vec*)((u8*)mario + 0xB0);
        *dst = vec[1];
    }
    {
        Vec* dst = (Vec*)((u8*)mario + 0x98);
        *dst = vec[2];
    }
    {
        Vec* dst = (Vec*)((u8*)mario + 0xBC);
        *dst = vec[3];
    }
    mario->dispFlags &= ~0x100;
    mario->dispFlags &= ~0x8;
    mario->flags &= ~0x01000000;
    marioPaperOff();
    marioAdjustMoveDir();
    marioChgPose("M_S_1");
    if (marioGetPtr()->motStruct != NULL) {
        __memFree(HEAP_DEFAULT, marioGetPtr()->motStruct);
        marioGetPtr()->motStruct = NULL;
    }
    allPartyRideOff();
}

BOOL marioRollCancel(void) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->flags & 0x01000000)) {
        return FALSE;
    }
    if (*(u32*)((u8*)mario + 0x2A4) & 0x200) {
        return FALSE;
    }
    mario->currSubMotionId = 0x14;
    camGetPtr(CAMERA_2D)->flags |= 0x200;
    return TRUE;
}

BOOL unk_8009f9c8(void) {
    BOOL res = FALSE;
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    s32 cond = 0;

    if (!(mario->flags & 0x01000000)) {
        return FALSE;
    }
    if (*(u32*)((u8*)mario + 0x2A4) & 0x200) {
        return FALSE;
    }
    m2 = marioGetPtr();
    if ((*(u32*)((u8*)m2 + 0x2A4) & 0x200) != 0) {
        cond = 1;
    } else if ((m2->flags & 0x08000000) != 0) {
        cond = 1;
    } else if (m2->hitobjStandOn == NULL) {
        cond = 1;
    } else if (marioSlitChkWallAround()) {
        cond = 1;
    } else {
        cond = 0;
    }
    if (cond == 0) {
        mario->currSubMotionId = 0x14;
        res = TRUE;
        camGetPtr(CAMERA_2D)->flags |= 0x200;
    }
    return res;
}

void marioForceRollAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn("p_roll");
    marioChgPose("M_I_U");
    marioChgPaper("PM_R_1B");
    mario->dispFlags |= 0x8;
    mario->dispFlags |= 0x100;
}

void marioResetRollSpd(void) {
    MarioWork* mario = marioGetPtr();
    f32 zero;

    if (mario->currMotionId == 0x16) {
        if (marioGetPtr()->motStruct != NULL) {
            zero = 0.0f;
            *(f32*)((u8*)marioGetPtr()->motStruct + 0x0) = zero;
            zero = 0.0f;
            *(f32*)((u8*)marioGetPtr()->motStruct + 0x4) = zero;
        }
        mario->baseSpeed = 0.0f;
    }
}

BOOL getRollEvtFlag(void) {
    BOOL r = FALSE;
    MarioWork* mario = marioGetPtr();

    if (mario->flags & 0x01000000) {
        if (*(u32*)((u8*)mario + 0x2A4) & 0x100) {
            r = TRUE;
        }
    }
    return r;
}

void clrRollEvtFlag(void) {
    MarioWork* mario = marioGetPtr();

    *(u32*)((u8*)mario + 0x2A4) &= ~0x100;
}

void setRollEvtFlag(void) {
    MarioWork* mario = marioGetPtr();

    *(u32*)((u8*)mario + 0x2A4) |= 0x100;
}

BOOL marioRollKeyDisable(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != 0x16) {
        return TRUE;
    }
    if ((s32)mario->currSubMotionId >= 0 && (s32)mario->currSubMotionId <= 1) {
        return FALSE;
    }
    if ((s32)mario->currSubMotionId >= 0x14 && (s32)mario->currSubMotionId <= 0x16) {
        return FALSE;
    }
    return TRUE;
}

BOOL unk_8009fcbc(void) {
    MarioWork* mario = marioGetPtr();

    if (*(u32*)((u8*)mario + 0x2A4) & 0x200) {
        return TRUE;
    }
    if (mario->flags & 0x08000000) {
        return TRUE;
    }
    if (mario->hitobjStandOn == NULL) {
        return TRUE;
    }
    return marioSlitChkWallAround() != 0;
}

void marioRollFixOn(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId == 0x16) {
        *(u32*)((u8*)mario + 0x2A4) |= 0x200;
    }
}

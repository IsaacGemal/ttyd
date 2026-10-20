#include "dolphin/types.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_sbr.h"
#include "pmario_sound.h"
#include "memory.h"
#include "driver/camdrv.h"
#include <string.h>
#include "motion/mot_roll.h"
#include "mario/mario_hit.h"
#include "party.h"
#include "mario/mario_cam.h"
#include "driver/hitdrv.h"
#include "mario/mariost.h"
#include "system.h"

// The original unit is built with pool_data on: every .rodata constant is reached base+offset from one
// base register. The Vec compound literals pool first, in source order (the camera offset, then the
// zero vectors of mot_roll_post and marioRollForceCancel, then rollSlopeOffset's ray templates); the
// sqrtf doubles, defined after mot_roll, follow them, then the strings.
#pragma pool_data on

extern f64 dbl_0p5;
extern f64 dbl_3;
extern f64 dbl_0;
extern f64 dbl_45;

// Include this after the prefix: <math.h> would emit its extern-inline sqrtf/sqrt weak-local _half/_three
// doubles ahead of the pooled constants and shift them off the target offsets. math_ext.h declares the
// same MSL ABI without those bodies; fabsf is kept as the __fabsf (fabs) intrinsic, as <math.h> defines it.
#include "math_ext.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern f32 rollData[];
#define fabsf(x) __fabsf(x)

// MSL quiet-NaN bit pattern, used by the inlined sqrtf negative/NaN paths (headers disagree on its type).
extern int __float_nan[];
// locals for symbols with no home header
extern s32 roll_flyout_flag2;

#ifdef MUST_MATCH
// An ordinary, immediately generated function pools these constants first and in this order (the original's first 14 .sdata2 entries);
// the deferred functions below then reuse them. Nothing calls it and the linker strips the body.
#pragma defer_codegen off
static void pollPoolPrefix(f32* o) {
    o[0] = 100.0f;
    o[1] = 0.0f;
    o[2] = 0.5f;
    o[3] = 3.1415927f;
    o[4] = 180.0f;
    o[5] = 6.0f;
    o[6] = 5.0f;
    o[7] = -4.0f;
    o[8] = 90.0f;
    o[9] = 270.0f;
    o[10] = 360.0f;
    o[11] = 10.0f;
    o[12] = 135.0f;
    o[13] = 1.2f;
}
#pragma defer_codegen reset
#endif

void marioRollFixOn(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId == 0x16) {
        *(u32*)((u8*)mario + 0x2A4) |= 0x200;
    }
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

void setRollEvtFlag(void) {
    MarioWork* mario = marioGetPtr();

    *(u32*)((u8*)mario + 0x2A4) |= 0x100;
}

void clrRollEvtFlag(void) {
    MarioWork* mario = marioGetPtr();

    *(u32*)((u8*)mario + 0x2A4) &= ~0x100;
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

// Camera follow parameters for the rolling pose.
static inline void rollSetCamParam(f32 a, f32 b, f32 c) {
    MarioWork* m = marioGetPtr();

    m->unk148 = a;
    m->unk150.z = b;
    m->field_68 = c;
    m->field_6C = (Vec){ 0.0f, 40.0f, 0.0f };
}

void mot_roll_post(void) {
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
        mario->dispReqDir = 0.0f;
        *(f32*)((u8*)mario + 0x19C) = 270.0f;
        mario->directionView = 270.0f;
    } else {
        mario->unk1A8 = 180.0f;
        mario->dispReqDir = 180.0f;
        *(f32*)((u8*)mario + 0x19C) = 90.0f;
        mario->directionView = 90.0f;
    }
    marioSetSpec();
    mario->field_0x8 = 0;
    *(Vec*)((u8*)mario + 0xB0) = (Vec){ 0.0f, 0.0f, 0.0f };
    *(Vec*)((u8*)mario + 0x98) = (Vec){ 0.0f, 0.0f, 0.0f };
    *(Vec*)((u8*)mario + 0xBC) = (Vec){ 0.0f, 0.0f, 0.0f };
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

void marioRollForceCancel(void) {
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
            mario->dispReqDir = 0.0f;
            *(f32*)((u8*)mario + 0x19C) = 270.0f;
            mario->directionView = 270.0f;
        } else {
            mario->unk1A8 = 180.0f;
            mario->dispReqDir = 180.0f;
            *(f32*)((u8*)mario + 0x19C) = 90.0f;
            mario->directionView = 90.0f;
        }
        marioSetSpec();
        mario->field_0x8 = 0;
        *(Vec*)((u8*)mario + 0xB0) = (Vec){ 0.0f, 0.0f, 0.0f };
        *(Vec*)((u8*)mario + 0x98) = (Vec){ 0.0f, 0.0f, 0.0f };
        *(Vec*)((u8*)mario + 0xBC) = (Vec){ 0.0f, 0.0f, 0.0f };
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

// helper inlined by MWCC into marioSetRollDispDir (100%); `hi` must be computed after the
// first `if` or the compiler hoists `a + range` and the schedule diverges.
static s32 rollDispAngleInRange(f32 angle, f32 ref, f32 range) {
    f32 a = (f32)(s32)(10.0f * angle) / 10.0f;
    f32 lo = a - range;
    f32 hi;
    s32 kind = 0;
    s32 res = 0;

    if (lo < 0.0f) {
        lo += 360.0f;
        kind = 1;
    }
    hi = a + range;
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

// helper inlined by MWCC into marioSetRollDispDir (100%).
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

void marioSetRollDispDir(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    f32 angle;
    f32 view;
    view = mario->directionView;
    angle = *(f32*)mario->unk19C;
    mario2 = marioGetPtr();

    if (rollDispAngleInRange(angle, view, 135.0f) == 1) {
        angle = revise360(180.0f + angle);
    }
    {
        s32 flg;

#ifdef MUST_MATCH
        /* The compiler schedules the targetYaw load before the angle multiply and gives it
           f0; forcing the load through a volatile pointer keeps the natural order so the
           angle multiply gets f0 and targetYaw gets f1, as the original does. */
        {
            f32 a10 = (f32)(s32)(10.0f * angle) / 10.0f;
            f32 t10 = (f32)(s32)(10.0f * *(volatile f32*)&mario2->targetYaw) / 10.0f;

            if (revise360(a10 - t10) < 180.0f) {
                flg = 0;
            } else {
                flg = 1;
            }
        }
#else
        if (revise360((f32)(s32)(10.0f * angle) / 10.0f -
                      (f32)(s32)(10.0f * mario2->targetYaw) / 10.0f) < 180.0f) {
            flg = 0;
        } else {
            flg = 1;
        }
#endif
        if (flg == 0) {
            if (fabsf(angle - view) > 90.0f && view > angle) {
                angle += 360.0f;
            }
        } else {
            if (fabsf(angle - view) > 90.0f && view < angle) {
                view += 360.0f;
            }
        }
    }
    if (!rollDispAngleInRange(angle, view, 90.0f)) {
        if (fabsf(angle - view) > 180.0f) {
            if (angle > view) {
                angle -= 360.0f;
            } else {
                view -= 360.0f;
            }
        }
    }
    angle = 1.2f * (view - angle) + angle;
    angle = revise360(angle);
    {
        f32 a = toMovedir(mario->unk1A8);

        if (*(s8*)((u8*)mario + 0x43) != 0) {
            a = revise360(180.0f + a);
        }
        if (rollDispAngleInRange(angle, a, 135.0f) == 1) {
            *(s8*)((u8*)mario + 0x43) =
                (*(s8*)((u8*)mario + 0x43) + 1) & 1;
        }
    }
    mario->unk1A8 = rollDispCalcTarget(mario, angle);
    if (mario->unk1A8 >= 200.0f) {
        mario->unk1A8 = rollDispCalcTarget(mario, angle);
    }
    if (mario->unk1A8 < 170.0f) {
        mario->unk1A8 = rollDispCalcTarget(mario, angle);
    }
}



void rollEvtJumpSetup(void) {
    MarioWork* mario = marioGetPtr();

    camFollowYOff();
    mario->flags |= 0x10000;
    mario->airTimer = 1;
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
    mario->mBaseDashSpeed = rollData[0];
    *(f32*)((u8*)marioGetPtr()->motStruct + 0x0) = 0.0f;
    *(f32*)((u8*)marioGetPtr()->motStruct + 0x4) = 0.0f;
    *(f32*)((u8*)mario + 0xB0) = -6.0f;
    *(f32*)((u8*)mario + 0xB4) = 6.0f;
    *(f32*)((u8*)mario + 0x98) = 6.0f;
    *(f32*)((u8*)mario + 0x9C) = 0.0f;
    mario->scale.x = 1.0f;
    mario->scale.y = 1.0f;
    mario->scale.z = 1.0f;
    *(f32*)((u8*)mario + 0x130) = 1.0f;
    allPartyForceRideOn();
}

// MSL's __fpclassifyf (float.h) inlined locally: the original sqrtf uses it for the
// negative/NaN paths, and the exact switch shape matters for codegen.
static inline s32 mot_roll_fpclassifyf(float value) {
    unsigned long integer = *(unsigned long*)&value;

    switch (integer & 0x7F800000) {
    case 0x7F800000:
        if ((integer & 0x7FFFFF) != 0) {
            return 1; // FP_QNAN
        }
        return 2; // FP_INFINITE
    case 0:
        if ((integer & 0x7FFFFF) != 0) {
            return 5; // FP_SUBNORMAL
        }
        return 3; // FP_ZERO
    }
    return 4; // FP_NORMAL
}

// The original binary inlines the full MSL `sqrtf` (math.h), whose negative/NaN paths the
// repo's dolsdk2004 `extern inline sqrtf` omits. Reproducing it locally keeps mot_roll's
// inlined sqrt sequence identical to the target.
static inline float mot_roll_sqrtf(float x) {
    float y;

    if (x > 0.0f) {
        double guess = __frsqrte((double)x);
        guess = dbl_0p5 * guess * (dbl_3 - guess * guess * x);
        guess = dbl_0p5 * guess * (dbl_3 - guess * guess * x);
        guess = dbl_0p5 * guess * (dbl_3 - guess * guess * x);
        y = (float)(x * guess);
        return y;
    }
    if (x < dbl_0) {
        return *(float*)__float_nan;
    }
    if (mot_roll_fpclassifyf(x) == 1) {
        return *(float*)__float_nan;
    }
    return x;
}

// Tilts the rolling pose to the slope of the ground below Mario.
static inline void rollSlopeOffset(void) {
    MarioWork* m = marioGetPtr();
    Vec a;
    Vec b;
    Vec c;
    Vec d;
    f32 s;
    f32 co;
    f32 dist = 100.0f;

    a = m->position;
    a.y += m->wPlayerCollisionBox.y;
    b = (Vec){ 0.0f, -1.0f, 0.0f };
    if (marioHitCheckVec(&a, &b, &c, &d, &dist) != NULL) {
        f32 len = mot_roll_sqrtf(d.x * d.x + d.z * d.z);
        f32 ang = angleABf(0.0f, 0.0f, len, -d.y);
        f32 ab = fabsf(ang);
        f32 n = (f32)(s32)(ang + 0.5f);

        if (ab < dbl_45) {
            f32 x;
            f32 y;
            f32 cs;

            s = (f32)sin(3.1415927f * n / 180.0f);
            cs = (f32)cos(3.1415927f * n / 180.0f);
            x = 5.0f * s;
            y = 5.0f * cs;
            co = cs;
            *(f32*)((u8*)m + 0x98) = 6.0f + x;
            *(f32*)((u8*)m + 0x9C) = -4.0f + y;
        } else {
            sincosf(&s, &co, 90.0f);
            dist = 100.0f;
            a = m->position;
            b = (Vec){ s, 0.0f, co };
            marioHitCheckVec(&a, &b, &c, &d, &dist);
        }
    }
}

// Whether the stick points away from the way Mario is rolling.
static inline s32 rollChkStickBack(f32 range) {
    s32 kind = 0;
    s32 res = 0;
    MarioWork* m;
    f32 a10;
    f32 lo;
    f32 hi;
    f32 ref;

    m = marioGetPtr();
    a10 = (f32)(s32)(10.0f * *(f32*)m->unk19C) / 10.0f;
    ref = angleABf(0.0f, 0.0f, (f32)(s8)m->wStickDir1, (f32)(s8)m->wStickDir2);
    lo = a10 - range;
    if (lo < 0.0f) {
        lo += 360.0f;
        kind = 1;
    }
    hi = a10 + range;
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

// MotRoll userdata is allocated here (0x14 bytes).
void mot_roll(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    MarioWork* m3;
    MotRoll* md;
    f32 f26;
    f32 f27;
    f32 f28;
    f32 f29;
    f32 f30;
    f32 f31;
    f32 fmax;
    f32 fqm;
    f32 rate;
    f32 ay;
    f32 spd;
    f32 speed;
    f32 accel = 0.025f;
#ifdef MUST_MATCH
    /* A named constant stops MWCC hoisting the 0.1 load, so the field is added before the constant as in
       the original. */
    f32 kOneTenth = 0.1f;
#endif
    s32 cond;
    s32 i;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        *(u32*)((u8*)mario + 0x2A4) = 0;
        *(u32*)((u8*)mario + 0x2A8) = 0;
        *(u32*)((u8*)mario + 0x2AC) = 0;
        *(u32*)((u8*)mario + 0x2B0) = 0;
        *(u32*)((u8*)mario + 0x2B4) = 0;
        md = __memAlloc(HEAP_DEFAULT, 0x14);
        marioGetPtr()->motStruct = md;
        memset(marioGetPtr()->motStruct, 0, 0x14);
        {
            MarioWork* mm = marioGetPtr();
            *(s32*)((u8*)mm + 0x2A8) = -1;
        }
        mario->currSubMotionId = 0;
    }

    switch (mario->currSubMotionId) {
    case 0:
        camGetPtr(CAMERA_2D)->flags |= 0x200;
        marioAdjustMoveDir();
        if (mario->unk1A8 < 90.0f || mario->unk1A8 > 270.0f) {
            *(s8*)((u8*)mario + 0x43) = 1;
            mario->directionView = revise360(270.0f - mario->targetYaw);
            *(f32*)((u8*)mario + 0x19C) = 270.0f;
        } else {
            *(s8*)((u8*)mario + 0x43) = 0;
            mario->directionView = revise360(90.0f - mario->targetYaw);
            *(f32*)((u8*)mario + 0x19C) = 90.0f;
        }
        marioGetRubInit();
        mario->flags |= 0x01000000;
        mario->field_0x8 = 0x10;
        mario->dispFlags |= 0x8;
        mario->dispFlags |= 0x100;
        marioPaperOn("p_roll");
        marioChgPose("M_I_U");
        marioChgPaper("PM_R_1A");
        mario->multiTimer = 0x41;
        m2 = marioGetPtr();
        m2->wPlayerCollisionBox.x = 14.8f;
        m2->wPlayerCollisionBox.y = 14.8f;
        m2->mBaseDashSpeed = ((f32*)&rollData)[0];
        f28 = 0.0f;
        ((MotRoll*)marioGetPtr()->motStruct)->velX = 0.0f;
        ((MotRoll*)marioGetPtr()->motStruct)->velY = 0.0f;
        *(f32*)((u8*)m2 + 0xB0) = -6.0f;
        *(f32*)((u8*)m2 + 0xB4) = 6.0f;
        *(f32*)((u8*)m2 + 0x98) = 6.0f;
        *(f32*)((u8*)m2 + 0x9C) = 0.0f;
        m2->scale.x = 1.0f;
        m2->scale.y = 1.0f;
        m2->scale.z = 1.0f;
        *(f32*)((u8*)m2 + 0x130) = 1.0f;
        mario->airTimer = 0;
        if (mario->forceMoveTimer == 0) {
            mario->baseSpeed = 0.0f;
        }
        rollSetCamParam(0.4f, 0.01f, 2.0f);
        ((MotRoll*)marioGetPtr()->motStruct)->unk8 = 0;
        ((MotRoll*)marioGetPtr()->motStruct)->state = 0;
        psndSFXOn_3D((char*)0x179, &mario->position);
        psndSFXOn_3D((char*)0x9F, &mario->position);
        mario->currSubMotionId = 1;
        break;

    case 1:
        if (mario->multiTimer == 0x36) {
            allPartyRideShip();
        }
#ifdef MUST_MATCH
        /* 0x9C is inside the header's field_0x98 block. */
        *(f32*)(mario->field_0x98 + 4) = *(f32*)(mario->field_0x98 + 4) + kOneTenth;
#else
        *(f32*)((u8*)mario + 0x9C) = *(f32*)((u8*)mario + 0x9C) + 0.1f;
#endif
        if (*(f32*)((u8*)mario + 0x9C) > 1.0f) {
            *(f32*)((u8*)mario + 0x9C) = 1.0f;
        }
        if ((s32)--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x1E;
            marioChgPose("M_I_U");
            marioChgPaper("PM_R_1B");
            camGetPtr(CAMERA_2D)->flags &= ~0x200;
        }
        break;

    case 0x14:
        mario->baseSpeed = 0.0f;
        *(f32*)((u8*)mario + 0xBC) = 0.0f;
        *(f32*)((u8*)mario + 0xC4) = 0.0f;
        *(f32*)((u8*)mario + 0xB0) = 0.0f;
        *(f32*)((u8*)mario + 0xB4) = 0.0f;
        *(f32*)((u8*)mario + 0x98) =
            (revise360(mario->directionView - mario->targetYaw) < 180.0f) ? 3.0f : -3.0f;
        *(f32*)((u8*)mario + 0x9C) = 0.0f;
        mario->multiTimer = 0x41;
        mario->currSubMotionId = 0x15;
        mario->unk1A8 = 0.0f;
        {
            MarioWork* m = marioGetPtr();
            u32 sfx = *(u32*)((u8*)m + 0x2A8);
            if (sfx != 0xFFFFFFFF) {
                psndSFXOff(sfx);
                *(s32*)((u8*)m + 0x2A8) = -1;
            }
        }
        {
            MarioWork* m = marioGetPtr();
            m->unk148 = 0.0f;
            m->unk150.z = 0.004f;
        }
        marioChgPaper("PM_R_1C");
        marioPaperLightOff();
        psndSFXOn_3D((char*)0x17A, &mario->position);
        /* fallthrough */
    case 0x15:
        if ((s32)--mario->multiTimer > 0) {
            if (revise360(mario->directionView - mario->targetYaw) < 180.0f) {
                f30 = 90.0f;
                f27 = 0.1f * (f30 - mario->unk1A8);
                f27 = f27 + mario->unk1A8;
                if (fabsf(f30 - f27 < 1.0f)) {
                    f27 = 90.0f;
                }
                mario->unk1A8 = f27;
                mario->dispReqDir = f27;
            } else {
                f30 = 270.0f;
                f27 = 0.1f * (f30 - mario->unk1A8);
                f27 = f27 + mario->unk1A8;
                if (fabsf(f30 - f27 < 1.0f)) {
                    f27 = 270.0f;
                }
                mario->unk1A8 = f27;
                mario->dispReqDir = f27;
            }
        } else {
            if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
                mario->unk1A8 = 0.0f;
                mario->dispReqDir = 0.0f;
                *(f32*)((u8*)mario + 0x19C) = 270.0f;
                mario->directionView = 270.0f;
            } else {
                mario->unk1A8 = 180.0f;
                mario->dispReqDir = 180.0f;
                *(f32*)((u8*)mario + 0x19C) = 90.0f;
                mario->directionView = 90.0f;
            }
            marioSetSpec();
            mario->field_0x8 = 0;
            marioOfsRotReset();
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
            camGetPtr(CAMERA_2D)->flags &= ~0x200;
            mario->currSubMotionId = 0x16;
        }
        break;

    case 0x16:
        marioChgMotSub(0, 0);
        break;

    default:
        break;
    }

    if ((s32)mario->currSubMotionId < 0x1E) {
        return;
    }

    if (mario->flags & 0x40000) {
        roll_upstairs();
        return;
    }

    if (mario->buttonNew & 0x200) {
        MarioWork* m = marioGetPtr();

        if (*(u32*)((u8*)m + 0x2A4) & 0x200) {
            cond = 1;
        } else if (m->flags & 0x08000000) {
            cond = 1;
        } else if (m->hitobjStandOn == NULL) {
            cond = 1;
        } else if (marioSlitChkWallAround() != 0) {
            cond = 1;
        } else {
            cond = 0;
        }
        if (cond == 0) {
            mario->currSubMotionId = 0x14;
            camGetPtr(CAMERA_2D)->flags |= 0x200;
        } else {
            mario->buttonNew |= 0x100;
        }
    }

    f26 = *(f32*)((u8*)mario + 0x130);
    if (((MotRoll*)marioGetPtr()->motStruct)->unk8 != 0) {
        ((MotRoll*)marioGetPtr()->motStruct)->unk8 = 0;
        ((MotRoll*)marioGetPtr()->motStruct)->state = 1;
        f26 = 1.0f;
    }
    switch (((MotRoll*)marioGetPtr()->motStruct)->state) {
    case 1:
        if (f26 < 1.4f) {
            f32 k = 1.0f;
            f26 = k * (1.4499999f - f26) + f26;
            if (f26 >= 1.4f) {
                f26 = 1.4f;
            }
        }
        if (mario->airTimer == 0) {
            ((MotRoll*)marioGetPtr()->motStruct)->state = 2;
        }
        break;
    case 2:
        if (f26 > 0.4f) {
            f26 = 0.4f * (0.35f - f26) + f26;
            if (f26 <= 0.4f) {
                f26 = 0.4f;
                ((MotRoll*)marioGetPtr()->motStruct)->state = 3;
            }
        }
        break;
    case 3:
        if (f26 < 1.0f) {
            f26 = 0.5f * (1.05f - f26) + f26;
            if (f26 >= 1.0f) {
                ((MotRoll*)marioGetPtr()->motStruct)->state = 0;
                f26 = 1.0f;
            }
        }
        break;
    default:
        break;
    }
    *(f32*)((u8*)mario + 0x130) = f26;

    if (mario->hitobjStandOn == NULL && *(void**)((u8*)mario + 0x1E0) != NULL &&
        !marioChkBero(*(HitEntry**)((u8*)mario + 0x1E0))) {
        f29 = ((MotRoll*)marioGetPtr()->motStruct)->velX;
        fqm = ((MotRoll*)marioGetPtr()->motStruct)->velY;
        f29 = f29 + 0.2f * (-f29);
        fqm = fqm + 0.2f * (-fqm);
        if (fabsf(f29) < 1.0f) {
            f29 = 0.0f;
        }
        if (fabsf(fqm) < 1.0f) {
            fqm = 0.0f;
        }
        ((MotRoll*)marioGetPtr()->motStruct)->velX = f29;
        ((MotRoll*)marioGetPtr()->motStruct)->velY = fqm;
    }

    if (mario->hitobjStandOn != NULL) {
        if (!(hitGetAttr(mario->hitobjStandOn) & 0xA00) && mario->airTimer == 0 &&
            (mario->buttonNew & 0x100) &&
            (strcmp(gp->currentMapName, "eki_04\0") != 0 ||
             !(fabsf(*(f32*)((u8*)mario + 0x214)) >= 40.0f))) {
            MarioWork* m = marioGetPtr();

            camFollowYOff();
            m->jumpVelY = 2.85f;
            m->jumpAccelY = -0.14f;
            m->jumpJerkY = 0.08f;
            m->jumpJerkStepY = -0.02f;
            m->flags |= 0x00010000;
            m->airTimer = 1;
            psndSFXOn_3D((char*)0x17C, &marioGetPtr()->position);
            mario->hitobjJumpFrom = mario->hitobjStandOn;
            mario->hitobjStandOn = NULL;
            *(Vec*)((u8*)mario + 0xD4) = mario->position;
            *(Vec*)((u8*)mario + 0x11C) = mario->position;
            camFollowYOff();
        }
        marioUpdateCamCollision(mario->position);
        mario->airTimer = 0;
    }

    if (mario->flags & 0x30000) {
        if (*(void**)((u8*)mario + 0x1F4) != NULL) {
            if (mario->jumpVelY >= 0.0f) {
                mario->jumpVelY = -0.03f;
                mario->jumpAccelY = -0.02f;
                mario->jumpJerkY = -0.02f;
                mario->jumpJerkStepY = -0.002f;
            }
            mario->flags |= 0x00020000;
        }
        if (mario->airTimer == 0) {
            if (mario->jumpVelY < 0.0f) {
                *(Vec*)((u8*)mario + 0xD4) = mario->position;
                camFollowYOn();
            }
        }
        if (mario->position.y < *(f32*)((u8*)mario + 0x120)) {
            camFollowYOn();
        }
        mario->airTimer = mario->airTimer + 1;
    }

    f29 = ((MotRoll*)marioGetPtr()->motStruct)->velX;
    f28 = ((MotRoll*)marioGetPtr()->motStruct)->velY;
    f30 = 0.0f;
    ay = f30;

    if (!(mario->flags & 0x30000)) {
        f26 = (f32)(s8)mario->wStickDir1;
        f30 = (f32)(s8)mario->wStickDir2;
        ay = distABf(0.0f, 0.0f, f26, f30);
        f26 = revise360(mario->targetYaw + angleABf(0.0f, 0.0f, f26, f30));
        f26 = 3.1415927f * f26 / 180.0f;
        f30 = ay * (f32)sin(f26);
        ay = ay * -(f32)cos(f26);
#ifdef MUST_MATCH
        /* Volatile read keeps the directionView load before the inlined 10.0 load, as in the original. */
        if (rollDispAngleInRange(*(volatile f32*)&mario->directionView,
                                 angleABf(0.0f, 0.0f, f30, ay), 135.0f) == 1) {
#else
        if (rollDispAngleInRange(mario->directionView,
                                 angleABf(0.0f, 0.0f, f30, ay), 135.0f) == 1) {
#endif
            ay = distABf(0.0f, 0.0f, f30, ay);
            f30 = -ay * (f32)sin(3.1415927f * mario->directionView / 180.0f);
            ay = ay * (f32)cos(3.1415927f * mario->directionView / 180.0f);
        }
    }

    if (!(mario->flags & 0x30000) && !(*(u32*)((u8*)mario + 0x2A4) & 0x20)) {
        f31 = angleABf(0.0f, 0.0f, f29, -f28);
        if (*(f32*)((u8*)mario + 0x220) > 0.05f) {
            f32 tmp;
            f26 = (f31 > 180.0f) ? ((f32*)&rollData)[5] : ((f32*)&rollData)[6];
            tmp = (f32)sin(3.1415927f * *(f32*)((u8*)mario + 0x220) / 180.0f);
            tmp = tmp * f26;
            f30 = f30 - tmp;
            if (f30 <= -100.0f) {
                f30 = -100.0f;
            }
        } else if (*(f32*)((u8*)mario + 0x220) < -0.05f) {
            f32 tmp;
            f26 = (f31 < 180.0f) ? ((f32*)&rollData)[5] : ((f32*)&rollData)[6];
            tmp = (f32)sin(3.1415927f * *(f32*)((u8*)mario + 0x220) / 180.0f);
            tmp = tmp * f26;
            f30 = f30 - tmp;
            if (f30 >= 100.0f) {
                f30 = 100.0f;
            }
        }
        if (*(f32*)((u8*)mario + 0x218) > 0.05f) {
            f32 tmp;
            f26 = (f31 > 180.0f) ? ((f32*)&rollData)[5] : ((f32*)&rollData)[6];
            tmp = (f32)sin(3.1415927f * *(f32*)((u8*)mario + 0x218) / 180.0f);
            tmp = tmp * f26;
            ay = ay - tmp;
            if (ay <= -100.0f) {
                ay = -100.0f;
            }
        } else if (*(f32*)((u8*)mario + 0x218) < -0.05f) {
            f32 tmp;
            f26 = (f31 < 180.0f) ? ((f32*)&rollData)[5] : ((f32*)&rollData)[6];
            tmp = (f32)sin(3.1415927f * *(f32*)((u8*)mario + 0x218) / 180.0f);
            tmp = tmp * f26;
            ay = ay - tmp;
            if (ay >= 100.0f) {
                ay = 100.0f;
            }
        }
#ifdef MUST_MATCH
        /* Volatile read keeps the directionView load before the inlined 10.0 load, as in the original. */
        if (rollDispAngleInRange(*(volatile f32*)&mario->directionView,
                                 angleABf(0.0f, 0.0f, f30, ay), 135.0f) == 1) {
#else
        if (rollDispAngleInRange(mario->directionView,
                                 angleABf(0.0f, 0.0f, f30, ay), 135.0f) == 1) {
#endif
            ay = distABf(0.0f, 0.0f, f30, -ay);
            f30 = -ay * (f32)sin(3.1415927f * mario->directionView / 180.0f);
            ay = ay * (f32)cos(3.1415927f * mario->directionView / 180.0f);
        }
    }

    if ((u32)mario->field_0x1D8 != 0) {
        if (!marioChkBero(*(HitEntry**)((u8*)mario + 0x1E0))) {
            f29 = f29 + 0.05f * (-f29);
            f28 = f28 + 0.05f * (-f28);
            if (fabsf(f29) < 0.1f) {
                f29 = 0.0f;
            }
            if (fabsf(f28) < 0.1f) {
                f28 = 0.0f;
            }
        }
    }

    if (*(void**)((u8*)mario + 0x1E0) != NULL && (s8)mario->wStickDir1 == 0 &&
        (s8)mario->wStickDir2 == 0 && !marioChkBero(*(HitEntry**)((u8*)mario + 0x1E0))) {
        f29 = f29 + 0.6f * (-f29);
        f28 = f28 + 0.6f * (-f28);
        if (fabsf(f29) < 0.1f) {
            f29 = 0.0f;
        }
        if (fabsf(f28) < 0.1f) {
            f28 = 0.0f;
        }
    }

    if (mario->forceMoveTimer == 0 && !f30 && !ay) {
        if (*(void**)((u8*)mario + 0x1F0) != NULL) {
            if (!marioChkBero(*(HitEntry**)((u8*)mario + 0x1F0))) {
                f29 = f29 + f29 * ((f32*)&rollData)[2];
                f28 = f28 + f28 * ((f32*)&rollData)[2];
            }
        } else if (mario->hitobjStandOn != NULL && mario->airTimer == 0) {
            if (fabsf(*(f32*)((u8*)mario + 0x214) <= 1.0f)) {
                f29 = f29 + f29 * ((f32*)&rollData)[1];
                f28 = f28 + f28 * ((f32*)&rollData)[1];
            } else {
                f29 = f29 + 6.4f * (f29 * ((f32*)&rollData)[1]);
                f28 = f28 + 6.4f * (f28 * ((f32*)&rollData)[1]);
            }
        }
        {
            f32 min = rollData[3];

            if (fabsf(f29) < min) {
                f29 = 0.0f;
            }
        }
        {
            f32 min = rollData[3];

            if (fabsf(f28) < min) {
                f28 = 0.0f;
            }
        }
    } else {
    if ((*(u32*)((u8*)mario + 0x2A4) & 0x10) && *(f32*)((u8*)mario + 0x220) == 0.0f &&
    rollChkStickBack(135.0f) == 1) {
        f29 = f29 + 0.2f * (-f29);
        f28 = f28 + 0.2f * (-f28);
        if (fabsf(f29) < 1.0f) {
            f29 = 0.0f;
        }
        if (fabsf(f28) < 1.0f) {
            f28 = 0.0f;
        }
        if (f29 == 0.0f && f28 == 0.0f) {
            *(f32*)((u8*)mario + 0x19C) = revise360(180.0f + *(f32*)((u8*)mario + 0x19C));
            mario->directionView = *(f32*)((u8*)mario + 0x19C);
        }
    }
    {
        fmax = ((f32*)&rollData)[0] / 0.05f;
        fqm = 1.0f;
        if (mario->forceMoveTimer == 0 && fabsf(*(f32*)((u8*)mario + 0x214)) >= 1.0f) {
            f32 ang = angleABf(0.0f, 0.0f, f29, -f28);
            if (*(f32*)((u8*)mario + 0x220) > 0.0f) {
                fqm = (ang > 180.0f) ? ((f32*)&rollData)[9] : 1.0f;
            } else if (*(f32*)((u8*)mario + 0x220) < 0.0f) {
                fqm = (ang < 180.0f) ? ((f32*)&rollData)[9] : 1.0f;
            }
        }
        fmax = fmax * fqm;
        fqm = f28 * f28;
        if (f29 * f29 + fqm <= fmax * fmax) {
            f29 = f29 + f30 * accel;
        } else {
            f29 = f29 + f30 * accel;
            if (f29 < -fmax) {
                f29 = -fmax;
            }
            if (f29 > fmax) {
                f29 = fmax;
            }
        }
    }
    {
        fmax = ((f32*)&rollData)[0] / 0.05f;
        f31 = 1.0f;
        if (mario->forceMoveTimer == 0 && fabsf(*(f32*)((u8*)mario + 0x214)) >= 1.0f) {
            f32 ang = angleABf(0.0f, 0.0f, f29, -f28);
            if (*(f32*)((u8*)mario + 0x218) > 0.0f) {
                f31 = (ang > 180.0f) ? ((f32*)&rollData)[9] : 1.0f;
            } else if (*(f32*)((u8*)mario + 0x218) < 0.0f) {
                f31 = (ang < 180.0f) ? ((f32*)&rollData)[9] : 1.0f;
            }
        }
        fmax = fmax * f31;
        if (f29 * f29 + fqm <= fmax * fmax) {
            f28 = f28 - ay * accel;
        } else {
            f28 = f28 - ay * accel;
            if (f28 < -fmax) {
                f28 = -fmax;
            }
            if (f28 > fmax) {
                f28 = fmax;
            }
        }
    }
    }

vel_apply:
    ((MotRoll*)marioGetPtr()->motStruct)->velX = f29;
    ((MotRoll*)marioGetPtr()->motStruct)->velY = f28;
    f31 = -f28;
    f30 = angleABf(0.0f, 0.0f, f29, f31);
    spd = 3.1415927f * f30 / 180.0f;
    f26 = (f32)cos(spd);
    {
        f32 s = (f32)sin(spd);
        speed = 0.05f * distABf(0.0f, 0.0f, f29 * s, f28 * f26);
    }
    {
        MarioWork* m;

        i = 0;
        m = marioGetPtr();
        if ((m->flags & 0x01000000) && (*(u32*)((u8*)m + 0x2A4) & 0x100)) {
            i = 1;
        }
        if (i == 0 && mario->forceMoveTimer == 0) {
            f30 = angleABf(0.0f, 0.0f, f29, f31);
            if (!(*(u32*)((u8*)mario + 0x2A4) & 0x10000)) {
                speed = speed * mario->playerGravity;
                if (speed < 0.05f) {
                    speed = 0.0f;
                }
                mario->baseSpeed = speed;
                if (speed) {
                    mario->directionView = f30;
                }
            }
        }
    }

    if (mario->baseSpeed) {
        MarioWork* m = marioGetPtr();
        if (*(u32*)((u8*)m + 0x2A8) == 0xFFFFFFFF) {
            *(s32*)((u8*)m + 0x2A8) = psndSFXOn_3D((char*)0x17B, &m->position);
            *(u32*)((u8*)m + 0x2A4) |= 0x1000;
        }
    } else {
        MarioWork* m = marioGetPtr();
        if (*(u32*)((u8*)m + 0x2A8) != 0xFFFFFFFF) {
            psndSFXOff(*(s32*)((u8*)m + 0x2A8));
            *(s32*)((u8*)m + 0x2A8) = -1;
        }
    }

    rate = ((f32*)&rollData)[4];
    {
#ifdef MUST_MATCH
        /* Volatile read keeps the directionView load before the inlined 10.0 load, as in the original. */
        s32 r = rollDispAngleInRange(*(volatile f32*)&mario->directionView,
                                     toMovedir(mario->unk1A8), 90.0f);
#else
        s32 r = rollDispAngleInRange(mario->directionView,
                                     toMovedir(mario->unk1A8), 90.0f);
#endif
        f32 t = mario->baseSpeed * rate;
        *(f32*)((u8*)mario + 0xC4) = revise360(t * ((r == 1) ? -1.0f : 1.0f) +
                                                 *(f32*)((u8*)mario + 0xC4));
    }

    if (mario->forceMoveTimer != 0) {
        goto roll_tail;
    }
    i = 0;
    {
        MarioWork* m = marioGetPtr();
        if ((m->flags & 0x01000000) && (*(u32*)((u8*)m + 0x2A4) & 0x100)) {
            i = 1;
        }
    }
    if (i != 0 || !speed) {
        goto roll_tail;
    }

    f28 = *(f32*)mario->unk19C;
    m3 = marioGetPtr();
    if (rollDispAngleInRange(f28, f30, 135.0f) == 1) {
        f28 = revise360(180.0f + f28);
    }
    {
        s32 flg;

#ifdef MUST_MATCH
        /* Same schedule fix as marioSetRollDispDir: the volatile targetYaw load keeps the
           angle*10 multiply in f0 and the targetYaw*10 in f1, as the original does. */
        {
            f32 a10 = (f32)(s32)(10.0f * f28) / 10.0f;
            f32 t10 = (f32)(s32)(10.0f * *(volatile f32*)&m3->targetYaw) / 10.0f;

            if (revise360(a10 - t10) < 180.0f) {
                flg = 0;
            } else {
                flg = 1;
            }
        }
#else
        if (revise360((f32)(s32)(10.0f * f28) / 10.0f -
                      (f32)(s32)(10.0f * m3->targetYaw) / 10.0f) < 180.0f) {
            flg = 0;
        } else {
            flg = 1;
        }
#endif
        if (flg == 0) {
            if (fabsf(f28 - f30) > 90.0f && f30 > f28) {
                f28 += 360.0f;
            }
        } else {
            if (fabsf(f28 - f30) > 90.0f && f30 < f28) {
                f30 += 360.0f;
            }
        }
    }
    if (rollDispAngleInRange(f28, f30, 90.0f) == 0) {
        if (fabsf(f28 - f30) > 180.0f) {
            if (f28 > f30) {
                f28 -= 360.0f;
            } else {
                f30 -= 360.0f;
            }
        }
    }
    f28 = 1.2f * (f30 - f28) + f28;
    f28 = revise360(f28);
    f30 = toMovedir(mario->unk1A8);
    if (*(s8*)((u8*)mario + 0x43) != 0) {
        f30 = revise360(180.0f + f30);
    }
    if (rollDispAngleInRange(f28, f30, 135.0f) == 1) {
        *(s8*)((u8*)mario + 0x43) = (*(s8*)((u8*)mario + 0x43) + 1) & 1;
    }
    f27 = 270.0f + mario->targetYaw - f28;
    f27 = f27 + ((*(s8*)((u8*)mario + 0x43) != 0) ? -180.0f : 0.0f);
    mario->unk1A8 = revise360(f27);
    if (mario->unk1A8 >= 200.0f) {
        f27 = 270.0f + mario->targetYaw - f28;
        f27 = f27 + ((*(s8*)((u8*)mario + 0x43) != 0) ? -180.0f : 0.0f);
        mario->unk1A8 = revise360(f27);
    }
    if (mario->unk1A8 < 170.0f) {
        f27 = 270.0f + mario->targetYaw - f28;
        f27 = f27 + ((*(s8*)((u8*)mario + 0x43) != 0) ? -180.0f : 0.0f);
        mario->unk1A8 = revise360(f27);
    }
    mario->directionView = f28;
    *(f32*)mario->unk19C = f28;

roll_tail:
    if (*(f32*)((u8*)mario + 0x220) != 0.0f) {
        *(f32*)((u8*)mario + 0xBC) =
            revise360(-*(f32*)((u8*)mario + 0x220) *
                      (f32)sin(3.1415927f * mario->unk1A8 / 180.0f));
    } else {
        *(f32*)((u8*)mario + 0xBC) = 0.0f;
    }
    if ((s8)mario->wStickDir1 != 0 || (s8)mario->wStickDir2 != 0) {
        *(u32*)((u8*)mario + 0x2A4) |= 0x4000;
    }
    if (mario->baseSpeed) {
        MarioWork* m = marioGetPtr();
        m->unk148 = 0.0f;
        m->unk150.z = 0.004f;
    }
    rollSlopeOffset();
}


__declspec(section ".rodata") f64 dbl_0p5 = 0.5;
__declspec(section ".rodata") f64 dbl_3 = 3.0;
__declspec(section ".rodata") f64 dbl_0 = 0.0;
__declspec(section ".rodata") f64 dbl_45 = 45.0;

void roll_upstairs_on(void) {
    MarioWork* mario = marioGetPtr();

    mario->flags |= 0x40000;
    mario->flags |= 0x80;
    marioClearJumpPara();
    mario->multiTimer = 3;
    {
        u32 t = mario->multiTimer;
        mario->jumpVelY =
            (*(f32*)((u8*)mario + 0x1C4) - mario->position.y) / (f32)(s32)t;
    }
    mario->jumpVelY = 4.0f;
    mario->jumpAccelY = -0.42f;
    mario->jumpJerkY = 0.04f;
    mario->jumpJerkStepY = -0.01f;
}
// dont_inline: original keeps roll_upstairs as a real call (bl roll_upstairs) from mot_roll;
// MWCC's -inline auto would otherwise inline this small function.
#pragma dont_inline on
void roll_upstairs(void) {
    MarioWork* mario = marioGetPtr();

    mario->position.y += mario->jumpVelY;
    if ((s32)--mario->multiTimer <= 0) {
        mario->flags &= ~0x80;
        mario->flags &= ~0x40;
        mario->flags &= ~0x40000;
    }
}
#pragma dont_inline reset
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

// .sdata: the roll-motion physics table (11 floats).
f32 rollData[11] = {
    3.0f, -0.05f, -0.5f, 0.5f, 7.0f, 180.0f, 140.0f, 10.0f, 1.2f, 2.0f, 0.8f,
};

// 4-byte zero word that ends the unit's .sdata.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80410554_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

// .sbss: uninitialised globals land in reverse declaration order, so the trailing gap is
// declared first. roll_flyout_flag2 starts the section, then roll_grz, roll_grx.
#pragma force_active on
u8 gap_08_8041888C_sbss[4];
#pragma force_active reset
f32 roll_grx;
f32 roll_grz;
s32 roll_flyout_flag2;


// 4-byte zero word that ends the unit's .sdata2.
#ifdef MUST_MATCH
// An unused function generated immediately flushes the deferred functions, so the force_active word below lands after their pool.
#pragma defer_codegen off
static void rollFlushPools(void) {}
#endif
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") static u32 gap_09_8041AD94_sdata2[1] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset
#ifdef MUST_MATCH
#pragma defer_codegen reset
#endif

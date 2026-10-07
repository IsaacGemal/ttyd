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

// The original unit is built with pool_data on: every .rodata constant is reached base+offset from the
// vec3_802c5d40 blob, its first object. Defining that prefix here and letting MWCC pool the rest places
// the sqrtf doubles at +0x70.. and the strings at +0x98, exactly where the target's lfd fX,0xNN(r30)
// loads expect them (mot_roll: 93.92% -> 95.48%).
#pragma pool_data on

__declspec(section ".rodata") Vec vec3_802c5d40[9] = {
    { 0.0f, 40.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
};
__declspec(section ".rodata") f64 dbl_0p5 = 0.5;
__declspec(section ".rodata") f64 dbl_3 = 3.0;
__declspec(section ".rodata") f64 dbl_0 = 0.0;
__declspec(section ".rodata") f64 dbl_45 = 45.0;


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

// 100%: same fix as marioRollForceCancel - the source pointer must not be `const`.
void mot_roll_post(void) {
    Vec* vec = vec3_802c5d40;
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

// 100%: the source array must be reached through a non-const pointer. A `const Vec*`
// made MWCC software-pipeline the three Vec copies (load/store interleaved, r0/r3
// swapped) instead of the original's load,load,store,store,load,store grouping.
void marioRollForceCancel(void) {
    Vec* vec = vec3_802c5d40;
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

// NONMATCHING (98.53%): all calls and immediates match; remaining diffs are MWCC register colouring (velocity
// X/Y f27<->f29 swap), scheduler ordering of the float loads / i2f sequence, and an extra anonymous i2f pool copy.
// MotRoll userdata is allocated here (0x14 bytes).
// Link status (2026-10-04): with the pool prefix helper and the .sdata2 flush helper the .rodata/.sdata/.sdata2 sections link byte-identical.
// The text is 635 bytes off, all inside mot_roll: (1) FP colouring: our named `f29` gets f27 and two hoisted temps get f29 where the original
// has the reverse (rasim FPR replay is exact; no declaration-order permutation of f26..fqm helped); (2) the final block keeps `s`/`co`
// as address-taken locals shared by both branches (declare them above the `if (ab < dbl_45)`, `tmp = 5.0f * s; ... -4.0f + 5.0f * co;`):
// that makes mot_roll 8084 bytes like the original, but the objdiff score drops 98.53 -> 98.38 until (1) is fixed, so it is not applied.
extern const f32 float_135_8041ace8;
extern const f32 float_5_8041acd0;
void mot_roll(void) {
#define vec vec3_802c5d40
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
#ifdef MUST_MATCH
    /* A named constant stops MWCC hoisting the 0.1 load, so the field is added before the constant as in
       the original. */
    f32 kOneTenth = 0.1f;
#endif
#ifdef MUST_MATCH
    /* A local zero makes MWCC load the pooled 0.0 into a register and keep the compared value as the first
       operand, as the original does (its float_0, not a folded literal). */
    f32 zero = 0.0f;
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
        {
            MarioWork* m = marioGetPtr();
            m->unk148 = 0.4f;
            m->unk150.z = 0.01f;
            m->field_68 = 2.0f;
            m->field_6C = vec[0];
        }
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
                *(f32*)((u8*)mario + 0x1AC) = f27;
            } else {
                f30 = 270.0f;
                f27 = 0.1f * (f30 - mario->unk1A8);
                f27 = f27 + mario->unk1A8;
                if (fabsf(f30 - f27 < 1.0f)) {
                    f27 = 270.0f;
                }
                mario->unk1A8 = f27;
                *(f32*)((u8*)mario + 0x1AC) = f27;
            }
        } else {
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

    if (*(u16*)((u8*)mario + 0x248) & 0x200) {
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
            *(u16*)((u8*)mario + 0x248) |= 0x100;
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
        f27 = ((MotRoll*)marioGetPtr()->motStruct)->velX;
        fqm = ((MotRoll*)marioGetPtr()->motStruct)->velY;
        f27 = f27 + 0.2f * (-f27);
        fqm = fqm + 0.2f * (-fqm);
        if (fabsf(f27) < 1.0f) {
            f27 = 0.0f;
        }
        if (fabsf(fqm) < 1.0f) {
            fqm = 0.0f;
        }
        ((MotRoll*)marioGetPtr()->motStruct)->velX = f27;
        ((MotRoll*)marioGetPtr()->motStruct)->velY = fqm;
    }

    if (mario->hitobjStandOn != NULL) {
        if (!(hitGetAttr(mario->hitobjStandOn) & 0xA00) && mario->airTimer == 0 &&
            (*(u16*)((u8*)mario + 0x248) & 0x100) &&
            (strcmp(gp->currentMapName, "eki_04\0") != 0 ||
             !(fabsf(*(f32*)((u8*)mario + 0x214)) >= 40.0f))) {
            MarioWork* m = marioGetPtr();

            camFollowYOff();
            *(f32*)((u8*)m + 0x7C) = 2.85f;
            *(f32*)((u8*)m + 0x80) = -0.14f;
            *(f32*)((u8*)m + 0x84) = 0.08f;
            *(f32*)((u8*)m + 0x88) = -0.02f;
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
            if (*(f32*)((u8*)mario + 0x7C) >= 0.0f) {
                *(f32*)((u8*)mario + 0x7C) = -0.03f;
                *(f32*)((u8*)mario + 0x80) = -0.02f;
                *(f32*)((u8*)mario + 0x84) = -0.02f;
                *(f32*)((u8*)mario + 0x88) = -0.002f;
            }
            mario->flags |= 0x00020000;
        }
        if (mario->airTimer == 0) {
            if (*(f32*)((u8*)mario + 0x7C) < 0.0f) {
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
    f27 = f30;

    if (!(mario->flags & 0x30000)) {
        f26 = (f32)(s8)mario->wStickDir1;
        f30 = (f32)(s8)mario->wStickDir2;
        f27 = distABf(0.0f, 0.0f, f26, f30);
        f26 = revise360(mario->targetYaw + angleABf(0.0f, 0.0f, f26, f30));
        f26 = 3.1415927f * f26 / 180.0f;
        f30 = f27 * (f32)sin(f26);
        f27 = f27 * -(f32)cos(f26);
#ifdef MUST_MATCH
        /* Volatile read keeps the directionView load before the inlined 10.0 load, as in the original. */
        if (rollDispAngleInRange(*(volatile f32*)&mario->directionView,
                                 angleABf(0.0f, 0.0f, f30, f27), 135.0f) == 1) {
#else
        if (rollDispAngleInRange(mario->directionView,
                                 angleABf(0.0f, 0.0f, f30, f27), 135.0f) == 1) {
#endif
            f27 = distABf(0.0f, 0.0f, f30, f27);
            f30 = -f27 * (f32)sin(3.1415927f * mario->directionView / 180.0f);
            f27 = f27 * (f32)cos(3.1415927f * mario->directionView / 180.0f);
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
            f27 = f27 - tmp;
            if (f27 <= -100.0f) {
                f27 = -100.0f;
            }
        } else if (*(f32*)((u8*)mario + 0x218) < -0.05f) {
            f32 tmp;
            f26 = (f31 < 180.0f) ? ((f32*)&rollData)[5] : ((f32*)&rollData)[6];
            tmp = (f32)sin(3.1415927f * *(f32*)((u8*)mario + 0x218) / 180.0f);
            tmp = tmp * f26;
            f27 = f27 - tmp;
            if (f27 >= 100.0f) {
                f27 = 100.0f;
            }
        }
#ifdef MUST_MATCH
        /* Volatile read keeps the directionView load before the inlined 10.0 load, as in the original. */
        if (rollDispAngleInRange(*(volatile f32*)&mario->directionView,
                                 angleABf(0.0f, 0.0f, f30, f27), 135.0f) == 1) {
#else
        if (rollDispAngleInRange(mario->directionView,
                                 angleABf(0.0f, 0.0f, f30, f27), 135.0f) == 1) {
#endif
            f27 = distABf(0.0f, 0.0f, f30, -f27);
            f30 = -f27 * (f32)sin(3.1415927f * mario->directionView / 180.0f);
            f27 = f27 * (f32)cos(3.1415927f * mario->directionView / 180.0f);
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

#ifdef MUST_MATCH
    if (mario->forceMoveTimer == 0 && f30 == zero && f27 == zero) {
#else
    if (mario->forceMoveTimer == 0 && f30 == 0.0f && f27 == 0.0f) {
#endif
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
        if (fabsf(f29) < ((f32*)&rollData)[3]) {
            f29 = 0.0f;
        }
        if (fabsf(f28) < ((f32*)&rollData)[3]) {
            f28 = 0.0f;
        }
    } else {
    if ((*(u32*)((u8*)mario + 0x2A4) & 0x10) && *(f32*)((u8*)mario + 0x220) == 0.0f) {
        MarioWork* m;
        f32 a10;
        f32 s1;
        f32 s2;
        s32 res = 0;
        s32 kind = 0;
        f32 lo;
        f32 hi;
        f32 ref;

        m = marioGetPtr();
        a10 = (f32)(s32)(10.0f * *(f32*)m->unk19C) / 10.0f;
        s1 = (f32)(s8)*(volatile u8*)&m->wStickDir1;
        s2 = (f32)(s8)*(volatile u8*)&m->wStickDir2;
        ref = angleABf(0.0f, 0.0f, s1, s2);
        lo = a10 - 135.0f;
        if (lo < 0.0f) {
            lo += 360.0f;
            kind = 1;
        }
        hi = a10 + float_135_8041ace8;
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
        if (res == 1) {
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
            f29 = f29 + 0.025f * f30;
        } else {
            f29 = f29 + 0.025f * f30;
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
            f28 = f28 - f27 * 0.025f;
        } else {
            f28 = f28 - f27 * 0.025f;
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
    f27 = 3.1415927f * f30 / 180.0f;
    {
        f32 c = (f32)cos(f27);
        f32 s = (f32)sin(f27);
        f27 = distABf(0.0f, 0.0f, f29 * s, f28 * c);
    }
    f27 = 0.05f * f27;
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
                f27 = f27 * mario->playerGravity;
                if (f27 < 0.05f) {
                    f27 = 0.0f;
                }
                mario->baseSpeed = f27;
                if (f27 != 0.0f) {
                    mario->directionView = f30;
                }
            }
        }
    }

#ifdef MUST_MATCH
    if (mario->baseSpeed != zero) {
#else
    if (mario->baseSpeed != 0.0f) {
#endif
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

    f28 = ((f32*)&rollData)[4];
    {
#ifdef MUST_MATCH
        /* Volatile read keeps the directionView load before the inlined 10.0 load, as in the original. */
        s32 r = rollDispAngleInRange(*(volatile f32*)&mario->directionView,
                                     toMovedir(mario->unk1A8), 90.0f);
#else
        s32 r = rollDispAngleInRange(mario->directionView,
                                     toMovedir(mario->unk1A8), 90.0f);
#endif
        f32 t = mario->baseSpeed * f28;
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
    if (i != 0 || f27 == 0.0f) {
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
#ifdef MUST_MATCH
    if (mario->baseSpeed != zero) {
#else
    if (mario->baseSpeed != 0.0f) {
#endif
        MarioWork* m = marioGetPtr();
        m->unk148 = 0.0f;
        m->unk150.z = 0.004f;
    }
    {
        MarioWork* m = marioGetPtr();
        Vec tb;
        Vec d;
        Vec c;
        Vec b;
        Vec a;
        f32 dist = 100.0f;

        a = m->position;
        a.y += m->wPlayerCollisionBox.y;
        b = vec[7];
        if (marioHitCheckVec(&a, &b, &c, &d, &dist) != NULL) {
            f32 ang;
            f32 len = mot_roll_sqrtf(d.x * d.x + d.z * d.z);

            ang = angleABf(0.0f, 0.0f, len, -d.y);
            {
                f32 ab = fabsf(ang);
                f32 n = (f32)(s32)(ang + 0.5f);
                if (ab < dbl_45) {
                    f32 s;
                    f32 c;
                    f32 tmp;
                    f26 = 3.1415927f * n / 180.0f;
                    s = (f32)sin(f26);
                    c = (f32)cos(f26);
                    tmp = float_5_8041acd0 * s;
                    *(f32*)((u8*)m + 0x98) = 6.0f + tmp;
                    tmp = 5.0f * c;
                    *(f32*)((u8*)m + 0x9C) = -4.0f + tmp;
                } else {
                    f32 co;
                    f32 s;

                    sincosf(&s, &co, 90.0f);
                    a = m->position;
                    tb = vec[8];
                    tb.x = s;
                    tb.z = co;
                    b = tb;
                    marioHitCheckVec(&a, &b, &c, &d, &dist);
                }
            }
        }
    }
}
#undef vec


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
// dont_inline: original keeps roll_upstairs as a real call (bl roll_upstairs) from mot_roll;
// MWCC's -inline auto would otherwise inline this small function.
#pragma dont_inline on
void roll_upstairs(void) {
    MarioWork* mario = marioGetPtr();

    mario->position.y += *(f32*)((u8*)mario + 0x7C);
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

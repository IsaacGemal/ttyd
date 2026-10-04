#include "motion/mot_slit.h"
#include "mot/mot_slit.h"
#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "driver/mapdrv.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_pouch.h"
#include "mario/mario_sbr.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>
#include <stdlib.h>
#include "mario/mario_hit.h"
#include "party/party_motion.h"
#include "math_ext.h"

// Local declarations for functions whose headers don't exist yet. Each one is
// listed in REPORT.md so it can move to a header later.

// Raw accessors for MarioWork fields that are not named in include/mario/mario.h.
// The header currently ends at a byte array at 0x2D8, but this unit uses many
// slit-specific fields around that offset. Offsets are listed in REPORT.md.
#define MW_U8(m, off) (*(u8*)((u8*)(m) + (off)))
#define MW_U16(m, off) (*(u16*)((u8*)(m) + (off)))
#define MW_S16(m, off) (*(s16*)((u8*)(m) + (off)))
#define MW_S32(m, off) (*(s32*)((u8*)(m) + (off)))
#define MW_U32(m, off) (*(u32*)((u8*)(m) + (off)))
#define MW_F32(m, off) (*(f32*)((u8*)(m) + (off)))

// .sdata2: the base {0, 0, 0, 255} colour both mapSetPaperAmbColor callers start from.
__declspec(section ".sdata2") const u32 dat_8041ae80 = 0x000000FF;

// Zero vectors that the unit uses as scratch/base for its pooled string
// accesses. They must be the first objects in .rodata for the string offsets
// to match the original.
const Vec vec3_802c5e78 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5e84 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5e90 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5e9c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5ea8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5eb4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5ec0 = { 0.0f, 0.0f, 0.0f };

void unk_800a4e40(void) {
    MarioWork* mario = marioGetPtr();

    marioChgMot2(0x15);
    MW_S16(mario, 0x2D8) = 0xA;
    mario->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING |
                      MARIO_FLAG_HAS_INPUT_JUMP);
}

void N_marioSlitSetAfterLand(void) {
    MarioWork* mario = marioGetPtr();

    marioChgMot2(0x15);
    mario->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING |
                      MARIO_FLAG_HAS_INPUT_JUMP);
}

BOOL marioSlitAbilityChk(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_SLIT_ABILITY_DISABLED) {
        return FALSE;
    }
    {
        s32 item = pouchCheckItem(2);
        if (item == 0) {
            return FALSE;
        }
    }
    if (mario->flags & MARIO_FLAG_IS_8_BIT) {
        return FALSE;
    }
    return marioBgmodeChk() != 1;
}

s32 marioSlitButton(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_USE_PARTNER) {
        return 0;
    }
    if (marioKeyOffChk()) {
        return 0;
    }
    if (marioBgmodeChk() == 1) {
        return 0;
    }
    if (!(mario->dispFlags & (1 << 24))) {
        if (MW_U16(mario, 0x246) & 0x20) {
            return 1;
        }
    } else {
        if (MW_U16(mario, 0x246) & 0x20) {
            return 2;
        }
        if (MW_U8(mario, 0x253) >= 0x32) {
            return 2;
        }
    }
    return 0;
}
BOOL marioSlitKeyDisable(void) {
    MarioWork* mario = marioGetPtr();
    s32 state;

    if (!(mario->flags & MARIO_FLAG_PAPER_MODE)) {
        return TRUE;
    }
    state = MW_S16(mario, 0x2D8);
    if (state >= 0x50 && state <= 0x52) {
        return FALSE;
    }
    if (state >= 0x5A && state <= 0x5E) {
        return FALSE;
    }
    return TRUE;
}

s32 U_marioSlitContinueChk(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2 = marioGetPtr();
    s32 state;

#ifdef MUST_MATCH
    // MUST_MATCH: the original shares a single `state = 0` tail between the
    // `!dispFlags` branch and the final else, so both jump to a common `zero`
    // label. Written as the plain nested if/else below, MWCC emits that block
    // twice (97.03%).
    if (mario2->trigFlags & MARIO_TRIG_FLAG_USE_PARTNER) {
        state = 0;
    } else if (marioKeyOffChk()) {
        state = 0;
    } else if (marioBgmodeChk() == 1) {
        state = 0;
    } else if (!(mario2->dispFlags & (1 << 24))) {
        if (MW_U16(mario2, 0x246) & 0x20) {
            state = 1;
        } else {
            goto zero;
        }
    } else {
        if (MW_U16(mario2, 0x246) & 0x20) {
            state = 2;
        } else if (MW_U8(mario2, 0x253) >= 0x32) {
            state = 2;
        } else {
            goto zero;
        }
    }
    goto done;
zero:
    state = 0;
done:
#else
    if (mario2->trigFlags & MARIO_TRIG_FLAG_USE_PARTNER) {
        state = 0;
    } else if (marioKeyOffChk()) {
        state = 0;
    } else if (marioBgmodeChk() == 1) {
        state = 0;
    } else if (!(mario2->dispFlags & (1 << 24))) {
        if (MW_U16(mario2, 0x246) & 0x20) {
            state = 1;
        } else {
            state = 0;
        }
    } else {
        if (MW_U16(mario2, 0x246) & 0x20) {
            state = 2;
        } else if (MW_U8(mario2, 0x253) >= 0x32) {
            state = 2;
        } else {
            state = 0;
        }
    }
#endif
    if (state != 0) {
        return 1;
    }
    if (marioSlitChkWallAround()) {
        return 1;
    }
    {
        Vec pos = mario->position;
        return marioCreviceWallChk(&pos) != 0;
    }
}

// The six marioHitCheckVec blocks match in call sequence and constants; only float-register allocation
// and the schedule differ. Checks 1/4 use `volatile` copies/loads (LHS `*(volatile Vec*)&A = *pos`, a
// volatile load read into `tmp` then stored back, a volatile `dir.z` store), `dist` written before the
// add, and a MUST_MATCH duplicated dead `dist = 20.0f;`, which is the schedule knob that scores best.
// The `f32 tmp;` local is a real temporary for the volatile `A.y` read; it (or an equivalent form such
// as `tmp = *(volatile f32*)&A.y + 10.0f; A.y = tmp;`) is worth ~0.04-0.5% here. The target's `A.y`
// store is an early word copy (`lwz`/`stw`), then the dir block, then the `A.y` add, then `A.x`/`A.z`;
// ours stores `A.x`/`A.y`/`A.z` together up front. The offset checks keep x+offx/z+offz/y+10/dist in
// f3/f0/f1/f4, ours f4/f3/f1/f0, and the dir block reads sinv/cosv into one register where the target
// keeps them in f1/f0. The MUST_MATCH duplicate emits two `stfs dist` per A block (target has one: 318
// real instructions, ours 320); removing it drops the score to 93.5. This is a permuter candidate: the
// source shape (`*(volatile Vec*)&A = *pos` + temp/volatile `A.y` add, `Vec a = vec3_*; a.x/z/y = ...;
// A = a;`) is right and all calls/constants match.
// NONMATCHING (95.08%): FP register allocation/scheduling only; calls and constants all match.
BOOL marioCreviceWallChk(Vec* pos) {
    Vec A, B, C, D;
    f32 sinv, cosv, dist;
    f32 offx, offz;
    f32 ten = 10.0f;
    f32 angle1;
    f32 angle2;
    f32 tmp;
    HitEntry* hit1;
    HitEntry* hit2;

    marioGetPtr();
    sincosf(&sinv, &cosv, toMovedirSimple(270.0f));
    angle1 = 0.0f;
    angle2 = 0.0f;
    offx = 0.5f * (20.0f * sinv);
    offz = 0.5f * (20.0f * cosv);
    sincosf(&sinv, &cosv, toMovedirSimple(180.0f));

    *(volatile Vec*)&A = *pos;
#ifdef MUST_MATCH
    // The duplicate is dead (overwritten by the line below), but its presence
    // steers MWCC's scheduler into the target's FP register allocation; see the
    // NONMATCHING note above the function.
    dist = 20.0f;
    dist = 20.0f;
#else
    dist = 20.0f;
#endif
    tmp = *(volatile f32*)&A.y;
    A.y = tmp + 10.0f;
    {
        Vec dir = vec3_802c5e78;
        dir.x = sinv;
        *(volatile f32*)&dir.z = cosv;
        B = dir;
    }
    hit1 = marioHitCheckVec(&A, &B, &C, &D, &dist);
    if (hit1) {
        angle1 = angleABf(0.0f, 0.0f, D.x, D.z);
    }
    if (!hit1) {
        {
            Vec a = vec3_802c5e84;
            a.x = pos->x + offx;
            a.z = pos->z + offz;
            a.y = pos->y + ten;
            dist = 20.0f;
            A = a;
        }
        hit1 = marioHitCheckVec(&A, &B, &C, &D, &dist);
        if (hit1) {
            angle1 = angleABf(0.0f, 0.0f, D.x, D.z);
        }
    }
    if (!hit1) {
        {
            Vec a = vec3_802c5e90;
            a.x = pos->x - offx;
            a.z = pos->z - offz;
            a.y = pos->y + ten;
            dist = 20.0f;
            A = a;
        }
        hit1 = marioHitCheckVec(&A, &B, &C, &D, &dist);
        if (hit1) {
            angle1 = angleABf(0.0f, 0.0f, D.x, D.z);
        }
    }

    sincosf(&sinv, &cosv, toMovedirSimple(0.0f));
    *(volatile Vec*)&A = *pos;
#ifdef MUST_MATCH
    // The duplicate is dead (overwritten by the line below), but its presence
    // steers MWCC's scheduler into the target's FP register allocation; see the
    // NONMATCHING note above the function.
    dist = 20.0f;
    dist = 20.0f;
#else
    dist = 20.0f;
#endif
    tmp = *(volatile f32*)&A.y;
    A.y = tmp + 10.0f;
    {
        Vec dir = vec3_802c5e9c;
        dir.x = sinv;
        *(volatile f32*)&dir.z = cosv;
        B = dir;
    }
    hit2 = marioHitCheckVec(&A, &B, &C, &D, &dist);
    if (hit2) {
        angle2 = angleABf(0.0f, 0.0f, D.x, D.z);
    }
    if (!hit2) {
        {
            Vec a = vec3_802c5ea8;
            a.x = pos->x + offx;
            a.z = pos->z + offz;
            a.y = pos->y + ten;
            dist = 20.0f;
            A = a;
        }
        hit2 = marioHitCheckVec(&A, &B, &C, &D, &dist);
        if (hit2) {
            angle2 = angleABf(0.0f, 0.0f, D.x, D.z);
        }
    }
    if (!hit2) {
        {
            Vec a = vec3_802c5eb4;
            a.x = pos->x - offx;
            a.z = pos->z - offz;
            a.y = pos->y + ten;
            dist = 20.0f;
            A = a;
        }
        hit2 = marioHitCheckVec(&A, &B, &C, &D, &dist);
        if (hit2) {
            angle2 = angleABf(0.0f, 0.0f, D.x, D.z);
        }
    }

    if (hit1 && hit2) {
        if (__fabsf(180.0f - 0.5f * (angle1 + angle2)) <= 10.0f) {
            return TRUE;
        }
    }
    return FALSE;
}

void marioClearSlitFloor(void) {
    MarioWork* mario = marioGetPtr();

    MW_S32(mario, 0x268) = 0;
}

BOOL marioGetSlitFloorPos(Vec* pos) {
    MarioWork* mario = marioGetPtr();

    if (MW_S32(mario, 0x268) != 0) {
        *pos = *(Vec*)&MW_U8(mario, 0x26C);
        return TRUE;
    }
    return FALSE;
}

BOOL marioChkSlitThrouh(void) {
    MarioWork* mario = marioGetPtr();
    s32 state;

    if (mario->currMotionId != MARIO_MOTION_SLIT) {
        return FALSE;
    }
    state = MW_S16(mario, 0x2D8);
    if (state >= 0x50 && state <= 0x5E) {
        return TRUE;
    }
    return FALSE;
}

BOOL marioChkSlitThrouhEnd(void) {
    MarioWork* mario = marioGetPtr();
    s32 state;

    if (mario->currMotionId != MARIO_MOTION_SLIT) {
        return FALSE;
    }
    state = MW_S16(mario, 0x2D8);
    if (state == 0x52 || state == 0x5E) {
        return TRUE;
    }
    return FALSE;
}

// opt_propagation off keeps the 0x14 local in r31 across the marioGetPtr call;
// on, MWCC rematerializes it as `li r0` after the call.
#pragma opt_propagation off
void motSlitContinue(void) {
    MarioWork* mario;
    s16 state;

    marioChgMot2(0x15);
    state = 0x14;
    mario = marioGetPtr();
    MW_S16(mario, 0x2D8) = state;
}
#pragma opt_propagation reset


BOOL marioChkSlitEnd(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_SLIT) {
        return FALSE;
    }
    return MW_S16(mario, 0x2D8) >= 100;
}

void motSlitCancel(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        MW_S16(mario, 0x2D8) = 100;
    }
}

void motSlitCancel2(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        allPartyForceSlitOff();
        MW_S16(mario, 0x2D8) = 0x6E;
    }
}

void motSlitCancel3(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        MW_S16(mario, 0x2D8) = 120;
    }
}

s32 marioGetMotSlitCancel3(void) {
    MarioWork* mario;
    s32 ret = 0;

    mario = marioGetPtr();
    if (mario->currMotionId == MARIO_MOTION_SLIT) {
        s32 state = MW_S16(mario, 0x2D8);
        if ((state >= 0x78 && state <= 0x7A) || (state >= 0x6E && state <= 0x70)) {
            ret = 1;
        }
    }
    return ret;
}

// mot_slit carries its own inline copy of the continue check (the branches of the final test differ from the
// standalone U_ function below).
static inline s32 slitContinueChk(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2 = marioGetPtr();
    s32 state;

#ifdef MUST_MATCH
    // MUST_MATCH: the original shares a single `state = 0` tail between the
    // `!dispFlags` branch and the final else, so both jump to a common `zero`
    // label. Written as the plain nested if/else below, MWCC emits that block
    // twice (97.03%).
    if (mario2->trigFlags & MARIO_TRIG_FLAG_USE_PARTNER) {
        state = 0;
    } else if (marioKeyOffChk()) {
        state = 0;
    } else if (marioBgmodeChk() == 1) {
        state = 0;
    } else if (!(mario2->dispFlags & (1 << 24))) {
        if (MW_U16(mario2, 0x246) & 0x20) {
            state = 1;
        } else {
            goto zero;
        }
    } else {
        if (MW_U16(mario2, 0x246) & 0x20) {
            state = 2;
        } else if (MW_U8(mario2, 0x253) >= 0x32) {
            state = 2;
        } else {
            goto zero;
        }
    }
    goto done;
zero:
    state = 0;
done:
#else
    if (mario2->trigFlags & MARIO_TRIG_FLAG_USE_PARTNER) {
        state = 0;
    } else if (marioKeyOffChk()) {
        state = 0;
    } else if (marioBgmodeChk() == 1) {
        state = 0;
    } else if (!(mario2->dispFlags & (1 << 24))) {
        if (MW_U16(mario2, 0x246) & 0x20) {
            state = 1;
        } else {
            state = 0;
        }
    } else {
        if (MW_U16(mario2, 0x246) & 0x20) {
            state = 2;
        } else if (MW_U8(mario2, 0x253) >= 0x32) {
            state = 2;
        } else {
            state = 0;
        }
    }
#endif
    if (state != 0) {
        return 1;
    }
    if (marioSlitChkWallAround()) {
        return 1;
    }
    {
        Vec pos = mario->position;
        if (marioCreviceWallChk(&pos) != 0) {
            return 1;
        }
        return 0;
    }
}

void marioSlitForceCancel(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        mario->flags &= ~MARIO_FLAG_PAPER_MODE;
        if (mario->directionView >= 180.0f) {
            mario->unk1A8 = 0.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
        } else {
            mario->unk1A8 = 180.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
        }
        mario->dispFlags &= 0xFEFFF3FF;
        mario->flags &= ~MARIO_FLAG_PARTY_WALK_DISABLED;
        mario->flags &= ~MARIO_FLAG_GONE_THROUGH_BARS;
        mario->wPlayerCollisionBox.x = 10.0f;
        mario->dispFlags &= 0xFEFFF3FF;
        marioPaperOff();
        mapResetPaperAmbColor();
        mario->dispFlags &= ~4;
        marioAdjustMoveDir();
        mario = marioGetPtr();
        MW_F32(mario, 0x148) = 0.0f;
        MW_F32(mario, 0x158) = 0.004f;
        allPartyForceSlitOff();
    }
}

void mot_slit_post(void) {
    MarioWork* mario = marioGetPtr();

    // The `id == 8` return is the second operand of a `||`, so MWCC keeps a
    // separate `b return` after the `bne` it emits for the short-circuit.
    if (mario->currMotionId == MARIO_MOTION_SLIT) {
        return;
    }
    if ((u16)(mario->currMotionId - 11) <= 1) {
        return;
    }
    if (mario->currMotionId <= 7 || mario->currMotionId == 8) {
        return;
    }
    if (!(mario->flags & MARIO_FLAG_PAPER_MODE)) {
        if (!(mario->dispFlags & (1 << 24))) {
            return;
        }
    MW_F32(mario, 0x1B4) = 20.0f;
    if (mario->directionView >= 180.0f) {
        mario->unk1A8 = 0.0f;
        MW_F32(mario, 0x1AC) = mario->unk1A8;
    } else {
        mario->unk1A8 = 180.0f;
        MW_F32(mario, 0x1AC) = mario->unk1A8;
    }
    mario->flags &= ~MARIO_FLAG_PARTY_WALK_DISABLED;
    mario->flags &= ~MARIO_FLAG_GONE_THROUGH_BARS;
    mario->dispFlags &= 0xFEFFF3FF;
    mario->dispFlags &= 0xFEFFF3FF;
    marioPaperOff();
    mapResetPaperAmbColor();
    mario->dispFlags &= ~4;
    marioAdjustMoveDir();
    allPartyForceSlitOff();
    } else {
        if (mario->currMotionId == MARIO_MOTION_FALL) {
            return;
        }
        mario->flags &= ~MARIO_FLAG_PAPER_MODE;
    MW_F32(mario, 0x1B4) = 20.0f;
    if (mario->directionView >= 180.0f) {
        mario->unk1A8 = 0.0f;
        MW_F32(mario, 0x1AC) = mario->unk1A8;
    } else {
        mario->unk1A8 = 180.0f;
        MW_F32(mario, 0x1AC) = mario->unk1A8;
    }
    mario->flags &= ~MARIO_FLAG_PARTY_WALK_DISABLED;
    mario->flags &= ~MARIO_FLAG_GONE_THROUGH_BARS;
    mario->dispFlags &= 0xFEFFF3FF;
    mario->dispFlags &= 0xFEFFF3FF;
    marioPaperOff();
    mapResetPaperAmbColor();
    mario->dispFlags &= ~4;
    marioAdjustMoveDir();
    allPartyForceSlitOff();
    }
}

void marioForceSlitAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn("p_slit");
    marioChgPose("M_S_1");
    marioChgPaper("PM_S_1A");
    mario->dispFlags |= 4;
}

void marioReInit_slit(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2 = marioGetPtr();

    MW_F32(mario2, 0x2A0) = 0.0f;
    MW_S32(mario2, 0x2A4) = 0;
    MW_S16(mario2, 0x2B6) = 0;
    MW_S16(mario2, 0x2B4) = 0;
    MW_S16(mario2, 0x2BA) = 0;
    mario2->trigFlags &= ~1u;
    mario2->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING |
                       MARIO_FLAG_HAS_INPUT_JUMP);
    mario2->dispFlags |= 4;
    mario2->dispFlags |= 0x01000000;
    if (mario2->paperPoseId == -1) {
#ifdef MUST_MATCH
        GXColor tmp;
        GXColor color;

        // The original builds the colour in a temporary at +0xC and
        // word-copies it into the argument at +0x8.
        tmp = *(GXColor*)&dat_8041ae80;
        tmp.r = 168;
        tmp.g = 168;
        tmp.b = 168;
        color = tmp;
#else
        GXColor color = (GXColor){ 168, 168, 168, 255 };
#endif
        mapSetPaperAmbColor(&color);
        marioPaperOn("p_slit");
    }
    // A named local makes MWCC load the field into f1 and the pool constant
    // into f0, matching the original's compare order.
    {
        f32 zero = 0.0f;
        if (mario2->controlStickSensitivity == zero) {
            marioChgPose("M_S_1");
        }
    }
    marioChgPaper("PM_S_1A");
    mario->trigFlags &= ~1u;
    mario->currMotionId = MARIO_MOTION_SLIT;
    MW_S16(mario, 0x2D8) = 0xA;
    mario->unk1A8 = 270.0f;
    MW_F32(mario, 0x1AC) = mario->unk1A8;
    marioChgPose("M_S_1");
    marioChgPaper("PM_S_1B");
    MW_S16(mario, 0x2D8) = 0xA;
    MW_S16(mario, 0x2B6) = 0;
    MW_F32(mario, 0x1B4) = 6.0f;
    mario->flags |= MARIO_FLAG_PAPER_MODE;
    mario->dispFlags |= 0x01000000;
    allPartySlitOn();
}

// The walk-speed cap the slit's walking states share: once the base speed reaches the dash speed it drops back
// to the walk speed and the run pose is dropped in (unless a pose change is already queued).
static inline void slitCapSpeed(MarioWork* mario) {
    if (mario->baseSpeed >= mario->mBaseDashSpeed) {
        mario->baseSpeed = mario->mBaseWalkSpeed;
        if (!(mario->trigFlags & 0x8000)) {
            marioChgPose("M_R_1");
        }
        MW_S16(mario, 0x2B8) = (s16)(MW_S16(mario, 0x2B8) | 1);
    }
}

// The walk speed for the slit: the base walk speed, halved while crawling slowly (stick inside a small radius
// in the crawl state) or while a 0x02000000 condition holds.
static inline void slitSetWalkSpeed(MarioWork* mario) {
    MarioWork* m = marioGetPtr();
    s32 flags = m->flags;
    f32 speed = MW_F32(m, 0x184);

    if (flags & 0x100000) {
        if (MW_S16(m, 0x2B4) != 0) {
#ifdef MUST_MATCH
            // The original keeps both arms of the empty test (`beq compute; b end`); a bare
            // empty arm is folded away, so keep a no-op here.
            speed = speed;
#endif
        } else {
#ifdef MUST_MATCH
            // The header declares wStickDir1/2 as u8, but the original treats them as signed
            // (`extsb`). Reading them through an s8 lvalue is what reproduces its register
            // allocation (`extsb r4,r4; mullw r3,r4,r4` instead of a copy to r3).
            s8 sx = *(s8*)&m->wStickDir1;
            s8 sy = *(s8*)&m->wStickDir2;
#else
            s32 sx = (s8)m->wStickDir1;
            s32 sy = (s8)m->wStickDir2;
#endif
            if (sx * sx + sy * sy <= 0xBD1) {
                speed *= 0.5f;
            }
        }
    } else if (flags & 0x02000000) {
        speed *= 0.5f;
    }
    mario->baseSpeed = speed;
}

// Resets the squeeze bounce: field 0x148 back to 0 and 0x158 to its resting 0.004.
static inline void slitResetBounce(void) {
    MarioWork* m = marioGetPtr();

    MW_F32(m, 0x148) = 0.0f;
    MW_F32(m, 0x158) = 0.004f;
}

// Mario in the slit (crevice): the paper-mode state machine for squeezing through a gap in a wall.
void mot_slit(void) {
    MarioWork* mario = marioGetPtr();
    s32 state;
    f32 zero = 0.0f;
    s32 count;
    s32 frames;
    s32 manhattan;
    s16 count16;
    f32 angleGoal;
    f32 slitSinv;
    f32 slitCosv;

    if (mario->controlStickSensitivity == zero) {
        mario->trigFlags &= 0xFFFF7FFF;
    }
    if (mario->trigFlags & 1) {
        MarioWork* mario2 = marioGetPtr();

        MW_F32(mario2, 0x2A0) = 0.0f;
        MW_S32(mario2, 0x2A4) = 0;
        MW_S16(mario2, 0x2B6) = 0;
        MW_S16(mario2, 0x2B4) = 0;
        MW_S16(mario2, 0x2BA) = 0;
        mario2->trigFlags &= ~1u;
        mario2->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING |
                           MARIO_FLAG_HAS_INPUT_JUMP);
        mario2->dispFlags |= 4;
        mario2->dispFlags |= 0x01000000;
        if (mario2->paperPoseId == -1) {
#ifdef MUST_MATCH
            GXColor tmp;
            GXColor color;

            tmp = *(GXColor*)&dat_8041ae80;
            tmp.r = 168;
            tmp.g = 168;
            tmp.b = 168;
            color = tmp;
#else
            GXColor color = (GXColor){ 168, 168, 168, 255 };
#endif
            mapSetPaperAmbColor(&color);
            marioPaperOn("p_slit");
        }
        if (mario2->controlStickSensitivity == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1A");
        MW_S16(mario, 0x2D8) = 0;
        MW_S16(mario, 0x50) = 0;
    }
    if (marioRollChgChk() != 0) {
        if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
            mario->unk1A8 = 0.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
        } else {
            mario->unk1A8 = 180.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
        }
        mario->flags &= 0xFFBFFFFF;
        mario->flags &= 0xFFDFFFFF;
        mario->dispFlags &= 0xFEFFF3FF;
        marioPaperOff();
        mario->flags &= 0xFFEFFFFF;
        return;
    }
    switch (MW_S16(mario, 0x2D8)) {
    case 0:
        MW_S32(mario, 0x2DC) = 0x1E;
        MW_S32(mario, 0x2A4) = 0;
        MW_S16(mario, 0x2D8) = 2;
        MW_F32(mario, 0x2AC) = mario->position.y;
        slitCapSpeed(mario);
        marioGetPtr();
        psndSFXOn_3D((const char*)0x177, (Vec*)((u8*)mario + 0x8C));
        break;
    case 2:
        if (mario->controlStickSensitivity == zero) {
            mario->baseSpeed = 0.0f;
            marioChgPose("M_S_1");
        }
        count = MW_S32(mario, 0x2DC) - 1;
        MW_S32(mario, 0x2DC) = count;
        if (count <= 0) {
            mario->flags |= 0x100000;
            MW_S16(mario, 0x2D8) = 4;
            MW_F32(mario, 0x1B4) = 6.0f;
        }
        if (MW_S32(mario, 0x2DC) == 0x14) {
            allPartySlitOn();
        }
        break;
    case 4:
        if (marioChkKey() != 0) {
            if (mario->controlStickSensitivity == zero) {
                mario->baseSpeed = 0.0f;
                marioChgPose("M_S_1");
            }
            mario->directionView = toMovedirSimple(mario->unk1A8);
        }
        slitCapSpeed(mario);
        mario->unk1A8 = 270.0f;
        MW_F32(mario, 0x1AC) = mario->unk1A8;
        if (mario->controlStickSensitivity == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1B");
        MW_S16(mario, 0x2D8) = 0xA;
        MW_S16(mario, 0x2B6) = 0;
        /* fallthrough */
    case 10:
        if (mario->position.y != MW_F32(mario, 0x2AC)) {
            slitResetBounce();
        }
        MW_F32(mario, 0x2AC) = mario->position.y;
        if (MW_S16(mario, 0x4E) != 0) {
            if (!(mario->trigFlags & 0x8000)) {
                marioChgPose("M_W_1");
            }
            marioChgPaper("PM_S_1D");
            if (revise360(mario->directionView - mario->targetYaw) <= 180.0f) {
                MW_S16(mario, 0x2BA) = 1;
                mario->unk1A8 = 260.0f;
            } else {
                MW_S16(mario, 0x2BA) = 2;
                mario->unk1A8 = 280.0f;
            }
            break;
        }
        slitCapSpeed(mario);
        if (marioChkKey() != 0) {
            if (slitContinueChk() != 1) {
                mario->directionView = mario->controlStickAngle;
                MW_S16(mario, 0x2D8) = 0x64;
                break;
            }
        }
        if (marioChkJump() != 0) {
            return;
        }
        if (mario->controlStickSensitivity == zero) {
            mario->baseSpeed = 0.0f;
            if (MW_S32(mario, 0x2A4) == 0) {
                marioChgPose("M_S_1");
                marioChgPaper("PM_S_1B");
            }
        } else {
            s16 flags2B8;
            f32 turn;

            if (strcmp(mario->animName, "M_I_Y") == 0) {
                marioChgPose("M_W_1");
            }
            flags2B8 = MW_S16(mario, 0x2B8);
            if (!(flags2B8 & 1)) {
                slitResetBounce();
            } else {
                MW_S16(mario, 0x2B8) = (s16)(flags2B8 & 0xFFFFFFFE);
                slitResetBounce();
            }
            slitSetWalkSpeed(mario);
            mario->directionView = mario->controlStickAngle;
            if (!(mario->trigFlags & 0x8000)) {
                marioChgPose("M_W_1");
            }
            turn = revise360(mario->controlStickAngle - mario->targetYaw);
            if (turn >= 315.0f || turn <= 45.0f || (turn >= 135.0f && turn <= 225.0f)) {
                marioChgPaper("PM_S_1E");
            } else {
                marioChgPaper("PM_S_1D");
                if (revise360(mario->directionView - mario->targetYaw) <= 180.0f) {
                    MW_S16(mario, 0x2BA) = 1;
                    mario->unk1A8 = 260.0f;
                } else {
                    MW_S16(mario, 0x2BA) = 2;
                    mario->unk1A8 = 280.0f;
                }
            }
            MW_S16(mario, 0x2D8) = 0x14;
        }
        break;
    }
    switch (MW_S16(mario, 0x2D8)) {
    case 20:
        if (MW_S16(mario, 0x2B4) > 0) {
            MW_S16(mario, 0x2B4) = 0;
            marioChgPaper("PM_S_1D");
            if (revise360(mario->directionView - mario->targetYaw) <= 180.0f) {
                angleGoal = 260.0f;
            } else {
                angleGoal = 280.0f;
            }
            mario->unk1A8 = angleGoal;
        }
        if (marioChkKey() != 0) {
            if (slitContinueChk() != 1) {
                MW_S16(mario, 0x2D8) = 0x64;
            }
        }
        if (!(mario->flags & 0x30000)) {
            marioChkJump();
        }
        if (marioChkKey() != 0) {
            if (mario->controlStickSensitivity == zero) {
                mario->baseSpeed = 0.0f;
                count = MW_S16(mario, 0x2B6) + 1;
                MW_S16(mario, 0x2B6) = (s16)count;
                if ((s16)count < 6) {
                    if (slitContinueChk() != 1) {
                        MW_S16(mario, 0x2D8) = 0x64;
                    }
                } else {
                    MW_S16(mario, 0x2D8) = 4;
                }
            } else {
                s8 sx;
                s8 sy;

                MW_S16(mario, 0x2B6) = 0;
                sx = mario->wStickDir1;
                sy = mario->wStickDir2;
                manhattan = abs(sx) + abs(sy);
                if (!(mario->trigFlags & 0x8000)) {
                    const char* pose = "M_R_1";
                    if (manhattan < 0x44) {
                        pose = "M_W_1";
                    }
                    marioChgPose(pose);
                }
                goto keyOffPath;
            }
        } else {
keyOffPath:
            {
                f32 turn;

                if (marioChkKey() != 0) {
                    mario->directionView = mario->controlStickAngle;
                    slitSetWalkSpeed(mario);
                }
                turn = revise360(mario->controlStickAngle - mario->targetYaw);
                if (turn >= 315.0f || turn <= 45.0f || (turn >= 135.0f && turn <= 225.0f)) {
                    marioChgPaper("PM_S_1E");
                    MW_S16(mario, 0x2B6) = 6;
                    MW_S16(mario, 0x2BA) = 0;
                    if (revise360(mario->directionView - mario->targetYaw) <= 180.0f) {
                        angleGoal = 260.0f;
                    } else {
                        angleGoal = 280.0f;
                    }
                    mario->unk1A8 = angleGoal;
                } else {
                    f32 diff;

                    marioChgPaper("PM_S_1D");
                    diff = revise360(mario->directionView - mario->targetYaw);
                    if (MW_S16(mario, 0x2BA) == 0) {
                        s16 side;
                        if (diff >= 180.0f) {
                            side = 2;
                        } else {
                            side = 1;
                        }
                        MW_S16(mario, 0x2BA) = side;
                    }
                    if (MW_S16(mario, 0x2BA) == 1) {
                        if (diff >= 180.0f) {
                            marioChgPaper("PM_S_2");
                            MW_S16(mario, 0x2B4) = 6;
                            MW_S16(mario, 0x2D8) = 0x15;
                            MW_S16(mario, 0x2BA) = 2;
                            slitSetWalkSpeed(mario);
                            mario->unk1A8 = 260.0f;
                        }
                    } else if (MW_S16(mario, 0x2BA) == 2) {
                        if (diff <= 180.0f) {
                            marioChgPaper("PM_S_2");
                            MW_S16(mario, 0x2B4) = 6;
                            MW_S16(mario, 0x2D8) = 0x15;
                            MW_S16(mario, 0x2BA) = 1;
                            slitSetWalkSpeed(mario);
                            mario->unk1A8 = 280.0f;
                        }
                    }
                }
            }
        }
        break;
    case 21:
        count = MW_S16(mario, 0x2B4) - 1;
        MW_S16(mario, 0x2B4) = (s16)count;
        if ((s16)count <= 0) {
            marioChgPaper("PM_S_1D");
            MW_S16(mario, 0x2D8) = 0x14;
            if (MW_S16(mario, 0x2BA) == 1) {
                angleGoal = 260.0f;
            } else {
                angleGoal = 280.0f;
            }
            mario->unk1A8 = angleGoal;
        }
        break;
    }
    if (mario->flags & 0x100000) {
        if ((u32)mario->hitobjPush != 0) {
            if ((u32)mario->hitobjStandOn != 0 && mario->currMotionId == 0x15) {
                if (hitGetAttr((HitEntry*)(s32)mario->hitobjPush) & 0x2000) {
                    f32 turn = revise360(mario->controlStickAngle - mario->targetYaw);

                    if (turn > 90.0f && turn < 270.0f) {
                        MW_F32(mario, 0x2A8) = 180.0f;
                    } else {
                        MW_F32(mario, 0x2A8) = 0.0f;
                    }
                    frames = sysMsec2Frame(0x32);
                    count = MW_S32(mario, 0x2A4) + 1;
                    MW_S32(mario, 0x2A4) = count;
                    if (count >= frames) {
                        HitCheckQuery query;
                        HitCheckQuery* q = &query;
                        Vec dir;

                        mario->directionView = revise360(MW_F32(mario, 0x2A8) + mario->targetYaw);
                        MW_S32(mario, 0x2A4) = 0;
                        sincosf(&slitSinv, &slitCosv, mario->directionView);
                        query.targetPos.x = mario->position.x;
                        dir = vec3_802c5ec0;
                        query.targetPos.y = 11.0f + mario->position.y;
                        query.targetPos.z = mario->position.z;
                        dir.x = slitSinv;
                        dir.z = slitCosv;
                        query.targetDir = dir;
                        query.targetDistance = 20.0f;
                        if (hitCheckVecHitObjXZ(q, (HitEntry*)(s32)mario->hitobjPush) != 0) {
                            query.targetPos.x = mario->position.x;
                            query.targetPos.z = mario->position.z;
                            query.targetDistance = 20.0f;
                            movePos(&q->targetPos.x, &q->targetPos.z, 1.0f,
                                    reviseAngle(90.0f + MW_F32(mario, 0x19C)));
                            if (hitCheckVecHitObjXZ(q, (HitEntry*)(s32)mario->hitobjPush) != 0) {
                                query.targetPos.x = mario->position.x;
                                query.targetPos.z = mario->position.z;
                                query.targetDistance = 20.0f;
                                movePos(&q->targetPos.x, &q->targetPos.z, 1.0f,
                                        reviseAngle(MW_F32(mario, 0x19C) - 90.0f));
                                if (hitCheckVecHitObjXZ(q, (HitEntry*)(s32)mario->hitobjPush) != 0) {
                                    MW_S16(mario, 0x2D8) = 0x50;
                                }
                            }
                        }
                    }
                } else {
                    MW_S32(mario, 0x2A4) = 0;
                }
            }
        } else {
            state = MW_S16(mario, 0x2D8);
            if ((state < 0x5A || state > 0x5E) && (u32)mario->hitobjStandOn != 0 &&
                (hitGetAttr((HitEntry*)(s32)mario->hitobjStandOn) & 0x2000)) {
                s32 count = MW_S32(mario, 0x2A4);

                MW_S32(mario, 0x2A4) = count + 3;
                marioChgPaper("PM_S_1H");
                if (MW_S32(mario, 0x2A4) >= 0xF && mario->currMotionId == 0x15) {
                    MW_S16(mario, 0x2D8) = 0x5A;
                }
            } else {
                MW_S32(mario, 0x2A4) = MW_S32(mario, 0x2A4) - 1;
                if (MW_S32(mario, 0x2A4) < 0) {
                    MW_S32(mario, 0x2A4) = 0;
                }
            }
        }
    }
    switch (MW_S16(mario, 0x2D8)) {
    case 0x50:
        camGetPtr(8)->flags |= 0x200;
        slitResetBounce();
        mario->flags |= 0x10000000;
        if (zero == MW_F32(mario, 0x2A8)) {
            marioChgPaper("PM_S_1F");
        } else {
            marioChgPaper("PM_S_1G");
        }
        mario->flags |= 0x400;
        MW_S32(mario, 0x2DC) = 0x14;
        mario->baseSpeed = (2.0f * MW_F32(mario, 0x1B4)) / (f32)MW_S32(mario, 0x2DC);
        MW_S16(mario, 0x2D8) = 0x51;
        mario->flags |= 0x200000;
        break;
    case 0x51:
        count = MW_S32(mario, 0x2DC) - 1;
        MW_S32(mario, 0x2DC) = count;
        if (count <= 0) {
            MW_S16(mario, 0x2D8) = 0x52;
        }
        break;
    case 0x52:
        camGetPtr(8)->flags &= 0xFFFFFDFF;
        mario->flags &= 0xFFFFFBFF;
        MW_S16(mario, 0x2D8) = 0x14;
        mario->flags &= 0xEFFFFFFF;
        break;
    }
    switch (MW_S16(mario, 0x2D8)) {
    case 0x5A:
        camGetPtr(8)->flags |= 0x200;
        slitResetBounce();
        mario->flags |= 0x10000000;
        marioChgPaper("PM_S_1H");
        MW_S32(mario, 0x2DC) = sysMsec2Frame(0x1F4);
        mario->baseSpeed = 0.0f;
        MW_S16(mario, 0x2D8) = 0x5B;
        {
            MarioWork* m = marioGetPtr();

            MW_S32(m, 0x268) = 1;
            *(Vec*)((u8*)m + 0x26C) = m->position;
        }
        /* fallthrough */
    case 0x5B:
        MW_S32(mario, 0x2DC) = 0xF;
        MW_S16(mario, 0x2D8) = 0x5C;
        mario->flags |= 0x100;
        marioSetFallPara();
        mario->flags &= 0xFFFEFFFF;
        mario->flags |= 0xA0000;
        /* fallthrough */
    case 0x5C:
        if (revise360(mario->directionView - mario->targetYaw) <= 180.0f) {
            angleGoal = 260.0f;
        } else {
            angleGoal = 280.0f;
        }
        mario->unk1A8 = angleGoal;
        count = MW_S32(mario, 0x2DC) - 1;
        MW_S32(mario, 0x2DC) = count;
        if (count <= 0) {
            MW_S16(mario, 0x2D8) = 0x5D;
            mario->flags &= 0xFFFFFEFF;
            mario->flags &= 0xEFFFFFFF;
    case 0x5D:
            if ((u32)mario->hitobjStandOn != 0) {
                mario->flags &= 0xFFF0FFFF;
                MW_S16(mario, 0x2D8) = 0x5E;
    case 0x5E:
                camGetPtr(8)->flags &= 0xFFFFFDFF;
                MW_S16(mario, 0x2D8) = 0x14;
            }
        }
        break;
    }
    switch (MW_S16(mario, 0x2D8)) {
    case 0x64:
        mario->flags &= 0xFFDFFFFF;
        MW_S16(mario, 0x2DA) = 0;
        mario->flags &= 0xFFEFFFFF;
        MW_F32(mario, 0x1B4) = 20.0f;
        if (zero == mario->baseSpeed) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1C");
        MW_S16(mario, 0x2DA) = 0x1E;
        MW_S16(mario, 0x2D8) = 0x65;
        if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
            mario->unk1A8 = 0.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
            if (mario->controlStickSensitivity == zero) {
                mario->directionView = toMovedirSimple(mario->unk1A8);
            }
        } else {
            mario->unk1A8 = 180.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
            if (mario->controlStickSensitivity == zero) {
                mario->directionView = toMovedirSimple(mario->unk1A8);
            }
        }
        psndSFXOn_3D((const char*)0x178, (Vec*)((u8*)mario + 0x8C));
        /* fallthrough */
    case 0x65:
        if (mario->controlStickSensitivity == zero) {
            mario->baseSpeed = 0.0f;
            marioChgPose("M_S_1");
        }
        count16 = MW_S16(mario, 0x2DA);
        count16--;
        MW_S16(mario, 0x2DA) = count16;
        if (count16 <= 0) {
            MW_S16(mario, 0x2D8) = 0x66;
        }
        return;
    case 0x66:
        if (mario->controlStickSensitivity == zero) {
            mario->baseSpeed = 0.0f;
            marioChgPose("M_S_1");
        }
        mario->flags &= 0xFFBFFFFF;
        mario->flags &= 0xFFDFFFFF;
        mario->dispFlags &= 0xFEFFF3FF;
        marioPaperOff();
        mapResetPaperAmbColor();
        mario->dispFlags &= 0xFFFFFFFB;
        marioAdjustMoveDir();
        allPartySlitOff();
        slitResetBounce();
        marioChgMotSub(0, 0);
        return;
    case 0x6E:
        mario->flags &= 0xFFBFFFFF;
        mario->flags &= 0xFFDFFFFF;
        mario->flags &= 0xEFFFFFFF;
        mario->flags &= 0xFFFFFBFF;
        MW_S16(mario, 0x2DA) = 0;
        mario->flags &= 0xFFEFFFFF;
        MW_F32(mario, 0x1B4) = 20.0f;
        if (mario->controlStickSensitivity == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1C");
        MW_S16(mario, 0x2DA) = 0x1E;
        MW_S16(mario, 0x2D8) = 0x6F;
        mario->baseSpeed = 0.0f;
        if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
            mario->unk1A8 = 0.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
            mario->directionView = toMovedirSimple(mario->unk1A8);
        } else {
            mario->unk1A8 = 180.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
            mario->directionView = toMovedirSimple(mario->unk1A8);
        }
        psndSFXOn_3D((const char*)0x178, (Vec*)((u8*)mario + 0x8C));
        /* fallthrough */
    case 0x6F:
        count16 = MW_S16(mario, 0x2DA);
        count16--;
        MW_S16(mario, 0x2DA) = count16;
        if (count16 <= 0) {
            MW_S16(mario, 0x2D8) = 0x70;
            return;
        }
        break;
    case 0x70:
        mario->flags &= 0xFFBFFFFF;
        mario->flags &= 0xFFDFFFFF;
        mario->dispFlags &= 0xFEFFF3FF;
        marioPaperOff();
        mapResetPaperAmbColor();
        mario->dispFlags &= 0xFFFFFFFB;
        marioAdjustMoveDir();
        allPartySlitOff();
        slitResetBounce();
        marioChgMot(0);
        return;
    case 0x78:
        MW_S16(mario, 0x2DA) = 0;
        mario->flags &= 0xFFEFFFFF;
        MW_F32(mario, 0x1B4) = 20.0f;
        if (mario->controlStickSensitivity == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1C");
        MW_S16(mario, 0x2DA) = 0x1E;
        MW_S16(mario, 0x2D8) = 0x79;
        mario->baseSpeed = 0.0f;
        if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
            mario->unk1A8 = 0.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
            mario->directionView = toMovedirSimple(mario->unk1A8);
        } else {
            mario->unk1A8 = 180.0f;
            MW_F32(mario, 0x1AC) = mario->unk1A8;
            mario->directionView = toMovedirSimple(mario->unk1A8);
        }
        psndSFXOn_3D((const char*)0x178, (Vec*)((u8*)mario + 0x8C));
        /* fallthrough */
    case 0x79:
        count16 = MW_S16(mario, 0x2DA);
        count16--;
        MW_S16(mario, 0x2DA) = count16;
        if (count16 <= 0) {
            MW_S16(mario, 0x2D8) = 0x7A;
            return;
        }
        break;
    case 0x7A:
        mario->flags &= 0xFFBFFFFF;
        mario->flags &= 0xFFDFFFFF;
        mario->flags &= 0xFFFFFBFF;
        mario->dispFlags &= 0xFEFFF3FF;
        marioPaperOff();
        mapResetPaperAmbColor();
        mario->dispFlags &= 0xFFFFFFFB;
        marioAdjustMoveDir();
        allPartySlitOff();
        slitResetBounce();
        marioChgMot(0);
        break;
    }
}

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

void marioClearSlitFloor(void) {
    MarioWork* mario = marioGetPtr();

    MW_S32(mario, 0x268) = 0;
}

void motSlitCancel(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        MW_S16(mario, 0x2D8) = 100;
    }
}

void motSlitCancel3(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        MW_S16(mario, 0x2D8) = 120;
    }
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

void N_marioSlitSetAfterLand(void) {
    MarioWork* mario = marioGetPtr();

    marioChgMot2(0x15);
    mario->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING |
                      MARIO_FLAG_HAS_INPUT_JUMP);
}

void unk_800a4e40(void) {
    MarioWork* mario = marioGetPtr();

    marioChgMot2(0x15);
    MW_S16(mario, 0x2D8) = 0xA;
    mario->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING |
                      MARIO_FLAG_HAS_INPUT_JUMP);
}

BOOL marioChkSlitEnd(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_SLIT) {
        return FALSE;
    }
    return MW_S16(mario, 0x2D8) >= 100;
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

BOOL marioGetSlitFloorPos(Vec* pos) {
    MarioWork* mario = marioGetPtr();

    if (MW_S32(mario, 0x268) != 0) {
        *pos = *(Vec*)((u8*)mario + 0x26C);
        return TRUE;
    }
    return FALSE;
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

void marioForceSlitAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn("p_slit");
    marioChgPose("M_S_1");
    marioChgPaper("PM_S_1A");
    mario->dispFlags |= 4;
}

void motSlitCancel2(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        allPartyForceSlitOff();
        MW_S16(mario, 0x2D8) = 0x6E;
    }
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
        tmp = (GXColor){ 0, 0, 0, 255 };
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
    MW_F32(mario, 0x1A8) = 270.0f;
    MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
    marioChgPose("M_S_1");
    marioChgPaper("PM_S_1B");
    MW_S16(mario, 0x2D8) = 0xA;
    MW_S16(mario, 0x2B6) = 0;
    MW_F32(mario, 0x1B4) = 6.0f;
    mario->flags |= MARIO_FLAG_PAPER_MODE;
    mario->dispFlags |= 0x01000000;
    allPartySlitOn();
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
        MW_F32(mario, 0x1A8) = 0.0f;
        MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
    } else {
        MW_F32(mario, 0x1A8) = 180.0f;
        MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
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
        MW_F32(mario, 0x1A8) = 0.0f;
        MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
    } else {
        MW_F32(mario, 0x1A8) = 180.0f;
        MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
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

void marioSlitForceCancel(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        mario->flags &= ~MARIO_FLAG_PAPER_MODE;
        if (mario->directionView >= 180.0f) {
            MW_F32(mario, 0x1A8) = 0.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
        } else {
            MW_F32(mario, 0x1A8) = 180.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
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

// NONMATCHING (63.22%): 41 instructions differ, 38 missing, 104 extra, plus register differences.
BOOL marioCreviceWallChk(Vec* pos) {
    Vec A, B, C, D;
    f32 dist;
    f32 sinv, cosv;
    f32 offx, offz;
    f32 angle1 = 0.0f;
    f32 angle2 = 0.0f;
    HitEntry* hit;
    HitEntry* hit1;
    HitEntry* hit2;

#define CREVICE_CHECK(base, ox, oz, target)                                            \
    {                                                                                  \
        Vec dir = (base);                                                              \
        A.x = pos->x + (ox);                                                           \
        A.y = pos->y + 10.0f;                                                          \
        A.z = pos->z + (oz);                                                           \
        dir.x = sinv;                                                                  \
        dir.z = cosv;                                                                  \
        B = dir;                                                                       \
        dist = 20.0f;                                                                  \
        hit = marioHitCheckVec(&A, &B, &C, &D, &dist);                                 \
        if (hit) {                                                                     \
            target = angleABf(0.0f, 0.0f, D.x, D.z);                                    \
        }                                                                              \
    }

    marioGetPtr();
    sincosf(&sinv, &cosv, toMovedirSimple(270.0f));
    offx = 0.5f * (20.0f * sinv);
    offz = 0.5f * (20.0f * cosv);
    sincosf(&sinv, &cosv, toMovedirSimple(180.0f));

    CREVICE_CHECK(vec3_802c5e78, 0.0f, 0.0f, angle1);
    hit1 = hit;
    if (!hit1) {
        CREVICE_CHECK(vec3_802c5e84, offx, offz, angle1);
        hit1 = hit;
        if (!hit1) {
            CREVICE_CHECK(vec3_802c5e90, -offx, -offz, angle1);
            hit1 = hit;
        }
    }

    sincosf(&sinv, &cosv, toMovedirSimple(0.0f));
    CREVICE_CHECK(vec3_802c5e9c, 0.0f, 0.0f, angle2);
    hit2 = hit;
    if (!hit2) {
        CREVICE_CHECK(vec3_802c5ea8, offx, offz, angle2);
        hit2 = hit;
        if (!hit2) {
            CREVICE_CHECK(vec3_802c5eb4, -offx, -offz, angle2);
            hit2 = hit;
        }
    }

    if (hit1 && hit2) {
        if (fabsf(180.0f - 0.5f * (angle1 + angle2)) <= 10.0f) {
            return TRUE;
        }
    }
    return FALSE;
#undef CREVICE_CHECK
}

// The walk-speed cap the slit's walking states share: once the base speed reaches the dash speed it drops back
// to the walk speed and the run pose is dropped in (unless a pose change is already queued).
static inline void slitCapSpeed(MarioWork* mario) {
    if (MW_F32(mario, 0x180) >= MW_F32(mario, 0x188)) {
        MW_F32(mario, 0x180) = MW_F32(mario, 0x184);
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
        } else {
            s8 sx = MW_U8(m, 0x24E);
            s8 sy = MW_U8(m, 0x24F);
            if (sx * sx + sy * sy <= 0xBD1) {
                speed *= 0.5f;
            }
        }
    } else if (flags & 0x02000000) {
        speed *= 0.5f;
    }
    MW_F32(mario, 0x180) = speed;
}

// Pushes the pose/paper hint that matches which way the stick points relative to the crevice.
static inline void slitResetBounce(void) {
    MarioWork* m = marioGetPtr();

    MW_F32(m, 0x148) = 0.0f;
    MW_F32(m, 0x158) = 0.004f;
}

// NONMATCHING (99.45%): the inlined walk-speed block (`slitSetWalkSpeed`) leaves its empty `if (field != 0) {}` arm as
// `beq next; b end` in the target and `bne end` here (four inline sites), and the registers of the s8 products differ.
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

    if (MW_F32(mario, 0x190) == zero) {
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

            tmp = (GXColor){ 0, 0, 0, 255 };
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
        if (revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198)) >= 180.0f) {
            MW_F32(mario, 0x1A8) = 0.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
        } else {
            MW_F32(mario, 0x1A8) = 180.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
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
        MW_F32(mario, 0x2AC) = MW_F32(mario, 0x90);
        slitCapSpeed(mario);
        marioGetPtr();
        psndSFXOn_3D((const char*)0x177, (Vec*)((u8*)mario + 0x8C));
        break;
    case 2:
        if (MW_F32(mario, 0x190) == zero) {
            MW_F32(mario, 0x180) = 0.0f;
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
            if (MW_F32(mario, 0x190) == zero) {
                MW_F32(mario, 0x180) = 0.0f;
                marioChgPose("M_S_1");
            }
            MW_F32(mario, 0x1A0) = toMovedirSimple(MW_F32(mario, 0x1A8));
        }
        slitCapSpeed(mario);
        MW_F32(mario, 0x1A8) = 270.0f;
        MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
        if (MW_F32(mario, 0x190) == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1B");
        MW_S16(mario, 0x2D8) = 0xA;
        MW_S16(mario, 0x2B6) = 0;
        /* fallthrough */
    case 10:
        if (MW_F32(mario, 0x90) != MW_F32(mario, 0x2AC)) {
            slitResetBounce();
        }
        MW_F32(mario, 0x2AC) = MW_F32(mario, 0x90);
        if (MW_S16(mario, 0x4E) != 0) {
            if (!(mario->trigFlags & 0x8000)) {
                marioChgPose("M_W_1");
            }
            marioChgPaper("PM_S_1D");
            if (revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198)) <= 180.0f) {
                MW_S16(mario, 0x2BA) = 1;
                MW_F32(mario, 0x1A8) = 260.0f;
            } else {
                MW_S16(mario, 0x2BA) = 2;
                MW_F32(mario, 0x1A8) = 280.0f;
            }
            break;
        }
        slitCapSpeed(mario);
        if (marioChkKey() != 0) {
            if (slitContinueChk() != 1) {
                MW_F32(mario, 0x1A0) = MW_F32(mario, 0x194);
                MW_S16(mario, 0x2D8) = 0x64;
                break;
            }
        }
        if (marioChkJump() == 0) {
            if (MW_F32(mario, 0x190) == zero) {
                MW_F32(mario, 0x180) = 0.0f;
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
                MW_F32(mario, 0x1A0) = MW_F32(mario, 0x194);
                if (!(mario->trigFlags & 0x8000)) {
                    marioChgPose("M_W_1");
                }
                turn = revise360(MW_F32(mario, 0x194) - MW_F32(mario, 0x198));
                if (turn >= 315.0f || turn <= 45.0f || (turn >= 135.0f && turn <= 225.0f)) {
                    marioChgPaper("PM_S_1E");
                } else {
                    marioChgPaper("PM_S_1D");
                    if (revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198)) <= 180.0f) {
                        MW_S16(mario, 0x2BA) = 1;
                        MW_F32(mario, 0x1A8) = 260.0f;
                    } else {
                        MW_S16(mario, 0x2BA) = 2;
                        MW_F32(mario, 0x1A8) = 280.0f;
                    }
                }
                MW_S16(mario, 0x2D8) = 0x14;
            }
        }
        break;
    }
    switch (MW_S16(mario, 0x2D8)) {
    case 20:
        if (MW_S16(mario, 0x2B4) > 0) {
            MW_S16(mario, 0x2B4) = 0;
            marioChgPaper("PM_S_1D");
            if (revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198)) <= 180.0f) {
                angleGoal = 260.0f;
            } else {
                angleGoal = 280.0f;
            }
            MW_F32(mario, 0x1A8) = angleGoal;
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
            if (MW_F32(mario, 0x190) == zero) {
                MW_F32(mario, 0x180) = 0.0f;
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
                sx = MW_U8(mario, 0x24E);
                sy = MW_U8(mario, 0x24F);
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
            if (marioChkKey() != 0) {
                f32 turn;

                MW_F32(mario, 0x1A0) = MW_F32(mario, 0x194);
                slitSetWalkSpeed(mario);
                turn = revise360(MW_F32(mario, 0x194) - MW_F32(mario, 0x198));
                if (turn >= 315.0f || turn <= 45.0f || (turn >= 135.0f && turn <= 225.0f)) {
                    marioChgPaper("PM_S_1E");
                    MW_S16(mario, 0x2B6) = 6;
                    MW_S16(mario, 0x2BA) = 0;
                    if (revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198)) <= 180.0f) {
                        angleGoal = 260.0f;
                    } else {
                        angleGoal = 280.0f;
                    }
                    MW_F32(mario, 0x1A8) = angleGoal;
                } else {
                    f32 diff;

                    marioChgPaper("PM_S_1D");
                    diff = revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198));
                    if (MW_S16(mario, 0x2BA) == 0) {
                        if (diff >= 180.0f) {
                            MW_S16(mario, 0x2BA) = 2;
                        } else {
                            MW_S16(mario, 0x2BA) = 1;
                        }
                    }
                    if (MW_S16(mario, 0x2BA) == 1) {
                        if (diff >= 180.0f) {
                            marioChgPaper("PM_S_2");
                            MW_S16(mario, 0x2B4) = 6;
                            MW_S16(mario, 0x2D8) = 0x15;
                            MW_S16(mario, 0x2BA) = 2;
                            slitSetWalkSpeed(mario);
                            MW_F32(mario, 0x1A8) = 260.0f;
                        }
                    } else if (MW_S16(mario, 0x2BA) == 2) {
                        if (diff <= 180.0f) {
                            marioChgPaper("PM_S_2");
                            MW_S16(mario, 0x2B4) = 6;
                            MW_S16(mario, 0x2D8) = 0x15;
                            MW_S16(mario, 0x2BA) = 1;
                            slitSetWalkSpeed(mario);
                            MW_F32(mario, 0x1A8) = 280.0f;
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
            MW_F32(mario, 0x1A8) = angleGoal;
        }
        break;
    }
    if (mario->flags & 0x100000) {
        if (MW_U32(mario, 0x1EC) != 0) {
            if (MW_U32(mario, 0x1E4) != 0 && MW_U16(mario, 0x2E) == 0x15) {
                if (hitGetAttr((HitEntry*)MW_S32(mario, 0x1E4)) & 0x2000) {
                    f32 turn = revise360(MW_F32(mario, 0x194) - MW_F32(mario, 0x198));

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
                        f32 sinv;
                        f32 cosv;

                        MW_F32(mario, 0x1A0) = revise360(MW_F32(mario, 0x2A8) + MW_F32(mario, 0x198));
                        MW_S32(mario, 0x2A4) = 0;
                        sincosf(&sinv, &cosv, MW_F32(mario, 0x1A0));
                        query.targetPos.x = MW_F32(mario, 0x8C);
                        dir = vec3_802c5ec0;
                        query.targetPos.y = 11.0f + MW_F32(mario, 0x90);
                        query.targetPos.z = MW_F32(mario, 0x94);
                        dir.x = sinv;
                        dir.z = cosv;
                        query.targetDir = dir;
                        query.targetDistance = 20.0f;
                        if (hitCheckVecHitObjXZ(q, (HitEntry*)MW_S32(mario, 0x1EC)) != 0) {
                            query.targetPos.x = MW_F32(mario, 0x8C);
                            query.targetPos.z = MW_F32(mario, 0x94);
                            query.targetDistance = 20.0f;
                            movePos(&q->targetPos.x, &q->targetPos.z, 1.0f,
                                    reviseAngle(90.0f + MW_F32(mario, 0x19C)));
                            if (hitCheckVecHitObjXZ(q, (HitEntry*)MW_S32(mario, 0x1EC)) != 0) {
                                query.targetPos.x = MW_F32(mario, 0x8C);
                                query.targetPos.z = MW_F32(mario, 0x94);
                                query.targetDistance = 20.0f;
                                movePos(&q->targetPos.x, &q->targetPos.z, 1.0f,
                                        reviseAngle(MW_F32(mario, 0x19C) - 90.0f));
                                if (hitCheckVecHitObjXZ(q, (HitEntry*)MW_S32(mario, 0x1EC)) != 0) {
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
            if ((state < 0x5A || state > 0x5E) && MW_U32(mario, 0x1E4) != 0 &&
                (hitGetAttr((HitEntry*)MW_S32(mario, 0x1E4)) & 0x2000)) {
                s32 count = MW_S32(mario, 0x2A4);

                MW_S32(mario, 0x2A4) = count + 3;
                marioChgPaper("PM_S_1H");
                if (MW_S32(mario, 0x2A4) >= 0xF && MW_U16(mario, 0x2E) == 0x15) {
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
        MW_F32(mario, 0x180) = (2.0f * MW_F32(mario, 0x1B4)) / (f32)MW_S32(mario, 0x2DC);
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
        MW_F32(mario, 0x180) = 0.0f;
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
        if (revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198)) <= 180.0f) {
            angleGoal = 260.0f;
        } else {
            angleGoal = 280.0f;
        }
        MW_F32(mario, 0x1A8) = angleGoal;
        count = MW_S32(mario, 0x2DC) - 1;
        MW_S32(mario, 0x2DC) = count;
        if (count <= 0) {
            MW_S16(mario, 0x2D8) = 0x5D;
            mario->flags &= 0xFFFFFEFF;
            mario->flags &= 0xEFFFFFFF;
    case 0x5D:
            if (MW_U32(mario, 0x1E4) != 0) {
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
        if (zero == MW_F32(mario, 0x180)) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1C");
        MW_S16(mario, 0x2DA) = 0x1E;
        MW_S16(mario, 0x2D8) = 0x65;
        if (revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198)) >= 180.0f) {
            MW_F32(mario, 0x1A8) = 0.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
            if (MW_F32(mario, 0x190) == zero) {
                MW_F32(mario, 0x1A0) = toMovedirSimple(MW_F32(mario, 0x1A8));
            }
        } else {
            MW_F32(mario, 0x1A8) = 180.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
            if (MW_F32(mario, 0x190) == zero) {
                MW_F32(mario, 0x1A0) = toMovedirSimple(MW_F32(mario, 0x1A8));
            }
        }
        psndSFXOn_3D((const char*)0x178, (Vec*)((u8*)mario + 0x8C));
        /* fallthrough */
    case 0x65:
        if (MW_F32(mario, 0x190) == zero) {
            MW_F32(mario, 0x180) = 0.0f;
            marioChgPose("M_S_1");
        }
        count16 = MW_S16(mario, 0x2DA) - 1;
        MW_S16(mario, 0x2DA) = count16;
        if (count16 <= 0) {
            MW_S16(mario, 0x2D8) = 0x66;
            return;
        }
    default:
        return;
    case 0x66:
        if (MW_F32(mario, 0x190) == zero) {
            MW_F32(mario, 0x180) = 0.0f;
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
        if (MW_F32(mario, 0x190) == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1C");
        MW_S16(mario, 0x2DA) = 0x1E;
        MW_S16(mario, 0x2D8) = 0x6F;
        MW_F32(mario, 0x180) = 0.0f;
        if (revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198)) >= 180.0f) {
            MW_F32(mario, 0x1A8) = 0.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
            MW_F32(mario, 0x1A0) = toMovedirSimple(MW_F32(mario, 0x1A8));
        } else {
            MW_F32(mario, 0x1A8) = 180.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
            MW_F32(mario, 0x1A0) = toMovedirSimple(MW_F32(mario, 0x1A8));
        }
        psndSFXOn_3D((const char*)0x178, (Vec*)((u8*)mario + 0x8C));
        /* fallthrough */
    case 0x6F:
        count16 = MW_S16(mario, 0x2DA) - 1;
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
        if (MW_F32(mario, 0x190) == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1C");
        MW_S16(mario, 0x2DA) = 0x1E;
        MW_S16(mario, 0x2D8) = 0x79;
        MW_F32(mario, 0x180) = 0.0f;
        if (revise360(MW_F32(mario, 0x1A0) - MW_F32(mario, 0x198)) >= 180.0f) {
            MW_F32(mario, 0x1A8) = 0.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
            MW_F32(mario, 0x1A0) = toMovedirSimple(MW_F32(mario, 0x1A8));
        } else {
            MW_F32(mario, 0x1A8) = 180.0f;
            MW_F32(mario, 0x1AC) = MW_F32(mario, 0x1A8);
            MW_F32(mario, 0x1A0) = toMovedirSimple(MW_F32(mario, 0x1A8));
        }
        psndSFXOn_3D((const char*)0x178, (Vec*)((u8*)mario + 0x8C));
        /* fallthrough */
    case 0x79:
        count16 = MW_S16(mario, 0x2DA) - 1;
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

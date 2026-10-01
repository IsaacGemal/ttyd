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

// NONMATCHING (71.07%): the original keeps the constant 0x14 in r31 across the
// marioGetPtr call (a live local); MWCC folds it to r0 here.
void motSlitContinue(void) {
    MarioWork* mario;
    s16 state;

    marioChgMot2(0x15);
    state = 0x14;
    mario = marioGetPtr();
    MW_S16(mario, 0x2D8) = state;
}

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

// NONMATCHING (96.36%): the GXColor is built in a stack temporary at +0xC and
// word-copied to +0x8; we build it directly. The controlStickSensitivity
// compare also loads the constant into f1 instead of the field.
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
        GXColor color;
        color = (GXColor){ 0, 0, 0, 255 };
        color.r = 168;
        color.g = 168;
        color.b = 168;
        mapSetPaperAmbColor(&color);
        marioPaperOn("p_slit");
    }
    if (0.0f == mario2->controlStickSensitivity) {
        marioChgPose("M_S_1");
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

// NONMATCHING (97.03%): emits the default `state = 0` block twice (dispFlags
// branch and final else) instead of sharing one; everything else matches.
s32 U_marioSlitContinueChk(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2 = marioGetPtr();
    s32 state;

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

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

// .sdata2: the base {0, 0, 0, 255} colour both mapSetPaperAmbColor callers start from.
__declspec(section ".sdata2") const u32 dat_8041ae80 = 0x000000FF;

void unk_800a4e40(void) {
    MarioWork* mario = marioGetPtr();

    marioChgMot2(0x15);
    mario->slitState = 0xA;
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
        if (mario->buttonHeld & 0x20) {
            return 1;
        }
    } else {
        if (mario->buttonHeld & 0x20) {
            return 2;
        }
        if (mario->triggerRight >= 0x32) {
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
    state = mario->slitState;
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

    if (marioSlitButton() != 0) {
        return 1;
    }
    if (marioSlitChkWallAround()) {
        return 1;
    }
    return marioCreviceWallChk(mario->position) != 0;
}

BOOL marioCreviceWallChk(Vec pos) {
    Vec A, B, C, D;
    f32 sinv, cosv, dist;
    f32 offx, offz;
    f32 ten = 10.0f;
    f32 angle1;
    f32 angle2;
    HitEntry* hit1;
    HitEntry* hit2;

    marioGetPtr();
    sincosf(&sinv, &cosv, toMovedirSimple(270.0f));
    angle1 = 0.0f;
    angle2 = 0.0f;
    offx = 0.5f * (20.0f * sinv);
    offz = 0.5f * (20.0f * cosv);
    sincosf(&sinv, &cosv, toMovedirSimple(180.0f));

    dist = 20.0f;
    A = pos;
    A.y += ten;
    B = (Vec){sinv, 0.0f, cosv};
    hit1 = marioHitCheckVec(&A, &B, &C, &D, &dist);
    if (hit1) {
        angle1 = angleABf(0.0f, 0.0f, D.x, D.z);
    }
    if (!hit1) {
        dist = 20.0f;
        A = (Vec){pos.x + offx, pos.y + ten, pos.z + offz};
        hit1 = marioHitCheckVec(&A, &B, &C, &D, &dist);
        if (hit1) {
            angle1 = angleABf(0.0f, 0.0f, D.x, D.z);
        }
    }
    if (!hit1) {
        dist = 20.0f;
        A = (Vec){pos.x - offx, pos.y + ten, pos.z - offz};
        hit1 = marioHitCheckVec(&A, &B, &C, &D, &dist);
        if (hit1) {
            angle1 = angleABf(0.0f, 0.0f, D.x, D.z);
        }
    }

    sincosf(&sinv, &cosv, toMovedirSimple(0.0f));
    dist = 20.0f;
    A = pos;
    A.y += ten;
    B = (Vec){sinv, 0.0f, cosv};
    hit2 = marioHitCheckVec(&A, &B, &C, &D, &dist);
    if (hit2) {
        angle2 = angleABf(0.0f, 0.0f, D.x, D.z);
    }
    if (!hit2) {
        dist = 20.0f;
        A = (Vec){pos.x + offx, pos.y + ten, pos.z + offz};
        hit2 = marioHitCheckVec(&A, &B, &C, &D, &dist);
        if (hit2) {
            angle2 = angleABf(0.0f, 0.0f, D.x, D.z);
        }
    }
    if (!hit2) {
        dist = 20.0f;
        A = (Vec){pos.x - offx, pos.y + ten, pos.z - offz};
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

    mario->slitFloorValid = 0;
}

BOOL marioGetSlitFloorPos(Vec* pos) {
    MarioWork* mario = marioGetPtr();

    if (mario->slitFloorValid != 0) {
        *pos = mario->slitFloorPos;
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
    state = mario->slitState;
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
    state = mario->slitState;
    if (state == 0x52 || state == 0x5E) {
        return TRUE;
    }
    return FALSE;
}

void motSlitContinue(void) {
    marioChgMot2(0x15);
    marioGetPtr()->slitState = 0x14;
}


BOOL marioChkSlitEnd(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_SLIT) {
        return FALSE;
    }
    return mario->slitState >= 100;
}

void motSlitCancel(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        mario->slitState = 100;
    }
}

void motSlitCancel2(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        allPartyForceSlitOff();
        mario->slitState = 0x6E;
    }
}

void motSlitCancel3(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        mario->slitState = 120;
    }
}

s32 marioGetMotSlitCancel3(void) {
    MarioWork* mario;
    s32 ret = 0;

    mario = marioGetPtr();
    if (mario->currMotionId == MARIO_MOTION_SLIT) {
        s32 state = mario->slitState;
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

    if (marioSlitButton() != 0) {
        return 1;
    }
    if (marioSlitChkWallAround()) {
        return 1;
    }
    if (marioCreviceWallChk(mario->position) != 0) {
        return 1;
    }
    return 0;
}

void marioSlitForceCancel(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        mario->flags &= ~MARIO_FLAG_PAPER_MODE;
        if (mario->directionView >= 180.0f) {
            mario->dispDir = 0.0f;
            mario->dispReqDir = mario->dispDir;
        } else {
            mario->dispDir = 180.0f;
            mario->dispReqDir = mario->dispDir;
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
        mario->unk148 = 0.0f;
        mario->unk150.z = 0.004f;
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
    mario->wPlayerCollisionBox.x = 20.0f;
    if (mario->directionView >= 180.0f) {
        mario->dispDir = 0.0f;
        mario->dispReqDir = mario->dispDir;
    } else {
        mario->dispDir = 180.0f;
        mario->dispReqDir = mario->dispDir;
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
    mario->wPlayerCollisionBox.x = 20.0f;
    if (mario->directionView >= 180.0f) {
        mario->dispDir = 0.0f;
        mario->dispReqDir = mario->dispDir;
    } else {
        mario->dispDir = 180.0f;
        mario->dispReqDir = mario->dispDir;
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

    mario2->spinSpeed = 0.0f;
    mario2->slitPushCount = 0;
    mario2->slitStillCount = 0;
    mario2->slitTurnTimer = 0;
    mario2->slitTurnSide = 0;
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
    mario->slitState = 0xA;
    mario->dispDir = 270.0f;
    mario->dispReqDir = mario->dispDir;
    marioChgPose("M_S_1");
    marioChgPaper("PM_S_1B");
    mario->slitState = 0xA;
    mario->slitStillCount = 0;
    mario->wPlayerCollisionBox.x = 6.0f;
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
        mario->slitFlags = (s16)(mario->slitFlags | 1);
    }
}

// The walk speed for the slit: the base walk speed, halved while crawling slowly (stick inside a small radius
// in the crawl state) or while a 0x02000000 condition holds.
static inline void slitSetWalkSpeed(MarioWork* mario) {
    MarioWork* m = marioGetPtr();
    s32 flags = m->flags;
    f32 speed = m->mBaseWalkSpeed;

    if (flags & 0x100000) {
        if (m->slitTurnTimer != 0) {
#ifdef MUST_MATCH
            // The original keeps both arms of the empty test (`beq compute; b end`); a bare
            // empty arm is folded away, so keep a no-op here.
            speed = speed;
#endif
        } else {
            // The header declares wStickDir1/2 as u8; the stick values are signed.
            s8 sx = m->wStickDir1;
            s8 sy = m->wStickDir2;
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

    m->unk148 = 0.0f;
    m->unk150.z = 0.004f;
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

        mario2->spinSpeed = 0.0f;
        mario2->slitPushCount = 0;
        mario2->slitStillCount = 0;
        mario2->slitTurnTimer = 0;
        mario2->slitTurnSide = 0;
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
        mario->slitState = 0;
        mario->airTimer = 0;
    }
    if (marioRollChgChk() != 0) {
        if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
            mario->dispDir = 0.0f;
            mario->dispReqDir = mario->dispDir;
        } else {
            mario->dispDir = 180.0f;
            mario->dispReqDir = mario->dispDir;
        }
        mario->flags &= 0xFFBFFFFF;
        mario->flags &= 0xFFDFFFFF;
        mario->dispFlags &= 0xFEFFF3FF;
        marioPaperOff();
        mario->flags &= 0xFFEFFFFF;
        return;
    }
    switch (mario->slitState) {
    case 0:
        mario->slitTimer = 0x1E;
        mario->slitPushCount = 0;
        mario->slitState = 2;
        mario->slitPrevY = mario->position.y;
        slitCapSpeed(mario);
        marioGetPtr();
        psndSFXOn_3D((const char*)0x177, &mario->position);
        break;
    case 2:
        if (mario->controlStickSensitivity == zero) {
            mario->baseSpeed = 0.0f;
            marioChgPose("M_S_1");
        }
        count = mario->slitTimer - 1;
        mario->slitTimer = count;
        if (count <= 0) {
            mario->flags |= 0x100000;
            mario->slitState = 4;
            mario->wPlayerCollisionBox.x = 6.0f;
        }
        if (mario->slitTimer == 0x14) {
            allPartySlitOn();
        }
        break;
    case 4:
        if (marioChkKey() != 0) {
            if (mario->controlStickSensitivity == zero) {
                mario->baseSpeed = 0.0f;
                marioChgPose("M_S_1");
            }
            mario->directionView = toMovedirSimple(mario->dispDir);
        }
        slitCapSpeed(mario);
        mario->dispDir = 270.0f;
        mario->dispReqDir = mario->dispDir;
        if (mario->controlStickSensitivity == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1B");
        mario->slitState = 0xA;
        mario->slitStillCount = 0;
        /* fallthrough */
    case 10:
        if (mario->position.y != mario->slitPrevY) {
            slitResetBounce();
        }
        mario->slitPrevY = mario->position.y;
        if (mario->forceMoveTimer != 0) {
            if (!(mario->trigFlags & 0x8000)) {
                marioChgPose("M_W_1");
            }
            marioChgPaper("PM_S_1D");
            if (revise360(mario->directionView - mario->targetYaw) <= 180.0f) {
                mario->slitTurnSide = 1;
                mario->dispDir = 260.0f;
            } else {
                mario->slitTurnSide = 2;
                mario->dispDir = 280.0f;
            }
            break;
        }
        slitCapSpeed(mario);
        if (marioChkKey() != 0) {
            if (slitContinueChk() != 1) {
                mario->directionView = mario->controlStickAngle;
                mario->slitState = 0x64;
                break;
            }
        }
        if (marioChkJump() != 0) {
            return;
        }
        if (mario->controlStickSensitivity == zero) {
            mario->baseSpeed = 0.0f;
            if (mario->slitPushCount == 0) {
                marioChgPose("M_S_1");
                marioChgPaper("PM_S_1B");
            }
        } else {
            s16 flags;
            f32 turn;

            if (strcmp(mario->animName, "M_I_Y") == 0) {
                marioChgPose("M_W_1");
            }
            flags = mario->slitFlags;
            if (!(flags & 1)) {
                slitResetBounce();
            } else {
                mario->slitFlags = (s16)(flags & 0xFFFFFFFE);
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
                    mario->slitTurnSide = 1;
                    mario->dispDir = 260.0f;
                } else {
                    mario->slitTurnSide = 2;
                    mario->dispDir = 280.0f;
                }
            }
            mario->slitState = 0x14;
        }
        break;
    }
    switch (mario->slitState) {
    case 20:
        if (mario->slitTurnTimer > 0) {
            mario->slitTurnTimer = 0;
            marioChgPaper("PM_S_1D");
            if (revise360(mario->directionView - mario->targetYaw) <= 180.0f) {
                angleGoal = 260.0f;
            } else {
                angleGoal = 280.0f;
            }
            mario->dispDir = angleGoal;
        }
        if (marioChkKey() != 0) {
            if (slitContinueChk() != 1) {
                mario->slitState = 0x64;
            }
        }
        if (!(mario->flags & 0x30000)) {
            marioChkJump();
        }
        if (marioChkKey() != 0) {
            if (mario->controlStickSensitivity == zero) {
                mario->baseSpeed = 0.0f;
                count = mario->slitStillCount + 1;
                mario->slitStillCount = (s16)count;
                if ((s16)count < 6) {
                    if (slitContinueChk() != 1) {
                        mario->slitState = 0x64;
                    }
                } else {
                    mario->slitState = 4;
                }
            } else {
                s8 sx;
                s8 sy;

                mario->slitStillCount = 0;
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
                    mario->slitStillCount = 6;
                    mario->slitTurnSide = 0;
                    if (revise360(mario->directionView - mario->targetYaw) <= 180.0f) {
                        angleGoal = 260.0f;
                    } else {
                        angleGoal = 280.0f;
                    }
                    mario->dispDir = angleGoal;
                } else {
                    f32 diff;

                    marioChgPaper("PM_S_1D");
                    diff = revise360(mario->directionView - mario->targetYaw);
                    if (mario->slitTurnSide == 0) {
                        s16 side;
                        if (diff >= 180.0f) {
                            side = 2;
                        } else {
                            side = 1;
                        }
                        mario->slitTurnSide = side;
                    }
                    if (mario->slitTurnSide == 1) {
                        if (diff >= 180.0f) {
                            marioChgPaper("PM_S_2");
                            mario->slitTurnTimer = 6;
                            mario->slitState = 0x15;
                            mario->slitTurnSide = 2;
                            slitSetWalkSpeed(mario);
                            mario->dispDir = 260.0f;
                        }
                    } else if (mario->slitTurnSide == 2) {
                        if (diff <= 180.0f) {
                            marioChgPaper("PM_S_2");
                            mario->slitTurnTimer = 6;
                            mario->slitState = 0x15;
                            mario->slitTurnSide = 1;
                            slitSetWalkSpeed(mario);
                            mario->dispDir = 280.0f;
                        }
                    }
                }
            }
        }
        break;
    case 21:
        count = mario->slitTurnTimer - 1;
        mario->slitTurnTimer = (s16)count;
        if ((s16)count <= 0) {
            marioChgPaper("PM_S_1D");
            mario->slitState = 0x14;
            if (mario->slitTurnSide == 1) {
                angleGoal = 260.0f;
            } else {
                angleGoal = 280.0f;
            }
            mario->dispDir = angleGoal;
        }
        break;
    }
    if (mario->flags & 0x100000) {
        if (mario->hitobjPush != NULL) {
            if (mario->hitobjStandOn != NULL && mario->currMotionId == 0x15) {
                if (hitGetAttr(mario->hitobjPush) & 0x2000) {
                    f32 turn = revise360(mario->controlStickAngle - mario->targetYaw);

                    if (turn > 90.0f && turn < 270.0f) {
                        mario->wCamVal1 = 180.0f;
                    } else {
                        mario->wCamVal1 = 0.0f;
                    }
                    frames = sysMsec2Frame(0x32);
                    count = mario->slitPushCount + 1;
                    mario->slitPushCount = count;
                    if (count >= frames) {
                        HitCheckQuery query;
                        HitCheckQuery* q = &query;

                        mario->directionView = revise360(mario->wCamVal1 + mario->targetYaw);
                        mario->slitPushCount = 0;
                        sincosf(&slitSinv, &slitCosv, mario->directionView);
                        query.targetPos.x = mario->position.x;
                        query.targetPos.y = 11.0f + mario->position.y;
                        query.targetPos.z = mario->position.z;
                        query.targetDir = (Vec){slitSinv, 0.0f, slitCosv};
                        query.targetDistance = 20.0f;
                        if (hitCheckVecHitObjXZ(q, mario->hitobjPush) != 0) {
                            query.targetPos.x = mario->position.x;
                            query.targetPos.z = mario->position.z;
                            query.targetDistance = 20.0f;
                            movePos(&q->targetPos.x, &q->targetPos.z, 1.0f,
                                    reviseAngle(90.0f + mario->moveDir));
                            if (hitCheckVecHitObjXZ(q, mario->hitobjPush) != 0) {
                                query.targetPos.x = mario->position.x;
                                query.targetPos.z = mario->position.z;
                                query.targetDistance = 20.0f;
                                movePos(&q->targetPos.x, &q->targetPos.z, 1.0f,
                                        reviseAngle(mario->moveDir - 90.0f));
                                if (hitCheckVecHitObjXZ(q, mario->hitobjPush) != 0) {
                                    mario->slitState = 0x50;
                                }
                            }
                        }
                    }
                } else {
                    mario->slitPushCount = 0;
                }
            }
        } else {
            state = mario->slitState;
            if ((state < 0x5A || state > 0x5E) && mario->hitobjStandOn != NULL &&
                (hitGetAttr(mario->hitobjStandOn) & 0x2000)) {
                s32 count = mario->slitPushCount;

                mario->slitPushCount = count + 3;
                marioChgPaper("PM_S_1H");
                if (mario->slitPushCount >= 0xF && mario->currMotionId == 0x15) {
                    mario->slitState = 0x5A;
                }
            } else {
                mario->slitPushCount = mario->slitPushCount - 1;
                if (mario->slitPushCount < 0) {
                    mario->slitPushCount = 0;
                }
            }
        }
    }
    switch (mario->slitState) {
    case 0x50:
        camGetPtr(8)->flags |= 0x200;
        slitResetBounce();
        mario->flags |= 0x10000000;
        if (zero == mario->wCamVal1) {
            marioChgPaper("PM_S_1F");
        } else {
            marioChgPaper("PM_S_1G");
        }
        mario->flags |= 0x400;
        mario->slitTimer = 0x14;
        mario->baseSpeed = (2.0f * mario->wPlayerCollisionBox.x) / (f32)mario->slitTimer;
        mario->slitState = 0x51;
        mario->flags |= 0x200000;
        break;
    case 0x51:
        count = mario->slitTimer - 1;
        mario->slitTimer = count;
        if (count <= 0) {
            mario->slitState = 0x52;
        }
        break;
    case 0x52:
        camGetPtr(8)->flags &= 0xFFFFFDFF;
        mario->flags &= 0xFFFFFBFF;
        mario->slitState = 0x14;
        mario->flags &= 0xEFFFFFFF;
        break;
    }
    switch (mario->slitState) {
    case 0x5A:
        camGetPtr(8)->flags |= 0x200;
        slitResetBounce();
        mario->flags |= 0x10000000;
        marioChgPaper("PM_S_1H");
        mario->slitTimer = sysMsec2Frame(0x1F4);
        mario->baseSpeed = 0.0f;
        mario->slitState = 0x5B;
        {
            MarioWork* m = marioGetPtr();

            m->slitFloorValid = 1;
            m->slitFloorPos = m->position;
        }
        /* fallthrough */
    case 0x5B:
        mario->slitTimer = 0xF;
        mario->slitState = 0x5C;
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
        mario->dispDir = angleGoal;
        count = mario->slitTimer - 1;
        mario->slitTimer = count;
        if (count <= 0) {
            mario->slitState = 0x5D;
            mario->flags &= 0xFFFFFEFF;
            mario->flags &= 0xEFFFFFFF;
    case 0x5D:
            if (mario->hitobjStandOn != NULL) {
                mario->flags &= 0xFFF0FFFF;
                mario->slitState = 0x5E;
    case 0x5E:
                camGetPtr(8)->flags &= 0xFFFFFDFF;
                mario->slitState = 0x14;
            }
        }
        break;
    }
    switch (mario->slitState) {
    case 0x64:
        mario->flags &= 0xFFDFFFFF;
        mario->slitPhaseTimer = 0;
        mario->flags &= 0xFFEFFFFF;
        mario->wPlayerCollisionBox.x = 20.0f;
        if (zero == mario->baseSpeed) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1C");
        mario->slitPhaseTimer = 0x1E;
        mario->slitState = 0x65;
        if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
            mario->dispDir = 0.0f;
            mario->dispReqDir = mario->dispDir;
            if (mario->controlStickSensitivity == zero) {
                mario->directionView = toMovedirSimple(mario->dispDir);
            }
        } else {
            mario->dispDir = 180.0f;
            mario->dispReqDir = mario->dispDir;
            if (mario->controlStickSensitivity == zero) {
                mario->directionView = toMovedirSimple(mario->dispDir);
            }
        }
        psndSFXOn_3D((const char*)0x178, &mario->position);
        /* fallthrough */
    case 0x65:
        if (mario->controlStickSensitivity == zero) {
            mario->baseSpeed = 0.0f;
            marioChgPose("M_S_1");
        }
        count16 = mario->slitPhaseTimer;
        count16--;
        mario->slitPhaseTimer = count16;
        if (count16 <= 0) {
            mario->slitState = 0x66;
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
        mario->slitPhaseTimer = 0;
        mario->flags &= 0xFFEFFFFF;
        mario->wPlayerCollisionBox.x = 20.0f;
        if (mario->controlStickSensitivity == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1C");
        mario->slitPhaseTimer = 0x1E;
        mario->slitState = 0x6F;
        mario->baseSpeed = 0.0f;
        if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
            mario->dispDir = 0.0f;
            mario->dispReqDir = mario->dispDir;
            mario->directionView = toMovedirSimple(mario->dispDir);
        } else {
            mario->dispDir = 180.0f;
            mario->dispReqDir = mario->dispDir;
            mario->directionView = toMovedirSimple(mario->dispDir);
        }
        psndSFXOn_3D((const char*)0x178, &mario->position);
        /* fallthrough */
    case 0x6F:
        count16 = mario->slitPhaseTimer;
        count16--;
        mario->slitPhaseTimer = count16;
        if (count16 <= 0) {
            mario->slitState = 0x70;
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
        mario->slitPhaseTimer = 0;
        mario->flags &= 0xFFEFFFFF;
        mario->wPlayerCollisionBox.x = 20.0f;
        if (mario->controlStickSensitivity == zero) {
            marioChgPose("M_S_1");
        }
        marioChgPaper("PM_S_1C");
        mario->slitPhaseTimer = 0x1E;
        mario->slitState = 0x79;
        mario->baseSpeed = 0.0f;
        if (revise360(mario->directionView - mario->targetYaw) >= 180.0f) {
            mario->dispDir = 0.0f;
            mario->dispReqDir = mario->dispDir;
            mario->directionView = toMovedirSimple(mario->dispDir);
        } else {
            mario->dispDir = 180.0f;
            mario->dispReqDir = mario->dispDir;
            mario->directionView = toMovedirSimple(mario->dispDir);
        }
        psndSFXOn_3D((const char*)0x178, &mario->position);
        /* fallthrough */
    case 0x79:
        count16 = mario->slitPhaseTimer;
        count16--;
        mario->slitPhaseTimer = count16;
        if (count16 <= 0) {
            mario->slitState = 0x7A;
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

#include "motion/mot_jump.h"
#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "koopa.h"
#include "koopa_motion.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_hit.h"
#include "mario/mario_motion.h"
#include "mario/mario_sbr.h"
#include "motion/mot_slit.h"
#include "motion/mot_stay.h"
#include "mot/mot_slit.h"
#include "pmario_sound.h"
#include "system.h"
#include "matching.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern const f32 float_0_8041abb8;
extern const f32 float_0p1_8041abbc;
extern const f32 float_0p8_8041abc0;
extern const f32 float_2p6_8041abc4;
extern const f32 float_neg0p06_8041abc8;
extern const f32 float_neg0p01_8041abcc;
extern const f32 float_neg0p002_8041abd0;
extern const f32 float_1_8041abd4;
extern const f32 float_neg0p04_8041abd8;
extern const f32 float_neg0p02_8041abdc;
extern const f32 float_0p5_8041abe0;
extern const f32 float_5_8041abe4;
extern const f32 float_30_8041abe8;
extern const f32 float_180_8041abec;
extern const f32 float_neg0p03_8041abf0;
extern const f32 float_0p04_8041abf4;
extern const f32 float_neg1_8041abf8;
#pragma pool_data on // the original addresses its .rodata through one base register

// Raw accessor for MarioWork 0x2B4, which mot_jump reads as an f32 (the s32 frame counter there is mot_stay's).
#define MWF32(m, off) (*(f32*)((u8*)(m) + (off)))

// The "M_J_1B" pose name is written with an explicit trailing NUL at its uses so the pooled
// literal has the original object's 8 bytes (six characters and two NULs).

// .sdata
static f32 marioJumpStandData[10] = {
    1.52f, -0.188f, 2.925f, -0.847f, -0.1f, -0.4f, 0.024f, -0.004f, 0.125f, 0.7f
};
f32 marioJumpSwData[8] = {
    2.5f, -0.1f, 0.02f, -0.01f, -0.08f, -0.4f, 0.0f, -0.002f
};


void marioPreJump(void) {
    MarioWork* mario = marioGetPtr();

    mario->flags &= ~0x20000;
    mario->flags |= 0x10000;
    mario->airTimer = 0;
    mario->airTimerPeak = 0;
    mario->hitobjJumpFrom = mario->hitobjStandOn;
    clear_hitobj_ride();
    marioSetJumpPara();
    mario->field_0x11C = mario->position;
}

f32 marioGetJumpHeight(void) {
    MarioWork* mario = marioGetPtr();

    return mario->jumpHeight;
}

void mot_jumpStand(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        MarioWork* mario2;

        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->baseSpeed = float_0_8041abb8;
        mario2 = marioGetPtr();
        mario2->flags &= ~0x20000;
        mario2->flags |= 0x10000;
        mario2->airTimer = 0;
        mario2->airTimerPeak = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        mario2->field_0x11C = mario2->position;
        mario->field_0x2B0 = float_0_8041abb8;
        mario->currSubMotionId = 0;
        mario->multiTimer = 0;
    }

    switch (mario->currSubMotionId) {
        case 0:
            if (++mario->multiTimer >= 3) {
                mario->currSubMotionId = 4;
                mario->jumpVelY = marioJumpStandData[0];
                mario->jumpAccelY = marioJumpStandData[1];
                mario->jumpJerkY = marioJumpStandData[2];
                mario->jumpJerkStepY = marioJumpStandData[3];
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose("M_J_1B\0");
                }
            }
            break;
        case 4: {
            f32 old = mario->jumpVelY;

            marioMakeJumpPara();
            if (old >= mario->jumpVelY) {
                mario->currSubMotionId = 6;
            }
            mario->position.y += mario->jumpVelY;
            mario->field_0x2B0 += mario->jumpVelY;
            break;
        }
        case 6:
            mario->position.y += mario->jumpVelY;
            mario->field_0x2B0 += mario->jumpVelY;
            if (mario->field_0x2B0 >= mario->jumpHeight * marioJumpStandData[9]) {
                mario->currSubMotionId = 8;
            }
            if (mario->jumpVelY <= float_0_8041abb8) {
                mario->currSubMotionId = 8;
            }
            break;
        case 8:
#ifdef MUST_MATCH
            // MUST_MATCH: the separate locals pin MWCC's FPR allocation; the natural form below
            // puts the fsubs result in f1 and the data[8] load in f2, which the original swaps.
            {
                f32 neg = MATCH_F32(float_neg1_8041abf8);
                f32 v = mario->jumpVelY;
                f32 diff = neg - v;

                mario->jumpVelY = diff * marioJumpStandData[8] + v;
            }
#else
            mario->jumpVelY = (float_neg1_8041abf8 - mario->jumpVelY)
                                     * marioJumpStandData[8]
                                 + mario->jumpVelY;
#endif
            mario->position.y += mario->jumpVelY;
            mario->field_0x2B0 += mario->jumpVelY;
            if (mario->jumpVelY <= float_0_8041abb8) {
                mario->currSubMotionId = 10;
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose("M_J_1C");
                }
                marioSetPeakYpos();
                mario->jumpVelY = marioJumpStandData[4];
                mario->jumpAccelY = marioJumpStandData[5];
                mario->jumpJerkY = marioJumpStandData[6];
                mario->jumpJerkStepY = marioJumpStandData[7];
                mario->flags &= ~0x10000;
                mario->flags |= 0x20000;
                mario->currSubMotionId = 0x14;
            }
            break;
        case 10:
            marioMakeJumpPara();
            if (mario->jumpVelY <= float_0_8041abb8) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose("M_J_1C");
                }
                marioSetPeakYpos();
                mario->jumpVelY = marioJumpStandData[4];
                mario->jumpAccelY = marioJumpStandData[5];
                mario->jumpJerkY = marioJumpStandData[6];
                mario->jumpJerkStepY = marioJumpStandData[7];
                mario->flags &= ~0x10000;
                mario->flags |= 0x20000;
                mario->currSubMotionId = 0x14;
            }
            mario->position.y += mario->jumpVelY;
            break;
        case 20:
            if (--mario->multiTimer <= 0) {
                mario->baseSpeed = float_0_8041abb8;
                mario->currSubMotionId = 0x1E;
            }
            break;
    }

    mario->airTimer++;
    if (marioSlitButton() != 2) {
        marioSlitForceCancel();
    }
}

// The pose strings and the int->float conversion double are compiler-pooled literals, and
// `#pragma pool_data on` reaches them all (including the double at 0x38) through one base
// register: the original's `lfd f2, 0x38(r30)`.  The zero-Vec templates below are copied into a
// local before their fields are overwritten, as the original does.
void mot_jumpSw(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        MarioWork* mario2;
        f32 sinv;
        f32 cosv;
        f32 x;
        f32 z;
        f32 dist;

        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        marioResetCamFollowRate();
        mario->flags |= 0x80;
        camFollowYOff();
        mario2 = marioGetPtr();
        mario2->flags &= ~0x20000;
        mario2->flags |= 0x10000;
        mario2->airTimer = 0;
        mario2->airTimerPeak = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        mario2->field_0x11C = mario2->position;
        if (!(mario->flags & 0x2000) && mario->characterId == 0) {
            marioChgPose("M_J_1B\0");
        }
        if (mario->hitobjJumpFrom != NULL) {
            if (hitGetAttr(mario->hitobjJumpFrom) & 0x80000000) {
                MapObjectEntry* obj = mobjHitObjPtrToPtr(mario->hitobjJumpFrom);

                MWF32(mario, 0x2B4) = obj->position.x;
                mario->field_0x2B8 = obj->position.z;
                if (marioChkFront(&dist,
                                  (Vec){ obj->position.x, obj->position.y + float_0p5_8041abe0 * mario->wPlayerCollisionBox.y,
                                         obj->position.z },
                                  mario->directionView,
                                  float_5_8041abe4 * mario->wPlayerCollisionBox.x)) {
                    if (dist >= float_0_8041abb8
                        && dist <= float_0p5_8041abe0 * mario->wPlayerCollisionBox.x + float_30_8041abe8) {
                        f32 d = revise360(float_180_8041abec + mario->directionView);

                        mario->directionView = d;
                        mario->moveDir = d;
                    }
                }
                sincosf(&sinv, &cosv, mario->directionView);
                {
                    f32 c30 = float_30_8041abe8;

                    x = c30 * sinv + obj->position.x;
                    z = c30 * cosv + obj->position.z;
                }
                mario->multiTimer = 0x14;
                mario->baseSpeed =
                    distABf(mario->position.x, mario->position.z, x, z)
                    / (f32)mario->multiTimer;
            }
        } else {
            f32 dir;

            dir = revise360(float_180_8041abec + mario->directionView);
            sincosf(&sinv, &cosv, dir);
            {
                f32 c30 = float_30_8041abe8;

                x = c30 * sinv + mario->position.x;
                z = c30 * cosv + mario->position.z;
            }
            mario->directionView = dir;
            mario->moveDir = dir;
            mario->multiTimer = 0x14;
            mario->baseSpeed = distABf(mario->position.x, mario->position.z, x, z)
                               / (f32)mario->multiTimer;
        }
        mario->currSubMotionId = 0;
    }

    switch (mario->currSubMotionId) {
        case 0:
            mario->multiTimer--;
            marioMakeJumpPara();
            if (mario->jumpVelY <= float_0_8041abb8) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose("M_J_1C");
                }
                marioSetPeakYpos();
                mario->jumpVelY = marioJumpSwData[4];
                mario->jumpAccelY = marioJumpSwData[5];
                mario->jumpJerkY = marioJumpSwData[6];
                mario->jumpJerkStepY = marioJumpSwData[7];
                mario->flags &= ~0x10000;
                mario->flags |= 0x20000;
                mario->currSubMotionId = 0xA;
            }
            mario->position.y += mario->jumpVelY;
            break;
        case 10:
            if (--mario->multiTimer <= 0) {
                mario->baseSpeed = float_0_8041abb8;
                mario->currSubMotionId = 0x14;
            }
            break;
    }

    mario->airTimer++;
}

// Same pool shape as mot_jumpSw: `#pragma pool_data on` puts the i2f double in the .rodata pool
// and reaches it through the single base register (`lfd f2, 0x38(r30)`).
void mot_jumpNpc(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->characterId == 2) {
        kpa_jumpNpc();
        return;
    }

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        MarioWork* mario2;

        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        marioResetCamFollowRate();
        if (mario->characterId != 2) {
            mario->flags |= 0x80;
        }
        camFollowYOff();
        mario2 = marioGetPtr();
        mario2->flags &= ~0x20000;
        mario2->flags |= 0x10000;
        mario2->airTimer = 0;
        mario2->airTimerPeak = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        mario2->field_0x11C = mario2->position;
        if (mario->characterId == 2) {
            mario->jumpVelY = MATCH_F32(float_2p6_8041abc4);
            mario->jumpAccelY = MATCH_F32(float_neg0p06_8041abc8);
            mario->jumpJerkY = MATCH_F32(float_neg0p01_8041abcc);
            mario->jumpJerkStepY = MATCH_F32(float_neg0p002_8041abd0);
        } else {
            mario->jumpVelY = MATCH_F32(float_1_8041abd4);
            mario->jumpAccelY = MATCH_F32(float_neg0p04_8041abd8);
            mario->jumpJerkY = MATCH_F32(float_neg0p01_8041abcc);
            mario->jumpJerkStepY = MATCH_F32(float_neg0p02_8041abdc);
        }
        if (!(mario->flags & 0x2000) && mario->characterId == 0) {
            marioChgPose("M_J_1B\0");
        }
        if (mario->hitobjJumpFrom != NULL) {
            if (hitGetAttr(mario->hitobjJumpFrom) & 0x80000000) {
                MapObjectEntry* obj = mobjHitObjPtrToPtr(mario->hitobjJumpFrom);
                f32 sinv;
                f32 cosv;
                f32 x;
                f32 z;
                f32 dist;

                MWF32(mario, 0x2B4) = obj->position.x;
                mario->field_0x2B8 = obj->position.z;
                if (marioChkFront(&dist,
                                  (Vec){ obj->position.x, obj->position.y + MATCH_F32(float_0p5_8041abe0) * mario->wPlayerCollisionBox.y,
                                         obj->position.z },
                                  mario->directionView,
                                  MATCH_F32(float_5_8041abe4) * mario->wPlayerCollisionBox.x)) {
                    if (dist >= MATCH_F32(float_0_8041abb8)
                        && dist <= MATCH_F32(float_0p5_8041abe0) * mario->wPlayerCollisionBox.x
                                       + MATCH_F32(float_30_8041abe8)) {
                        f32 d = revise360(MATCH_F32(float_180_8041abec) + mario->directionView);

                        mario->directionView = d;
                        mario->moveDir = d;
                    }
                }
                sincosf(&sinv, &cosv, mario->directionView);
                {
                    f32 c30 = MATCH_F32(float_30_8041abe8);
                    x = c30 * sinv + obj->position.x;
                    z = c30 * cosv + obj->position.z;
                }
                mario->multiTimer = 0x14;
                mario->baseSpeed = distABf(mario->position.x, mario->position.z, x, z)
                                   / (f32)mario->multiTimer;
            }
        } else {
            f32 dir;
            f32 sinv;
            f32 cosv;
            f32 x;
            f32 z;

            revise360(MATCH_F32(float_180_8041abec) + mario->directionView);
            dir = mario->directionView;
            sincosf(&sinv, &cosv, dir);
            {
                f32 c30 = MATCH_F32(float_30_8041abe8);
                x = c30 * sinv + mario->position.x;
                z = c30 * cosv + mario->position.z;
            }
            mario->directionView = dir;
            mario->moveDir = dir;
            mario->multiTimer = 0x14;
            mario->baseSpeed = distABf(mario->position.x, mario->position.z, x, z)
                               / (f32)mario->multiTimer;
        }
        mario->currSubMotionId = 0;
    }

    switch (mario->currSubMotionId) {
        case 0:
            mario->multiTimer--;
            marioMakeJumpPara();
            if (mario->jumpVelY <= MATCH_F32(float_0_8041abb8)) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose("M_J_1C");
                }
                marioSetPeakYpos();
                mario->jumpVelY = MATCH_F32(float_neg0p03_8041abf0);
                mario->jumpAccelY = MATCH_F32(float_neg0p02_8041abdc);
                mario->jumpJerkY = MATCH_F32(float_0p04_8041abf4);
                mario->jumpJerkStepY = MATCH_F32(float_0_8041abb8);
                mario->flags &= ~0x10000;
                mario->flags |= 0x20000;
                mario->currSubMotionId = 0x14;
            }
            mario->position.y += mario->jumpVelY;
            break;
        case 0x14:
            if (--mario->multiTimer <= 0) {
                mario->baseSpeed = MATCH_F32(float_0_8041abb8);
                mario->currSubMotionId = 0x1E;
            }
            break;
    }

    mario->airTimer++;
}

void mot_jump(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->characterId == 2) {
        kpa_jump();
        return;
    }

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        MarioWork* mario2;

        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        marioResetCamFollowRate();
        mario->flags |= 0x80000;
        mario->multiTimer = 2;
        mario->currSubMotionId = 0;
        mario2 = marioGetPtr();
        mario2->flags &= ~0x20000;
        mario2->flags |= 0x10000;
        mario2->airTimer = 0;
        mario2->airTimerPeak = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        mario2->field_0x11C = mario2->position;
        if (mario->currMotionId == 8) {
            camFollowYOn();
        } else {
            camFollowYOff();
        }
        if (!(mario->flags & 0x2000) && mario->characterId == 0) {
            marioChgPose("M_J_1B\0");
        }
        if (mario->currMotionId != 8) {
            s32 r = irand(0x2710);
            s32 id;

            if (r < 0x7D0) {
                id = 0x9D;
            } else if (r < 0xFA0) {
                id = 0x9E;
            } else if (r < 0x1770) {
                id = 0x9F;
            } else if (r < 0x1F40) {
                id = 0xA0;
            } else {
                id = 0xA1;
            }
            psndSFXOn_3D((const char*)id, &mario->position);
        }
        psndSFXOn_3D((const char*)0x14D, &mario->position);
        if (mario->airTimer == 0) {
            mario->lastGroundPos = mario->position;
        }
        mario->trigFlags &= ~0x10000;
    }

    switch (mario->currSubMotionId) {
        case 0:
            if (--mario->multiTimer <= 0) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose("M_J_1B\0");
                }
                mario->currSubMotionId = 0xA;
            }
            break;
    }

    mario->airTimer++;
}

void mot_jumpSmall(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->characterId == 2) {
        kpa_jump();
        return;
    }

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        MarioWork* mario2;

        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        marioResetCamFollowRate();
        mario->flags |= 0x80000;
        mario->multiTimer = 2;
        mario->currSubMotionId = 0;
        mario2 = marioGetPtr();
        mario2->flags &= ~0x20000;
        mario2->flags |= 0x10000;
        mario2->airTimer = 0;
        mario2->airTimerPeak = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        mario2->field_0x11C = mario2->position;
        if (mario->currMotionId != 8) {
            camFollowYOff();
        }
        if (!(mario->flags & 0x2000) && mario->characterId == 0) {
            marioChgPose("M_J_1B\0");
        }
        if (mario->currMotionId != 8) {
            s32 r = irand(0x2710);
            s32 id;

            if (r < 0x7D0) {
                id = 0x9D;
            } else if (r < 0xFA0) {
                id = 0x9E;
            } else if (r < 0x1770) {
                id = 0x9F;
            } else if (r < 0x1F40) {
                id = 0xA0;
            } else {
                id = 0xA1;
            }
            psndSFXOn_3D((const char*)id, &mario->position);
        }
        psndSFXOn_3D((const char*)0x14D, &mario->position);
        if (mario->airTimer == 0) {
            mario->lastGroundPos = mario->position;
        }
        mario->trigFlags &= ~0x10000;
    }

    switch (mario->currSubMotionId) {
        case 0:
            if (--mario->multiTimer <= 0) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose("M_J_1B\0");
                }
                mario->currSubMotionId = 0xA;
            }
            break;
    }

    mario->airTimer++;
}

void mot_upstairs(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags |= 0x40000;
        mario->flags |= 0x80;
        marioClearJumpPara();
        mario->multiTimer = 3;
        mario->jumpVelY = (mario->field_0x1C4 - mario->position.y)
                             / (f32)mario->multiTimer;
        mario->currSubMotionId = 0;
    }

    switch (mario->currSubMotionId) {
        case 0:
            mario->position.y += mario->jumpVelY;
            if (--mario->multiTimer <= 0) {
                mario->flags &= ~0x80;
                mario->flags &= ~0x40;
                mario->flags &= ~0x40000;
                if (mario->flags & 0x100000) {
                    motSlitContinue();
                } else if (mario->dispFlags & 0x1000000) {
                    marioChgMot2(0x15);
                } else {
                    marioChgMotAuto();
                }
            }
            break;
    }
}

void mot_fall(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->characterId == 2) {
        kpa_fall_80213C38();
        return;
    }

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags &= ~0x10000;
        mario->flags |= 0x20000;
        if (mario->sfxIds[2] != 0xFFFFFFFFu) {
            psndSFXOff(mario->sfxIds[2]);
            mario->sfxIds[2] = 0xFFFFFFFFu;
        }
        marioResetCamFollowRate();
        mario->currSubMotionId = 0;
        if (mario->currMotionId != 0xB) {
            if (mario->characterId == 2) {
                kpaChgPose("KPA_J_1B", "KPA2_J_1B");
            } else if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                marioChgPose("M_J_1C");
            }
        } else {
            mario->flags |= 0x80;
        }
        if (mario->airTimer == 0) {
            mario->lastGroundPos = mario->position;
        }
    }

    switch (mario->currSubMotionId) {
        case 0:
            if (mario->position.y <= mario->field_0x11C.y) {
                if (!marioBgmodeChk()) {
                    camFollowYOn();
                }
                mario->currSubMotionId = 0xA;
            }
            break;
    }

    mario->airTimer++;
    if (mario->screenY >= 0x1AE) {
        f32 rate;

        rate = marioGetCamFollowRate();
#ifdef MUST_MATCH
        rate += MATCH_F32(float_0p1_8041abbc);
        if (rate >= float_0p8_8041abc0) {
            rate = *(const f32*)((const u8*)&float_0p8_8041abc0);
        }
#else
        rate += float_0p1_8041abbc;
        if (rate >= float_0p8_8041abc0) {
            rate = float_0p8_8041abc0;
        }
#endif
        marioSetCamFollowRate(rate);
    }
}

void mot_fall_post(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->sfxIds[2] != 0xFFFFFFFFu) {
        psndSFXOff(mario->sfxIds[2]);
        mario->sfxIds[2] = 0xFFFFFFFFu;
    }
}

void mot_land(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags &= ~0xF0000;
        mario->airTimer = 0;
        mario->airTimerPeak = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = MATCH_F32(float_0_8041abb8);
        marioResetCamFollowRate();
        if (mario->sfxIds[2] != 0xFFFFFFFFu) {
            psndSFXOff(mario->sfxIds[2]);
            mario->sfxIds[2] = 0xFFFFFFFFu;
        }
    }

    switch (mario->currSubMotionId) {
        case 0:
            marioChgMot(0);
            break;
        case 1:
            if (--mario->multiTimer <= 0) {
                marioChgMot(0);
            }
            break;
        case 10:
            if (--mario->multiTimer <= 0) {
                marioChgMot(0);
            }
            break;
    }
}

// .sdata2
const f32 float_0_8041abb8 = 0.0f;
const f32 float_0p1_8041abbc = 0.1f;
const f32 float_0p8_8041abc0 = 0.8f;
const f32 float_2p6_8041abc4 = 2.6f;
const f32 float_neg0p06_8041abc8 = -0.06f;
const f32 float_neg0p01_8041abcc = -0.01f;
const f32 float_neg0p002_8041abd0 = -0.002f;
const f32 float_1_8041abd4 = 1.0f;
const f32 float_neg0p04_8041abd8 = -0.04f;
const f32 float_neg0p02_8041abdc = -0.02f;
const f32 float_0p5_8041abe0 = 0.5f;
const f32 float_5_8041abe4 = 5.0f;
const f32 float_30_8041abe8 = 30.0f;
const f32 float_180_8041abec = 180.0f;
const f32 float_neg0p03_8041abf0 = -0.03f;
const f32 float_0p04_8041abf4 = 0.04f;
const f32 float_neg1_8041abf8 = -1.0f;

// Trailing zero word at the end of the .sdata2 section.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") u32 gap_09_8041ABFC_sdata2 = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

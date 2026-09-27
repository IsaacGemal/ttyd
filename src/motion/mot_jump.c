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

// Raw accessors for MarioWork fields not named in include/mario/mario.h.
// Offsets are listed in REPORT.md.
#define MW(m, off) (*(u32*)((u8*)(m) + (off)))
#define MWS32(m, off) (*(s32*)((u8*)(m) + (off)))
#define MWS16(m, off) (*(s16*)((u8*)(m) + (off)))
#define MWF32(m, off) (*(f32*)((u8*)(m) + (off)))

// .rodata
const Vec vec3_802c5c40 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5c4c = { 0.0f, 0.0f, 0.0f };
const char str_802C5C58[] = "KPA_J_1B";
const char str_802C5C64[] = "KPA2_J_1B";
const char str_802C5C70[] = "M_J_1C";
const f64 double_to_int_802c5c78 = 4503601774854144.0;
const char str_802C5C80[] = "M_J_1B";

// The original addresses these .rodata objects as offsets from one base register (vec3_802c5c40@l);
// functions that use JUMP_RODATA declare that base with JUMP_RODATA_BASE.
#ifdef MUST_MATCH
#define JUMP_RODATA(T, off, name) ((T)(base + (off)))
#define JUMP_RODATA_BASE const u8* base = (const u8*)&vec3_802c5c40;
#else
#define JUMP_RODATA(T, off, name) ((T)&(name))
#define JUMP_RODATA_BASE
#endif

// .sdata
static f32 marioJumpStandData[10] = {
    1.52f, -0.188f, 2.925f, -0.847f, -0.1f, -0.4f, 0.024f, -0.004f, 0.125f, 0.7f
};
f32 marioJumpSwData[8] = {
    2.5f, -0.1f, 0.02f, -0.01f, -0.08f, -0.4f, 0.0f, -0.002f
};

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

void mot_land(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags &= ~0xF0000;
        mario->airTimer = 0;
        MWS16(mario, 0x52) = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = MATCH_F32(float_0_8041abb8);
        marioResetCamFollowRate();
        if (MW(mario, 0x280) != 0xFFFFFFFFu) {
            psndSFXOff(MW(mario, 0x280));
            MW(mario, 0x280) = 0xFFFFFFFFu;
        }
    }

    switch (mario->currSubMotionId) {
        case 0:
            marioChgMot(0);
            break;
        case 1:
            if ((s32)--mario->multiTimer <= 0) {
                marioChgMot(0);
            }
            break;
        case 10:
            if ((s32)--mario->multiTimer <= 0) {
                marioChgMot(0);
            }
            break;
    }
}

void mot_fall_post(void) {
    MarioWork* mario = marioGetPtr();

    if (MW(mario, 0x280) != 0xFFFFFFFFu) {
        psndSFXOff(MW(mario, 0x280));
        MW(mario, 0x280) = 0xFFFFFFFFu;
    }
}

void mot_fall(void) {
    JUMP_RODATA_BASE
    MarioWork* mario = marioGetPtr();

    if (mario->characterId == 2) {
        kpa_fall_80213C38();
        return;
    }

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags &= ~0x10000;
        mario->flags |= 0x20000;
        if (MW(mario, 0x280) != 0xFFFFFFFFu) {
            psndSFXOff(MW(mario, 0x280));
            MW(mario, 0x280) = 0xFFFFFFFFu;
        }
        marioResetCamFollowRate();
        mario->currSubMotionId = 0;
        if (mario->currMotionId != 0xB) {
            if (mario->characterId == 2) {
                kpaChgPose(JUMP_RODATA(char*, 0x18, str_802C5C58), JUMP_RODATA(char*, 0x24, str_802C5C64));
            } else if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                marioChgPose(JUMP_RODATA(char*, 0x30, str_802C5C70));
            }
        } else {
            mario->flags |= 0x80;
        }
        if (mario->airTimer == 0) {
            *(Vec*)((u8*)mario + 0xD4) = mario->position;
        }
    }

    switch ((s32)mario->currSubMotionId) {
        case 0:
            if (mario->position.y <= MWF32(mario, 0x120)) {
                if (!marioBgmodeChk()) {
                    camFollowYOn();
                }
                mario->currSubMotionId = 0xA;
            }
            break;
    }

    mario->airTimer++;
    if (mario->unk16C >= 0x1AE) {
        f32 rate;

        rate = marioGetCamFollowRate();
        rate += float_0p1_8041abbc;
        if (rate >= float_0p8_8041abc0) {
            rate = float_0p8_8041abc0;
        }
        marioSetCamFollowRate(rate);
    }
}

void mot_upstairs(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags |= 0x40000;
        mario->flags |= 0x80;
        marioClearJumpPara();
        mario->multiTimer = 3;
        MWF32(mario, 0x7C) = (MWF32(mario, 0x1C4) - mario->position.y)
                             / (f32)(s32)mario->multiTimer;
        mario->currSubMotionId = 0;
    }

    switch ((s32)mario->currSubMotionId) {
        case 0:
            mario->position.y += MWF32(mario, 0x7C);
            if ((s32)--mario->multiTimer <= 0) {
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
        MWS16(mario2, 0x52) = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        *(Vec*)((u8*)mario2 + 0x11C) = mario2->position;
        if (mario->currMotionId != 8) {
            camFollowYOff();
        }
        if (!(mario->flags & 0x2000) && mario->characterId == 0) {
            marioChgPose(str_802C5C80);
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
            *(Vec*)((u8*)mario + 0xD4) = mario->position;
        }
        mario->trigFlags &= ~0x10000;
    }

    switch ((s32)mario->currSubMotionId) {
        case 0:
            if ((s32)--mario->multiTimer <= 0) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose(str_802C5C80);
                }
                mario->currSubMotionId = 0xA;
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
        MWS16(mario2, 0x52) = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        *(Vec*)((u8*)mario2 + 0x11C) = mario2->position;
        if (mario->currMotionId == 8) {
            camFollowYOn();
        } else {
            camFollowYOff();
        }
        if (!(mario->flags & 0x2000) && mario->characterId == 0) {
            marioChgPose(str_802C5C80);
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
            *(Vec*)((u8*)mario + 0xD4) = mario->position;
        }
        mario->trigFlags &= ~0x10000;
    }

    switch ((s32)mario->currSubMotionId) {
        case 0:
            if ((s32)--mario->multiTimer <= 0) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose(str_802C5C80);
                }
                mario->currSubMotionId = 0xA;
            }
            break;
    }

    mario->airTimer++;
}

// NONMATCHING (90.61%): 11 instructions differ, 8 missing, 11 extra, plus register differences.
void mot_jumpNpc(void) {
    JUMP_RODATA_BASE
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
        MWS16(mario2, 0x52) = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        *(Vec*)((u8*)mario2 + 0x11C) = mario2->position;
        if (mario->characterId == 2) {
            MWF32(mario, 0x7C) = MATCH_F32(float_2p6_8041abc4);
            MWF32(mario, 0x80) = MATCH_F32(float_neg0p06_8041abc8);
            MWF32(mario, 0x84) = MATCH_F32(float_neg0p01_8041abcc);
            MWF32(mario, 0x88) = MATCH_F32(float_neg0p002_8041abd0);
        } else {
            MWF32(mario, 0x7C) = MATCH_F32(float_1_8041abd4);
            MWF32(mario, 0x80) = MATCH_F32(float_neg0p04_8041abd8);
            MWF32(mario, 0x84) = MATCH_F32(float_neg0p01_8041abcc);
            MWF32(mario, 0x88) = MATCH_F32(float_neg0p02_8041abdc);
        }
        if (!(mario->flags & 0x2000) && mario->characterId == 0) {
            marioChgPose(JUMP_RODATA(const char*, 0x40, str_802C5C80));
        }
        if (mario->hitobjJumpFrom != NULL) {
            if (hitGetAttr(mario->hitobjJumpFrom) & 0x80000000) {
                MapObjectEntry* obj = mobjHitObjPtrToPtr(mario->hitobjJumpFrom);
                f32 dist;
                Vec pos;
                Vec tmp = *JUMP_RODATA(const Vec*, 0xC, vec3_802c5c4c);

                MWF32(mario, 0x2B4) = obj->position.x;
                MWF32(mario, 0x2B8) = obj->position.z;
                tmp.x = obj->position.x;
                tmp.y = obj->position.y + 0.5f * mario->wPlayerCollisionBox.y;
                tmp.z = obj->position.z;
                pos = tmp;
                if (marioChkFront(&dist, &pos, mario->directionView,
                                  5.0f * mario->wPlayerCollisionBox.x)) {
                    if (dist >= 0.0f
                        && dist <= 0.5f * mario->wPlayerCollisionBox.x + 30.0f) {
                        mario->directionView = revise360(180.0f + mario->directionView);
                        *(f32*)((u8*)mario + 0x19C) = mario->directionView;
                    }
                }
                {
                    f32 sinv;
                    f32 cosv;
                    f32 x;
                    f32 z;

                    sincosf(&sinv, &cosv, mario->directionView);
                    x = 30.0f * sinv + obj->position.x;
                    z = 30.0f * cosv + obj->position.z;
                    mario->multiTimer = 0x14;
                    mario->baseSpeed =
                        distABf(mario->position.x, mario->position.z, x, z)
                        / (f32)(s32)mario->multiTimer;
                }
            }
        } else {
            f32 dir;
            f32 sinv;
            f32 cosv;
            f32 x;
            f32 z;

            revise360(180.0f + mario->directionView);
            dir = mario->directionView;
            sincosf(&sinv, &cosv, dir);
            x = 30.0f * sinv + mario->position.x;
            z = 30.0f * cosv + mario->position.z;
            mario->multiTimer = 0x14;
            mario->baseSpeed = distABf(mario->position.x, mario->position.z, x, z)
                               / (f32)(s32)mario->multiTimer;
            mario->directionView = dir;
            *(f32*)((u8*)mario + 0x19C) = dir;
        }
        mario->currSubMotionId = 0;
    }

    switch ((s32)mario->currSubMotionId) {
        case 0:
            mario->multiTimer--;
            marioMakeJumpPara();
            if (MWF32(mario, 0x7C) <= 0.0f) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose(JUMP_RODATA(const char*, 0x30, str_802C5C70));
                }
                marioSetPeakYpos();
                MWF32(mario, 0x7C) = MATCH_F32(float_neg0p03_8041abf0);
                MWF32(mario, 0x80) = MATCH_F32(float_neg0p02_8041abdc);
                MWF32(mario, 0x84) = MATCH_F32(float_0p04_8041abf4);
                MWF32(mario, 0x88) = MATCH_F32(float_0_8041abb8);
                mario->flags &= ~0x10000;
                mario->flags |= 0x20000;
                mario->currSubMotionId = 0x14;
            }
            mario->position.y += MWF32(mario, 0x7C);
            break;
        case 0x14:
            if ((s32)--mario->multiTimer <= 0) {
                mario->baseSpeed = MATCH_F32(float_0_8041abb8);
                mario->currSubMotionId = 0x1E;
            }
            break;
    }

    mario->airTimer++;
}

// NONMATCHING (89.45%): 11 instructions differ, 8 missing, 11 extra, plus register differences.
void mot_jumpSw(void) {
    JUMP_RODATA_BASE
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        MarioWork* mario2;

        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        marioResetCamFollowRate();
        mario->flags |= 0x80;
        camFollowYOff();
        mario2 = marioGetPtr();
        mario2->flags &= ~0x20000;
        mario2->flags |= 0x10000;
        mario2->airTimer = 0;
        MWS16(mario2, 0x52) = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        *(Vec*)((u8*)mario2 + 0x11C) = mario2->position;
        if (!(mario->flags & 0x2000) && mario->characterId == 0) {
            marioChgPose(JUMP_RODATA(const char*, 0x40, str_802C5C80));
        }
        if (mario->hitobjJumpFrom != NULL) {
            if (hitGetAttr(mario->hitobjJumpFrom) & 0x80000000) {
                MapObjectEntry* obj = mobjHitObjPtrToPtr(mario->hitobjJumpFrom);
                f32 dist;
                Vec pos;
                Vec tmp = *JUMP_RODATA(const Vec*, 0x0, vec3_802c5c40);

                MWF32(mario, 0x2B4) = obj->position.x;
                MWF32(mario, 0x2B8) = obj->position.z;
                tmp.x = obj->position.x;
                tmp.y = obj->position.y + 0.5f * mario->wPlayerCollisionBox.y;
                tmp.z = obj->position.z;
                pos = tmp;
                if (marioChkFront(&dist, &pos, mario->directionView,
                                  5.0f * mario->wPlayerCollisionBox.x)) {
                    if (dist >= 0.0f
                        && dist <= 0.5f * mario->wPlayerCollisionBox.x + 30.0f) {
                        mario->directionView = revise360(180.0f + mario->directionView);
                        *(f32*)((u8*)mario + 0x19C) = mario->directionView;
                    }
                }
                {
                    f32 sinv;
                    f32 cosv;
                    f32 x;
                    f32 z;

                    sincosf(&sinv, &cosv, mario->directionView);
                    x = 30.0f * sinv + obj->position.x;
                    z = 30.0f * cosv + obj->position.z;
                    mario->multiTimer = 0x14;
                    mario->baseSpeed =
                        distABf(mario->position.x, mario->position.z, x, z)
                        / (f32)(s32)mario->multiTimer;
                }
            }
        } else {
            f32 dir;
            f32 sinv;
            f32 cosv;
            f32 x;
            f32 z;

            dir = revise360(180.0f + mario->directionView);
            sincosf(&sinv, &cosv, dir);
            x = 30.0f * sinv + mario->position.x;
            z = 30.0f * cosv + mario->position.z;
            mario->multiTimer = 0x14;
            mario->baseSpeed = distABf(mario->position.x, mario->position.z, x, z)
                               / (f32)(s32)mario->multiTimer;
            mario->directionView = dir;
            *(f32*)((u8*)mario + 0x19C) = dir;
        }
        mario->currSubMotionId = 0;
    }

    switch ((s32)mario->currSubMotionId) {
        case 0:
            mario->multiTimer--;
            marioMakeJumpPara();
            if (MWF32(mario, 0x7C) <= 0.0f) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose(JUMP_RODATA(const char*, 0x30, str_802C5C70));
                }
                marioSetPeakYpos();
                MWF32(mario, 0x7C) = marioJumpSwData[4];
                MWF32(mario, 0x80) = marioJumpSwData[5];
                MWF32(mario, 0x84) = marioJumpSwData[6];
                MWF32(mario, 0x88) = marioJumpSwData[7];
                mario->flags &= ~0x10000;
                mario->flags |= 0x20000;
                mario->currSubMotionId = 0xA;
            }
            mario->position.y += MWF32(mario, 0x7C);
            break;
        case 10:
            if ((s32)--mario->multiTimer <= 0) {
                mario->baseSpeed = 0.0f;
                mario->currSubMotionId = 0x14;
            }
            break;
    }

    mario->airTimer++;
}

// NONMATCHING (99.44%): 2 instructions differ, plus register differences.
void mot_jumpStand(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        MarioWork* mario2;

        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->baseSpeed = 0.0f;
        mario2 = marioGetPtr();
        mario2->flags &= ~0x20000;
        mario2->flags |= 0x10000;
        mario2->airTimer = 0;
        MWS16(mario2, 0x52) = 0;
        mario2->hitobjJumpFrom = mario2->hitobjStandOn;
        clear_hitobj_ride();
        marioSetJumpPara();
        *(Vec*)((u8*)mario2 + 0x11C) = mario2->position;
        MWF32(mario, 0x2B0) = 0.0f;
        mario->currSubMotionId = 0;
        mario->multiTimer = 0;
    }

    switch ((s32)mario->currSubMotionId) {
        case 0:
            if (++mario->multiTimer >= 3) {
                mario->currSubMotionId = 4;
                MWF32(mario, 0x7C) = marioJumpStandData[0];
                MWF32(mario, 0x80) = marioJumpStandData[1];
                MWF32(mario, 0x84) = marioJumpStandData[2];
                MWF32(mario, 0x88) = marioJumpStandData[3];
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose(str_802C5C80);
                }
            }
            break;
        case 4: {
            f32 old = MWF32(mario, 0x7C);

            marioMakeJumpPara();
            if (old >= MWF32(mario, 0x7C)) {
                mario->currSubMotionId = 6;
            }
            mario->position.y += MWF32(mario, 0x7C);
            MWF32(mario, 0x2B0) += MWF32(mario, 0x7C);
            break;
        }
        case 6:
            mario->position.y += MWF32(mario, 0x7C);
            MWF32(mario, 0x2B0) += MWF32(mario, 0x7C);
            if (MWF32(mario, 0x2B0) >= MWF32(mario, 0x2B8) * marioJumpStandData[9]) {
                mario->currSubMotionId = 8;
            }
            if (MWF32(mario, 0x7C) <= 0.0f) {
                mario->currSubMotionId = 8;
            }
            break;
        case 8:
            MWF32(mario, 0x7C) = (float_neg1_8041abf8 - MWF32(mario, 0x7C))
                                     * marioJumpStandData[8]
                                 + MWF32(mario, 0x7C);
            mario->position.y += MWF32(mario, 0x7C);
            MWF32(mario, 0x2B0) += MWF32(mario, 0x7C);
            if (MWF32(mario, 0x7C) <= 0.0f) {
                mario->currSubMotionId = 10;
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose(str_802C5C70);
                }
                marioSetPeakYpos();
                MWF32(mario, 0x7C) = marioJumpStandData[4];
                MWF32(mario, 0x80) = marioJumpStandData[5];
                MWF32(mario, 0x84) = marioJumpStandData[6];
                MWF32(mario, 0x88) = marioJumpStandData[7];
                mario->flags &= ~0x10000;
                mario->flags |= 0x20000;
                mario->currSubMotionId = 0x14;
            }
            break;
        case 10:
            marioMakeJumpPara();
            if (MWF32(mario, 0x7C) <= 0.0f) {
                if (!(mario->flags & 0x2000) && mario->characterId == 0) {
                    marioChgPose(str_802C5C70);
                }
                marioSetPeakYpos();
                MWF32(mario, 0x7C) = marioJumpStandData[4];
                MWF32(mario, 0x80) = marioJumpStandData[5];
                MWF32(mario, 0x84) = marioJumpStandData[6];
                MWF32(mario, 0x88) = marioJumpStandData[7];
                mario->flags &= ~0x10000;
                mario->flags |= 0x20000;
                mario->currSubMotionId = 0x14;
            }
            mario->position.y += MWF32(mario, 0x7C);
            break;
        case 20:
            if ((s32)--mario->multiTimer <= 0) {
                mario->baseSpeed = 0.0f;
                mario->currSubMotionId = 0x1E;
            }
            break;
    }

    mario->airTimer++;
    if (marioSlitButton() != 2) {
        marioSlitForceCancel();
    }
}

f32 marioGetJumpHeight(void) {
    MarioWork* mario = marioGetPtr();

    return MWF32(mario, 0x2B8);
}

void marioPreJump(void) {
    MarioWork* mario = marioGetPtr();

    mario->flags &= ~0x20000;
    mario->flags |= 0x10000;
    mario->airTimer = 0;
    MWS16(mario, 0x52) = 0;
    mario->hitobjJumpFrom = mario->hitobjStandOn;
    clear_hitobj_ride();
    marioSetJumpPara();
    *(Vec*)((u8*)mario + 0x11C) = mario->position;
}

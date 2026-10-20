#include <dolphin/types.h>
#include <dolphin/mtx.h>

#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_cam.h"
#include "mario/mario_hit.h"
#include "mario/mario_sbr.h"
#include "motion/mot_jump.h"
#include "driver/animdrv.h"
#include "manager/evtmgr.h"
#include "evt/evt_cmd.h"
#include "evt/evt_cam.h"
#include "pmario_sound.h"
#include "system.h"
#include "koopa.h"
#include "koopa_motion.h"
#include "koopa_hit.h"
#include "eff/eff_mizutama.h"
#include "math_ext.h"

#define MW_F32(m, off) (*(f32*)((u8*)(m) + (off)))
#define MW_S32(m, off) (*(s32*)((u8*)(m) + (off)))
#define MW_U16(m, off) (*(u16*)((u8*)(m) + (off)))
#define MW_S16(m, off) (*(s16*)((u8*)(m) + (off)))

// .rodata: the unit's strings and the int->float magic/2^52 doubles are compiler-pooled literals
// (used at their sites), not named objects.  MWCC pools them in function-emission order, so the
// functions below are in the ORIGINAL's order (reverse source order, `tools/fn_order.py want`):
// kpaHipAttackOk, kpaStickSpd, kpa_stay, kpa_walk, kpa_dash, kpa_jump, kpa_fall, kpa_hip, kpaPowUp,
// kpaPowDown, kpaSetSpec, kpa_powup, kpa_powdown, kpaSwimMove, kpa_swim, quake_kpaLandOn,
// quake_kpaHipAttack.  With any of the strings or doubles declared here, the pooled copies pile up
// after the named ones and the loads miss the original's slots.

// .sdata
f32 _walkSpdTbl[4] = {0.6f, 1.4f, 1.4f, 1.4f};
f32 _dashSpdTbl[4] = {1.2f, 2.0f, 2.0f, 2.0f};
s32 _rateTbl[4] = {0, 0, 0, 0};
s32 _rateTbl2[4] = {0, 0, 0, 0};
f32 scaleDt[9] = {1.0f, 0.0f, 2.0f, 1.0f, 3.0f, 2.0f, 4.0f, 3.0f, 5.0f};

s32 quake_kpaLandOn_evt[] = {
    EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.009765625f), 0x1E,
    EVT_CMD(0, OPCODE_END_EVENT), OPCODE_END_SCRIPT
};

s32 quake_kpaHipAttack_evt[] = {
    EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01953125f), 0x3C,
    EVT_CMD(0, OPCODE_END_EVENT), OPCODE_END_SCRIPT
};

// .sdata2
extern const f32 float_0_80420da0;
extern const f32 float_280_80420da4;
extern const f32 float_0p6_80420da8;
extern const f32 float_225_80420dac;
extern const f32 float_135_80420db0;
extern const f32 float_180_80420db4;
extern const f32 float_3_80420db8;
extern const f32 float_1_80420dbc;
extern const f32 float_neg0p0001_80420dc0;
extern const f32 float_1p52_80420dc4;
extern const f32 float_0p005_80420dc8;
extern const f32 float_0p006_80420dcc;
extern const f32 float_neg0p0008_80420dd0;
extern const f32 float_0p02_80420dd4;
extern const f32 float_neg2_80420dd8;
extern const f32 float_370_80420ddc;
extern const f32 float_2_80420de0;
extern const f32 float_neg0p03_80420de4;
extern const f32 float_0p06_80420de8;
extern const f32 float_5_80420dec;
extern const f32 float_neg0p13_80420df0;
extern const f32 float_neg0p001_80420df4;
extern const f32 float_0p5_80420df8;
extern const f32 float_neg0p08_80420dfc;
extern const f32 float_0p009_80420e00;
extern const f32 float_2p6_80420e04;
extern const f32 float_0p21_80420e08;
extern const f32 float_neg0p283_80420e0c;
extern const f32 float_neg0p023_80420e10;
extern const f32 float_0p01_80420e14;
extern const f32 float_1225_80420e18;
extern const f32 float_40_80420e1c;
extern const f32 float_24_80420e20;
extern const f32 float_74_80420e24;
extern const f32 float_103p6_80420e28;
extern const f32 float_222_80420e2c;
extern const f32 float_20_80420e30;
extern const f32 float_neg0p4744_80420e34;
extern const f32 float_0p4704_80420e38;
extern const f32 float_neg0p1904_80420e3c;
extern const f32 float_neg100_80420e40;
extern const f32 float_75_80420e44;
extern const f32 float_neg0p526_80420e48;
extern const f32 float_neg0p02_80420e4c;
extern const f32 float_70_80420e50;
extern const f32 float_neg0p2_80420e54;
extern const f32 float_neg0p7_80420e58;
extern const f32 float_0p2_80420e5c;
extern const f32 float_1p3_80420e60;
extern const f32 float_0p633_80420e64;
extern const f32 float_2p3_80420e68;
extern const f32 float_0p002_80420e6c;

s32 kpaHipAttackOk(void) {
    MarioWork* mario = marioGetPtr();
    if (mario->currMotionId == MARIO_MOTION_HIP) {
        return 0;
    }
    return ((KPA(mario)->flags & KPA_FLAG_NO_HIP) >> 4) ^ 1;
}

// The folded int->float magic pools as the original's first .rodata double (+0x60); MWCC would
// otherwise hold float_70 in a register across both clamps, so the second load vanished and
// the conversion FPRs shifted. opt_common_subs off rematerialises it and restores the target
// allocation (double=f2, const=f0, stack=f1).
#pragma opt_common_subs off
f32 kpaStickSpd(s32 arg) {
    MarioWork* mario = marioGetPtr();
    f32 stick = (f32)(s8)mario->wStickDir1;
    f32 f30;
    f32 f2;
    f32 f31;

    if (stick >= float_70_80420e50) {
        stick = float_70_80420e50;
    }
    if (stick <= -float_70_80420e50) {
        stick = -float_70_80420e50;
    }
    f30 = MW_F32(mario, 0x2C4);
    if (stick == float_0_80420da0) {
        f32 decel = float_neg0p2_80420e54;
        if (*(void**)((u8*)mario + 0x1F0) != NULL) {
            decel = float_neg0p7_80420e58;
        }
        f30 += f30 * decel;
        if (__fabsf(f30) < float_0p01_80420e14) {
            f30 = float_0_80420da0;
        }
    } else {
        if (arg == 0) {
            f2 = (kpaGetBodyStatus() == 2) ? float_0p2_80420e5c : float_1p3_80420e60;
        } else {
            f2 = (kpaGetBodyStatus() == 2) ? float_0p633_80420e64 : float_2p3_80420e68;
        }
        if (stick * f30 > float_0_80420da0) {
            f30 = float_0p002_80420e6c * stick + f30;
            if (f30 >= f2) {
                f30 = f2;
            }
            if (f30 <= -f2) {
                f30 = -f2;
            }
        } else {
            f30 = float_0p002_80420e6c * stick + f30;
            if (f30 >= f2) {
                f30 = f2;
            }
            if (f30 <= -f2) {
                f30 = -f2;
            }
        }
    }
    f31 = __fabsf(f30);
    MW_F32(mario, 0x2C4) = f30;
    mario->baseSpeed = f31;
    {
        f32 ang = angleABf(float_0_80420da0, float_0_80420da0, f30, float_0_80420da0);
        if (mario->baseSpeed != float_0_80420da0) {
            mario->directionView = ang;
        }
    }
    return mario->baseSpeed;
}
#pragma opt_common_subs reset


void kpa_stay(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    f32 f31;
    s32 flag;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->airTimer = 0;
        mario->multiTimer = 0;
        mario->currSubMotionId = 0;
        if (kpaGetAttackMode() == 0) {
            if (!(KPA(mario)->flags & KPA_FLAG_ALT_POSE)) {
                kpaChgPose("KPA_S_1", "KPA2_S_1");
            } else {
                kpaChgPose("KPA_S_2", "KPA2_S_1");
            }
        }
        if (!(mario->flags & 0x20)) {
            mario->baseSpeed = float_0_80420da0;
        }
    }
    if (kpaGetAttackMode() != 0) {
        return;
    }
    m2 = marioGetPtr();
    flag = 0;
    if (kpaGetStageViewType() == 1) {
        flag = 0;
    } else if (!(m2->buttonNew & (1 << 8))) {
        flag = 0;
    } else if (kpaGetBodyStatus() == 2) {
        marioChgMot(MARIO_MOTION_KPA_SWIM);
        flag = 1;
    } else {
        marioChgMot(MARIO_MOTION_JUMP);
        flag = 1;
    }
    if (flag != 0) {
        return;
    }
    if (mario->buttonNew & (1 << 9)) {
        kpaAttackStart();
        return;
    }
    if (mario->forceMoveTimer == 0) {
        f31 = kpaStickSpd(0);
    } else {
        f31 = mario->baseSpeed;
    }
    if (kpaGetStageViewType() == 0) {
        mario->baseSpeed = f31;
        if (mario->baseSpeed > float_0_80420da0) {
            marioChgMot(MARIO_MOTION_WALK);
        }
    } else if (mario->controlStickSensitivity > float_0_80420da0) {
        marioChgMot(MARIO_MOTION_WALK);
    }
}

// The speed/rate locals make both table loads precede their stores. The u16->f32 magic now pools
// at the original's +0xd0 (see the file-order note at the top): the functions are in the reverse
// source order the original used, so MWCC's pooled constants land in the target's slots.
void kpa_walk(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    s32 flag;
    s32 angle;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->currSubMotionId = 0;
        mario->multiTimer = 0;
        if (!(KPA(mario)->flags & KPA_FLAG_ALT_POSE)) {
            kpaChgPose("KPA_W_1", "KPA2_W_1");
        } else {
            kpaChgPose("KPA_W_2", "KPA2_W_1");
        }
        MW_U16(mario, 0x2A0) = 0;
        MW_U16(mario, 0x2A2) = 0;
        KPA(mario)->field_0x10 = float_0p02_80420dd4;
        KPA(mario)->field_0x14 = float_neg0p02_80420e4c;
    }
    m2 = marioGetPtr();
    flag = 0;
    if (kpaGetStageViewType() == 1) {
        flag = 0;
    } else if (!(m2->buttonNew & (1 << 8))) {
        flag = 0;
    } else if (kpaGetBodyStatus() == 2) {
        marioChgMot(MARIO_MOTION_KPA_SWIM);
        flag = 1;
    } else {
        marioChgMot(MARIO_MOTION_JUMP);
        flag = 1;
    }
    if (flag != 0) {
        return;
    }
    if (mario->buttonNew & (1 << 9)) {
        kpaAttackStart();
        marioChgMot(MARIO_MOTION_STAY);
        return;
    }
    if (kpaGetBodyStatus() == 2) {
        MW_U16(mario, 0x2A0)++;
        if (MW_U16(mario, 0x2A0) > 0x50) {
            MW_U16(mario, 0x2A0) = 0;
        }
        mario->poseTime = (MW_U16(mario, 0x2A0) >> 1) & 0xFFFF;
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)MW_U16(mario, 0x28));
        if (kpaGetBodyStatus() == 0) {
            if (mario->multiTimer % 0x28 == 0) {
                psndSFXOn_3D((char*)0x819, &mario->position);
            } else if (mario->multiTimer % 0x14 == 0) {
                psndSFXOn_3D((char*)0x818, &mario->position);
            }
            mario->multiTimer++;
        }
    } else if (kpaGetBodyStatus() == 0) {
        if (mario->multiTimer % 0x28 == 0) {
            psndSFXOn_3D((char*)0x819, &mario->position);
        } else if (mario->multiTimer % 0x14 == 0) {
            psndSFXOn_3D((char*)0x818, &mario->position);
        }
        mario->multiTimer++;
    }
    if (mario->forceMoveTimer == 0) {
        if (kpaGetStageViewType() == 0) {
            mario->baseSpeed = kpaStickSpd(0);
        } else {
            s32 level;
            f32 speed;
            s32 rate;
            m2 = marioGetPtr();
            level = kpaGetLevel();
            speed = _walkSpdTbl[level];
            rate = _rateTbl[level];
            KPA(m2)->moveRate = rate;
            mario->baseSpeed = speed;
        }
    }
    if (kpaGetStageViewType() == 0) {
        if (mario->baseSpeed == float_0_80420da0 && !(mario->flags & 0x20)) {
            marioChgMot(MARIO_MOTION_STAY);
            return;
        }
    } else if (mario->controlStickSensitivity == float_0_80420da0 && !(mario->flags & 0x20)) {
        marioChgMot(MARIO_MOTION_STAY);
        return;
    }
    if (marioKeyOffChk() != 0) {
        return;
    }
    marioCamZoomOffReq();
    if (kpaGetStageViewType() == 1) {
        mario->directionView = mario->controlStickAngle;
    }
    angle = (s32)(s8)mario->wStickDir1 * (s32)(s8)mario->wStickDir1 +
            (s32)(s8)mario->wStickDir2 * (s32)(s8)mario->wStickDir2;
    if (angle > 0xBD1) {
        marioChgMot(MARIO_MOTION_DASH);
    }
}
// Matched. The `& 0xFFFF` (MUST_MATCH) forces the original's extrwi for `poseTime`; staging the
// dash speed in a temp issues both table loads before the stores. The u16->f32 2^52 mask pools
// at the original's +0xd0 slot (see the file-order note at the top).
void kpa_dash(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    s32 flag;
    s32 angle;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        kpaChgPose("KPA_R_1", "KPA2_R_1");
        mario->currSubMotionId = 0;
        mario->multiTimer = 0;
        MW_U16(mario, 0x2A0) = 0;
    }
    m2 = marioGetPtr();
    flag = 0;
    if (kpaGetStageViewType() == 1) {
        flag = 0;
    } else if (!(m2->buttonNew & (1 << 8))) {
        flag = 0;
    } else if (kpaGetBodyStatus() == 2) {
        marioChgMot(MARIO_MOTION_KPA_SWIM);
        flag = 1;
    } else {
        marioChgMot(MARIO_MOTION_JUMP);
        flag = 1;
    }
    if (flag != 0) {
        return;
    }
    if (mario->buttonNew & (1 << 9)) {
        kpaAttackStart();
        marioChgMot(MARIO_MOTION_STAY);
        return;
    }
    if (kpaGetBodyStatus() == 2) {
        MW_U16(mario, 0x2A0)++;
        if (MW_U16(mario, 0x2A0) > 0x50) {
            MW_U16(mario, 0x2A0) = 0;
        }
#ifdef MUST_MATCH
        mario->poseTime = (MW_U16(mario, 0x2A0) >> 1) & 0xFFFF; // mask forces extrwi like the original
#else
        mario->poseTime = MW_U16(mario, 0x2A0) >> 1;
#endif
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)MW_U16(mario, 0x28));
    }
    if (kpaGetBodyStatus() == 0) {
        if (mario->multiTimer % 0x1E == 0) {
            psndSFXOn_3D((char*)0x819, &mario->position);
        } else if (mario->multiTimer % 0xF == 0) {
            psndSFXOn_3D((char*)0x818, &mario->position);
        }
        mario->multiTimer++;
    }
    if (mario->forceMoveTimer == 0) {
        if (kpaGetStageViewType() == 0) {
            mario->baseSpeed = kpaStickSpd(1);
        } else {
            s32 lvl;
            f32 spd;
            m2 = marioGetPtr();
            lvl = kpaGetLevel();
            spd = _dashSpdTbl[lvl];
            KPA(m2)->moveRate = _rateTbl2[lvl];
            mario->baseSpeed = spd;
        }
    }
    if (kpaGetStageViewType() == 0) {
        if (mario->baseSpeed == float_0_80420da0 && !(mario->flags & 0x20)) {
            marioChgMot(MARIO_MOTION_STAY);
            return;
        }
    } else if (mario->controlStickSensitivity == float_0_80420da0 && !(mario->flags & 0x20)) {
        marioChgMot(MARIO_MOTION_STAY);
        return;
    }
    if (marioKeyOffChk() != 0) {
        return;
    }
    marioCamZoomOffReq();
    if (kpaGetStageViewType() == 1) {
        mario->directionView = mario->controlStickAngle;
    }
    angle = (s32)(s8)mario->wStickDir1 * (s32)(s8)mario->wStickDir1 +
            (s32)(s8)mario->wStickDir2 * (s32)(s8)mario->wStickDir2;
    if (angle <= 0xBD1) {
        marioChgMot(MARIO_MOTION_WALK);
    }
}

#pragma opt_common_subs off // needed so the second compare's field/zero stay in the original f1/f0 (default CSE swaps them)
void kpa_jump(void) {
    MarioWork* mario = marioGetPtr();
    f32 f31;
    f32 f1;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags |= 0x00080000;
        mario->multiTimer = 2;
        mario->currSubMotionId = 0;
        marioPreJump();
        if (mario->currMotionId != MARIO_MOTION_JUMP_3) {
            camFollowYOff();
        }
        kpaChgPose("KPA_J_1A", "KPA2_J_1A");
        psndSFXOn_3D((char*)0x81E, &mario->position);
        if (mario->airTimer == 0) {
            mario->lastGroundPos = mario->position;
        }
        MW_U16(mario, 0x2A0) = 0;
    }
    mario->airTimer++;
    if (mario->currMotionId != MARIO_MOTION_JUMP_3) {
        f31 = mario->jumpVelY;
        marioMakeJumpPara();
        f1 = mario->jumpVelY;
        if (f1 > float_0_80420da0 && mario->airTimer >= 4 && !(mario->buttonHeld & (1 << 8))) {
            mario->jumpVelY = f1 + float_neg0p526_80420e48;
        }
        if (mario->jumpVelY <= float_0_80420da0) {
            if (f31 > float_0_80420da0) {
                MW_S16(mario, 0x52) = mario->airTimer;
            }
            marioSetPeakYpos();
            kpaSetFallPara();
            marioMakeJumpPara();
            marioChgMot(MARIO_MOTION_FALL);
        }
        mario->position.y += mario->jumpVelY;
    }
}
#pragma opt_common_subs reset


void kpa_fall_80213C38(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x00010000;
        mario->flags |= 0x00020000;
        mario->currSubMotionId = 0;
        if (mario->currMotionId != MARIO_MOTION_FALL_2) {
            if (mario->characterId == 2) {
                if (kpaFireAttackCheck() == 0) {
                    kpaChgPose("KPA_J_1B", "KPA2_J_1B");
                }
            } else if (!(mario->flags & 0x2000)) {
                marioChgPose("M_J_1C");
            }
        } else {
            mario->flags |= 0x80;
        }
        if (mario->airTimer == 0) {
            mario->lastGroundPos = mario->position;
        }
    }
    switch ((s32)mario->currSubMotionId) {
    case 0:
        if (mario->position.y <= MW_F32(mario, 0x120)) {
            if (mario->position.y <= float_75_80420e44) {
                if ((u32)(mario->dispFlags |= 0x00020000) != 0) {
                    camFollowYOn();
                }
            }
            kpaCamFollow();
            mario->currSubMotionId = 0xA;
        }
        break;
    }
    mario->airTimer++;
    if (kpaGetStageType() == 1 && kpaGetBodyStatus() <= 1) {
        KPA(mario)->flags |= KPA_FLAG_SPLASH_PENDING;
    }
    if (kpaGetBodyStatus() == 2) {
        mario = marioGetPtr();
        if (mario->currMotionId != MARIO_MOTION_KPA_SWIM) {
            marioChgMot2(MARIO_MOTION_KPA_SWIM);
        }
        kpaClearHitobjRide();
        mario->flags &= ~0x00010000;
        mario->flags |= 0x00020000;
        mario->currSubMotionId = 0x1E;
        mario->airTimer = 0;
        mario->multiTimer = 0;
        mario->baseSpeed = float_0_80420da0;
        MW_F32(mario, 0x2C4) = float_0_80420da0;
    }
}

void kpa_hip(void) {
    MarioWork* mario = marioGetPtr();
    f32 f31;
    f32 landY;
    void* hit;
    s32 timer;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x00080000;
        psndSFXOn_3D((char*)0x81F, &mario->position);
        marioAdjustMoveDir();
        mario->currSubMotionId = 0xA;
        mario->multiTimer = 6;
        mario->jumpVelY = float_0_80420da0;
        mario->jumpAccelY = float_0_80420da0;
        mario->jumpJerkY = float_0_80420da0;
        mario->jumpJerkStepY = float_0_80420da0;
        mario->baseSpeed = float_0_80420da0;
        marioChgPose("KPA2_J_1C");
    }
    if (mario->lastGroundPos.y >= mario->position.y && (mario->dispFlags & 0x20000)) {
        camFollowYOn();
    }
    switch (mario->currSubMotionId) {
    case 0xA:
        marioChkLandon(&landY, float_0_80420da0);
        if (--mario->multiTimer > 0) {
            break;
        }
        mario->currSubMotionId = 0x14;
        mario->jumpAccelY = float_neg0p4744_80420e34;
        mario->jumpJerkY = float_0p4704_80420e38;
        mario->jumpJerkStepY = float_neg0p1904_80420e3c;
        mario->jumpVelY = mario->jumpAccelY;
        /* fallthrough */
    case 0x14:
        f31 = marioGetFallSpd();
        hit = marioChkLandon(&landY, f31);
        if (f31 < float_neg100_80420e40) {
            f31 = float_neg100_80420e40;
            mario->jumpJerkStepY = float_0_80420da0;
            mario->jumpJerkY = float_0_80420da0;
            mario->jumpAccelY = float_0_80420da0;
            mario->jumpVelY = float_neg100_80420e40;
        }
        if (hit != NULL) {
            mario->hitobjStandOn = hit;
            mario->hitobjJumpFrom = NULL;
            mario->currMotionId = MARIO_MOTION_HIP_2;
            mario->currSubMotionId = 0x15;
            mario->multiTimer = 0x14;
            mario->position.y = landY;
            psndSFXOn_3D((char*)0x820, &mario->position);
            evtEntry(quake_kpaHipAttack_evt, 0, 0);
            mario->airTimer = 0;
            MW_S16(mario, 0x52) = 0;
        } else {
            mario->position.y += f31;
        }
        break;
    case 0x15:
        if (--mario->multiTimer > 0) {
            break;
        }
        if (mario->position.y > float_75_80420e44) {
            camFollowYOn();
        }
        if ((mario->buttonHeld & (1 << 8)) && mario->hitobjStandOn == NULL) {
            mario->currSubMotionId = 0x14;
            mario->jumpAccelY = float_neg0p4744_80420e34;
            mario->jumpJerkY = float_0p4704_80420e38;
            mario->jumpJerkStepY = float_neg0p1904_80420e3c;
            mario->jumpVelY = mario->jumpAccelY;
        } else {
            mario->flags &= ~0x80;
            marioChgMotSub(0, 0);
        }
        break;
    default:
        break;
    }
}

s32 kpaPowUp(void) {
    MarioWork* mario = marioGetPtr();
    if (kpaGetLevel() < 3) {
        KPA(mario)->level++;
        kpaCancelAttackMode();
        marioChgMot(MARIO_MOTION_KPA_POW_UP);
        return 1;
    }
    kpaMSetMutekiTimer();
    kpaMutekiOn();
    return 0;
}

s32 kpaPowDown(void) {
    MarioWork* mario = marioGetPtr();
    kpaCancelAttackMode();
    if (kpaGetLevel() > 0) {
        KPA(mario)->level--;
        MW_S16(mario, 0x78) = sysMsec2Frame(3000);
        marioChgMot(MARIO_MOTION_KPA_POW_DOWN);
        return 1;
    }
    KPA(mario)->flags |= 0x10000000;
    return 0;
}

void kpaSetSpec(void) {
    MarioWork* mario = marioGetPtr();
    mario->wPlayerCollisionBox.x = kpaGetWidth(KPA(mario)->level);
    if (KPA(mario)->level == 0) {
        mario->wPlayerCollisionBox.y = float_24_80420e20;
    } else if (KPA(mario)->level == 1) {
        mario->wPlayerCollisionBox.y = float_74_80420e24;
    } else if (KPA(mario)->level == 2) {
        mario->wPlayerCollisionBox.y = float_103p6_80420e28;
    } else if (KPA(mario)->level == 3) {
        mario->wPlayerCollisionBox.y = float_222_80420e2c;
    }
}

void kpa_powup(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    MarioWork* m3;
    f32 f31;
    f32 f0;
    s32 level;
    f32 f2;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->airTimer = 0;
        mario->multiTimer = 0;
        kpaChgPose("KPA_Y_1", "KPA2_Y_1");
        mario->currSubMotionId = 0;
        if (!(mario->flags & 0x20)) {
            mario->baseSpeed = float_0_80420da0;
        }
        mario->flags |= 0x10000000;
        psndSFXOn_3D((char*)0x111, &mario->position);
    }
    level = kpaGetLevel();
    if (level <= 3) {
        f31 = kpaGetBaseScale(level - 1);
        f0 = kpaGetBaseScale(level);
        f0 = (f0 - f31) / float_5_80420dec;
    } else {
        marioChgMot(MARIO_MOTION_STAY);
        return;
    }
    switch ((s32)mario->currSubMotionId) {
    case 0:
        mario->multiTimer = 5;
        mario->currSubMotionId = 1;
        psndSFXOn_3D((char*)0x823, &mario->position);
        return;
    default:
        if (--mario->multiTimer > 0) {
            return;
        }
        break;
    }
    f2 = scaleDt[mario->currSubMotionId - 1];
    mario->multiTimer = 5;
    mario->scale.x = f0 * f2 + f31;
    mario->scale.y = mario->scale.x;
    mario->currSubMotionId++;
    if (float_5_80420dec == f2) {
        m2 = marioGetPtr();
        if (KPA(m2)->level == 3) {
            kpaMSetMutekiTimer();
            kpaMutekiOn();
        }
        m3 = marioGetPtr();
        m3->wPlayerCollisionBox.x = kpaGetWidth(KPA(m3)->level);
        if (KPA(m3)->level == 0) {
            m3->wPlayerCollisionBox.y = float_24_80420e20;
        } else if (KPA(m3)->level == 1) {
            m3->wPlayerCollisionBox.y = float_74_80420e24;
        } else if (KPA(m3)->level == 2) {
            m3->wPlayerCollisionBox.y = float_103p6_80420e28;
        } else if (KPA(m3)->level == 3) {
            m3->wPlayerCollisionBox.y = float_222_80420e2c;
        }
        marioResetCamShiftRate();
        m2->unk138 = float_20_80420e30 * (f32)(-KPA(m2)->level);
        mario->flags &= ~0x10000000;
        marioChgMot(MARIO_MOTION_STAY);
        mario->unk4C = 0;
    }
}
// The currSubMotionId dispatcher is a two-case switch in the original (the target emits `beq case0; b default`);
// writing it as `if (...) {...return;} else if (--multiTimer > 0) return;` folds the two branches into one `bne`.
void kpa_powdown(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    MarioWork* m3;
    f32 f31;
    f32 f0;
    s32 level;
    f32 f2;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->airTimer = 0;
        mario->multiTimer = 0;
        kpaChgPose("KPA_D_1", "KPA2_D_1");
        mario->currSubMotionId = 0;
        if (!(mario->flags & 0x20)) {
            mario->baseSpeed = float_0_80420da0;
        }
        mario->flags |= 0x10000000;
        psndSFXOn_3D((char*)0x11D, &mario->position);
    }
    level = kpaGetLevel();
    if (level >= 0) {
        f31 = kpaGetBaseScale(level + 1);
        f0 = kpaGetBaseScale(level);
        f0 = (f31 - f0) / float_5_80420dec;
    } else {
        marioChgMot(MARIO_MOTION_STAY);
        return;
    }
    switch ((s32)mario->currSubMotionId) {
    case 0:
        mario->multiTimer = 5;
        mario->currSubMotionId = 1;
        psndSFXOn_3D((char*)0x824, &mario->position);
        return;
    default:
        if (--mario->multiTimer > 0) {
            return;
        }
        break;
    }
    f2 = scaleDt[mario->currSubMotionId - 1];
    mario->multiTimer = 5;
    mario->scale.x = f31 - f0 * f2;
    mario->scale.y = mario->scale.x;
    mario->currSubMotionId++;
    if (float_5_80420dec == f2) {
        m2 = marioGetPtr();
        kpaMutekiOff();
        m3 = marioGetPtr();
        m3->wPlayerCollisionBox.x = kpaGetWidth(KPA(m3)->level);
        if (KPA(m3)->level == 0) {
            m3->wPlayerCollisionBox.y = float_24_80420e20;
        } else if (KPA(m3)->level == 1) {
            m3->wPlayerCollisionBox.y = float_74_80420e24;
        } else if (KPA(m3)->level == 2) {
            m3->wPlayerCollisionBox.y = float_103p6_80420e28;
        } else if (KPA(m3)->level == 3) {
            m3->wPlayerCollisionBox.y = float_222_80420e2c;
        }
        marioResetCamShiftRate();
        m2->unk138 = float_20_80420e30 * (f32)(-KPA(m2)->level);
        mario->flags &= ~0x10000000;
        marioChgMot(MARIO_MOTION_STAY);
    }
}

// dont_inline keeps kpaSwimMove out of kpa_swim (retail emits a real `bl kpaSwimMove`).
#pragma dont_inline on
// opt_common_subs off keeps CSE from rotating the int->float magic with the zero (f1/f2); default scores 97.07%.
#pragma opt_common_subs off
void kpaSwimMove(void) {
    MarioWork* mario = marioGetPtr();
    f32 stick = (f32)(s8)mario->wStickDir1;
    f32 speed = MW_F32(mario, 0x2C4);
    f32 f31;
    f32 ang;

    if (stick != float_0_80420da0) {
        if (stick * speed < float_0_80420da0) {
            speed = float_0_80420da0;
        }
        if (speed * speed <= float_1225_80420e18) {
            speed = stick * float_0p01_80420e14 + speed;
        } else {
            speed = stick * float_0p01_80420e14 + speed;
            if (speed < -float_40_80420e1c) {
                speed = -float_40_80420e1c;
            }
            if (speed > float_40_80420e1c) {
                speed = float_40_80420e1c;
            }
        }
    }
    MW_F32(mario, 0x2C4) = speed;
    f31 = float_0p06_80420de8 * __fabsf(speed);
    if (f31 >= float_2_80420de0) {
        f31 = float_2_80420de0;
    }
    ang = angleABf(float_0_80420da0, float_0_80420da0, speed, float_0_80420da0);
    if (f31 < float_0p01_80420e14) {
        f31 = float_0_80420da0;
    }
    mario->baseSpeed = f31;
    if (f31 != float_0_80420da0) {
        mario->directionView = ang;
    }
    MW_F32(mario, 0x18C) = f31;
}
#pragma opt_common_subs reset
#pragma dont_inline reset


// The three periodic effMizutama blocks and the irand int->float conversion now match: with the
// functions in the original's order, MWCC pools the conversion's 2^52 double as the original's
// first .rodata entry at +0x60 (see the file-order note at the top).
void kpa_swim(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    f32 f31;
    void* hit;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->airTimer = 0;
        mario->multiTimer = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = float_0_80420da0;
        MW_F32(mario, 0x2C4) = float_0_80420da0;
        KPA(marioGetPtr())->flags |= KPA_FLAG_NO_HIP;
    }
    if (mario->buttonNew & (1 << 9)) {
        kpaAttackStart2();
    }
    if (kpaFireAttackCheck() == 1) {
        mario->wStickDir1 = 0;
        mario->wStickDir2 = 0;
    } else if (kpaGetBodyStatus() == 2 && (mario->buttonNew & (1 << 8))) {
        if ((s32)mario->currSubMotionId >= 0 && (s32)mario->currSubMotionId <= 1 &&
            mario->position.y < float_280_80420da4) {
            mario->currSubMotionId = 0;
        } else if ((s32)mario->currSubMotionId >= 0x1E && (s32)mario->currSubMotionId <= 0x20) {
            mario->currSubMotionId = 0;
        }
    }
    kpaSwimMove();
    m2 = marioGetPtr();
    KPA(m2)->bubbleTimer += 1;
    if ((s32)m2->currSubMotionId == 0x1E || (s32)m2->currSubMotionId == 0x1F) {
        if ((s32)KPA(m2)->bubbleTimer > 0) {
            f32 angle;
            f32 dx;
            f32 dy;
            KPA(m2)->bubbleTimer = 0;
            dx = (f32)(irand(0x28) - 0x14);
            dy = (f32)(irand(0x28) - 0x14);
            angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
            effMizutamaEntry(0, m2->position.x + dx,
                             m2->position.y + dy + float_0p6_80420da8 * m2->wPlayerCollisionBox.y,
                             m2->position.z, angle);
        }
    } else {
        s32 limit = ((s32)m2->currSubMotionId >= 0 && (s32)m2->currSubMotionId < 0x14) ? 0xA : 0x1E;
        if ((s32)KPA(m2)->bubbleTimer > limit) {
            f32 angle;
            KPA(m2)->bubbleTimer = 0;
            angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
            effMizutamaEntry(0, m2->position.x,
                             m2->position.y + m2->wPlayerCollisionBox.y / float_3_80420db8,
                             m2->position.z, angle);
        }
    }
    switch (mario->currSubMotionId) {
        case 0:
            if (mario->hitobjStandOn != NULL) {
                mario->jumpVelY = float_1_80420dbc;
                mario->jumpAccelY = float_0_80420da0;
                mario->jumpJerkY = float_0_80420da0;
                mario->jumpJerkStepY = float_neg0p0001_80420dc0;
            } else {
                mario->jumpVelY = float_1p52_80420dc4;
                mario->jumpAccelY = float_0p005_80420dc8;
                mario->jumpJerkY = float_0p006_80420dcc;
                mario->jumpJerkStepY = float_neg0p0008_80420dd0;
            }
            kpaClearHitobjRide();
            mario->currSubMotionId = 1;
            mario->flags &= ~0x00020000;
            mario->flags |= 0x00010000;
            psndSFXOn_3D("SFX_KUPPA_SWIM1", &mario->position);
            if (kpaGetAttackMode() == 0) {
                kpaChgPose("KPA_W_1", "KPA2_W_2");
            }
            /* fallthrough */
        case 1:
            hit = kpaGetHitobjHead2();
            if (hit != NULL) {
                mario->currSubMotionId = 0x1E;
                mario->jumpVelY += (float_neg2_80420dd8 - mario->jumpVelY) * float_0p02_80420dd4;
                mario->jumpAccelY = float_0_80420da0;
                mario->jumpJerkY = float_0_80420da0;
                mario->jumpJerkStepY = float_0_80420da0;
                break;
            }
            f31 = marioMakeJumpPara();
            mario->position.y += f31;
            if (mario->position.y + mario->wPlayerCollisionBox.y >= float_370_80420ddc) {
                if (f31 < float_0_80420da0) {
                    mario->currSubMotionId = 0x28;
                }
                mario->jumpVelY += (float_neg2_80420dd8 - mario->jumpVelY) * float_0p02_80420dd4;
                mario->jumpAccelY = float_0_80420da0;
                mario->jumpJerkY = float_0_80420da0;
                mario->jumpJerkStepY = float_0_80420da0;
            } else if (f31 < float_0_80420da0) {
                mario->currSubMotionId = 0x1E;
            }
            break;
        case 10:
            kpaClearHitobjRide();
            mario->jumpVelY = float_2_80420de0;
            mario->jumpAccelY = float_neg0p03_80420de4;
            mario->jumpJerkY = float_0_80420da0;
            mario->jumpJerkStepY = float_0_80420da0;
            mario->currSubMotionId = 0xB;
            mario->flags &= ~0x00020000;
            mario->flags |= 0x00010000;
            if (kpaGetAttackMode() == 0) {
                kpaChgPose("KPA_W_1", "KPA2_W_2");
            }
            /* fallthrough */
        case 11:
            f31 = marioMakeJumpPara();
            mario->position.y += f31;
            if (kpaGetBodyStatus() == 1) {
                mario->jumpVelY += (float_neg2_80420dd8 - mario->jumpVelY) * float_0p06_80420de8;
                mario->jumpAccelY = float_0_80420da0;
                mario->jumpJerkY = float_0_80420da0;
                mario->jumpJerkStepY = float_0_80420da0;
                if (f31 < float_0_80420da0) {
                    mario->currSubMotionId = 0x28;
                }
            } else if (f31 < float_0_80420da0) {
                mario->currSubMotionId = 0x1E;
            }
            break;
        case 0x14:
            KPA(mario)->flags |= KPA_FLAG_SPLASH_PENDING;
            mario->flags &= ~0x00020000;
            mario->flags |= 0x00010000;
            kpaChgPose("KPA_J_1A", "KPA2_J_1A");
            psndSFXOn_3D("SFX_KUPPA_WATER_UP1", &mario->position);
            mario->jumpVelY = float_5_80420dec;
            mario->jumpAccelY = float_neg0p13_80420df0;
            mario->jumpJerkY = float_0_80420da0;
            mario->jumpJerkStepY = float_neg0p001_80420df4;
            mario->currSubMotionId = 0x15;
            /* fallthrough */
        case 0x15:
            f31 = marioMakeJumpPara();
            mario->position.y += f31;
            if (f31 <= float_0p5_80420df8) {
                mario->flags &= ~0x000F0000;
                kpaSetFallPara();
                marioMakeJumpPara();
                marioChgMot(MARIO_MOTION_FALL);
                return;
            }
            break;
        case 0x16:
        default:
            break;
        }
    switch (mario->currSubMotionId) {
    case 0x1E:
        if (KPA(mario)->flags & KPA_FLAG_SPLASH_PENDING) {
            KPA(mario)->flags &= ~KPA_FLAG_SPLASH_PENDING;
            psndSFXOn_3D((char*)0x827, &mario->position);
        }
        if (kpaGetAttackMode() == 0) {
            kpaChgPose("KPA_W_1", "KPA2_Z_2");
        }
        mario->flags &= ~0x00010000;
        mario->flags |= 0x00020000;
        if (mario->jumpVelY < float_0_80420da0) {
            mario->currSubMotionId = 0x1F;
        } else {
            mario->jumpVelY = float_neg0p08_80420dfc;
            mario->jumpAccelY = float_neg0p03_80420de4;
            mario->jumpJerkY = float_0p009_80420e00;
            mario->jumpJerkStepY = float_0_80420da0;
            mario->currSubMotionId = 0x20;
        }
        m2 = marioGetPtr();
        KPA(m2)->bubbleTimer += 1;
        if ((s32)m2->currSubMotionId == 0x1E || (s32)m2->currSubMotionId == 0x1F) {
            if ((s32)KPA(m2)->bubbleTimer > 0) {
                f32 angle;
                f32 dx;
                f32 dy;
                KPA(m2)->bubbleTimer = 0;
                dx = (f32)(irand(0x28) - 0x14);
                dy = (f32)(irand(0x28) - 0x14);
                angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
                effMizutamaEntry(0, m2->position.x + dx,
                                 m2->position.y + dy + float_0p6_80420da8 * m2->wPlayerCollisionBox.y,
                                 m2->position.z, angle);
            }
        } else {
            s32 limit = ((s32)m2->currSubMotionId >= 0 && (s32)m2->currSubMotionId < 0x14) ? 0xA : 0x1E;
            if ((s32)KPA(m2)->bubbleTimer > limit) {
                f32 angle;
                KPA(m2)->bubbleTimer = 0;
                angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
                effMizutamaEntry(0, m2->position.x,
                                 m2->position.y + m2->wPlayerCollisionBox.y / float_3_80420db8,
                                 m2->position.z, angle);
            }
        }
        if (marioBgmodeChk() == 1) {
            break;
        }
        camFollowYOn();
        break;
    case 0x1F:
        {
            f32 factor = (float_2_80420de0 - mario->jumpVelY) * (f32)(kpaGetBodyStatus() == 2);
            f32 delta = (factor != float_0_80420da0) ? float_2p6_80420e04 : float_0p21_80420e08;
            mario->jumpVelY += delta;
            mario->jumpAccelY = float_0_80420da0;
            mario->jumpJerkY = float_0_80420da0;
            mario->jumpJerkStepY = float_0_80420da0;
            if (mario->jumpVelY >= float_0_80420da0) {
                mario->jumpVelY = float_neg0p08_80420dfc;
                mario->jumpAccelY = float_neg0p03_80420de4;
                mario->jumpJerkY = float_0p009_80420e00;
                mario->jumpJerkStepY = float_0_80420da0;
                mario->currSubMotionId = 0x20;
            }
        }
        if ((u32)kpaChkHitobjRide() != 0) {
            MW_F32(mario, 0x2C4) = float_0_80420da0;
            mario->baseSpeed = float_0_80420da0;
        }
        m2 = marioGetPtr();
        KPA(m2)->bubbleTimer += 1;
        if ((s32)m2->currSubMotionId == 0x1E || (s32)m2->currSubMotionId == 0x1F) {
            if ((s32)KPA(m2)->bubbleTimer > 0) {
                f32 angle;
                f32 dx;
                f32 dy;
                KPA(m2)->bubbleTimer = 0;
                dx = (f32)(irand(0x28) - 0x14);
                dy = (f32)(irand(0x28) - 0x14);
                angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
                effMizutamaEntry(0, m2->position.x + dx,
                                 m2->position.y + dy + float_0p6_80420da8 * m2->wPlayerCollisionBox.y,
                                 m2->position.z, angle);
            }
        } else {
            s32 limit = ((s32)m2->currSubMotionId >= 0 && (s32)m2->currSubMotionId < 0x14) ? 0xA : 0x1E;
            if ((s32)KPA(m2)->bubbleTimer > limit) {
                f32 angle;
                KPA(m2)->bubbleTimer = 0;
                angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
                effMizutamaEntry(0, m2->position.x,
                                 m2->position.y + m2->wPlayerCollisionBox.y / float_3_80420db8,
                                 m2->position.z, angle);
            }
        }
        break;
    case 0x20:
        if ((u32)kpaChkHitobjRide() != 0) {
            MW_F32(mario, 0x2C4) = float_0_80420da0;
            mario->baseSpeed = float_0_80420da0;
        }
        break;
    case 0x28:
        if (kpaGetAttackMode() == 0) {
            kpaChgPose("KPA_W_1", "KPA2_Z_2");
        }
        mario->flags &= ~0x00010000;
        mario->flags |= 0x00020000;
        mario->jumpVelY = float_neg0p283_80420e0c;
        mario->jumpAccelY = float_neg0p023_80420e10;
        mario->jumpJerkY = float_0p01_80420e14;
        mario->jumpJerkStepY = float_neg0p001_80420df4;
        mario->currSubMotionId = 0x29;
        mario->multiTimer = 0x18;
        /* fallthrough */
    case 0x29:
        if ((mario->buttonNew & (1 << 8)) && kpaFireAttackCheck() == 0) {
            mario->currSubMotionId = 0x14;
        } else {
            mario->multiTimer--;
            if (mario->multiTimer <= 0) {
                mario->currSubMotionId = 0x1E;
            }
        }
        break;
    default:
        break;
    }
}

void quake_kpaLandOn(void) {
    MarioWork* mario = marioGetPtr();
    if (kpaGetStageType() != 1) {
        psndSFXOn_3D((char*)0x81D, &mario->position);
        if (kpaGetLevel() >= 2) {
            evtEntry(quake_kpaLandOn_evt, 0, 0);
        }
    }
}
void quake_kpaHipAttack(void) {
    evtEntry(quake_kpaHipAttack_evt, 0, 0);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_0_80420da0 = 0.0f;
const f32 float_280_80420da4 = 280.0f;
const f32 float_0p6_80420da8 = 0.6f;
const f32 float_225_80420dac = 225.0f;
const f32 float_135_80420db0 = 135.0f;
const f32 float_180_80420db4 = 180.0f;
const f32 float_3_80420db8 = 3.0f;
const f32 float_1_80420dbc = 1.0f;
const f32 float_neg0p0001_80420dc0 = -0.0001f;
const f32 float_1p52_80420dc4 = 1.52f;
const f32 float_0p005_80420dc8 = 0.005f;
const f32 float_0p006_80420dcc = 0.006f;
const f32 float_neg0p0008_80420dd0 = -0.0008f;
const f32 float_0p02_80420dd4 = 0.02f;
const f32 float_neg2_80420dd8 = -2.0f;
const f32 float_370_80420ddc = 370.0f;
const f32 float_2_80420de0 = 2.0f;
const f32 float_neg0p03_80420de4 = -0.03f;
const f32 float_0p06_80420de8 = 0.06f;
const f32 float_5_80420dec = 5.0f;
const f32 float_neg0p13_80420df0 = -0.13f;
const f32 float_neg0p001_80420df4 = -0.001f;
const f32 float_0p5_80420df8 = 0.5f;
const f32 float_neg0p08_80420dfc = -0.08f;
const f32 float_0p009_80420e00 = 0.009f;
const f32 float_2p6_80420e04 = 2.6f;
const f32 float_0p21_80420e08 = 0.21f;
const f32 float_neg0p283_80420e0c = -0.283f;
const f32 float_neg0p023_80420e10 = -0.023f;
const f32 float_0p01_80420e14 = 0.01f;
const f32 float_1225_80420e18 = 1225.0f;
const f32 float_40_80420e1c = 40.0f;
const f32 float_24_80420e20 = 24.0f;
const f32 float_74_80420e24 = 74.0f;
const f32 float_103p6_80420e28 = 103.6f;
const f32 float_222_80420e2c = 222.0f;
const f32 float_20_80420e30 = 20.0f;
const f32 float_neg0p4744_80420e34 = -0.4744f;
const f32 float_0p4704_80420e38 = 0.4704f;
const f32 float_neg0p1904_80420e3c = -0.1904f;
const f32 float_neg100_80420e40 = -100.0f;
const f32 float_75_80420e44 = 75.0f;
const f32 float_neg0p526_80420e48 = -0.526f;
const f32 float_neg0p02_80420e4c = -0.02f;
const f32 float_70_80420e50 = 70.0f;
const f32 float_neg0p2_80420e54 = -0.2f;
const f32 float_neg0p7_80420e58 = -0.7f;
const f32 float_0p2_80420e5c = 0.2f;
const f32 float_1p3_80420e60 = 1.3f;
const f32 float_0p633_80420e64 = 0.633f;
const f32 float_2p3_80420e68 = 2.3f;
const f32 float_0p002_80420e6c = 0.002f;

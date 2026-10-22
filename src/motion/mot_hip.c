#include "dolphin/types.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_pouch.h"
#include "mario/mario_sbr.h"
#include "driver/hitdrv.h"
#include "pmario_sound.h"
#include "system.h"
#include "manager/evtmgr.h"
#include "evt/evt_cmd.h"
#include "mario/mario_hit.h"
#include "mario/mario_cam.h"
#include "evt/evt_cam.h"
#include "motion/mot_damage.h"
#include "motion/mot_hip.h"
#include "koopa_motion.h"

// Locals for symbols with no home header (see REPORT.md so they can move later).

// Event scripts. The original symbols are local (.sdata), so keep them static.
static s32 quake_evt[] = {
    EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.01953125f), 0x6E,
    EVT_CMD(0, OPCODE_END_EVENT), OPCODE_END_SCRIPT
};

static s32 quake_bgmode_evt[] = {
    EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 4, FLOAT(0.0f), FLOAT(0.009765625f), 0x6E,
    EVT_CMD(0, OPCODE_END_EVENT), OPCODE_END_SCRIPT
};

s32 marioChkHipAttack(void) {
    s32 res;
    MarioWork* mario;

    res = 0;
    mario = marioGetPtr();
    if (mario->airTimerPeak >= 10 && (mario->buttonNew & 0x100) && pouchGetJumpLv() >= 2) {
        res = 1;
    }
    return res;
}
s32 marioChkHipBump(void) {
    s32 res;
    MarioWork* mario;

    res = 0;
    mario = marioGetPtr();
    if (mario->currMotionId == MARIO_MOTION_HIP_2) {
        res = 1;
    }
    return res;
}

// NONMATCHING (99.98%): the trigFlags init emits the commuted `fmuls f2, f2, f3` where the
// original has `fmuls f2, f3, f2` (dispPivot.y += (14.0f * scale) * 0.5f); every source shape tried
// either keeps the commuted form or moves the product to f1. All other instructions match.
void mot_hip(void) {
    MarioWork* mario = marioGetPtr();
    f32 fallSpd;
    f32 landY;
    f32 outX;
    f32 outZ;
    HitEntry* hit;
    MarioWork* m2;
    f32 angle;
    f32 dy;
    f32 dx;
    f32 tmp;
    f32 base;
    s32 flag;
    void* evt;

    if (mario->characterId == 2) {
        kpa_hip();
        return;
    }
    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->flags |= 0x00020000;
        mario->flags |= 0x80;
        marioAdjustMoveDir();
        angle = mario->dispReqDir;
        flag = (angle >= 90.0f) && (angle <= 270.0f);
        if (flag) {
            angle = 180.0f;
        } else {
            angle = 0.0f;
        }
        mario->moveDir = toMovedir(angle);
        mario->directionView = mario->moveDir;
        marioChgPose("M_A_1A");
        mario->jumpVelY = 5.2f;
        mario->jumpAccelY = 0.0f;
        mario->jumpJerkY = 0.0f;
        mario->jumpJerkStepY = 0.0f;
        dy = marioGetScale() * -2.0f;
        dy *= 0.5f;
        mario->position.y += dy;
        dx = 14.0f * marioGetScale() / 2.0f;
        mario->dispPivot.y += dx;
        mario->dispOfs2.y = 0.0f;
        mario->currSubMotionId = 0;
        mario->motionSfxId0 = -1;
        mario->motionSfxId1 = -1;
    }

    switch (mario->currSubMotionId) {
    case 0:
        mario->motionSfxId0 = psndSFXOn_3D((char*)0x158, &mario->position);
        mario->motionSfxId1 = psndSFXOn_3D((char*)0xDB, &mario->position);
        mario->spinSpeed = 0.0f;
        mario->baseSpeed = 0.0f;
        mario->currSubMotionId = 1;
        // fallthrough
    case 1:
        base = mario->spinSpeed;
        tmp = 20.0f;
        tmp = base + tmp;
        mario->spinSpeed = tmp;
        if (tmp >= 360.0f) {
            mario->spinSpeed = 360.0f;
        }
        mario->jumpVelY -= 0.42f;
        if (mario->hitobjHead == NULL) {
            mario->dispOfs2.y += mario->jumpVelY;
        }
        if (mario->spinSpeed == 360.0f && mario->jumpVelY <= 0.0f) {
            mario->multiTimer = 5;
            mario->currSubMotionId = 2;
            mario->jumpVelY = 2.0f;
        }
        switch (U_chkground()) {
        case 1:
            return;
        case 2:
            marioChgMotSub(0, 0);
            m2 = marioGetPtr();
            marioOfsRotReset();
            m2->airTimer = 0;
            m2->airTimerPeak = 0;
            if ((u32)m2->motionSfxId0 != 0xFFFFFFFF) {
                psndSFXOff(m2->motionSfxId0);
                m2->motionSfxId0 = -1;
            }
            if ((u32)m2->motionSfxId1 != 0xFFFFFFFF) {
                psndSFXOff(m2->motionSfxId1);
                m2->motionSfxId1 = -1;
            }
            return;
        }
        break;
    case 2:
        if (mario->jumpVelY >= 0.0f) {
            mario->jumpVelY -= 1.4f;
            if (mario->hitobjHead == NULL) {
                mario->dispOfs2.y += mario->jumpVelY;
            }
        }
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0xA;
        }
        switch (U_chkground()) {
        case 1:
            return;
        case 2:
            marioChgMotSub(0, 0);
            m2 = marioGetPtr();
            marioOfsRotReset();
            m2->airTimer = 0;
            m2->airTimerPeak = 0;
            if ((u32)m2->motionSfxId0 != 0xFFFFFFFF) {
                psndSFXOff(m2->motionSfxId0);
                m2->motionSfxId0 = -1;
            }
            if ((u32)m2->motionSfxId1 != 0xFFFFFFFF) {
                psndSFXOff(m2->motionSfxId1);
                m2->motionSfxId1 = -1;
            }
            return;
        }
        break;
    case 0xA:
        marioChkLandon(&landY, 0.0f);
        base = mario->spinSpeed;
        tmp = 22.5f;
        tmp = base + tmp;
        mario->spinSpeed = tmp;
        if (tmp >= 360.0f) {
            mario->flags &= ~0x10000;
            mario->flags |= 0x20000;
            mario->spinSpeed = 0.0f;
            mario->currSubMotionId = 0x14;
            mario->jumpAccelY = -3.4744f;
            mario->jumpJerkY = 0.4704f;
            mario->jumpJerkStepY = -1.1904f;
            mario->jumpVelY = mario->jumpAccelY;
        }
        switch (U_chkground()) {
        case 1:
            return;
        case 2:
            marioChgMotSub(0, 0);
            m2 = marioGetPtr();
            marioOfsRotReset();
            m2->airTimer = 0;
            m2->airTimerPeak = 0;
            if ((u32)m2->motionSfxId0 != 0xFFFFFFFF) {
                psndSFXOff(m2->motionSfxId0);
                m2->motionSfxId0 = -1;
            }
            if ((u32)m2->motionSfxId1 != 0xFFFFFFFF) {
                psndSFXOff(m2->motionSfxId1);
                m2->motionSfxId1 = -1;
            }
            return;
        }
        break;
    case 0x14:
        mario->multiTimer = sysMsec2Frame(0xC8);
        mario->currSubMotionId = 0x15;
        // fallthrough
    case 0x15:
        if (--mario->multiTimer <= 0) {
            fallSpd = marioGetFallSpd();
            hit = marioSearchGround(&landY, &outX, &outZ, fallSpd, fallSpd);
            if (fallSpd < -100.0f) {
                fallSpd = -100.0f;
                mario->jumpJerkStepY = 0.0f;
                mario->jumpJerkY = 0.0f;
                mario->jumpAccelY = 0.0f;
                mario->jumpVelY = -100.0f;
            }
            if (hit != NULL) {
                mario->currMotionId = MARIO_MOTION_HIP_2;
                mario->currSubMotionId = 0x16;
                mario->multiTimer = 0x14;
                mario->hitobjStandOn = hit;
                mario->hitobjJumpFrom = NULL;
                mario->position.y = landY;
                mario->spinSpeed = 0.0f;
                mario->dispPivot.y = 0.0f;
                mario->airTimer = 0;
                mario->airTimerPeak = 0;
                mario->dispOfs2.y = 0.0f;
                marioChgPose("M_A_1B");
                {
                    s32 attr = hitGetAttr(hit);
                    if (attr & 0x200) {
                        set_damage_root_ypos(mario->position.y);
                        mario->jumpVelY = -0.4f;
                        mario->jumpAccelY = -0.4f;
                        mario->jumpJerkY = 0.0f;
                        mario->jumpJerkStepY = 0.0f;
                        marioChgMot(0x1F);
                        return;
                    }
                    if (attr & 0x400) {
                        set_damage_root_ypos(mario->position.y);
                        marioChgMot(0x20);
                        return;
                    }
                    if (attr & 0x800) {
                        set_damage_root_ypos(mario->position.y);
                        marioChgMot(0x20);
                        return;
                    }
                }
                psndSFXOn_3D((char*)0x159, &mario->position);
                if (marioBgmodeChk() == 0) {
                    evt = quake_evt;
                } else {
                    evt = quake_bgmode_evt;
                }
                evtEntryType(evt, 0, 0, 0);
            } else {
                mario->position.y += fallSpd;
            }
            if (mario->screenY > 0x190) {
                if (marioBgmodeChk() == 0) {
                    camFollowYOn();
                }
            }
        }
        break;
    case 0x16:
        if (U_chkground() == 1) {
            return;
        }
        if (--mario->multiTimer <= 0) {
            mario->dispFlags &= ~0x08000000;
            if (marioBgmodeChk() == 0) {
                camFollowYOn();
            }
            mario->dispFlags &= ~4;
            mario->flags &= ~0x80;
            marioChgMotSub(0, 0);
            m2 = marioGetPtr();
            marioOfsRotReset();
            m2->airTimer = 0;
            m2->airTimerPeak = 0;
            if ((u32)m2->motionSfxId0 != 0xFFFFFFFF) {
                psndSFXOff(m2->motionSfxId0);
                m2->motionSfxId0 = -1;
            }
            if ((u32)m2->motionSfxId1 != 0xFFFFFFFF) {
                psndSFXOff(m2->motionSfxId1);
                m2->motionSfxId1 = -1;
            }
        }
        break;
    case 0x32:
        marioChgPose("M_A_1A");
        mario->jumpVelY = 0.0f;
        mario->jumpAccelY = 0.0f;
        mario->jumpJerkY = 0.0f;
        mario->jumpJerkStepY = 0.0f;
        mario->spinSpeed += (f32)keyGetSubStickY(2) / 10.0f;
        break;
    default:
        break;
    }

    mario->dispRot.z = mario->spinSpeed;
    if (mario->airTimer != 0) {
        mario->riderDelta.x = 0.0f;
        mario->riderDelta.z = 0.0f;
        mario->riderDelta.y = 0.0f;
    }
}

s32 U_chkground(void) {
    MarioWork* mario = marioGetPtr();
    f32 landY;
    HitEntry* hit;
    s32 attr;

    hit = marioChkLandon(&landY, 1.0f);
    if (hit != NULL) {
        mario->hitobjStandOn = hit;
        mario->hitobjJumpFrom = NULL;
        mario->position.y = landY;
        mario->spinSpeed = 0.0f;
        mario->dispPivot.y = 0.0f;
        mario->dispOfs2.y = 0.0f;
        mario->airTimer = 0;
        mario->airTimerPeak = 0;
        mario->dispFlags &= ~4;
        mario->flags &= ~0x80;
        camFollowYOn();
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            set_damage_root_ypos(mario->position.y);
            mario->jumpVelY = -0.4f;
            mario->jumpAccelY = -0.4f;
            mario->jumpJerkY = 0.0f;
            mario->jumpJerkStepY = 0.0f;
            marioChgMot(0x1F);
            return 1;
        }
        if (attr & 0xC00) {
            set_damage_root_ypos(mario->position.y);
            marioChgMot(0x20);
            return 1;
        }
        return 2;
    }
    return 0;
}

void mot_hip_post(void) {
    MarioWork* mario = marioGetPtr();

    marioOfsRotReset();
    mario->airTimer = 0;
    mario->airTimerPeak = 0;
    if ((u32)mario->motionSfxId0 != 0xFFFFFFFF) {
        psndSFXOff(mario->motionSfxId0);
        mario->motionSfxId0 = -1;
    }
    if ((u32)mario->motionSfxId1 != 0xFFFFFFFF) {
        psndSFXOff(mario->motionSfxId1);
        mario->motionSfxId1 = -1;
    }
}


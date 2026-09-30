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
    if (*(s16*)((u8*)mario + 0x52) >= 10 && (*(u16*)((u8*)mario + 0x248) & 0x100) && pouchGetJumpLv() >= 2) {
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
// original has `fmuls f2, f3, f2` (0x0B4 += (14.0f * scale) * 0.5f); every source shape tried
// either keeps the commuted form or moves the product to f1. All other instructions match.
void mot_hip(void) {
    MarioWork* mario = marioGetPtr();
    f32 fallSpd;
    f32 landY;
    f32 outX;
    f32 outZ;
    void* hit;
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
        angle = *(f32*)((u8*)mario + 0x1AC);
        flag = (angle >= 90.0f) && (angle <= 270.0f);
        if (flag) {
            angle = 180.0f;
        } else {
            angle = 0.0f;
        }
        *(f32*)((u8*)mario + 0x19C) = toMovedir(angle);
        mario->directionView = *(f32*)((u8*)mario + 0x19C);
        marioChgPose("M_A_1A");
        *(f32*)((u8*)mario + 0x7C) = 5.2f;
        *(f32*)((u8*)mario + 0x80) = 0.0f;
        *(f32*)((u8*)mario + 0x84) = 0.0f;
        *(f32*)((u8*)mario + 0x88) = 0.0f;
        dy = marioGetScale() * -2.0f;
        dy *= 0.5f;
        mario->position.y += dy;
        dx = 14.0f * marioGetScale() / 2.0f;
        *(f32*)((u8*)mario + 0xB4) += dx;
        *(f32*)((u8*)mario + 0xA8) = 0.0f;
        mario->currSubMotionId = 0;
        *(s32*)((u8*)mario + 0x2A4) = -1;
        *(s32*)((u8*)mario + 0x2A8) = -1;
    }

    switch (mario->currSubMotionId) {
    case 0:
        *(s32*)((u8*)mario + 0x2A4) = psndSFXOn_3D((char*)0x158, &mario->position);
        *(s32*)((u8*)mario + 0x2A8) = psndSFXOn_3D((char*)0xDB, &mario->position);
        *(f32*)((u8*)mario + 0x2A0) = 0.0f;
        mario->baseSpeed = 0.0f;
        mario->currSubMotionId = 1;
        // fallthrough
    case 1:
        base = *(f32*)((u8*)mario + 0x2A0);
        tmp = 20.0f;
        tmp = base + tmp;
        *(f32*)((u8*)mario + 0x2A0) = tmp;
        if (tmp >= 360.0f) {
            *(f32*)((u8*)mario + 0x2A0) = 360.0f;
        }
        *(f32*)((u8*)mario + 0x7C) -= 0.42f;
        if (*(void**)((u8*)mario + 0x1F4) == NULL) {
            *(f32*)((u8*)mario + 0xA8) += *(f32*)((u8*)mario + 0x7C);
        }
        if (*(f32*)((u8*)mario + 0x2A0) == 360.0f && *(f32*)((u8*)mario + 0x7C) <= 0.0f) {
            mario->multiTimer = 5;
            mario->currSubMotionId = 2;
            *(f32*)((u8*)mario + 0x7C) = 2.0f;
        }
        switch (U_chkground()) {
        case 1:
            return;
        case 2:
            marioChgMotSub(0, 0);
            m2 = marioGetPtr();
            marioOfsRotReset();
            m2->airTimer = 0;
            *(s16*)((u8*)m2 + 0x52) = 0;
            if (*(u32*)((u8*)m2 + 0x2A4) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)m2 + 0x2A4));
                *(s32*)((u8*)m2 + 0x2A4) = -1;
            }
            if (*(u32*)((u8*)m2 + 0x2A8) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)m2 + 0x2A8));
                *(s32*)((u8*)m2 + 0x2A8) = -1;
            }
            return;
        }
        break;
    case 2:
        if (*(f32*)((u8*)mario + 0x7C) >= 0.0f) {
            *(f32*)((u8*)mario + 0x7C) -= 1.4f;
            if (*(void**)((u8*)mario + 0x1F4) == NULL) {
                *(f32*)((u8*)mario + 0xA8) += *(f32*)((u8*)mario + 0x7C);
            }
        }
        if ((s32)--mario->multiTimer <= 0) {
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
            *(s16*)((u8*)m2 + 0x52) = 0;
            if (*(u32*)((u8*)m2 + 0x2A4) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)m2 + 0x2A4));
                *(s32*)((u8*)m2 + 0x2A4) = -1;
            }
            if (*(u32*)((u8*)m2 + 0x2A8) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)m2 + 0x2A8));
                *(s32*)((u8*)m2 + 0x2A8) = -1;
            }
            return;
        }
        break;
    case 0xA:
        marioChkLandon(&landY, 0.0f);
        base = *(f32*)((u8*)mario + 0x2A0);
        tmp = 22.5f;
        tmp = base + tmp;
        *(f32*)((u8*)mario + 0x2A0) = tmp;
        if (tmp >= 360.0f) {
            mario->flags &= ~0x10000;
            mario->flags |= 0x20000;
            *(f32*)((u8*)mario + 0x2A0) = 0.0f;
            mario->currSubMotionId = 0x14;
            *(f32*)((u8*)mario + 0x80) = -3.4744f;
            *(f32*)((u8*)mario + 0x84) = 0.4704f;
            *(f32*)((u8*)mario + 0x88) = -1.1904f;
            *(f32*)((u8*)mario + 0x7C) = *(f32*)((u8*)mario + 0x80);
        }
        switch (U_chkground()) {
        case 1:
            return;
        case 2:
            marioChgMotSub(0, 0);
            m2 = marioGetPtr();
            marioOfsRotReset();
            m2->airTimer = 0;
            *(s16*)((u8*)m2 + 0x52) = 0;
            if (*(u32*)((u8*)m2 + 0x2A4) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)m2 + 0x2A4));
                *(s32*)((u8*)m2 + 0x2A4) = -1;
            }
            if (*(u32*)((u8*)m2 + 0x2A8) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)m2 + 0x2A8));
                *(s32*)((u8*)m2 + 0x2A8) = -1;
            }
            return;
        }
        break;
    case 0x14:
        mario->multiTimer = sysMsec2Frame(0xC8);
        mario->currSubMotionId = 0x15;
        // fallthrough
    case 0x15:
        if ((s32)--mario->multiTimer <= 0) {
            fallSpd = marioGetFallSpd();
            hit = marioSearchGround(&landY, &outX, &outZ, fallSpd, fallSpd);
            if (fallSpd < -100.0f) {
                fallSpd = -100.0f;
                *(f32*)((u8*)mario + 0x88) = 0.0f;
                *(f32*)((u8*)mario + 0x84) = 0.0f;
                *(f32*)((u8*)mario + 0x80) = 0.0f;
                *(f32*)((u8*)mario + 0x7C) = -100.0f;
            }
            if (hit != NULL) {
                mario->currMotionId = MARIO_MOTION_HIP_2;
                mario->currSubMotionId = 0x16;
                mario->multiTimer = 0x14;
                mario->hitobjStandOn = hit;
                mario->hitobjJumpFrom = NULL;
                mario->position.y = landY;
                *(f32*)((u8*)mario + 0x2A0) = 0.0f;
                *(f32*)((u8*)mario + 0xB4) = 0.0f;
                mario->airTimer = 0;
                *(s16*)((u8*)mario + 0x52) = 0;
                *(f32*)((u8*)mario + 0xA8) = 0.0f;
                marioChgPose("M_A_1B");
                {
                    s32 attr = hitGetAttr((HitEntry*)hit);
                    if (attr & 0x200) {
                        set_damage_root_ypos(mario->position.y);
                        *(f32*)((u8*)mario + 0x7C) = -0.4f;
                        *(f32*)((u8*)mario + 0x80) = -0.4f;
                        *(f32*)((u8*)mario + 0x84) = 0.0f;
                        *(f32*)((u8*)mario + 0x88) = 0.0f;
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
            if (mario->unk16C > 0x190) {
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
        if ((s32)--mario->multiTimer <= 0) {
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
            *(s16*)((u8*)m2 + 0x52) = 0;
            if (*(u32*)((u8*)m2 + 0x2A4) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)m2 + 0x2A4));
                *(s32*)((u8*)m2 + 0x2A4) = -1;
            }
            if (*(u32*)((u8*)m2 + 0x2A8) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)m2 + 0x2A8));
                *(s32*)((u8*)m2 + 0x2A8) = -1;
            }
        }
        break;
    case 0x32:
        marioChgPose("M_A_1A");
        *(f32*)((u8*)mario + 0x7C) = 0.0f;
        *(f32*)((u8*)mario + 0x80) = 0.0f;
        *(f32*)((u8*)mario + 0x84) = 0.0f;
        *(f32*)((u8*)mario + 0x88) = 0.0f;
        *(f32*)((u8*)mario + 0x2A0) += (f32)keyGetSubStickY(2) / 10.0f;
        break;
    default:
        break;
    }

    *(f32*)((u8*)mario + 0xC4) = *(f32*)((u8*)mario + 0x2A0);
    if (mario->airTimer != 0) {
        *(f32*)((u8*)mario + 0x174) = 0.0f;
        *(f32*)((u8*)mario + 0x17C) = 0.0f;
        *(f32*)((u8*)mario + 0x178) = 0.0f;
    }
}

// U_chkground is called (not inlined) by mot_hip in the original.
#pragma dont_inline on
s32 U_chkground(void) {
    MarioWork* mario = marioGetPtr();
    f32 landY;
    void* hit;
    s32 attr;

    hit = marioChkLandon(&landY, 1.0f);
    if (hit != NULL) {
        mario->hitobjStandOn = hit;
        mario->hitobjJumpFrom = NULL;
        mario->position.y = landY;
        *(f32*)((u8*)mario + 0x2A0) = 0.0f;
        *(f32*)((u8*)mario + 0xB4) = 0.0f;
        *(f32*)((u8*)mario + 0xA8) = 0.0f;
        mario->airTimer = 0;
        *(s16*)((u8*)mario + 0x52) = 0;
        mario->dispFlags &= ~4;
        mario->flags &= ~0x80;
        camFollowYOn();
        attr = hitGetAttr((HitEntry*)hit);
        if (attr & 0x200) {
            set_damage_root_ypos(mario->position.y);
            *(f32*)((u8*)mario + 0x7C) = -0.4f;
            *(f32*)((u8*)mario + 0x80) = -0.4f;
            *(f32*)((u8*)mario + 0x84) = 0.0f;
            *(f32*)((u8*)mario + 0x88) = 0.0f;
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
#pragma dont_inline reset

void mot_hip_post(void) {
    MarioWork* mario = marioGetPtr();

    marioOfsRotReset();
    mario->airTimer = 0;
    *(s16*)((u8*)mario + 0x52) = 0;
    if (*(u32*)((u8*)mario + 0x2A4) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)mario + 0x2A4));
        *(s32*)((u8*)mario + 0x2A4) = -1;
    }
    if (*(u32*)((u8*)mario + 0x2A8) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)mario + 0x2A8));
        *(s32*)((u8*)mario + 0x2A8) = -1;
    }
}


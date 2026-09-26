#include "dolphin/types.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_cam.h"
#include "mario/mario_pouch.h"
#include "mario/mario_sbr.h"
#include "mot/mot_slit.h"
#include "pmario_sound.h"
#include "system.h"
#include "peach.h"
#include <string.h>
#include "matching.h"

// Local declarations for functions whose headers don't declare them yet.
// Listed in REPORT.md so they can move to a header later.
void kpa_stay(void);
void peach_stay(void);
void marioClearJumpPara(void);
void marioChgMot2(s32 motion);
BOOL marioChkJabara(void);
void motSlitCancel(void);

// Raw accessors for MarioWork fields not named in include/mario/mario.h.
// Offsets are listed in REPORT.md.
#define MW(m, off) (*(u32*)((u8*)(m) + (off)))
#define MWS32(m, off) (*(s32*)((u8*)(m) + (off)))
#define MWU16(m, off) (*(u16*)((u8*)(m) + (off)))
#define MWS16(m, off) (*(s16*)((u8*)(m) + (off)))
#define MWVEC(m, off) (*(Vec*)((u8*)(m) + (off)))
#define MW_ZERO MATCH_F32(float_0_8041aef8)

// .rodata
const char str_802C5F28[] = "M_S_1R";
const char str_802C5F30[] = "M_N_5A";

// .sdata2
const char str_8041AEF0[] = "M_S_1";
const f32 float_0_8041aef8 = 0.0f;
const char str_8041AEFC[] = "M_I_S";
const char str_8041AF04[] = "M_S_3";
const char str_8041AF0C[] = "M_I_Y";
const char str_8041AF14[] = "M_U_2";
const char str_8041AF1C[] = "M_I_5";

void marioVoiceGlareOn(void) {
    MarioWork* mario = marioGetPtr();

    if (MW(mario, 0x27C) == 0xFFFFFFFFu) {
        MW(mario, 0x27C) = psndSFXOn_3D((const char*)0xDC, &mario->position);
    }
}

void marioVoiceGlareOff(void) {
    MarioWork* mario = marioGetPtr();

    if (MW(mario, 0x27C) != 0xFFFFFFFFu) {
        psndSFXOff(MW(mario, 0x27C));
        MW(mario, 0x27C) = 0xFFFFFFFFu;
    }
}

BOOL marioChkDeepSleep(void) {
    BOOL ret = FALSE;
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId == MARIO_MOTION_STAY) {
        if (MWS32(mario, 0x2B4) >= sysMsec2Frame(0x2710)) {
            ret = TRUE;
        }
    }
    return ret;
}

void mot_stay(void) {
    MarioWork* mario = marioGetPtr();
    s32 flag = 0;
    s32 charId = mario->characterId;

    if (charId == 2) {
        kpa_stay();
        return;
    }
    if (charId == 1) {
        peach_stay();
        return;
    }

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags &= ~0xF0000;
        marioClearJumpPara();
        mario->airTimer = 0;
        flag = 1;
        mario->multiTimer = 0;
        mario->currSubMotionId = 0;
        mario->wMotionTimer = 0;
        MWVEC(mario, 0x11C) = mario->position;
        MWS32(mario, 0x2B0) = 0;
        MWS32(mario, 0x2B8) = 0;
        MWS32(mario, 0x2B4) = 0;
        MWS16(mario, 0x5A) = 0;
        marioSoundInit();
        marioResetCamFollowRate();

        if (mario->flags & MARIO_FLAG_IS_8_BIT) {
            marioChgPose(str_8041AEF0);
            if (!(mario->flags & 0x20)) {
                mario->baseSpeed = MW_ZERO;
            }
            mario->flags &= ~0x8000;
        } else {
            if (mario->flags & MARIO_FLAG_PAPER_MODE) {
                motSlitContinue();
            } else if (mario->dispFlags & 0x1000000) {
                marioChgMot2(MARIO_MOTION_SLIT);
                motSlitCancel();
            }
            if (!(mario->flags & MARIO_FLAG_IS_CARRYING_BOBBERY)) {
                if (pouchGetHP() == 0) {
                    marioChgPose(str_8041AEFC);
                    psndSFXOn_3D((const char*)0x173, &mario->position);
                } else {
                    s32 low = pouchGetHP() <= 5;

                    if (low && marioChkKey()) {
                        mario->dispFlags |= 4;
                        marioChgPose(str_8041AF04);
                        MWS16(mario, 0x58) = 0;
                    } else if (strcmp(mario->animName, str_8041AF0C) != 0) {
                        marioChgPose(str_8041AEF0);
                    }
                }
            } else {
                marioChgPoseTime(str_8041AF14, 0xA);
            }
        }
        if (!(mario->flags & 0x20)) {
            mario->baseSpeed = MW_ZERO;
        }
        mario->flags &= ~0x8000;
    }

    if (mario->flags & MARIO_FLAG_IS_8_BIT) {
        marioChkJump();
        if (mario->controlStickSensitivity > MW_ZERO) {
            s8 stickDir1 = mario->wStickDir1;
            s8 stickDir2 = mario->wStickDir2;

            if ((stickDir1 * stickDir1) + (stickDir2 * stickDir2) > 0xBD1) {
                if (!pouchEquipCheckBadge(0x143)) {
                    marioChgMot(MARIO_MOTION_DASH);
                } else {
                    marioChgMot(MARIO_MOTION_WALK);
                }
            } else {
                marioChgMot(MARIO_MOTION_WALK);
            }
        }
        return;
    }

    if (++MWS16(mario, 0x5A) > sysMsec2Frame(1000)) {
        if (!(mario->flags & MARIO_FLAG_IS_CARRYING_BOBBERY) && marioChkKey()) {
            s32 low = pouchGetHP() <= 5;

            if (low) {
                if (strcmp(mario->animName, str_8041AF04) != 0) {
                    MWS16(mario, 0x58) = 0;
                }
                mario->dispFlags |= 4;
                marioChgPose(str_8041AF04);
            } else {
                if ((s32)mario->currSubMotionId == 0 && MWS16(mario, 0x5A) != 0) {
                    MWS16(mario, 0x5A) = 0;
                    if (strcmp(mario->animName, str_8041AF0C) != 0) {
                        marioChgPose(str_8041AEF0);
                    }
                }
            }
        }
    }

    if (strcmp(mario->animName, str_8041AF04) == 0 && marioChkKey()) {
        MarioWork* m = marioGetPtr();

        MWS16(m, 0x58)++;
        if (MWS16(m, 0x58) > 0xF0) {
            MWS16(m, 0x58) = 0;
        }
        if (MWS16(m, 0x58) == 0xF) {
            MW(m, 0x278) = psndSFXOn_3D((const char*)0xE1, &m->position);
        }
        if (MWS16(m, 0x58) == 0x87) {
            MW(m, 0x278) = psndSFXOn_3D((const char*)0xE2, &m->position);
        }
    }

    if (mario->flags & MARIO_FLAG_IS_CARRYING_BOBBERY) {
        if (mario->controlStickSensitivity > MW_ZERO) {
            marioChgMot(MARIO_MOTION_WALK);
        }
        return;
    }

    if (!flag && marioChkItemMotion()) {
        return;
    }
    if (MWU16(mario, 0x246) & 0x200) {
        return;
    }
    if (marioSlitAbilityChk() && marioSlitButton() == 1) {
        marioChgMot(MARIO_MOTION_SLIT);
        return;
    }

    if (++MWS32(mario, 0x2B0) >= 0xA) {
        MWS32(mario, 0x2B0) = 0xA;
        if (marioChkJabara()) {
            marioChgMot(MARIO_MOTION_JABARA);
            return;
        }
    }

    marioChkJump();
    marioChkTransform();
    {
        BOOL bail = FALSE;

        if (MWU16(mario, 0x246) & 0x100) {
            MWS32(mario, 0x2B8)++;
        } else {
            MWS32(mario, 0x2B8) = 0;
        }
        if (MWU16(mario, 0x246) & 0x100) {
            if (MWS32(mario, 0x2B8) < 0xA) {
                bail = FALSE;
            } else {
                bail = TRUE;
            }
        }

        if (mario->wMotionTimer == 0 && mario->controlStickSensitivity > MW_ZERO && !bail) {
            s8 stickDir1 = mario->wStickDir1;
            s8 stickDir2 = mario->wStickDir2;

            if ((stickDir1 * stickDir1) + (stickDir2 * stickDir2) > 0xBD1) {
                if (!pouchEquipCheckBadge(0x143)) {
                    marioChgMot(MARIO_MOTION_DASH);
                } else {
                    marioChgMot(MARIO_MOTION_WALK);
                }
            } else {
                marioChgMot(MARIO_MOTION_WALK);
            }
        }
    }

    if ((mario->flags & 0x8) || (mario->dispFlags & 0x40000000)) {
        if ((s32)mario->currSubMotionId > 0) {
            if (strcmp(mario->animName, str_8041AEF0) != 0
                && strcmp(mario->animName, str_802C5F28) != 0) {
                marioChgPose(str_8041AEF0);
                mario->currSubMotionId = 0;
            }
        }
        return;
    }

    {
        s32 low = pouchGetHP() <= 5;

        if (low) {
            return;
        }
        switch ((s32)mario->currSubMotionId) {
            case 0:
                if ((s32)++mario->multiTimer >= sysMsec2Frame(50000)) {
                    mario->currSubMotionId = 1;
                }
                break;
            case 1:
                marioChgEvtPose(str_8041AF1C);
                marioAdjustMoveDir();
                {
                    MarioWork* m = marioGetPtr();
                    MW(m, 0x278) = psndSFXOn_3D((const char*)0xEF, &m->position);
                }
                mario->multiTimer = 0x32;
                mario->currSubMotionId = 2;
                // fallthrough
            case 2:
                if ((s32)--mario->multiTimer < 0) {
                    marioChgPose(str_8041AEF0);
                    mario->multiTimer = 0x64;
                    mario->currSubMotionId = 3;
                }
                break;
            case 3:
                if ((s32)--mario->multiTimer < 0) {
                    marioChgEvtPose(str_802C5F30);
                    mario->multiTimer = 0;
                    mario->currSubMotionId = 4;
                    MWS32(mario, 0x2B4) = 0;
                    {
                        MarioWork* m = marioGetPtr();
                        MW(m, 0x278) = psndSFXOn_3D((const char*)0xE4, &m->position);
                    }
                }
                break;
            case 4:
                if ((s32)++MWS32(mario, 0x2B4) >= sysMsec2Frame(0x2710)) {
                    MWS32(mario, 0x2B4) = sysMsec2Frame(0x2710);
                }
                if ((s32)++mario->multiTimer >= 0x8C) {
                    mario->multiTimer = 0;
                    {
                        MarioWork* m = marioGetPtr();
                        MW(m, 0x278) = psndSFXOn_3D((const char*)0xE4, &m->position);
                    }
                }
                if (!(mario->flags & 0x8)) {
                    s32 low = pouchGetHP() <= 5;

                    if (!low) {
                        break;
                    }
                }
                marioChgPose(str_8041AEF0);
                mario->currSubMotionId = 0;
                break;
        }
    }
}

void marioChgMotAuto(void) {
    MarioWork* mario = marioGetPtr();
    s8* stick = (s8*)&mario->wStickDir1;

    if (stick[0] == 0 && stick[1] == 0) {
        marioChgMot(MARIO_MOTION_STAY);
        return;
    }
    if ((stick[0] * stick[0]) + (stick[1] * stick[1]) > 0xBD1) {
        if (!pouchEquipCheckBadge(0x143)) {
            marioChgMot(MARIO_MOTION_DASH);
        } else {
            marioChgMot(MARIO_MOTION_WALK);
        }
    } else {
        marioChgMot(MARIO_MOTION_WALK);
    }
}

void mot_stay_post(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_HAMMER_2 && mario->currMotionId != MARIO_MOTION_JABARA) {
        mario = marioGetPtr();
        if (MW(mario, 0x278) != 0xFFFFFFFFu) {
            psndSFXOff(MW(mario, 0x278));
            MW(mario, 0x278) = 0xFFFFFFFFu;
        }
        mario = marioGetPtr();
        if (MW(mario, 0x27c) != 0xFFFFFFFFu) {
            psndSFXOff(MW(mario, 0x27c));
            MW(mario, 0x27c) = 0xFFFFFFFFu;
        }
    } else {
        mario = marioGetPtr();
        if (MW(mario, 0x278) != 0xFFFFFFFFu) {
            psndSFXOff(MW(mario, 0x278));
            MW(mario, 0x278) = 0xFFFFFFFFu;
        }
    }
}

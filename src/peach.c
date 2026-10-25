#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_sbr.h"
#include "mot/mot_walk.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"
#include "driver/animdrv.h"
#include <string.h>
#include "matching.h"
#include "peach.h"
#include "mario/mario_hit.h"
#include "mario/mario_cam.h"
#include "driver/npcdrv.h"
#include "motion/mot_walk.h"
#include "mario/mario_balloon.h"
#include "mario/mario_kemuri.h"

// locals for symbols with no home header (see REPORT.md)

// .sdata2
const f32 float_0_80420250 = 0.0f;
const char peach_pose_talk[] = "P_T_1";
const char peach_pose_dash[] = "P_R_1";
const char peach_pose_walk[] = "P_W_1";
const char peach_pose_stay[] = "P_S_1";
const char str_P_U_1[] = "P_U_1";
const char str_P_N_1[] = "P_N_1";
const f32 float_47_80420284 = 47.0f;
const f32 float_20_80420288 = 20.0f;
const char peach_pose_transformed_stay[] = "S_1";
const char peach_pose_transformed_talk[] = "T_1";
const f32 float_30_80420294 = 30.0f;
const char str_P_Z_1[] = "P_Z_1";
const char str_Z_1[] = "Z_1";
const char peach_pose_transformed_walk[] = "W_1";
const char peach_pose_transformed_dash[] = "R_1";
const char str_P_Z_2[] = "P_Z_2";
const char peach_pose_aftershower_stay[] = "P_S_2";
const char peach_pose_aftershower_walk[] = "P_W_2";
const char peach_pose_aftershower_dash[] = "P_R_2";
const char peach_pose_aftershower_talk[] = "P_T_2";
const char str_P_Z_3[] = "P_Z_3";
const char peach_pose_invisible_stay[] = "P_S_3";
const char peach_pose_invisible_walk[] = "P_W_3";
const char peach_pose_invisible_dash[] = "P_R_3";
const char peach_pose_invisible_talk[] = "P_T_3";
const char str_P_Z_4[] = "P_Z_4";
const char peach_pose_uneasy_stay[] = "P_S_4";
const char peach_pose_uneasy_walk[] = "P_W_4";
const char peach_pose_uneasy_dash[] = "P_R_4";
const char peach_pose_uneasy_talk[] = "P_T_4";
const char str_P_Z_5[] = "P_Z_5";
const char peach_pose_happy_stay[] = "P_S_5";
const char peach_pose_happy_walk[] = "P_W_5";
const char peach_pose_happy_dash[] = "P_R_5";
const char peach_pose_happy_talk[] = "P_T_5";
const char str_P_Z_6[] = "P_Z_6";
const char peach_pose_angry_stay[] = "P_S_6";
const char peach_pose_angry_walk[] = "P_W_6";
const char peach_pose_angry_dash[] = "P_R_6";
const char peach_pose_angry_talk[] = "P_T_6";
const char str_P_Z_7[] = "P_Z_7";
const char peach_pose_ordinary_stay[] = "P_S_7";
const char peach_pose_ordinary_walk[] = "P_W_7";
const char peach_pose_ordinary_dash[] = "P_R_7";
const char str_P_Z_8[] = "P_Z_8";
const char peach_pose_carry_stay[] = "P_S_8";
const char peach_pose_carry_walk[] = "P_W_8";
const char peach_pose_carry_dash[] = "P_R_8";
// 4 zero bytes after the last pose string in the original's .sdata2; kept so the file links.
const char gap_09_804203B4_sdata2[4] = {0, 0, 0, 0};



void peachSetStsNormal(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->peachFlags & PEACH_FLAG_TRANSFORMED) {
        mp = marioGetPtr();
        if (mp->characterId != 1) {
            marioSetCharMode(1);
        }
        if (mp->peachFlags & PEACH_FLAG_TRANSFORMED) {
            mp->peachFlags &= ~PEACH_FLAG_TRANSFORMED;
            if (mp->paperAnimGroupId[0] >= 0) {
                animPoseRelease(mp->paperAnimGroupId[0]);
            }
            mp->paperAnimGroupId[0] = animPoseEntry("c_peach", 2);
            npcSetMarioAutoTalkPose(peach_pose_stay, peach_pose_talk);
            mp->wPlayerCollisionBox.y = MATCH_F32(float_47_80420284);
            mp->wPlayerCollisionBox.x = MATCH_F32(float_20_80420288);
        }
    } else {
        mp->peachFlags &= ~PEACH_FLAG_STS_MASK;
        peachChgPose(peach_pose_stay);
    }
}

void peachSetStsAftershower(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~PEACH_FLAG_STS_MASK;
    mp->peachFlags |= PEACH_FLAG_AFTERSHOWER;
    peachChgPose(peach_pose_stay);
}

void peachSetStsInvisible(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~PEACH_FLAG_STS_MASK;
    mp->peachFlags |= PEACH_FLAG_INVISIBLE;
    peachChgPose(peach_pose_stay);
}

void peachSetStsHappy(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~PEACH_FLAG_STS_MASK;
    mp->peachFlags |= PEACH_FLAG_HAPPY;
    peachChgPose(peach_pose_stay);
}

void peachSetStsUneasy(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~PEACH_FLAG_STS_MASK;
    mp->peachFlags |= PEACH_FLAG_UNEASY;
    peachChgPose(peach_pose_stay);
}

void peachSetStsAngry(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~PEACH_FLAG_STS_MASK;
    mp->peachFlags |= PEACH_FLAG_ANGRY;
    peachChgPose(peach_pose_stay);
}

void peachSetStsOrdinary(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~PEACH_FLAG_STS_MASK;
    mp->peachFlags |= PEACH_FLAG_ORDINARY;
    peachChgPose(peach_pose_stay);
}

void peachSetStsCarry(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~PEACH_FLAG_STS_MASK;
    mp->peachFlags |= PEACH_FLAG_CARRY;
    peachChgPose(peach_pose_stay);
}

void peachChgPose(const char* pose) {
    MarioWork* mp = marioGetPtr();
    if (mp->dispFlags & 0x2) {
        return;
    }
    if (mp->peachFlags & PEACH_FLAG_TRANSFORMED) {
        if (strcmp(pose, str_P_Z_1) == 0) {
            pose = str_Z_1;
        } else if (strcmp(pose, peach_pose_stay) == 0) {
            pose = peach_pose_transformed_stay;
        } else if (strcmp(pose, peach_pose_walk) == 0) {
            pose = peach_pose_transformed_walk;
        } else if (strcmp(pose, peach_pose_dash) == 0) {
            pose = peach_pose_transformed_dash;
        } else if (strcmp(pose, peach_pose_talk) == 0) {
            pose = peach_pose_transformed_talk;
        }
    } else if (mp->peachFlags & PEACH_FLAG_AFTERSHOWER) {
        if (strcmp(pose, str_P_Z_1) == 0) {
            pose = str_P_Z_2;
        } else if (strcmp(pose, peach_pose_stay) == 0) {
            pose = peach_pose_aftershower_stay;
        } else if (strcmp(pose, peach_pose_walk) == 0) {
            pose = peach_pose_aftershower_walk;
        } else if (strcmp(pose, peach_pose_dash) == 0) {
            pose = peach_pose_aftershower_dash;
        } else if (strcmp(pose, peach_pose_talk) == 0) {
            pose = peach_pose_aftershower_talk;
        }
    } else if (mp->peachFlags & PEACH_FLAG_INVISIBLE) {
        if (strcmp(pose, str_P_Z_1) == 0) {
            pose = str_P_Z_3;
        } else if (strcmp(pose, peach_pose_stay) == 0) {
            pose = peach_pose_invisible_stay;
        } else if (strcmp(pose, peach_pose_walk) == 0) {
            pose = peach_pose_invisible_walk;
        } else if (strcmp(pose, peach_pose_dash) == 0) {
            pose = peach_pose_invisible_dash;
        } else if (strcmp(pose, peach_pose_talk) == 0) {
            pose = peach_pose_invisible_talk;
        }
    } else if (mp->peachFlags & PEACH_FLAG_UNEASY) {
        if (strcmp(pose, str_P_Z_1) == 0) {
            pose = str_P_Z_4;
        } else if (strcmp(pose, peach_pose_stay) == 0) {
            pose = peach_pose_uneasy_stay;
        } else if (strcmp(pose, peach_pose_walk) == 0) {
            pose = peach_pose_uneasy_walk;
        } else if (strcmp(pose, peach_pose_dash) == 0) {
            pose = peach_pose_uneasy_dash;
        } else if (strcmp(pose, peach_pose_talk) == 0) {
            pose = peach_pose_uneasy_talk;
        }
    } else if (mp->peachFlags & PEACH_FLAG_HAPPY) {
        if (strcmp(pose, str_P_Z_1) == 0) {
            pose = str_P_Z_5;
        } else if (strcmp(pose, peach_pose_stay) == 0) {
            pose = peach_pose_happy_stay;
        } else if (strcmp(pose, peach_pose_walk) == 0) {
            pose = peach_pose_happy_walk;
        } else if (strcmp(pose, peach_pose_dash) == 0) {
            pose = peach_pose_happy_dash;
        } else if (strcmp(pose, peach_pose_talk) == 0) {
            pose = peach_pose_happy_talk;
        }
    } else if (mp->peachFlags & PEACH_FLAG_ANGRY) {
        if (strcmp(pose, str_P_Z_1) == 0) {
            pose = str_P_Z_6;
        } else if (strcmp(pose, peach_pose_stay) == 0) {
            pose = peach_pose_angry_stay;
        } else if (strcmp(pose, peach_pose_walk) == 0) {
            pose = peach_pose_angry_walk;
        } else if (strcmp(pose, peach_pose_dash) == 0) {
            pose = peach_pose_angry_dash;
        } else if (strcmp(pose, peach_pose_talk) == 0) {
            pose = peach_pose_angry_talk;
        }
    } else if (mp->peachFlags & PEACH_FLAG_ORDINARY) {
        if (strcmp(pose, str_P_Z_1) == 0) {
            pose = str_P_Z_7;
        } else if (strcmp(pose, peach_pose_stay) == 0) {
            pose = peach_pose_ordinary_stay;
        } else if (strcmp(pose, peach_pose_walk) == 0) {
            pose = peach_pose_ordinary_walk;
        } else if (strcmp(pose, peach_pose_dash) == 0) {
            pose = peach_pose_ordinary_dash;
        } else if (strcmp(pose, peach_pose_talk) == 0) {
            pose = "P_T_13";
        }
    } else if (mp->peachFlags & PEACH_FLAG_CARRY) {
        if (strcmp(pose, str_P_Z_1) == 0) {
            pose = str_P_Z_8;
        } else if (strcmp(pose, peach_pose_stay) == 0) {
            pose = peach_pose_carry_stay;
        } else if (strcmp(pose, peach_pose_walk) == 0) {
            pose = peach_pose_carry_walk;
        } else if (strcmp(pose, peach_pose_dash) == 0) {
            pose = peach_pose_carry_dash;
        } else if (strcmp(pose, peach_pose_talk) == 0) {
            pose = "P_T_14\0\0\0\0\0";
        }
    }
    if (mp->animName == NULL || strcmp(mp->animName, pose) != 0) {
        mp->animName = pose;
        mp->poseTime = 0;
        mp->dispFlags &= ~0x30000000;
        mp->trigFlags |= 0x1000;
    }
}

void peachTransformOn(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 1) {
        marioSetCharMode(1);
    }
    if (!(mp->peachFlags & PEACH_FLAG_TRANSFORMED)) {
        mp->peachFlags |= PEACH_FLAG_TRANSFORMED;
        if (mp->paperAnimGroupId[0] >= 0) {
            animPoseRelease(mp->paperAnimGroupId[0]);
        }
        mp->paperAnimGroupId[0] = animPoseEntry("c_zako_n", 2);
        npcSetMarioAutoTalkPose(peach_pose_transformed_stay, peach_pose_transformed_talk);
        {
            f32 f30 = MATCH_F32(float_30_80420294);
            mp->wPlayerCollisionBox.y = f30;
            mp->wPlayerCollisionBox.x = f30;
        }
    }
}

void peachTransformOff(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 1) {
        marioSetCharMode(1);
    }
    if (mp->peachFlags & PEACH_FLAG_TRANSFORMED) {
        mp->peachFlags &= ~PEACH_FLAG_TRANSFORMED;
        if (mp->paperAnimGroupId[0] >= 0) {
            animPoseRelease(mp->paperAnimGroupId[0]);
        }
        mp->paperAnimGroupId[0] = animPoseEntry("c_peach", 2);
        npcSetMarioAutoTalkPose(peach_pose_stay, peach_pose_talk);
        mp->wPlayerCollisionBox.y = MATCH_F32(float_47_80420284);
        mp->wPlayerCollisionBox.x = MATCH_F32(float_20_80420288);
    }
}

void peachPreInit(void) {
}

s32 peachMain(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->currMotionId == MARIO_MOTION_JUMP_3) {
        mp->prevPosition = mp->position;
        {
            f32 zero = *(const f32*)&float_0_80420250;
            mp->riderDelta.x = zero;
            mp->riderDelta.y = zero;
            mp->riderDelta.z = zero;
        }
        marioBottomless();
        marioPreDisp();
        marioBalloonMain();
        mario_kemuri_80130474();
        partyMain();
        marioCamMain();
        mp->frameCount += 1;
        return 1;
    }
    if (mp->flags & 0x40000) {
        marioMove();
        marioChkOverhead();
        if (mp->airTimer > 0x64) {
            marioChgMot(0xA);
            marioSetFallPara();
        }
    } else if (mp->flags & 0x10000) {
        marioJump();
        marioMove();
        marioChkOverhead();
        if (mp->airTimer > 5) {
            marioChkGnd2();
        }
    } else if (mp->flags & 0x20000) {
        marioMove();
        marioFall();
        marioChkOverhead();
        if (mp->currMotionId != 0x10 && mp->currMotionId != 0x11 && mp->airTimer > 5) {
            marioChkGnd2();
        }
    } else {
        marioMove();
        marioChkOverhead();
        marioChkGnd();
    }
    if (mp->flags & 0x20) {
        mp->forceMoveTimer--;
        if (mp->forceMoveTimer <= 0) {
            mp->forceMoveTimer = 0;
            mp->flags &= ~0x20;
        }
    }
    marioBoots();
    marioBottomless();
    marioPreDisp();
    marioBalloonMain();
    mario_kemuri_80130474();
    partyMain();
    marioCamMain();
    {
        f32 zero = *(const f32*)&float_0_80420250;
        mp->frameCount += 1;
        mp->prevPosition = mp->position;
        mp->riderDelta.x = zero;
        mp->riderDelta.y = zero;
        mp->riderDelta.z = zero;
    }
    return 1;
}

void peach_stay(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->trigFlags & 1) {
        mp->trigFlags &= ~1;
        mp->flags &= ~0xF0000;
        mp->airTimer = 0;
        mp->multiTimer = 0;
        mp->currSubMotionId = 0;
        peachChgPose(peach_pose_stay);
        if (!(mp->flags & 0x20)) {
            mp->baseSpeed = MATCH_F32(float_0_80420250);
        }
    }
    if (mp->controlStickSensitivity > MATCH_F32(float_0_80420250)) {
        marioChgMot(1);
    }
    if ((mp->flags & 0x8) || (mp->dispFlags & 0x40000000)) {
        return;
    }
    if (mp->peachFlags & (PEACH_FLAG_TRANSFORMED | PEACH_FLAG_STS_MASK)) {
        return;
    }
    switch (mp->currSubMotionId) {
        case 0:
            if ((++mp->multiTimer) >= sysMsec2Frame(50000)) {
                marioChgPose(str_P_U_1);
                mp->multiTimer = 0x50;
                mp->currSubMotionId = 1;
                mp->sfxIds[0] = psndSFXOn_3D((const char*)0x101, &mp->position);
                marioAdjustMoveDir();
            }
            break;
        case 1:
            if ((--mp->multiTimer) < 0) {
                marioChgPose(peach_pose_stay);
                mp->multiTimer = 0x64;
                mp->currSubMotionId = 2;
            }
            break;
        case 2:
            if ((--mp->multiTimer) < 0) {
                marioChgPose(str_P_N_1);
                mp->multiTimer = 0;
                mp->currSubMotionId = 0x14;
                mp->sleepTimer = 0;
            }
            break;
        case 0x14:
            if ((++mp->sleepTimer) >= sysMsec2Frame(0x2710)) {
                mp->sleepTimer = sysMsec2Frame(0x2710);
            }
            if ((++mp->multiTimer) >= 0x64) {
                mp->multiTimer = 0;
            }
            if (mp->flags & 0x8) {
                marioChgPose(peach_pose_stay);
                mp->currSubMotionId = 0;
            }
            break;
    }
}

void peach_walk(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->trigFlags & 1) {
        mp->trigFlags &= ~1;
        mp->flags &= ~0xF0000;
        if (mp->forceMoveTimer == 0) {
            mp->baseSpeed = marioGetWalkSpd();
        }
        peachChgPose(peach_pose_walk);
        mp->currSubMotionId = 0;
        mp->multiTimer = 0;
    }
    if (mp->forceMoveTimer == 0) {
        mp->baseSpeed = marioGetWalkSpd();
    }
    {
        s8 d1 = mp->wStickDir1;
        s8 d2 = mp->wStickDir2;
        if (d1 * d1 + d2 * d2 > 0xbd1) {
            marioChgMot(2);
        } else {
            mp->directionView = mp->controlStickAngle;
            {
                MarioWork* mp2 = marioGetPtr();
                u32 snd;
                u32 snd2;
                u32 pf = mp2->peachFlags;
                if (!(pf & PEACH_FLAG_TRANSFORMED)) {
                    if ((pf & PEACH_FLAG_INVISIBLE) && MATCH_F32(float_0_80420250) == mp2->scale.y) {
                        snd = 0x80c;
                        snd2 = 0x80b;
                    } else {
                        snd = 0x7c4;
                        snd2 = 0x7c3;
                    }
                } else {
                    snd = 0x7e9;
                    snd2 = 0x7e8;
                }
                {
                    s32 mt = mp2->multiTimer;
                    if (mt % 40 == 0) {
                        psndSFXOn_3D((const char*)snd, &mp2->position);
                    } else if (mt % 20 == 0) {
                        psndSFXOn_3D((const char*)snd2, &mp2->position);
                    }
                }
                mp2->multiTimer++;
            }
            if (mp->controlStickSensitivity == MATCH_F32(float_0_80420250)) {
                marioChgMot(0);
            }
        }
    }
}

void peach_dash(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->trigFlags & 1) {
        mp->trigFlags &= ~1;
        mp->flags &= ~0xF0000;
        if (mp->forceMoveTimer == 0) {
            mp->baseSpeed = marioGetDashSpd();
        }
        peachChgPose(peach_pose_dash);
        mp->currSubMotionId = 0;
        mp->multiTimer = 0;
    }
    if (mp->forceMoveTimer == 0) {
        mp->baseSpeed = marioGetDashSpd();
    }
    if (mp->flags & 0x20) {
        MarioWork* mp2 = marioGetPtr();
        u32 snd2;
        u32 snd;
        u32 pf = mp2->peachFlags;
        if (!(pf & PEACH_FLAG_TRANSFORMED)) {
            if ((pf & PEACH_FLAG_INVISIBLE) && MATCH_F32(float_0_80420250) == mp2->scale.y) {
                snd = 0x80c;
                snd2 = 0x80b;
            } else {
                snd = 0x7c4;
                snd2 = 0x7c3;
            }
        } else {
            snd = 0x7e9;
            snd2 = 0x7e8;
        }
        {
            s32 mt = mp2->multiTimer;
            if (mt % 20 == 0) {
                psndSFXOn_3D((const char*)snd, &mp2->position);
            } else if (mt % 10 == 0) {
                psndSFXOn_3D((const char*)snd2, &mp2->position);
            }
        }
        mp2->multiTimer++;
    } else {
        s8 d1 = mp->wStickDir1;
        s8 d2 = mp->wStickDir2;
        if (d1 * d1 + d2 * d2 <= 0xbd1) {
            marioChgMot(1);
        } else {
            mp->directionView = mp->controlStickAngle;
            {
                MarioWork* mp2 = marioGetPtr();
                u32 snd2;
                u32 snd;
                u32 pf = mp2->peachFlags;
                if (!(pf & PEACH_FLAG_TRANSFORMED)) {
                    if ((pf & PEACH_FLAG_INVISIBLE) && MATCH_F32(float_0_80420250) == mp2->scale.y) {
                        snd = 0x80c;
                        snd2 = 0x80b;
                    } else {
                        snd = 0x7c4;
                        snd2 = 0x7c3;
                    }
                } else {
                    snd = 0x7e9;
                    snd2 = 0x7e8;
                }
                {
                    s32 mt = mp2->multiTimer;
                    if (mt % 20 == 0) {
                        psndSFXOn_3D((const char*)snd, &mp2->position);
                    } else if (mt % 10 == 0) {
                        psndSFXOn_3D((const char*)snd2, &mp2->position);
                    }
                }
                mp2->multiTimer++;
            }
            if (mp->controlStickSensitivity == MATCH_F32(float_0_80420250)) {
                marioChgMot(0);
            }
        }
    }
}

void peach_talk(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->trigFlags & 1) {
        f32 t;
        mp->trigFlags &= ~1;
        t = *(const f32*)&float_0_80420250;
        mp->multiTimer = 0;
        mp->airTimer = 0;
        mp->currSubMotionId = 0;
        mp->baseSpeed = t;
        marioAdjustMoveDir();
        peachChgPose(peach_pose_talk);
    }
}

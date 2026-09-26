#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_sbr.h"
#include "mot/mot_walk.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"
#include "driver/animdrv.h"
#include <string.h>

// locals for symbols with no home header (see REPORT.md)
void peachChgPose(const char* name);
f32 marioGetWalkSpd(void);
f32 marioChkOverhead(void);
void marioSetFallPara(void);
void marioChkGnd2(void);
void marioChkGnd(void);
void marioBoots(void);
void marioBottomless(void);
void marioCamMain(void);
void marioBalloonMain(void);
void mario_kemuri_80130474(void);
void partyMain(void);
void marioJump(void);
void marioFall(void);
void npcSetMarioAutoTalkPose(const char* stayPose, const char* talkPose);

// .rodata
const char str_802FC8F8[] = "c_peach";
const char str_802FC900[] = "c_zako_n\0\0\0";
const char str_802FC90C[] = "P_T_13\0";
const char str_802FC914[] = "P_T_14\0\0\0\0\0";

// .sdata2
const f32 float_0_80420250 = 0.0f;
const char str_80420254[] = "P_T_1";
const char str_8042025C[] = "P_R_1";
const char str_80420264[] = "P_W_1";
const char str_8042026C[] = "P_S_1";
const char str_80420274[] = "P_U_1";
const char str_8042027C[] = "P_N_1";
const f32 float_47_80420284 = 47.0f;
const f32 float_20_80420288 = 20.0f;
const char str_8042028C[] = "S_1";
const char str_80420290[] = "T_1";
const f32 float_30_80420294 = 30.0f;
const char str_80420298[] = "P_Z_1";
const char str_804202A0[] = "Z_1";
const char str_804202A4[] = "W_1";
const char str_804202A8[] = "R_1";
const char str_804202AC[] = "P_Z_2";
const char str_804202B4[] = "P_S_2";
const char str_804202BC[] = "P_W_2";
const char str_804202C4[] = "P_R_2";
const char str_804202CC[] = "P_T_2";
const char str_804202D4[] = "P_Z_3";
const char str_804202DC[] = "P_S_3";
const char str_804202E4[] = "P_W_3";
const char str_804202EC[] = "P_R_3";
const char str_804202F4[] = "P_T_3";
const char str_804202FC[] = "P_Z_4";
const char str_80420304[] = "P_S_4";
const char str_8042030C[] = "P_W_4";
const char str_80420314[] = "P_R_4";
const char str_8042031C[] = "P_T_4";
const char str_80420324[] = "P_Z_5";
const char str_8042032C[] = "P_S_5";
const char str_80420334[] = "P_W_5";
const char str_8042033C[] = "P_R_5";
const char str_80420344[] = "P_T_5";
const char str_8042034C[] = "P_Z_6";
const char str_80420354[] = "P_S_6";
const char str_8042035C[] = "P_W_6";
const char str_80420364[] = "P_R_6";
const char str_8042036C[] = "P_T_6";
const char str_80420374[] = "P_Z_7";
const char str_8042037C[] = "P_S_7";
const char str_80420384[] = "P_W_7";
const char str_8042038C[] = "P_R_7";
const char str_80420394[] = "P_Z_8";
const char str_8042039C[] = "P_S_8";
const char str_804203A4[] = "P_W_8";
const char str_804203AC[] = "P_R_8";
const char gap_09_804203B4_sdata2[4] = {0, 0, 0, 0};



#pragma opt_propagation off // keep named .sdata2 float refs (no duplicate literals) + exact scheduling
void peachSetStsNormal(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->peachFlags & 0x2000) {
        mp = marioGetPtr();
        if (mp->characterId != 1) {
            marioSetCharMode(1);
        }
        if (mp->peachFlags & 0x2000) {
            mp->peachFlags &= ~0x2000;
            if (mp->paperAnimGroupId[0] >= 0) {
                animPoseRelease(mp->paperAnimGroupId[0]);
            }
            mp->paperAnimGroupId[0] = animPoseEntry(str_802FC8F8, 2);
            npcSetMarioAutoTalkPose(str_8042026C, str_80420254);
            mp->wPlayerCollisionBox.y = *(volatile const f32*)&float_47_80420284;
            mp->wPlayerCollisionBox.x = *(volatile const f32*)&float_20_80420288;
        }
    } else {
        mp->peachFlags &= ~0x7F0;
        peachChgPose(str_8042026C);
    }
}
#pragma opt_propagation on

void peachSetStsAftershower(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~0x7F0;
    mp->peachFlags |= 0x10;
    peachChgPose(str_8042026C);
}

void peachSetStsInvisible(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~0x7F0;
    mp->peachFlags |= 0x20;
    peachChgPose(str_8042026C);
}

void peachSetStsHappy(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~0x7F0;
    mp->peachFlags |= 0x40;
    peachChgPose(str_8042026C);
}

void peachSetStsUneasy(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~0x7F0;
    mp->peachFlags |= 0x80;
    peachChgPose(str_8042026C);
}

void peachSetStsAngry(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~0x7F0;
    mp->peachFlags |= 0x100;
    peachChgPose(str_8042026C);
}

void peachSetStsOrdinary(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~0x7F0;
    mp->peachFlags |= 0x200;
    peachChgPose(str_8042026C);
}

void peachSetStsCarry(void) {
    MarioWork* mp = marioGetPtr();
    mp->peachFlags &= ~0x7F0;
    mp->peachFlags |= 0x400;
    peachChgPose(str_8042026C);
}

void peachChgPose(const char* pose) {
    MarioWork* mp = marioGetPtr();
    if (mp->dispFlags & 0x2) {
        return;
    }
    if (mp->peachFlags & 0x2000) {
        if (strcmp(pose, str_80420298) == 0) {
            pose = str_804202A0;
        } else if (strcmp(pose, str_8042026C) == 0) {
            pose = str_8042028C;
        } else if (strcmp(pose, str_80420264) == 0) {
            pose = str_804202A4;
        } else if (strcmp(pose, str_8042025C) == 0) {
            pose = str_804202A8;
        } else if (strcmp(pose, str_80420254) == 0) {
            pose = str_80420290;
        }
    } else if (mp->peachFlags & 0x10) {
        if (strcmp(pose, str_80420298) == 0) {
            pose = str_804202AC;
        } else if (strcmp(pose, str_8042026C) == 0) {
            pose = str_804202B4;
        } else if (strcmp(pose, str_80420264) == 0) {
            pose = str_804202BC;
        } else if (strcmp(pose, str_8042025C) == 0) {
            pose = str_804202C4;
        } else if (strcmp(pose, str_80420254) == 0) {
            pose = str_804202CC;
        }
    } else if (mp->peachFlags & 0x20) {
        if (strcmp(pose, str_80420298) == 0) {
            pose = str_804202D4;
        } else if (strcmp(pose, str_8042026C) == 0) {
            pose = str_804202DC;
        } else if (strcmp(pose, str_80420264) == 0) {
            pose = str_804202E4;
        } else if (strcmp(pose, str_8042025C) == 0) {
            pose = str_804202EC;
        } else if (strcmp(pose, str_80420254) == 0) {
            pose = str_804202F4;
        }
    } else if (mp->peachFlags & 0x80) {
        if (strcmp(pose, str_80420298) == 0) {
            pose = str_804202FC;
        } else if (strcmp(pose, str_8042026C) == 0) {
            pose = str_80420304;
        } else if (strcmp(pose, str_80420264) == 0) {
            pose = str_8042030C;
        } else if (strcmp(pose, str_8042025C) == 0) {
            pose = str_80420314;
        } else if (strcmp(pose, str_80420254) == 0) {
            pose = str_8042031C;
        }
    } else if (mp->peachFlags & 0x40) {
        if (strcmp(pose, str_80420298) == 0) {
            pose = str_80420324;
        } else if (strcmp(pose, str_8042026C) == 0) {
            pose = str_8042032C;
        } else if (strcmp(pose, str_80420264) == 0) {
            pose = str_80420334;
        } else if (strcmp(pose, str_8042025C) == 0) {
            pose = str_8042033C;
        } else if (strcmp(pose, str_80420254) == 0) {
            pose = str_80420344;
        }
    } else if (mp->peachFlags & 0x100) {
        if (strcmp(pose, str_80420298) == 0) {
            pose = str_8042034C;
        } else if (strcmp(pose, str_8042026C) == 0) {
            pose = str_80420354;
        } else if (strcmp(pose, str_80420264) == 0) {
            pose = str_8042035C;
        } else if (strcmp(pose, str_8042025C) == 0) {
            pose = str_80420364;
        } else if (strcmp(pose, str_80420254) == 0) {
            pose = str_8042036C;
        }
    } else if (mp->peachFlags & 0x200) {
        if (strcmp(pose, str_80420298) == 0) {
            pose = str_80420374;
        } else if (strcmp(pose, str_8042026C) == 0) {
            pose = str_8042037C;
        } else if (strcmp(pose, str_80420264) == 0) {
            pose = str_80420384;
        } else if (strcmp(pose, str_8042025C) == 0) {
            pose = str_8042038C;
        } else if (strcmp(pose, str_80420254) == 0) {
            pose = str_802FC90C;
        }
    } else if (mp->peachFlags & 0x400) {
        if (strcmp(pose, str_80420298) == 0) {
            pose = str_80420394;
        } else if (strcmp(pose, str_8042026C) == 0) {
            pose = str_8042039C;
        } else if (strcmp(pose, str_80420264) == 0) {
            pose = str_804203A4;
        } else if (strcmp(pose, str_8042025C) == 0) {
            pose = str_804203AC;
        } else if (strcmp(pose, str_80420254) == 0) {
            pose = str_802FC914;
        }
    }
    if (mp->animName == NULL || strcmp(mp->animName, pose) != 0) {
        mp->animName = pose;
        mp->poseTime = 0;
        mp->dispFlags &= ~0x30000000;
        mp->trigFlags |= 0x1000;
    }
}

#pragma opt_propagation off // keep named .sdata2 float refs (no duplicate literals) + exact scheduling
void peachTransformOn(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 1) {
        marioSetCharMode(1);
    }
    if (!(mp->peachFlags & 0x2000)) {
        mp->peachFlags |= 0x2000;
        if (mp->paperAnimGroupId[0] >= 0) {
            animPoseRelease(mp->paperAnimGroupId[0]);
        }
        mp->paperAnimGroupId[0] = animPoseEntry(str_802FC900, 2);
        npcSetMarioAutoTalkPose(str_8042028C, str_80420290);
        {
            f32 f30 = *(volatile const f32*)&float_30_80420294;
            mp->wPlayerCollisionBox.y = f30;
            mp->wPlayerCollisionBox.x = f30;
        }
    }
}
#pragma opt_propagation on

#pragma opt_propagation off // keep named .sdata2 float refs (no duplicate literals) + exact scheduling
void peachTransformOff(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 1) {
        marioSetCharMode(1);
    }
    if (mp->peachFlags & 0x2000) {
        mp->peachFlags &= ~0x2000;
        if (mp->paperAnimGroupId[0] >= 0) {
            animPoseRelease(mp->paperAnimGroupId[0]);
        }
        mp->paperAnimGroupId[0] = animPoseEntry(str_802FC8F8, 2);
        npcSetMarioAutoTalkPose(str_8042026C, str_80420254);
        mp->wPlayerCollisionBox.y = *(volatile const f32*)&float_47_80420284;
        mp->wPlayerCollisionBox.x = *(volatile const f32*)&float_20_80420288;
    }
}
#pragma opt_propagation on

void peachPreInit(void) {
}

#pragma opt_propagation off // keep named .sdata2 float refs (no duplicate literals) + exact scheduling
s32 peachMain(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->currMotionId == MARIO_MOTION_JUMP_3) {
        *(Vec*)(mp->unk104 + 0xC) = mp->position;
        {
            f32 zero = *(const f32*)&float_0_80420250;
            *(f32*)(mp->unk170 + 4) = zero;
            *(f32*)(mp->unk170 + 8) = zero;
            *(f32*)(mp->unk170 + 0xC) = zero;
        }
        marioBottomless();
        marioPreDisp();
        marioBalloonMain();
        mario_kemuri_80130474();
        partyMain();
        marioCamMain();
        *(s32*)(mp->field_0x32 + 2) += 1;
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
        *(s32*)(mp->field_0x32 + 2) += 1;
        *(Vec*)(mp->unk104 + 0xC) = mp->position;
        *(f32*)(mp->unk170 + 4) = zero;
        *(f32*)(mp->unk170 + 8) = zero;
        *(f32*)(mp->unk170 + 0xC) = zero;
    }
    return 1;
}
#pragma opt_propagation on

#pragma opt_propagation off // keep named .sdata2 float refs (no duplicate literals) + exact scheduling
void peach_stay(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->trigFlags & 1) {
        mp->trigFlags &= ~1;
        mp->flags &= ~0xF0000;
        mp->airTimer = 0;
        mp->multiTimer = 0;
        mp->currSubMotionId = 0;
        peachChgPose(str_8042026C);
        if (!(mp->flags & 0x20)) {
            mp->baseSpeed = *(volatile const f32*)&float_0_80420250;
        }
    }
    if (mp->controlStickSensitivity > *(volatile const f32*)&float_0_80420250) {
        marioChgMot(1);
    }
    if ((mp->flags & 0x8) || (mp->dispFlags & 0x40000000)) {
        return;
    }
    if (mp->peachFlags & 0x27f0) {
        return;
    }
    switch (mp->currSubMotionId) {
        case 0:
            if ((s32)(++mp->multiTimer) >= sysMsec2Frame(50000)) {
                marioChgPose(str_80420274);
                mp->multiTimer = 0x50;
                mp->currSubMotionId = 1;
                *(s32*)(mp->field_0x250 + 0x28) = psndSFXOn_3D((const char*)0x101, &mp->position);
                marioAdjustMoveDir();
            }
            break;
        case 1:
            if ((s32)(--mp->multiTimer) < 0) {
                marioChgPose(str_8042026C);
                mp->multiTimer = 0x64;
                mp->currSubMotionId = 2;
            }
            break;
        case 2:
            if ((s32)(--mp->multiTimer) < 0) {
                marioChgPose(str_8042027C);
                mp->multiTimer = 0;
                mp->currSubMotionId = 0x14;
                *(s32*)(mp->field_0x2AC + 8) = 0;
            }
            break;
        case 0x14:
            if ((s32)(++*(s32*)(mp->field_0x2AC + 8)) >= sysMsec2Frame(0x2710)) {
                *(s32*)(mp->field_0x2AC + 8) = sysMsec2Frame(0x2710);
            }
            if ((s32)(++mp->multiTimer) >= 0x64) {
                mp->multiTimer = 0;
            }
            if (mp->flags & 0x8) {
                marioChgPose(str_8042026C);
                mp->currSubMotionId = 0;
            }
            break;
    }
}
#pragma opt_propagation on

void peach_walk(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->trigFlags & 1) {
        mp->trigFlags &= ~1;
        mp->flags &= ~0xF0000;
        if (mp->forceMoveTimer == 0) {
            mp->baseSpeed = marioGetWalkSpd();
        }
        peachChgPose(str_80420264);
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
                if (!(pf & 0x2000)) {
                    if ((pf & 0x20) && *(volatile const f32*)&float_0_80420250 == mp2->scale.y) {
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
                    s32 mt = (s32)mp2->multiTimer;
                    if (mt % 40 == 0) {
                        psndSFXOn_3D((const char*)snd, &mp2->position);
                    } else if (mt % 20 == 0) {
                        psndSFXOn_3D((const char*)snd2, &mp2->position);
                    }
                }
                mp2->multiTimer++;
            }
            if (mp->controlStickSensitivity == *(volatile const f32*)&float_0_80420250) {
                marioChgMot(0);
            }
        }
    }
}

#pragma opt_propagation off // keep named .sdata2 float refs (no duplicate literals) + exact scheduling
void peach_dash(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->trigFlags & 1) {
        mp->trigFlags &= ~1;
        mp->flags &= ~0xF0000;
        if (mp->forceMoveTimer == 0) {
            mp->baseSpeed = marioGetDashSpd();
        }
        peachChgPose(str_8042025C);
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
        if (!(pf & 0x2000)) {
            if ((pf & 0x20) && *(volatile const f32*)&float_0_80420250 == mp2->scale.y) {
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
            s32 mt = (s32)mp2->multiTimer;
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
                if (!(pf & 0x2000)) {
                    if ((pf & 0x20) && *(volatile const f32*)&float_0_80420250 == mp2->scale.y) {
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
                    s32 mt = (s32)mp2->multiTimer;
                    if (mt % 20 == 0) {
                        psndSFXOn_3D((const char*)snd, &mp2->position);
                    } else if (mt % 10 == 0) {
                        psndSFXOn_3D((const char*)snd2, &mp2->position);
                    }
                }
                mp2->multiTimer++;
            }
            if (mp->controlStickSensitivity == *(volatile const f32*)&float_0_80420250) {
                marioChgMot(0);
            }
        }
    }
}
#pragma opt_propagation on

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
        peachChgPose(str_80420254);
    }
}

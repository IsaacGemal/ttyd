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

// .rodata
const char str_802FE218[] = "SFX_KUPPA_SWIM1";
const char str_802FE228[] = "KPA_W_1";
const char str_802FE230[] = "KPA2_W_2";
const char str_802FE23C[] = "KPA_J_1A";
const char str_802FE248[] = "KPA2_J_1A";
const char str_802FE254[] = "SFX_KUPPA_WATER_UP1";
const char str_802FE268[] = "KPA2_Z_2";
const f64 double_to_int_802fe278 = 4503601774854144.0;
const char str_802FE280[] = "KPA_D_1";
const char str_802FE288[] = "KPA2_D_1";
const char str_802FE294[] = "KPA_Y_1";
const char str_802FE29C[] = "KPA2_Y_1";
const char str_802FE2A8[] = "KPA2_J_1C";
const char str_802FE2B4[] = "KPA_J_1B";
const char str_802FE2C0[] = "KPA2_J_1B";
const char str_802FE2CC[] = "M_J_1C";
const char str_802FE2D4[] = "KPA_R_1";
const char str_802FE2DC[] = "KPA2_R_1";
const f64 double_to_int_mask_802fe2e8 = 4503599627370496.0;
const char str_802FE2F0[] = "KPA2_W_1";
const char str_802FE2FC[] = "KPA_W_2";
const char str_802FE304[] = "KPA_S_1";
const char str_802FE30C[] = "KPA2_S_1";
const char str_802FE318[] = "KPA_S_2";

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

void quake_kpaHipAttack(void) {
    evtEntry(quake_kpaHipAttack_evt, 0, 0);
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

// NONMATCHING (94.58%): big function; the three periodic effMizutama blocks and the switch dispatch still differ, and the irand int->float conversion loads a folded @N double instead of double_to_int_802fe278. Structure/register allocation otherwise close.
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
        KPA(marioGetPtr())->flags |= 0x10;
    }
    if (MW_U16(mario, 0x248) & (1 << 9)) {
        kpaAttackStart2();
    }
    if (kpaFireAttackCheck() == 1) {
        mario->wStickDir1 = 0;
        mario->wStickDir2 = 0;
    } else if (kpaGetBodyStatus() == 2 && (MW_U16(mario, 0x248) & (1 << 8))) {
        if ((s32)mario->currSubMotionId >= 0 && (s32)mario->currSubMotionId <= 1) {
            if (mario->position.y < float_280_80420da4) {
                mario->currSubMotionId = 0;
            }
        } else if ((s32)mario->currSubMotionId >= 0x1E && (s32)mario->currSubMotionId <= 0x20) {
            mario->currSubMotionId = 0;
        }
    }
    kpaSwimMove();
    m2 = marioGetPtr();
    MW_S32(KPA(m2), 0x150) += 1;
    if ((s32)m2->currSubMotionId == 0x1E || (s32)m2->currSubMotionId == 0x1F) {
        if ((s32)MW_S32(KPA(m2), 0x150) > 0) {
            f32 angle;
            f32 dx;
            f32 dy;
            MW_S32(KPA(m2), 0x150) = 0;
            dx = (f32)(irand(0x28) - 0x14);
            dy = (f32)(irand(0x28) - 0x14);
            angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
            effMizutamaEntry(0, m2->position.x + dx,
                             m2->position.y + dy + float_0p6_80420da8 * m2->wPlayerCollisionBox.y,
                             m2->position.z, angle);
        }
    } else {
        s32 limit = 0x1E;
        if ((s32)m2->currSubMotionId >= 0 && (s32)m2->currSubMotionId < 0x14) {
            limit = 0xA;
        }
        if ((s32)MW_S32(KPA(m2), 0x150) > limit) {
            f32 angle;
            MW_S32(KPA(m2), 0x150) = 0;
            angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
            effMizutamaEntry(0, m2->position.x,
                             m2->position.y + m2->wPlayerCollisionBox.y / float_3_80420db8,
                             m2->position.z, angle);
        }
    }
    if ((s32)mario->currSubMotionId <= 0x16) {
        switch (mario->currSubMotionId) {
        case 0:
            if (mario->hitobjStandOn != NULL) {
                MW_F32(mario, 0x7C) = float_1_80420dbc;
                MW_F32(mario, 0x80) = float_0_80420da0;
                MW_F32(mario, 0x84) = float_0_80420da0;
                MW_F32(mario, 0x88) = float_neg0p0001_80420dc0;
            } else {
                MW_F32(mario, 0x7C) = float_1p52_80420dc4;
                MW_F32(mario, 0x80) = float_0p005_80420dc8;
                MW_F32(mario, 0x84) = float_0p006_80420dcc;
                MW_F32(mario, 0x88) = float_neg0p0008_80420dd0;
            }
            kpaClearHitobjRide();
            mario->currSubMotionId = 1;
            mario->flags &= ~0x00020000;
            mario->flags |= 0x00010000;
            psndSFXOn_3D(str_802FE218, &mario->position);
            if (kpaGetAttackMode() == 0) {
                kpaChgPose(str_802FE228, str_802FE230);
            }
            /* fallthrough */
        case 1:
            hit = kpaGetHitobjHead2();
            if (hit != NULL) {
                mario->currSubMotionId = 0x1E;
                MW_F32(mario, 0x7C) = float_0p02_80420dd4 * (float_neg2_80420dd8 - MW_F32(mario, 0x7C)) + MW_F32(mario, 0x7C);
                MW_F32(mario, 0x80) = float_0_80420da0;
                MW_F32(mario, 0x84) = float_0_80420da0;
                MW_F32(mario, 0x88) = float_0_80420da0;
                break;
            }
            f31 = marioMakeJumpPara();
            mario->position.y += f31;
            if (mario->position.y + mario->wPlayerCollisionBox.y < float_370_80420ddc) {
                if (f31 < float_0_80420da0) {
                    mario->currSubMotionId = 0x1E;
                }
                break;
            }
            if (f31 < float_0_80420da0) {
                mario->currSubMotionId = 0x28;
            }
            MW_F32(mario, 0x7C) = float_0p02_80420dd4 * (float_neg2_80420dd8 - MW_F32(mario, 0x7C)) + MW_F32(mario, 0x7C);
            MW_F32(mario, 0x80) = float_0_80420da0;
            MW_F32(mario, 0x84) = float_0_80420da0;
            MW_F32(mario, 0x88) = float_0_80420da0;
            break;
        case 10:
            kpaClearHitobjRide();
            MW_F32(mario, 0x7C) = float_2_80420de0;
            MW_F32(mario, 0x80) = float_neg0p03_80420de4;
            MW_F32(mario, 0x84) = float_0_80420da0;
            MW_F32(mario, 0x88) = float_0_80420da0;
            mario->currSubMotionId = 0xB;
            mario->flags &= ~0x00020000;
            mario->flags |= 0x00010000;
            if (kpaGetAttackMode() == 0) {
                kpaChgPose(str_802FE228, str_802FE230);
            }
            /* fallthrough */
        case 11:
            f31 = marioMakeJumpPara();
            mario->position.y += f31;
            if (kpaGetBodyStatus() == 1) {
                MW_F32(mario, 0x7C) = float_0p06_80420de8 * (float_neg2_80420dd8 - MW_F32(mario, 0x7C)) + MW_F32(mario, 0x7C);
                MW_F32(mario, 0x80) = float_0_80420da0;
                MW_F32(mario, 0x84) = float_0_80420da0;
                MW_F32(mario, 0x88) = float_0_80420da0;
                if (f31 < float_0_80420da0) {
                    mario->currSubMotionId = 0x28;
                }
            } else if (f31 < float_0_80420da0) {
                mario->currSubMotionId = 0x1E;
            }
            break;
        case 0x14:
            KPA(mario)->flags |= 0x100;
            mario->flags &= ~0x00020000;
            mario->flags |= 0x00010000;
            kpaChgPose(str_802FE23C, str_802FE248);
            psndSFXOn_3D(str_802FE254, &mario->position);
            MW_F32(mario, 0x7C) = float_5_80420dec;
            MW_F32(mario, 0x80) = float_neg0p13_80420df0;
            MW_F32(mario, 0x84) = float_0_80420da0;
            MW_F32(mario, 0x88) = float_neg0p001_80420df4;
            mario->currSubMotionId = 0x15;
            /* fallthrough */
        case 0x15:
            f31 = marioMakeJumpPara();
            mario->position.y += f31;
            if (f31 <= float_0p5_80420df8) {
                mario->flags &= ~0x000F0000;
                kpaSetFallPara();
                marioMakeJumpPara();
                marioChgMot(0xA);
                return;
            }
            break;
        default:
            break;
        }
    }
    switch (mario->currSubMotionId) {
    case 0x1E:
        if (KPA(mario)->flags & 0x100) {
            KPA(mario)->flags &= ~0x100;
            psndSFXOn_3D((char*)0x827, &mario->position);
        }
        if (kpaGetAttackMode() == 0) {
            kpaChgPose(str_802FE228, str_802FE268);
        }
        mario->flags &= ~0x00010000;
        mario->flags |= 0x00020000;
        if (MW_F32(mario, 0x7C) < float_0_80420da0) {
            mario->currSubMotionId = 0x1F;
        } else {
            MW_F32(mario, 0x7C) = float_neg0p08_80420dfc;
            MW_F32(mario, 0x80) = float_neg0p03_80420de4;
            MW_F32(mario, 0x84) = float_0p009_80420e00;
            MW_F32(mario, 0x88) = float_0_80420da0;
            mario->currSubMotionId = 0x20;
        }
        m2 = marioGetPtr();
        MW_S32(KPA(m2), 0x150) += 1;
        if ((s32)m2->currSubMotionId == 0x1E || (s32)m2->currSubMotionId == 0x1F) {
            if ((s32)MW_S32(KPA(m2), 0x150) > 0) {
                f32 angle;
                f32 dx;
                f32 dy;
                MW_S32(KPA(m2), 0x150) = 0;
                dx = (f32)(irand(0x28) - 0x14);
                dy = (f32)(irand(0x28) - 0x14);
                angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
                effMizutamaEntry(0, m2->position.x + dx,
                                 m2->position.y + dy + float_0p6_80420da8 * m2->wPlayerCollisionBox.y,
                                 m2->position.z, angle);
            }
        } else {
            s32 limit = 0x1E;
            if ((s32)m2->currSubMotionId >= 0 && (s32)m2->currSubMotionId < 0x14) {
                limit = 0xA;
            }
            if ((s32)MW_S32(KPA(m2), 0x150) > limit) {
                f32 angle;
                MW_S32(KPA(m2), 0x150) = 0;
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
            f32 factor = (float_2_80420de0 - MW_F32(mario, 0x7C)) * (f32)(kpaGetBodyStatus() == 2);
            f32 delta = (factor == float_0_80420da0) ? float_0p21_80420e08 : float_2p6_80420e04;
            MW_F32(mario, 0x7C) += delta;
            MW_F32(mario, 0x80) = float_0_80420da0;
            MW_F32(mario, 0x84) = float_0_80420da0;
            MW_F32(mario, 0x88) = float_0_80420da0;
            if (MW_F32(mario, 0x7C) >= float_0_80420da0) {
                MW_F32(mario, 0x7C) = float_neg0p08_80420dfc;
                MW_F32(mario, 0x80) = float_neg0p03_80420de4;
                MW_F32(mario, 0x84) = float_0p009_80420e00;
                MW_F32(mario, 0x88) = float_0_80420da0;
                mario->currSubMotionId = 0x20;
            }
        }
        if (kpaChkHitobjRide() != 0) {
            MW_F32(mario, 0x2C4) = float_0_80420da0;
            mario->baseSpeed = float_0_80420da0;
        }
        m2 = marioGetPtr();
        MW_S32(KPA(m2), 0x150) += 1;
        if ((s32)m2->currSubMotionId == 0x1E || (s32)m2->currSubMotionId == 0x1F) {
            if ((s32)MW_S32(KPA(m2), 0x150) > 0) {
                f32 angle;
                f32 dx;
                f32 dy;
                MW_S32(KPA(m2), 0x150) = 0;
                dx = (f32)(irand(0x28) - 0x14);
                dy = (f32)(irand(0x28) - 0x14);
                angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
                effMizutamaEntry(0, m2->position.x + dx,
                                 m2->position.y + dy + float_0p6_80420da8 * m2->wPlayerCollisionBox.y,
                                 m2->position.z, angle);
            }
        } else {
            s32 limit = 0x1E;
            if ((s32)m2->currSubMotionId >= 0 && (s32)m2->currSubMotionId < 0x14) {
                limit = 0xA;
            }
            if ((s32)MW_S32(KPA(m2), 0x150) > limit) {
                f32 angle;
                MW_S32(KPA(m2), 0x150) = 0;
                angle = (m2->directionView < float_180_80420db4) ? float_225_80420dac : float_135_80420db0;
                effMizutamaEntry(0, m2->position.x,
                                 m2->position.y + m2->wPlayerCollisionBox.y / float_3_80420db8,
                                 m2->position.z, angle);
            }
        }
        break;
    case 0x20:
        if (kpaChkHitobjRide() != 0) {
            MW_F32(mario, 0x2C4) = float_0_80420da0;
            mario->baseSpeed = float_0_80420da0;
        }
        break;
    case 0x28:
        if (kpaGetAttackMode() == 0) {
            kpaChgPose(str_802FE228, str_802FE268);
        }
        mario->flags &= ~0x00010000;
        mario->flags |= 0x00020000;
        MW_F32(mario, 0x7C) = float_neg0p283_80420e0c;
        MW_F32(mario, 0x80) = float_neg0p023_80420e10;
        MW_F32(mario, 0x84) = float_0p01_80420e14;
        MW_F32(mario, 0x88) = float_neg0p001_80420df4;
        mario->currSubMotionId = 0x29;
        mario->multiTimer = 0x18;
        /* fallthrough */
    case 0x29:
        if ((MW_U16(mario, 0x248) & (1 << 8)) && kpaFireAttackCheck() == 0) {
            mario->currSubMotionId = 0x14;
        } else if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x1E;
        }
        break;
    default:
        break;
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
        kpaChgPose(str_802FE280, str_802FE288);
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
        marioChgMot(0);
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
        marioChgMot(0);
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
        kpaChgPose(str_802FE294, str_802FE29C);
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
        marioChgMot(0);
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
        marioChgMot(0);
        mario->unk4C = 0;
    }
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

s32 kpaPowDown(void) {
    MarioWork* mario = marioGetPtr();
    kpaCancelAttackMode();
    if (kpaGetLevel() > 0) {
        KPA(mario)->level--;
        MW_S16(mario, 0x78) = sysMsec2Frame(3000);
        marioChgMot(0x27);
        return 1;
    }
    KPA(mario)->flags |= 0x10000000;
    return 0;
}

s32 kpaPowUp(void) {
    MarioWork* mario = marioGetPtr();
    if (kpaGetLevel() < 3) {
        KPA(mario)->level++;
        kpaCancelAttackMode();
        marioChgMot(0x26);
        return 1;
    }
    kpaMSetMutekiTimer();
    kpaMutekiOn();
    return 0;
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
        MW_F32(mario, 0x7C) = float_0_80420da0;
        MW_F32(mario, 0x80) = float_0_80420da0;
        MW_F32(mario, 0x84) = float_0_80420da0;
        MW_F32(mario, 0x88) = float_0_80420da0;
        mario->baseSpeed = float_0_80420da0;
        marioChgPose(str_802FE2A8);
    }
    if (MW_F32(mario, 0xD8) >= mario->position.y && (mario->dispFlags & 0x20000)) {
        camFollowYOn();
    }
    switch (mario->currSubMotionId) {
    case 0xA:
        marioChkLandon(&landY, float_0_80420da0);
        if (--mario->multiTimer > 0) {
            break;
        }
        mario->currSubMotionId = 0x14;
        MW_F32(mario, 0x80) = float_neg0p4744_80420e34;
        MW_F32(mario, 0x84) = float_0p4704_80420e38;
        MW_F32(mario, 0x88) = float_neg0p1904_80420e3c;
        MW_F32(mario, 0x7C) = MW_F32(mario, 0x80);
        /* fallthrough */
    case 0x14:
        f31 = marioGetFallSpd();
        hit = marioChkLandon(&landY, f31);
        if (f31 < float_neg100_80420e40) {
            f31 = float_neg100_80420e40;
            MW_F32(mario, 0x88) = float_0_80420da0;
            MW_F32(mario, 0x84) = float_0_80420da0;
            MW_F32(mario, 0x80) = float_0_80420da0;
            MW_F32(mario, 0x7C) = float_neg100_80420e40;
        }
        if (hit != NULL) {
            mario->hitobjStandOn = hit;
            mario->hitobjJumpFrom = NULL;
            mario->currMotionId = 0x11;
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
        if ((MW_U16(mario, 0x246) & (1 << 8)) && mario->hitobjStandOn == NULL) {
            mario->currSubMotionId = 0x14;
            MW_F32(mario, 0x80) = float_neg0p4744_80420e34;
            MW_F32(mario, 0x84) = float_0p4704_80420e38;
            MW_F32(mario, 0x88) = float_neg0p1904_80420e3c;
            MW_F32(mario, 0x7C) = MW_F32(mario, 0x80);
        } else {
            mario->flags &= ~0x80;
            marioChgMotSub(0, 0);
        }
        break;
    default:
        break;
    }
}

void kpa_fall_80213C38(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x00010000;
        mario->flags |= 0x00020000;
        mario->currSubMotionId = 0;
        if (mario->currMotionId != 0xB) {
            if (mario->characterId == 2) {
                if (kpaFireAttackCheck() == 0) {
                    kpaChgPose(str_802FE2B4, str_802FE2C0);
                }
            } else if (!(mario->flags & 0x2000)) {
                marioChgPose(str_802FE2CC);
            }
        } else {
            mario->flags |= 0x80;
        }
        if (mario->airTimer == 0) {
            *(Vec*)mario->field_0xD4 = mario->position;
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
        KPA(mario)->flags |= 0x100;
    }
    if (kpaGetBodyStatus() == 2) {
        mario = marioGetPtr();
        if (mario->currMotionId != 0x25) {
            marioChgMot2(0x25);
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
        if (mario->currMotionId != 8) {
            camFollowYOff();
        }
        kpaChgPose(str_802FE23C, str_802FE248);
        psndSFXOn_3D((char*)0x81E, &mario->position);
        if (mario->airTimer == 0) {
            *(Vec*)mario->field_0xD4 = mario->position;
        }
        MW_U16(mario, 0x2A0) = 0;
    }
    mario->airTimer++;
    if (mario->currMotionId != 8) {
        f31 = MW_F32(mario, 0x7C);
        marioMakeJumpPara();
        f1 = MW_F32(mario, 0x7C);
        if (f1 > float_0_80420da0 && mario->airTimer >= 4 && !(MW_U16(mario, 0x246) & (1 << 8))) {
            MW_F32(mario, 0x7C) = f1 + float_neg0p526_80420e48;
        }
        if (MW_F32(mario, 0x7C) <= float_0_80420da0) {
            if (f31 > float_0_80420da0) {
                MW_S16(mario, 0x52) = mario->airTimer;
            }
            marioSetPeakYpos();
            kpaSetFallPara();
            marioMakeJumpPara();
            marioChgMot(0xA);
        }
        mario->position.y += MW_F32(mario, 0x7C);
    }
}
#pragma opt_common_subs reset

// Matched. The `& 0xFFFF` (MUST_MATCH) forces the original's extrwi for `poseTime`; staging the
// dash speed in a temp issues both table loads before the stores. The u16->f32 conversion still
// prints as an anonymous @789@ rather than double_to_int_mask_802fe2e8; objdiff scores it 100%.
void kpa_dash(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* m2;
    s32 flag;
    s32 angle;

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        kpaChgPose(str_802FE2D4, str_802FE2DC);
        mario->currSubMotionId = 0;
        mario->multiTimer = 0;
        MW_U16(mario, 0x2A0) = 0;
    }
    m2 = marioGetPtr();
    flag = 0;
    if (kpaGetStageViewType() == 1) {
        flag = 0;
    } else if (!(MW_U16(m2, 0x248) & (1 << 8))) {
        flag = 0;
    } else if (kpaGetBodyStatus() == 2) {
        marioChgMot(0x25);
        flag = 1;
    } else {
        marioChgMot(3);
        flag = 1;
    }
    if (flag != 0) {
        return;
    }
    if (MW_U16(mario, 0x248) & (1 << 9)) {
        kpaAttackStart();
        marioChgMot(0);
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
            MW_S32(KPA(m2), 0xD0) = _rateTbl2[lvl];
            mario->baseSpeed = spd;
        }
    }
    if (kpaGetStageViewType() == 0) {
        if (mario->baseSpeed == float_0_80420da0 && !(mario->flags & 0x20)) {
            marioChgMot(0);
            return;
        }
    } else if (mario->controlStickSensitivity == float_0_80420da0 && !(mario->flags & 0x20)) {
        marioChgMot(0);
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
        marioChgMot(1);
    }
}

// NONMATCHING (99.99%): the magic int->float double sits at pool +0x110 instead of +0xd0 (see below).
// Every instruction agrees; only the pool slot differs. The u16->f32 conversion's 2^52 magic
// pools as its own anonymous .rodata entry at +0x110 where the original's named
// double_to_int_mask_802fe2e8 sits at +0xd0; objdiff reports that relocation as an
// arg_mismatch but still scores the function 100%. Defining the double in C makes MWCC
// emit a fresh anonymous copy for the implicit conversion (a project-wide limitation),
// so the named symbol stays pinned at +0xd0 by the data definition above. The speed/rate
// locals make both table loads precede their stores.
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
        if (!(KPA(mario)->flags & 0x01000000)) {
            kpaChgPose(str_802FE228, str_802FE2F0);
        } else {
            kpaChgPose(str_802FE2FC, str_802FE2F0);
        }
        MW_U16(mario, 0x2A0) = 0;
        MW_U16(mario, 0x2A2) = 0;
        MW_F32(KPA(mario), 0x10) = float_0p02_80420dd4;
        MW_F32(KPA(mario), 0x14) = float_neg0p02_80420e4c;
    }
    m2 = marioGetPtr();
    flag = 0;
    if (kpaGetStageViewType() == 1) {
        flag = 0;
    } else if (!(MW_U16(m2, 0x248) & (1 << 8))) {
        flag = 0;
    } else if (kpaGetBodyStatus() == 2) {
        marioChgMot(0x25);
        flag = 1;
    } else {
        marioChgMot(3);
        flag = 1;
    }
    if (flag != 0) {
        return;
    }
    if (MW_U16(mario, 0x248) & (1 << 9)) {
        kpaAttackStart();
        marioChgMot(0);
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
            MW_S32(KPA(m2), 0xD0) = rate;
            mario->baseSpeed = speed;
        }
    }
    if (kpaGetStageViewType() == 0) {
        if (mario->baseSpeed == float_0_80420da0 && !(mario->flags & 0x20)) {
            marioChgMot(0);
            return;
        }
    } else if (mario->controlStickSensitivity == float_0_80420da0 && !(mario->flags & 0x20)) {
        marioChgMot(0);
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
        marioChgMot(2);
    }
}

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
            if (!(KPA(mario)->flags & 0x01000000)) {
                kpaChgPose(str_802FE304, str_802FE30C);
            } else {
                kpaChgPose(str_802FE318, str_802FE30C);
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
    } else if (!(MW_U16(m2, 0x248) & (1 << 8))) {
        flag = 0;
    } else if (kpaGetBodyStatus() == 2) {
        marioChgMot(0x25);
        flag = 1;
    } else {
        marioChgMot(3);
        flag = 1;
    }
    if (flag != 0) {
        return;
    }
    if (MW_U16(mario, 0x248) & (1 << 9)) {
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
            marioChgMot(1);
        }
    } else if (mario->controlStickSensitivity > float_0_80420da0) {
        marioChgMot(1);
    }
}

// The folded int->float magic pools as double_to_int_802fe278 in the original; MWCC would
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

s32 kpaHipAttackOk(void) {
    MarioWork* mario = marioGetPtr();
    if (mario->currMotionId == 0x10) {
        return 0;
    }
    return ((KPA(mario)->flags >> 4) & 1) ^ 1;
}

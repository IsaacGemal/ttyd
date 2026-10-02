#include "mario/mario.h"
#include "mario/mariost.h"
#include "mario/mario_motion.h"
#include "mario/mario_cam.h"
#include "party.h"
#include "driver/camdrv.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "pmario_sound.h"
#include "system.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <string.h>
#include "koopa.h"
#include "mario/mario_hit.h"
#include "mario/mario_sbr.h"
#include "mario/mario_balloon.h"
#include "mario/mario_kemuri.h"
#include "motion/mot_jump.h"
#include "koopa_motion.h"
#include "koopa_hit.h"
#include "eff/eff_gonbaba_breath.h"

// locals for symbols with no home header
extern f32 marioJumpData[];

#define MW_U8(m, off)  (*(u8*)((u8*)(m) + (off)))
#define MW_U16(m, off) (*(u16*)((u8*)(m) + (off)))
#define MW_S16(m, off) (*(s16*)((u8*)(m) + (off)))
#define MW_U32(m, off) (*(u32*)((u8*)(m) + (off)))
#define MW_S32(m, off) (*(s32*)((u8*)(m) + (off)))
#define MW_F32(m, off) (*(f32*)((u8*)(m) + (off)))

u32 kpaPauseSeTable[28] = {
    0x10C, 0x10D, 0x10E, 0x10F, 0x111, 0x112, 0x113, 0x114,
    0x116, 0x117, 0x118, 0x119, 0x11B, 0x11E, 0x11F, 0x120,
    0x122, 0x123, 0x124, 0x125, 0x127, 0x128, 0x129, 0x12A,
    0x12C, 0x12D, 0x12E, 0x12F,
};

f32 kpaWidthDat[4] = {20.0f, 60.0f, 84.0f, 186.0f};
f32 baseScaleTbl[4] = {0.3f, 1.0f, 1.4f, 3.1f};
void* fireefp;
const Vec vec3_802fc920 = {0.0f, 0.0f, 0.0f};
#define str_802FC938 "KPA_A_1A"
#define str_802FC944 "KPA2_A_1A"
#define str_802FC950 "KPA_A_1B"
#define str_802FC95C "KPA2_A_1B"
#define str_802FC968 "KPA_A_1C"
#define str_802FC974 "KPA2_A_1C"
#define str_802FC980 "KPA_A_1D"
#define str_802FC98C "KPA2_A_1D"
#define str_802FC998 "KPA_S_1"
#define str_802FC9A0 "KPA2_S_1"
#define str_802FC9AC "KPA_W_1"
#define str_802FC9B4 "KPA2_W_1"
#define str_802FC9C0 "KPA_R_1"
#define str_802FC9C8 "KPA2_R_1"
#define str_802FC9D4 "KPA2_Z_2"
#define str_802FC9E0 "kpa_03"
#define str_802FC9E8 "M_J_1B"
#define str_802FC9F0 "M_J_1C"
#define str_802FC9F8 "kpa_02"
#define str_802FCA00 "kpa_04"
#define str_802FCA08 "kpa_05"
#define str_802FCA10 "kpa_06"

void* getKoopaFireEfp(void) {
    return fireefp;
}

s32 kpaNoHitEnemyCheck(void) {
    return MW_S16(marioGetPtr(), 0x78);
}

s32 kpaMutekiCheck(void) {
    return (KPA(marioGetPtr())->flags >> 16) & 1;
}

void kpaMutekiOn(void) {
    MarioWork* mp = marioGetPtr();
    animPoseSetMaterialFlagOn(mp->paperAnimGroupId[0], 0x80000000);
    KPA(mp)->flags |= 0x10000;
}

void kpaMutekiOff(void) {
    MarioWork* mp = marioGetPtr();
    animPoseSetMaterialFlagOff(mp->paperAnimGroupId[0], 0x80000000);
    KPA(mp)->flags &= ~0x10000;
}

void kpaMSetMutekiTimer(void) {
    MarioWork* mp = marioGetPtr();
    KPA(mp)->mutekiTimer = sysMsec2Frame(0x2EE0);
    MW_S16(mp, 0x78) = 0;
}

void kpaChgPose(const char* a, const char* b) {
    marioChgPose((((MW_S32(marioGetPtr(), 0x14) & 1) ^ 1) == 1) ? a : b);
}

s32 kpaGetLevel(void) {
    s32 level = 0;
    MarioWork* mp = marioGetPtr();
    if (mp->characterId == 2) {
        level = KPA(mp)->level;
    }
    return level;
}

// Creates the flame effect node for the current attack step. Inlined at every step that emits fire.
static inline void kpaFireAttackEntry(Vec pos) {
    MarioWork* mp;
    MarioWork* m;
    f32 sign;
    f32 scale;
    f32 x, y, z;
    f32 f4;
    s32 lvl;
    s32 type;

    mp = marioGetPtr();
    if (fireefp != NULL) {
        return;
    }
    sign = (mp->unk1A8 >= 90.0f && mp->unk1A8 <= 270.0f) ? 1.0f : -1.0f;
    lvl = 0;
    m = marioGetPtr();
    if (m->characterId == 2) {
        lvl = KPA(m)->level;
    }
#ifdef MUST_MATCH
    // MUST_MATCH: letting the reused `scale` temp hold `40.0f * sign` and then reloading the level scale
    // into `sign` via kpaGetBaseScale() makes the allocator coalesce sign/scale into f30 and keep x in f31
    // for blocks 2-4, as retail; the direct baseScaleTbl[lvl] index leaves all four copies 12 insns off and
    // `x = 40.0f * sign * scale` swaps sign/x.
    scale = 40.0f * sign;
    sign = kpaGetBaseScale(lvl);
    x = scale * sign + pos.x;
    {
        s32 mflags = MW_S32(marioGetPtr(), 0x14);
        y = pos.y + (20.0f * sign + (((mflags & 1) ^ 1) == 1 ? 6.0f : 0.0f));
    }
#else
    scale = baseScaleTbl[lvl];
    x = 40.0f * sign * scale + pos.x;
    {
        s32 mflags = MW_S32(marioGetPtr(), 0x14);
        y = pos.y + (20.0f * scale + (((mflags & 1) ^ 1) == 1 ? 6.0f : 0.0f));
    }
#endif
    z = pos.z;
    type = 7;
    f4 = 0.4f;
    switch (KPA(mp)->level) {
    case 0:
        f4 = 0.3f;
        break;
    case 1:
        f4 = 1.0f;
        break;
    case 2:
        f4 = 1.4f;
        break;
    case 3:
        f4 = 1.5f;
        type = 0;
        break;
    }
    fireefp = effGonbabaBreathEntry(type, 0, x, y, z, f4);
    ((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->unk48 =
        (MW_F32(mp, 0x1AC) >= 90.0f && MW_F32(mp, 0x1AC) <= 270.0f) ? 180 : 0;
    if (((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->unk4C != NULL) {
        ((KpaFireNode*)((EffectEntry*)((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->unk4C)->userdata)->unk48 =
            (MW_F32(mp, 0x1AC) >= 90.0f && MW_F32(mp, 0x1AC) <= 270.0f) ? 180 : 0;
    }
}

void kpaSetLevel(s32 level) {
    MarioWork* mp = marioGetPtr();
    if (mp->characterId == 2) {
        KpaWork* kpa = KPA(mp);
        if (kpa != NULL) {
            f32 scale;
            kpa->level = level;
            scale = baseScaleTbl[level];
            mp->scale.y = scale;
            mp->scale.x = scale;
            mp->scale.z = 1.0f;
            kpaSetSpec();
        }
    }
}

f32 kpaGetWidth(s32 index) {
    return kpaWidthDat[index];
}

s32 kpaGetBodyStatus(void) {
    s32 result;
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 2) {
        return -1;
    }
    result = 0;
    if (strcmp(gp->currentMapName, str_802FC9E0) == 0) {
        if (mp->position.y > 370.0f) {
            return 0;
        }
        if (mp->position.y + mp->wPlayerCollisionBox.y < 370.0f) {
            result = 2;
        } else {
            result = 1;
        }
    }
    return result;
}

s32 kpaGetStageType(void) {
    return (MW_U32(marioGetPtr(), 0x14) >> 1) & 1;
}

s32 kpaGetStageViewType(void) {
    return !(MW_U32(marioGetPtr(), 0x14) & 1);
}

s32 kpaCamFollow(void) {
    MarioWork* mp = marioGetPtr();
    if (marioBgmodeChk() == 1 || ((MW_S32(marioGetPtr(), 0x14) & 1) ^ 1) == 1) {
        return 0;
    }
    if (mp->position.y > 76.0f) {
        camFollowYOn();
    }
    return 1;
}

// The five map-name strings are addressed as `addi r4, r31, off` off the vec3_802fc920 rodata base.
// MWCC pools literals in emitted (reverse-source) first-use order, so kpaPreInit sits before
// kpa_jumpNpc here: kpaFall/kpaGetBodyStatus emit kpa_03 first, then kpa_jumpNpc's M_J_1B/M_J_1C,
// then kpaPreInit's kpa_02/kpa_04/kpa_05/kpa_06, matching the target .rodata. The kpaFireAttack
// pool entries above must also be present (see the #ifdef MUST_MATCH block).
void kpaPreInit(void) {
    MarioWork* mp = marioGetPtr();

    if (KPA(mp) == NULL) {
        KPA(mp) = __memAlloc(0, 0x154);
    }
    memset(KPA(mp), 0, 0x154);
    KPA(mp)->level = 1;
    KPA(mp)->unkD0 = 1;
    KPA(mp)->unkE0 = 0;
    KPA(mp)->unkD8 = 1;
    mp = marioGetPtr();
    KPA(mp)->sfxId = -1;
    mp = marioGetPtr();
    if (strcmp(gp->currentMapName, str_802FC9F8) == 0
        || strcmp(gp->currentMapName, str_802FC9E0) == 0
        || strcmp(gp->currentMapName, str_802FCA00) == 0
        || strcmp(gp->currentMapName, str_802FCA08) == 0
        || strcmp(gp->currentMapName, str_802FCA10) == 0) {
        MW_U32(mp, 0x14) |= 1;
    } else {
        MW_U32(mp, 0x14) &= ~1;
    }
    mp = marioGetPtr();
    if (strcmp(gp->currentMapName, str_802FC9E0) == 0) {
        MW_U32(mp, 0x14) |= 2;
    } else {
        MW_U32(mp, 0x14) &= ~2;
    }
    fireefp = NULL;
}

void kpaMain(void) {
    if (MW_U32(marioGetPtr(), 0x14) & 1) {
        kpa2DMain();
    } else {
        kpa3DMain();
    }
}

void kpa2DMain(void) {
    MarioWork* mp = marioGetPtr();
    MarioWork* m;
    MarioWork* m2;
    Vec* prevPos;
    KpaWork* kpa;
    f32 f31;
    f32 sn, cs;
    Vec delta;
    Vec pos;

    kpaHitCheck();
    if (MW_S16(mp, 0x78) != 0) {
        MW_S16(mp, 0x78)--;
    }
    MW_U32(mp, 0x1F4) = 0;
    MW_U32(mp, 0x1E0) = 0;
    MW_U32(mp, 0x1DC) = 0;
    mp->hitobjPush = NULL;
    MW_U32(mp, 0x1F0) = 0;
    marioItemGetChk();
    kpaFireAttack();
    kpaFireMain();
    m = marioGetPtr();
    kpa = KPA(m);
    if (kpa->mutekiTimer > 0) {
        if (--kpa->mutekiTimer <= 0) {
            KPA(m)->mutekiTimer = 0;
            m2 = marioGetPtr();
            animPoseSetMaterialFlagOff(m2->paperAnimGroupId[0], 0x80000000);
            KPA(m2)->flags &= ~0x10000;
        }
    }
    if (mp->currMotionId == 8) {
        prevPos = (Vec*)((u8*)mp + 0x110);
    *prevPos = mp->position;
        MW_F32(mp, 0x174) = 0.0f;
        MW_F32(mp, 0x178) = 0.0f;
        MW_F32(mp, 0x17C) = 0.0f;
        marioBottomless();
        marioPreDisp();
        mario_kemuri_80130474();
        marioCamMain();
        MW_U32(mp, 0x34)++;
        return;
    }
    pos = mp->position;
    f31 = 0.0f;
    if (!(mp->flags & 0x80)) {
        if (mp->airTimer != 0) {
            f31 = mp->controlStickSensitivity * mp->playerGravity;
            f31 *= 0.015f;
            if (!(mp->flags & 0x70000)) {
                f31 *= 0.25f;
            }
            sincosf(&sn, &cs, mp->controlStickAngle);
            f31 *= sn;
        }
    }
    sincosf(&sn, &cs, mp->directionView);
    {
        f32 sum = mp->baseSpeed * sn + f31;
        f32 angle;
        f31 = __fabsf(sum);
        angle = angleABf(0.0f, 0.0f, sum, 0.0f);
        {
            f32 limit = mp->mBaseDashSpeed * mp->playerGravity;
            if (f31 > limit) {
                f31 = limit;
            }
        }
        marioCheckWall(f31, angle);
    }
    mp->position.y += MW_F32(mp, 0x178);
    PSVECSubtract(&mp->position, &pos, &delta);
    {
        f32 mag = PSVECMag(&delta);
        if (mag > f31) {
            PSVECScale(&delta, &delta, f31 / mag);
            PSVECAdd(&pos, &delta, &mp->position);
        }
    }
    if (mp->flags & 0x10000) {
        marioChkOverhead();
        if (mp->airTimer > 5) {
            marioChkGnd2();
        }
    } else if (mp->flags & 0x20000) {
        kpaFall();
        marioChkOverhead();
        if (mp->currMotionId != 0x10 && mp->currMotionId != 0x11 && mp->airTimer > 5) {
            marioChkGnd2();
        }
    } else {
        marioChkOverhead();
        marioChkGnd();
    }
    marioBoots();
    marioForceMoveMain();
    marioBottomless();
    marioPreDisp();
    marioBalloonMain();
    mario_kemuri_80130474();
    marioCamMain();
    MW_U32(mp, 0x34)++;
    prevPos = (Vec*)((u8*)mp + 0x110);
    *prevPos = mp->position;
    MW_F32(mp, 0x174) = 0.0f;
    MW_F32(mp, 0x178) = 0.0f;
    MW_F32(mp, 0x17C) = 0.0f;
}

void kpa3DMain(void) {
    MarioWork* mp = marioGetPtr();
    MarioWork* m;
    MarioWork* m2;
    KpaWork* kpa;
    void* hit;
    Vec* prevPos;

    hit = kpaHitCheck();
    if (hit != NULL) {
        MW_U32(mp, 0x1F4) = (u32)hit;
    }
    if (MW_S16(mp, 0x78) != 0) {
        MW_S16(mp, 0x78)--;
    }
    if (mp->currMotionId == 0x18 || mp->currMotionId == 0x1F || mp->currMotionId == 0x20) {
        marioPreDisp();
        marioBottomless();
        MW_U32(mp, 0x34)++;
        partyMain();
        marioCamMain();
        MW_U32(mp, 0x1F4) = 0;
        MW_U32(mp, 0x1E0) = 0;
        MW_U32(mp, 0x1DC) = 0;
        mp->hitobjPush = NULL;
        MW_U32(mp, 0x1F0) = 0;
        return;
    }
    marioItemGetChk();
    kpaFireAttack();
    kpaFireMain();
    m = marioGetPtr();
    kpa = KPA(m);
    if (kpa->mutekiTimer > 0) {
        if (--kpa->mutekiTimer <= 0) {
            KPA(m)->mutekiTimer = 0;
            m2 = marioGetPtr();
            animPoseSetMaterialFlagOff(m2->paperAnimGroupId[0], 0x80000000);
            KPA(m2)->flags &= ~0x10000;
        }
    }
    if (mp->currMotionId == 8) {
        prevPos = (Vec*)((u8*)mp + 0x110);
    *prevPos = mp->position;
        MW_F32(mp, 0x174) = 0.0f;
        MW_F32(mp, 0x178) = 0.0f;
        MW_F32(mp, 0x17C) = 0.0f;
        marioBottomless();
        marioPreDisp();
        marioBalloonMain();
        mario_kemuri_80130474();
        partyMain();
        marioCamMain();
        MW_U32(mp, 0x34)++;
        return;
    }
    if (mp->flags & 0x40000) {
        marioMove();
        marioChkOverhead();
        if (mp->airTimer > 0x64) {
            marioChgMot(0xA);
            marioSetFallPara();
        }
    } else if (mp->flags & 0x10000) {
        marioMove();
        marioChkOverhead();
        if (mp->airTimer > 5) {
            marioChkGnd2();
        }
    } else if (mp->flags & 0x20000) {
        marioMove();
        kpaFall();
        kpaSearchFrontUnder();
        marioChkOverhead();
        if (mp->currMotionId != 0x10 && mp->currMotionId != 0x11 && mp->airTimer > 5) {
            marioChkGnd2();
        }
    } else {
        marioMove();
        kpaSearchFrontUnder();
        marioChkOverhead();
        marioChkGnd();
    }
    marioBoots();
    marioForceMoveMain();
    marioBottomless();
    marioPreDisp();
    marioBalloonMain();
    mario_kemuri_80130474();
    partyMain();
    marioCamMain();
    MW_U32(mp, 0x34)++;
    prevPos = (Vec*)((u8*)mp + 0x110);
    *prevPos = mp->position;
    MW_F32(mp, 0x174) = 0.0f;
    MW_F32(mp, 0x178) = 0.0f;
    MW_F32(mp, 0x17C) = 0.0f;
}

f32 kpaSetJumpPara(void) {
    f32* const jump = marioJumpData;
    MarioWork* mp = marioGetPtr();
    MW_F32(mp, 0x7C) = jump[0];
    MW_F32(mp, 0x80) = jump[1];
    MW_F32(mp, 0x84) = jump[2];
    MW_F32(mp, 0x88) = jump[3];
    switch (KPA(mp)->level) {
        case 0:
            MW_F32(mp, 0x7C) = 7.0f;
            MW_F32(mp, 0x80) = -0.4f;
            MW_F32(mp, 0x84) = 0.08f;
            MW_F32(mp, 0x88) = -0.02f;
            break;
        case 1:
            MW_F32(mp, 0x7C) = 8.1f;
            MW_F32(mp, 0x80) = -0.3f;
            MW_F32(mp, 0x84) = 0.08f;
            MW_F32(mp, 0x88) = -0.02f;
            break;
        case 2:
            MW_F32(mp, 0x7C) = 8.4f;
            MW_F32(mp, 0x80) = -0.1f;
            MW_F32(mp, 0x84) = 0.08f;
            MW_F32(mp, 0x88) = -0.018f;
            break;
        case 3:
            MW_F32(mp, 0x7C) = 10.5f;
            MW_F32(mp, 0x80) = -0.12f;
            MW_F32(mp, 0x84) = 0.08f;
            MW_F32(mp, 0x88) = -0.018f;
            break;
    }
    return MW_F32(mp, 0x7C);
}

// The original reserves f1 for a float return value (the float temps start at f2),
// so the source declared this returning f32 even though it stores and returns nothing.
f32 kpaSetFallPara(void) {
    MarioWork* mp = marioGetPtr();
    if (MW_U16(mp, 0x246) & 0x100) {
        MW_F32(mp, 0x7C) = -0.05f;
        MW_F32(mp, 0x80) = -0.08f;
        MW_F32(mp, 0x84) = -0.006f;
        MW_F32(mp, 0x88) = -0.002f;
    } else {
        MW_F32(mp, 0x7C) = -0.1f;
        MW_F32(mp, 0x80) = -0.2f;
        MW_F32(mp, 0x84) = -0.024f;
        MW_F32(mp, 0x88) = -0.01f;
    }
}


void kpa_jumpNpc(void) {
    MarioWork* mp = marioGetPtr();

    if (mp->trigFlags & 1) {
        mp->trigFlags &= ~1;
        marioResetCamFollowRate();
        mp->flags &= ~0x80;
        camFollowYOff();
        marioPreJump();
        MW_F32(mp, 0x7C) = 2.6f;
        MW_F32(mp, 0x80) = -0.06f;
        MW_F32(mp, 0x84) = -0.01f;
        MW_F32(mp, 0x88) = -0.002f;
        if (!(mp->flags & 0x2000)) {
            if (mp->characterId == 0) {
                marioChgPose(str_802FC9E8);
            }
        }
        mp->flags |= 0x10000;
        mp->currSubMotionId = 0;
    }
    switch ((s32)mp->currSubMotionId) {
    case 0:
        marioMakeJumpPara();
        if (MW_F32(mp, 0x7C) <= 0.0f) {
            if (!(mp->flags & 0x2000)) {
                if (mp->characterId == 0) {
                    marioChgPose(str_802FC9F0);
                }
            }
            marioSetPeakYpos();
            MW_F32(mp, 0x80) = -0.06f;
            MW_F32(mp, 0x84) = -0.004f;
            MW_F32(mp, 0x88) = -0.002f;
            mp->flags &= ~0x10000;
            mp->flags |= 0x20000;
        }
        break;
    }
    mp->position.y += MW_F32(mp, 0x7C);
    mp->airTimer += 1;
}

void kpaFall(void) {
    MarioWork* mp = marioGetPtr();
    f32 spd;
    void* res;
    f32 landY;
    if (strcmp(gp->currentMapName, str_802FC9E0) == 0) {
        mp->hitobjStandOn = NULL;
        kpaClearHitobjRide();
        spd = marioGetFallSpd();
        if (spd < -20.0f) {
            spd = -20.0f;
        }
        if (!spd) {
            MarioWork* mp2 = marioGetPtr();
            if (MW_U16(mp2, 0x246) & 0x100) {
                MW_F32(mp2, 0x7C) = -0.05f;
                MW_F32(mp2, 0x80) = -0.08f;
                MW_F32(mp2, 0x84) = -0.006f;
                MW_F32(mp2, 0x88) = -0.002f;
            } else {
                MW_F32(mp2, 0x7C) = -0.1f;
                MW_F32(mp2, 0x80) = -0.2f;
                MW_F32(mp2, 0x84) = -0.024f;
                MW_F32(mp2, 0x88) = -0.01f;
            }
            spd = marioGetFallSpd();
        }
        res = marioChkLandon(&landY, spd);
        if (res != NULL) {
            mp->hitobjStandOn = res;
            mp->hitobjJumpFrom = NULL;
            mp->position.y = landY;
        } else {
            mp->position.y += spd;
        }
        if (mp->hitobjStandOn != NULL) {
            marioLandOn();
            psndSFXOn_3D((const char*)0x81D, &mp->position);
        }
    } else {
        if (mp->currMotionId == 0x10 || mp->currMotionId == 0x11) {
            return;
        }
        if (marioKeyOffChk() == 0 || marioCtrlOffChk() == 0) {
            if (!(mp->flags & 0x20)) {
                f32 base = mp->baseSpeed;
                base = base + 0.01f * (-0.5f - base);
                if (base < 0.0f) {
                    base = 0.0f;
                }
                mp->baseSpeed = base;
            }
        }
        if (mp->currMotionId != 4) {
            if (MW_U16(mp, 0x246) & 0x100) {
                MW_F32(mp, 0x88) = -0.001f;
            } else {
                MW_F32(mp, 0x88) = -0.01f;
            }
        }
        mp->hitobjStandOn = NULL;
        kpaClearHitobjRide();
        spd = marioGetFallSpd();
        if (spd < -20.0f) {
            spd = -20.0f;
        }
        res = marioChkLandon(&landY, spd);
        if (res == NULL) {
            mp->position.y += spd;
        } else {
            mp->hitobjStandOn = res;
            mp->hitobjJumpFrom = NULL;
            if (hitGetAttr((HitEntry*)res) & 0x200) {
                mp->position.y += spd;
            } else {
                mp->position.y = landY;
            }
        }
        if (res != NULL || (u32)kpaChkHitobjRide() != 0u) {
            marioLandOn();
        }
    }
}

s32 kpaFireAttackCheck(void) {
    MarioWork* mp = marioGetPtr();
    s32 mode;

    if (!(mp->flags & 1)) {
        return 0;
    }
    mode = KPA(mp)->attackMode;
    if ((mode >= 0x14 && mode <= 0x17) || (mode >= 0x28 && mode <= 0x2B)) {
        return 1;
    }
    return 0;
}

void kpaAttackStart(void) {
    MarioWork* mp = marioGetPtr();
    if (KPA(mp)->attackMode == 0) {
        marioChgMot(0);
        KPA(mp)->attackMode = 1;
    }
}

void kpaAttackStart2(void) {
    KpaWork* kpa = KPA(marioGetPtr());
    if (kpa->attackMode == 0) {
        kpa->attackMode = 0x1E;
    }
}

// 8 of the 20 diffs: the int->float conversion constant pools as an anonymous @NNN@ at .rodata+0xf8
// instead of the named double_to_int_802fc930 at +0x10 (GC/2.6 never reuses a named double for the idiom;
// project-wide, see win_badge.c). The other 12: in the four inlined kpaFireAttackEntry copies `sign`
// lands in f31 where retail keeps it in f30; its two lfs and the fmuls differ (scale and x both match).
// NONMATCHING (99.98%): compiler allocation only, 20 of 967 instructions.
void kpaFireAttack(void) {
    MarioWork* mp = marioGetPtr();

    switch (KPA(mp)->attackMode) {
    case 1:
        mp->flags &= ~0x000F0000;
        MW_S16(mp, 0x50) = 0;
        KPA(mp)->unkD8 = (KPA(mp)->level == 0) ? 0xC : 8;
        KPA(mp)->unkE0 = 0;
        MW_S32(KPA(mp), 0x140) = 0;
        KPA(mp)->attackMode = 0x14;
        if (!(mp->flags & 0x20)) {
            MW_F32(mp, 0x180) = 0.0f;
            MW_F32(mp, 0x2C4) = 0.0f;
        }
        // fall through
    case 0x14:
        kpaChgPose(str_802FC938, str_802FC944);
        MW_S32(KPA(mp), 0x140) = 6;
        KPA(mp)->attackMode = 0x15;
        break;
    case 0x15:
        if (--MW_S32(KPA(mp), 0x140) > 0) {
            break;
        }
        kpaChgPose(str_802FC950, str_802FC95C);
        MW_S32(KPA(mp), 0x140) = 8;
        KPA(mp)->attackMode = 0x16;
        {
            MarioWork* m = marioGetPtr();
            if (KPA(m)->sfxId == 0xFFFFFFFF) {
                KPA(m)->sfxId = psndSFXOn_3D((const char*)0x821, &m->position);
                psndSFXOn_3D((const char*)0x131, &m->position);
            }
        }
        break;
    case 0x16:
        {
            Vec pos = mp->position;
            marioGetPtr();
            kpaFireAttackEntry(pos);
        }
        if (--MW_S32(KPA(mp), 0x140) <= 0) {
            MW_S32(KPA(mp), 0x140) = sysMsec2Frame(0x190);
            kpaChgPose(str_802FC968, str_802FC974);
            KPA(mp)->attackMode = 0x17;
        }
        break;
    case 0x17:
        {
            Vec pos = mp->position;
            marioGetPtr();
            kpaFireAttackEntry(pos);
        }
        if (--MW_S32(KPA(mp), 0x140) <= 0) {
            if (!(MW_U16(mp, 0x246) & 0x200) && MW_S32(KPA(mp), 0x144) == 0) {
                kpaChgPose(str_802FC980, str_802FC98C);
                KPA(mp)->attackMode = 0x18;
                MW_S32(KPA(mp), 0x140) = 6;
                {
                    MarioWork* m = marioGetPtr();
                    if (KPA(m)->sfxId != 0xFFFFFFFF) {
                        psndSFXOff(KPA(m)->sfxId);
                        KPA(m)->sfxId = 0xFFFFFFFF;
                    }
                }
            }
        }
        break;
    case 0x18:
        if (--MW_S32(KPA(mp), 0x140) <= 0) {
            KPA(mp)->attackMode = 0;
            kpaChgPose(str_802FC998, str_802FC9A0);
        }
        break;
    }
    switch (KPA(mp)->attackMode) {
    case 0x1E:
        KPA(mp)->unkD8 = (KPA(mp)->level == 0) ? 0xC : 8;
        KPA(mp)->unkE0 = 0;
        MW_S32(KPA(mp), 0x140) = 0;
        KPA(mp)->attackMode = 0x28;
        if (!(mp->flags & 0x20)) {
            MW_F32(mp, 0x180) = 0.0f;
            MW_F32(mp, 0x2C4) = 0.0f;
        }
        // fall through
    case 0x28:
        kpaChgPose(str_802FC938, str_802FC944);
        MW_S32(KPA(mp), 0x140) = 6;
        KPA(mp)->attackMode = 0x29;
        break;
    case 0x29:
        if (--MW_S32(KPA(mp), 0x140) > 0) {
            break;
        }
        kpaChgPose(str_802FC950, str_802FC95C);
        MW_S32(KPA(mp), 0x140) = 8;
        KPA(mp)->attackMode = 0x2A;
        {
            MarioWork* m = marioGetPtr();
            if (KPA(m)->sfxId == 0xFFFFFFFF) {
                KPA(m)->sfxId = psndSFXOn_3D((const char*)0x821, &m->position);
                psndSFXOn_3D((const char*)0x131, &m->position);
            }
        }
        break;
    case 0x2A:
        {
            Vec pos = mp->position;
            marioGetPtr();
            kpaFireAttackEntry(pos);
        }
        if (--MW_S32(KPA(mp), 0x140) <= 0) {
            MW_S32(KPA(mp), 0x140) = sysMsec2Frame(0x190);
            kpaChgPose(str_802FC968, str_802FC974);
            KPA(mp)->attackMode = 0x2B;
        }
        break;
    case 0x2B:
        {
            Vec pos = mp->position;
            marioGetPtr();
            kpaFireAttackEntry(pos);
        }
        if (--MW_S32(KPA(mp), 0x140) <= 0) {
            if (!(MW_U16(mp, 0x246) & 0x200) && MW_S32(KPA(mp), 0x144) == 0) {
                kpaChgPose(str_802FC980, str_802FC98C);
                KPA(mp)->attackMode = 0x2C;
                MW_S32(KPA(mp), 0x140) = 6;
                {
                    MarioWork* m = marioGetPtr();
                    if (KPA(m)->sfxId != 0xFFFFFFFF) {
                        psndSFXOff(KPA(m)->sfxId);
                        KPA(m)->sfxId = 0xFFFFFFFF;
                    }
                }
            }
        }
        break;
    case 0x2C:
        if (--MW_S32(KPA(mp), 0x140) <= 0) {
            KPA(mp)->attackMode = 0;
            switch (MW_U16(mp, 0x2E)) {
            case 0:
                kpaChgPose(str_802FC998, str_802FC9A0);
                break;
            case 1:
                kpaChgPose(str_802FC9AC, str_802FC9B4);
                break;
            case 2:
                kpaChgPose(str_802FC9C0, str_802FC9C8);
                break;
            case 0x25:
                kpaChgPose(str_802FC998, str_802FC9D4);
                break;
            }
        }
        break;
    }
}


void kpaCancelAttackMode(void) {
    MarioWork* mp;
    mp = marioGetPtr();
    KPA(mp)->attackMode = 0;
    mp = marioGetPtr();
    if (KPA(mp)->sfxId != 0xFFFFFFFFu) {
        psndSFXOff(KPA(mp)->sfxId);
        KPA(mp)->sfxId = 0xFFFFFFFFu;
    }
}

s32 kpaGetAttackMode(void) {
    return KPA(marioGetPtr())->attackMode != 0;
}

void unk_801f8cd0(void) {
    if (fireefp != NULL) {
        MarioWork* mp;
        effSoftDelete(fireefp);
        fireefp = NULL;
        mp = marioGetPtr();
        if (KPA(mp)->sfxId != 0xFFFFFFFFu) {
            psndSFXOff(KPA(mp)->sfxId);
            KPA(mp)->sfxId = 0xFFFFFFFFu;
        }
    }
}

// The only instruction difference left is the loop preheader: MWCC emits `addi r29,r29,0x50` before
// `li r28,1` where the original has the reverse. `#pragma scheduling off` flips the pair but changes
// ~60 other instructions, and every source shape tried (i=1 first, comma and for-init forms, root+1 /
// &root[1] / pointer-increment inits, an inline helper, and the opt_propagation / opt_lifetimes /
// global_optimizer / peephole / dead-assignment toggles) leaves the order as the scheduler chose it.
// The other `!!` arg mismatches are only .sdata2 pool slots: this file is in asm order, so the pool is
// emitted in reverse; in the canonical (reverse-source) position kpaFireMain's constants pool in the
// original order. Compiler stamps 02040701/02040201; the paired function kpaEnemyFireHitChk has the
// identical preheader quirk.
// NONMATCHING (99.52%): compiler scheduling only. In the matrix loop's preheader the `li r28, 1`
// (loop index) and `addi r29, r29, 0x50` (first node pointer) are emitted in the opposite order.
void kpaFireMain(void) {
    MarioWork* mp = marioGetPtr();
    f32 sign;
    f32 scale;
    f32 x, y, z;
    f32 f30, f31;
    s32 level;
    EffectEntry* efp;
    KpaFireNode* root;
    KpaFireNode* node;
    s32 i;
    Mtx m2, m0, m1, m3; // declaration order puts the matrices in the original's stack slots
    Vec out;
    Vec v;
    void* hit;
    f32 rootScale;
    f32 py;
    f32 off;
#ifdef MUST_MATCH
    MarioWork* mp2;
#endif

    KPA(mp)->unk18 = NULL;
    if (fireefp == NULL) {
        goto cleanup;
    }
    sign = (mp->unk1A8 >= 90.0f && mp->unk1A8 <= 270.0f) ? 1.0f : -1.0f;
    level = kpaGetLevel();
    scale = baseScaleTbl[level];
    x = 40.0f * sign * scale + mp->position.x;
#ifdef MUST_MATCH
    // MUST_MATCH: taking the flags pointer into a named temp and loading position.y between that call and
    // the ternary keeps the 0x90 load and 20.0f in the pre-branch block, as the original does; the inline
    // form pulls both after the branch.
    mp2 = marioGetPtr();
    py = mp->position.y;
    off = (((MW_S32(mp2, 0x14) & 1) ^ 1) == 1) ? 6.0f : 0.0f;
#else
    off = (((MW_S32(marioGetPtr(), 0x14) & 1) ^ 1) == 1) ? 6.0f : 0.0f;
    py = mp->position.y;
#endif
    y = py + (20.0f * scale + off);
    z = mp->position.z;
    ((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->pos.x = x;
    ((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->pos.y = y;
    ((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->pos.z = z;
    if (((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->unk4C != NULL) {
        ((KpaFireNode*)((EffectEntry*)((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->unk4C)->userdata)->pos.x = x;
        ((KpaFireNode*)((EffectEntry*)((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->unk4C)->userdata)->pos.y = y;
        ((KpaFireNode*)((EffectEntry*)((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->unk4C)->userdata)->pos.z = z;
    }
    level = kpaGetLevel();
    if (level == 0) {
        goto cleanup;
    }
    efp = fireefp;
    camGetPtr(4);
    root = efp->userdata;
    rootScale = root->unk40;
    PSMTXTrans(m0, root->pos.x, root->pos.y, root->pos.z);
    PSMTXRotRad(m1, 0x79, (root->unk48 - camGetPtr(4)->viewYaw) * 0.017453292f);
    if (root->unk48 > 90.0f && root->unk48 < 270.0f) {
        PSMTXScale(m2, root->unk40, root->unk40, -root->unk40);
    } else {
        PSMTXScale(m2, root->unk40, root->unk40, root->unk40);
    }
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m3);
    node = (KpaFireNode*)((u8*)root + 0x50);
    for (i = 1; i < efp->count; i++, node = (KpaFireNode*)((u8*)node + 0x50)) {
        if ((i - 1) & 3) {
            continue;
        }
        if (node->unk44 != 0) {
            continue;
        }
        if (node->unk1C <= 0x10) {
            continue;
        }
        f30 = 7.0f * node->unk40;
        f31 = f30 * rootScale;
        if (f31 < 7.0f) {
            continue;
        }
        PSMTXTrans(m0, node->pos.x, node->pos.y, node->pos.z);
        PSMTXScale(m2, f30, f30, f30);
        PSMTXConcat(m0, m2, m0);
        PSMTXConcat(m3, m0, m0);
        v = vec3_802fc920;
        PSMTXMultVec(m0, &v, &out);
        hit = kpaFlameHitCheck(out.x, out.y, out.z, f31);
        if (hit != NULL) {
            KPA(mp)->unk18 = hit;
            goto cleanup;
        }
    }
cleanup:
    if (!kpaFireAttackCheck()) {
        if (fireefp != NULL) {
            effSoftDelete(fireefp);
            fireefp = NULL;
        }
    }
}

f32 kpaGetBaseScale(s32 index) {
    return baseScaleTbl[index];
}

BOOL kpaHipAttackCheck(void) {
    BOOL result = FALSE;
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 2) {
        return FALSE;
    }
    if (mp->currMotionId == 0x11) {
        result = TRUE;
    }
    return result;
}

s32 kpaJumpChk(void) {
    return marioGetPtr()->airTimer != 0;
}

void kpaPauseSE2(void) {
    psndSFXOn((const char*)(kpaPauseSeTable[irand(0x1C)] | 0x20000));
}


#ifdef MUST_MATCH
// Match-only pool order: kpaFireAttack's int->float conversion and the kpaFireAttack/kpaPreInit pose strings
// share .rodata. The target has the conversion's magic double at +0x10, ahead of the strings; this compiler
// pools a string before a double first used after it, and kpaFireAttack's first pose string (case 0x14) is
// emitted before its first conversion (case 0x16). Requesting the double from a function emitted ahead of
// kpaFireAttack (source after it) reproduces the target order. kpaPoolConv exists only for that: it is
// unreferenced, so the linker strips it and the dol stays byte-identical (cf. TRKDoFlushCache, CONTEXT).
static f32 kpaPoolConv(s32 x) {
    return (f32)x;
}
#endif

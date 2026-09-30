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

// locals for symbols with no home header (see REPORT.md)
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
#ifdef MUST_MATCH
// Match-only rodata: these pool entries belong to the not-yet-written kpaFireAttack; kpaPreInit
// addresses its map-name strings as offsets from vec3_802fc920, so its .rodata must hold them.
const double double_to_int_802fc930 = 4503601774854144.0;
const char str_802FC938[0xC] = "KPA_A_1A";
const char str_802FC944[0xC] = "KPA2_A_1A";
const char str_802FC950[0xC] = "KPA_A_1B";
const char str_802FC95C[0xC] = "KPA2_A_1B";
const char str_802FC968[0xC] = "KPA_A_1C";
const char str_802FC974[0xC] = "KPA2_A_1C";
const char str_802FC980[0xC] = "KPA_A_1D";
const char str_802FC98C[0xC] = "KPA2_A_1D";
const char str_802FC998[0x8] = "KPA_S_1";
const char str_802FC9A0[0xC] = "KPA2_S_1";
const char str_802FC9AC[0x8] = "KPA_W_1";
const char str_802FC9B4[0xC] = "KPA2_W_1";
const char str_802FC9C0[0x8] = "KPA_R_1";
const char str_802FC9C8[0xC] = "KPA2_R_1";
const char str_802FC9D4[0xC] = "KPA2_Z_2";
#endif


void kpaPauseSE2(void) {
    psndSFXOn((const char*)(kpaPauseSeTable[irand(0x1C)] | 0x20000));
}

s32 kpaJumpChk(void) {
    return marioGetPtr()->airTimer != 0;
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

f32 kpaGetBaseScale(s32 index) {
    return baseScaleTbl[index];
}

// NONMATCHING (95.28%): only the y-offset temp (f2 vs f0) and the PSMTXRotRad camera read (f0 vs f2)
// differ, and neither responds to source shape. The prologue, f29/f30/f31, r28-r31 and every
// store/loop instruction match; the file's compiler build stamps .comment 02040701 vs the object's 02040201.
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
    CameraEntry* cam;
    Mtx m2, m0, m1, m3; // declaration order puts the matrices in the original's stack slots
    Vec out;
    Vec v;
    void* hit;
    f32 rootScale;
    f32 py;
    f32 off;

    KPA(mp)->unk18 = NULL;
    if (fireefp == NULL) {
        goto cleanup;
    }
    sign = (mp->unk1A8 >= 90.0f && mp->unk1A8 <= 270.0f) ? 1.0f : -1.0f;
    level = kpaGetLevel();
    scale = baseScaleTbl[level];
    x = 40.0f * sign * scale + mp->position.x;
    off = (((MW_S32(marioGetPtr(), 0x14) & 1) ^ 1) == 1) ? 6.0f : 0.0f;
    py = mp->position.y;
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
    cam = camGetPtr(4);
    root = efp->userdata;
    rootScale = root->unk40;
    PSMTXTrans(m0, root->pos.x, root->pos.y, root->pos.z);
    cam = camGetPtr(4);
    PSMTXRotRad(m1, 0x79, 0.017453292f * (root->unk48 - MW_F32(cam, 0x114)));
    if (root->unk48 > 90.0f && root->unk48 < 270.0f) {
        PSMTXScale(m2, rootScale, rootScale, -rootScale);
    } else {
        PSMTXScale(m2, rootScale, rootScale, rootScale);
    }
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m3);
    i = 1;
    node = (KpaFireNode*)((u8*)root + 0x50);
    for (; i < efp->count; i++, node = (KpaFireNode*)((u8*)node + 0x50)) {
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

s32 kpaGetAttackMode(void) {
    return KPA(marioGetPtr())->attackMode != 0;
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

void kpaAttackStart2(void) {
    KpaWork* kpa = KPA(marioGetPtr());
    if (kpa->attackMode == 0) {
        kpa->attackMode = 0x1E;
    }
}

void kpaAttackStart(void) {
    MarioWork* mp = marioGetPtr();
    if (KPA(mp)->attackMode == 0) {
        marioChgMot(0);
        KPA(mp)->attackMode = 1;
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

void kpaFall(void) {
    MarioWork* mp = marioGetPtr();
    f32 spd;
    void* res;
    f32 landY;
    if (strcmp(gp->currentMapName, "kpa_03") == 0) {
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
    if (strcmp(gp->currentMapName, "kpa_02") == 0
        || strcmp(gp->currentMapName, "kpa_03") == 0
        || strcmp(gp->currentMapName, "kpa_04") == 0
        || strcmp(gp->currentMapName, "kpa_05") == 0
        || strcmp(gp->currentMapName, "kpa_06") == 0) {
        MW_U32(mp, 0x14) |= 1;
    } else {
        MW_U32(mp, 0x14) &= ~1;
    }
    mp = marioGetPtr();
    if (strcmp(gp->currentMapName, "kpa_03") == 0) {
        MW_U32(mp, 0x14) |= 2;
    } else {
        MW_U32(mp, 0x14) &= ~2;
    }
    fireefp = NULL;
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
                marioChgPose("M_J_1B");
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
                    marioChgPose("M_J_1C");
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

void kpaMain(void) {
    if (MW_U32(marioGetPtr(), 0x14) & 1) {
        kpa2DMain();
    } else {
        kpa3DMain();
    }
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

s32 kpaGetStageViewType(void) {
    return !(MW_U32(marioGetPtr(), 0x14) & 1);
}

s32 kpaGetStageType(void) {
    return (MW_U32(marioGetPtr(), 0x14) >> 1) & 1;
}

s32 kpaGetBodyStatus(void) {
    s32 result;
    MarioWork* mp = marioGetPtr();
    if (mp->characterId != 2) {
        return -1;
    }
    result = 0;
    if (strcmp(gp->currentMapName, "kpa_03") == 0) {
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

f32 kpaGetWidth(s32 index) {
    return kpaWidthDat[index];
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

s32 kpaGetLevel(void) {
    s32 level = 0;
    MarioWork* mp = marioGetPtr();
    if (mp->characterId == 2) {
        level = KPA(mp)->level;
    }
    return level;
}

void kpaChgPose(const char* a, const char* b) {
    marioChgPose((((MW_S32(marioGetPtr(), 0x14) & 1) ^ 1) == 1) ? a : b);
}

void kpaMSetMutekiTimer(void) {
    MarioWork* mp = marioGetPtr();
    KPA(mp)->mutekiTimer = sysMsec2Frame(0x2EE0);
    MW_S16(mp, 0x78) = 0;
}

void kpaMutekiOff(void) {
    MarioWork* mp = marioGetPtr();
    animPoseSetMaterialFlagOff(mp->paperAnimGroupId[0], 0x80000000);
    KPA(mp)->flags &= ~0x10000;
}

void kpaMutekiOn(void) {
    MarioWork* mp = marioGetPtr();
    animPoseSetMaterialFlagOn(mp->paperAnimGroupId[0], 0x80000000);
    KPA(mp)->flags |= 0x10000;
}

s32 kpaMutekiCheck(void) {
    return (KPA(marioGetPtr())->flags >> 16) & 1;
}

s32 kpaNoHitEnemyCheck(void) {
    return MW_S16(marioGetPtr(), 0x78);
}

void* getKoopaFireEfp(void) {
    return fireefp;
}

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

// locals for symbols with no home header (see REPORT.md)
void psndSFXOff(s32 id);
f32 marioGetFallSpd(void);
void marioLandOn(void);
void* marioChkLandon(f32* pos, f32 spd);
void marioSetPeakYpos(void);
void marioCheckWall(f32 a, f32 b);
void marioMakeJumpPara(void);
void marioPreJump(void);
void marioSetFallPara(void);
void marioBoots(void);
void marioForceMoveMain(void);
void marioChkGnd2(void);
void marioChkGnd(void);
f32 marioChkOverhead(void);
void marioBottomless(void);
void marioCamMain(void);
void marioBalloonMain(void);
void mario_kemuri_80130474(void);
void camFollowYOn(void);
void camFollowYOff(void);
void partyMain(void);
void kpaSetSpec(void);
void* kpaHitCheck(void);
void* kpaFlameHitCheck(f32 x, f32 y, f32 z, f32 rad);
void kpaSearchFrontUnder(void);
void kpaClearHitobjRide(void);
s32 kpaChkHitobjRide(void);
extern f32 marioJumpData[];
void kpa2DMain(void);
void kpa3DMain(void);
void kpaFireAttack(void);
void kpaFireMain(void);

// userdata of the fire effect (fireefp->userdata); nodes are 0x50 bytes apart
typedef struct KpaFireNode {
    u8  unk00[0x04];
    Vec pos;         // 0x04
    u8  unk10[0x1C - 0x10];
    s32 unk1C;       // 0x1C
    u8  unk20[0x40 - 0x20];
    f32 unk40;       // 0x40
    s32 unk44;       // 0x44
    f32 unk48;       // 0x48
    void* unk4C;     // 0x4C
} KpaFireNode;

// Koopa's per-character work, allocated at mario->field_0x28C (size 0x154).
typedef struct KpaWork {
    u32 flags;       // 0x0
    s32 level;       // 0x4
    u8  unk08[0x18 - 0x8];
    void* unk18;     // 0x18
    u8  unk1C[0xD0 - 0x1C];
    s32 unkD0;       // 0xD0
    u8  unkD4[0xD8 - 0xD4];
    s32 unkD8;       // 0xD8
    u8  unkDC[0xE0 - 0xDC];
    s32 unkE0;       // 0xE0
    u8  unkE4[0x138 - 0xE4];
    s32 mutekiTimer; // 0x138
    s32 attackMode;  // 0x13C
    u8  unk140[0x14C - 0x140];
    u32 sfxId;       // 0x14C
} KpaWork;

#define KPA(m) (*(KpaWork**)((u8*)(m) + 0x28C))
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

// NONMATCHING (80%): register allocation differs (compiler keeps extra live ints); constant pool labels cosmetic
void kpaFireMain(void) {
    MarioWork* mp = marioGetPtr();
    f32 sign;
    f32 scale;
    f32 x, y, z;
    f32 f30, f31;
    s32 level;
    s32 i;
    EffectEntry* efp;
    KpaFireNode* root;
    KpaFireNode* node;
    CameraEntry* cam;
    Mtx m0, m1, m2, m3;
    Vec v;
    Vec out;
    void* hit;

    KPA(mp)->unk18 = NULL;
    if (fireefp == NULL) {
        goto cleanup;
    }
    sign = (mp->unk1A8 >= 90.0f && mp->unk1A8 <= 270.0f) ? 1.0f : -1.0f;
    level = 0;
    if (marioGetPtr()->characterId == 2) {
        level = KPA(marioGetPtr())->level;
    }
    scale = baseScaleTbl[level];
    x = 40.0f * sign * scale + mp->position.x;
    if (((MW_U32(marioGetPtr(), 0x14) & 1) ^ 1) == 1) {
        y = mp->position.y + 20.0f * scale + 6.0f;
    } else {
        y = mp->position.y + 20.0f * scale;
    }
    z = mp->position.z;
    efp = fireefp;
    root = efp->userdata;
    root->pos.x = x;
    root->pos.y = y;
    root->pos.z = z;
    if (root->unk4C != NULL) {
        KpaFireNode* sub = ((EffectEntry*)root->unk4C)->userdata;
        sub->pos.x = x;
        sub->pos.y = y;
        sub->pos.z = z;
    }
    level = 0;
    if (marioGetPtr()->characterId == 2) {
        level = KPA(marioGetPtr())->level;
    }
    if (level == 0) {
        goto cleanup;
    }
    efp = fireefp;
    cam = camGetPtr(4);
    root = efp->userdata;
    PSMTXTrans(m0, root->pos.x, root->pos.y, root->pos.z);
    cam = camGetPtr(4);
    PSMTXRotRad(m1, 0x79, (root->unk48 - MW_F32(cam, 0x114)) * 0.017453292f);
    if (root->unk48 > 90.0f && root->unk48 < 270.0f) {
        PSMTXScale(m2, root->unk40, root->unk40, -root->unk40);
    } else {
        PSMTXScale(m2, root->unk40, root->unk40, root->unk40);
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
        f31 = f30 * root->unk40;
        if (f31 < 7.0f) {
            continue;
        }
        PSMTXTrans(m0, node->pos.x, node->pos.y, node->pos.z);
        PSMTXScale(m2, f30, f30, f30);
        PSMTXConcat(m0, m2, m0);
        PSMTXConcat(m3, m0, m3);
        v = vec3_802fc920;
        PSMTXMultVec(m0, &v, &out);
        hit = kpaFlameHitCheck(out.x, out.y, out.z, f31);
        if (hit != NULL) {
            KPA(mp)->unk18 = hit;
            goto cleanup;
        }
    }
cleanup:
    mp = marioGetPtr();
    if (mp->flags & 1) {
        s32 mode = KPA(mp)->attackMode;
        if ((mode >= 0x14 && mode <= 0x17) || (mode >= 0x28 && mode <= 0x2B)) {
            return;
        }
    }
    if (fireefp != NULL) {
        effSoftDelete(fireefp);
        fireefp = NULL;
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

// NONMATCHING (97%): baseSpeed fmadds register and hitobj reload
void kpaFall(void) {
    MarioWork* mp = marioGetPtr();
    if (strcmp(gp->currentMapName, "kpa_03") == 0) {
        f32 spd;
        void* res;
        f32 landY;

        mp->hitobjStandOn = NULL;
        kpaClearHitobjRide();
        spd = marioGetFallSpd();
        if (spd < -20.0f) {
            spd = -20.0f;
        }
        if (spd == 0.0f) {
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
        f32 spd;
        void* res;
        f32 landY;

        if (mp->currMotionId == 0x10 || mp->currMotionId == 0x11) {
            return;
        }
        if (marioKeyOffChk() == 0 || marioCtrlOffChk() == 0) {
            if (!(mp->flags & 0x20)) {
                mp->baseSpeed = 0.01f * (-0.5f - mp->baseSpeed) + mp->baseSpeed;
                if (mp->baseSpeed < 0.0f) {
                    mp->baseSpeed = 0.0f;
                }
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
            if (hitGetAttr((HitEntry*)mp->hitobjStandOn) & 0x200) {
                mp->position.y += spd;
            } else {
                mp->position.y = landY;
            }
        }
        if (res != NULL || kpaChkHitobjRide() != 0) {
            marioLandOn();
        }
    }
}

// NONMATCHING (98%): branch layout for currSubMotionId check (cmpwi + inverse branch)
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
    if (!(s32)mp->currSubMotionId) {
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
    }
    MW_F32(mp, 0x90) += MW_F32(mp, 0x7C);
    mp->airTimer += 1;
}

// NONMATCHING (99%): float temp register choice (f1 vs f2)
void kpaSetFallPara(void) {
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

// NONMATCHING (89%): marioJumpData base address materialization (lfsu vs callee-saved r31)
f32 kpaSetJumpPara(void) {
    f32* jump = marioJumpData;
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

// NONMATCHING (96%): mp callee-saved register (r30 vs r31) and word-copy scheduling
void kpa3DMain(void) {
    MarioWork* mp = marioGetPtr();
    MarioWork* m2;
    KpaWork* kpa;
    void* hit;

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
        MW_U32(mp, 0x1EC) = 0;
        MW_U32(mp, 0x1F0) = 0;
        return;
    }
    marioItemGetChk();
    kpaFireAttack();
    kpaFireMain();
    mp = marioGetPtr();
    kpa = KPA(mp);
    if (kpa->mutekiTimer > 0) {
        if (--kpa->mutekiTimer <= 0) {
            KPA(mp)->mutekiTimer = 0;
            m2 = marioGetPtr();
            animPoseSetMaterialFlagOff(m2->paperAnimGroupId[0], 0x80000000);
            KPA(m2)->flags &= ~0x10000;
        }
    }
    if (mp->currMotionId == 8) {
        MW_U32(mp, 0x110) = MW_U32(mp, 0x8C);
        MW_U32(mp, 0x114) = MW_U32(mp, 0x90);
        MW_U32(mp, 0x118) = MW_U32(mp, 0x94);
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
    MW_U32(mp, 0x110) = MW_U32(mp, 0x8C);
    MW_U32(mp, 0x114) = MW_U32(mp, 0x90);
    MW_U32(mp, 0x118) = MW_U32(mp, 0x94);
    MW_F32(mp, 0x174) = 0.0f;
    MW_F32(mp, 0x178) = 0.0f;
    MW_F32(mp, 0x17C) = 0.0f;
}

// NONMATCHING (97%): mp callee-saved register (r30 vs r31) and pos/delta stack slots
void kpa2DMain(void) {
    MarioWork* mp = marioGetPtr();
    MarioWork* m2;
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
    MW_U32(mp, 0x1EC) = 0;
    MW_U32(mp, 0x1F0) = 0;
    marioItemGetChk();
    kpaFireAttack();
    kpaFireMain();
    mp = marioGetPtr();
    kpa = KPA(mp);
    if (kpa->mutekiTimer > 0) {
        if (--kpa->mutekiTimer <= 0) {
            KPA(mp)->mutekiTimer = 0;
            m2 = marioGetPtr();
            animPoseSetMaterialFlagOff(m2->paperAnimGroupId[0], 0x80000000);
            KPA(m2)->flags &= ~0x10000;
        }
    }
    if (mp->currMotionId == 8) {
        MW_U32(mp, 0x110) = MW_U32(mp, 0x8C);
        MW_U32(mp, 0x114) = MW_U32(mp, 0x90);
        MW_U32(mp, 0x118) = MW_U32(mp, 0x94);
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
            f31 = mp->controlStickSensitivity * mp->playerGravity * 0.015f;
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
    MW_U32(mp, 0x110) = MW_U32(mp, 0x8C);
    MW_U32(mp, 0x114) = MW_U32(mp, 0x90);
    MW_U32(mp, 0x118) = MW_U32(mp, 0x94);
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

// NONMATCHING (95%): extra callee-saved register for initial mp
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
    KPA(marioGetPtr())->sfxId = 0xFFFFFFFFu;
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
        if (mp->position.y + MW_F32(mp, 0x1B8) < 370.0f) {
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

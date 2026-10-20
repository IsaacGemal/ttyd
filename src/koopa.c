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
#include "eff/eff_gonbaba_breath.h"
#include "koopa_hit.h"

// locals for symbols with no home header
extern f32 marioJumpData[];

#define MW_U8(m, off)  (*(u8*)((u8*)(m) + (off)))
#define MW_U16(m, off) (*(u16*)((u8*)(m) + (off)))
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

// Defined after the code: these follow the breath helpers' eight pooled floats,
// before the remaining deferred functions' literals. Early helper references need
// the section declared here, too, or MWCC places their constants in .sdata.
#ifdef MUST_MATCH
__declspec(section ".sdata2") extern const f32 float_deg2rad_804203d8;
__declspec(section ".sdata2") extern const f32 float_7_804203dc;
__declspec(section ".sdata2") extern const f32 float_0p4_804203e0;
__declspec(section ".sdata2") extern const f32 float_0p3_804203e4;
__declspec(section ".sdata2") extern const f32 float_1p4_804203e8;
__declspec(section ".sdata2") extern const f32 float_1p5_804203ec;
__declspec(section ".sdata2") extern const f32 float_neg0p05_804203f0;
__declspec(section ".sdata2") extern const f32 float_neg0p08_804203f4;
__declspec(section ".sdata2") extern const f32 float_neg0p006_804203f8;
__declspec(section ".sdata2") extern const f32 float_neg0p002_804203fc;
__declspec(section ".sdata2") extern const f32 float_neg0p1_80420400;
__declspec(section ".sdata2") extern const f32 float_neg0p2_80420404;
__declspec(section ".sdata2") extern const f32 float_neg0p024_80420408;
__declspec(section ".sdata2") extern const f32 float_neg0p01_8042040c;
#else
#define float_deg2rad_804203d8 0.017453292f
#define float_7_804203dc 7.0f
#define float_0p4_804203e0 0.4f
#define float_0p3_804203e4 0.3f
#define float_1p4_804203e8 1.4f
#define float_1p5_804203ec 1.5f
#define float_neg0p05_804203f0 -0.05f
#define float_neg0p08_804203f4 -0.08f
#define float_neg0p006_804203f8 -0.006f
#define float_neg0p002_804203fc -0.002f
#define float_neg0p1_80420400 -0.1f
#define float_neg0p2_80420404 -0.2f
#define float_neg0p024_80420408 -0.024f
#define float_neg0p01_8042040c -0.01f
#endif

// The original inlines this breath spawn into kpaFireAttack's four breath states; keep it one helper so the four
// copies share a body.
// This copy (one declarator `scale, x`) drives the first breath block; the `#ifdef` twin below drives the
// other three and exists only to give MWCC the declaration order it needs for the right FPR colouring.
#ifdef MUST_MATCH
// An ordinary, immediately generated helper pools the i2f double before the deferred
// functions' strings. Its calls still inline; the linker strips the unused body.
#pragma defer_codegen off // Pool the conversion double before kpaFireAttack's strings.
static void kpaFireBreath(MarioWork* mp, Vec* pos) {
#else
static inline void kpaFireBreath(MarioWork* mp, Vec* pos) {
#endif
    f32 sign;
    f32 scale, x;
    f32 y;
    f32 z;
    f32 escale;
    s32 level;
    s32 type;

    sign = (mp->unk1A8 >= 90.0f && mp->unk1A8 <= 270.0f) ? 1.0f : -1.0f;
    level = 0;
    {
        MarioWork* m2 = marioGetPtr();
        if (m2->characterId == 2) {
            level = KPA(m2)->level;
        }
    }
    scale = baseScaleTbl[level];
    x = 40.0f * sign * scale + pos->x;
    {
        MarioWork* mp2 = marioGetPtr();
        f32 py = pos->y;
        f32 off = (((MW_S32(mp2, 0x14) & 1) ^ 1) == 1) ? 6.0f : 0.0f;
        y = py + (20.0f * scale + off);
    }
    z = pos->z;
    type = 7;
    escale = float_0p4_804203e0;
    switch (KPA(mp)->level) {
    case 0:
        escale = float_0p3_804203e4;
        break;
    case 1:
        escale = 1.0f;
        break;
    case 2:
        escale = float_1p4_804203e8;
        break;
    case 3:
        escale = float_1p5_804203ec;
        type = 0;
        break;
    }
    fireefp = effGonbabaBreathEntry(type, 0, x, y, z, escale);
    ((effGonbabaBreathData*)((EffectEntry*)fireefp)->userdata)->rotation =
        (mp->dispReqDir >= 90.0f && mp->dispReqDir <= 270.0f) ? 0xB4 : 0;
    if (((effGonbabaBreathData*)((EffectEntry*)fireefp)->userdata)->child != NULL) {
        ((effGonbabaBreathData*)((EffectEntry*)((effGonbabaBreathData*)((EffectEntry*)fireefp)->userdata)->child)->userdata)->rotation =
            (mp->dispReqDir >= 90.0f && mp->dispReqDir <= 270.0f) ? 0xB4 : 0;
    }
}

#ifdef MUST_MATCH
// A second identical body with the two floats declared on separate lines: MWCC colours callee-saved FPRs
// per inlined copy, and the first breath block wants `scale=f30, x=f31` (one declarator) while the other
// three want the opposite declaration order. The two helpers exist only to give each copy the order it
// needs; both are semantically identical and the call sites stay the same as the original's four.
static inline void kpaFireBreath2(MarioWork* mp, Vec* pos) {
    f32 sign;
    f32 x;
    f32 scale;
    f32 y;
    f32 z;
    f32 escale;
    s32 level;
    s32 type;

    sign = (mp->unk1A8 >= 90.0f && mp->unk1A8 <= 270.0f) ? 1.0f : -1.0f;
    level = 0;
    {
        MarioWork* m2 = marioGetPtr();
        if (m2->characterId == 2) {
            level = KPA(m2)->level;
        }
    }
    scale = baseScaleTbl[level];
    x = 40.0f * sign * scale + pos->x;
    {
        MarioWork* mp2 = marioGetPtr();
        f32 py = pos->y;
        f32 off = (((MW_S32(mp2, 0x14) & 1) ^ 1) == 1) ? 6.0f : 0.0f;
        y = py + (20.0f * scale + off);
    }
    z = pos->z;
    type = 7;
    escale = float_0p4_804203e0;
    switch (KPA(mp)->level) {
    case 0:
        escale = float_0p3_804203e4;
        break;
    case 1:
        escale = 1.0f;
        break;
    case 2:
        escale = float_1p4_804203e8;
        break;
    case 3:
        escale = float_1p5_804203ec;
        type = 0;
        break;
    }
    fireefp = effGonbabaBreathEntry(type, 0, x, y, z, escale);
    ((effGonbabaBreathData*)((EffectEntry*)fireefp)->userdata)->rotation =
        (mp->dispReqDir >= 90.0f && mp->dispReqDir <= 270.0f) ? 0xB4 : 0;
    if (((effGonbabaBreathData*)((EffectEntry*)fireefp)->userdata)->child != NULL) {
        ((effGonbabaBreathData*)((EffectEntry*)((effGonbabaBreathData*)((EffectEntry*)fireefp)->userdata)->child)->userdata)->rotation =
            (mp->dispReqDir >= 90.0f && mp->dispReqDir <= 270.0f) ? 0xB4 : 0;
    }
}
#else
#define kpaFireBreath2(m, pos) kpaFireBreath(m, pos)
#endif

#ifdef MUST_MATCH
#pragma defer_codegen reset // Restore reverse-source emission for the public functions.
#endif

void* getKoopaFireEfp(void) {
    return fireefp;
}

s32 kpaNoHitEnemyCheck(void) {
    return marioGetPtr()->noHitEnemyCt;
}

s32 kpaMutekiCheck(void) {
    return (KPA(marioGetPtr())->flags & KPA_FLAG_MUTEKI) != 0;
}

void kpaMutekiOn(void) {
    MarioWork* mp = marioGetPtr();
    animPoseSetMaterialFlagOn(mp->paperAnimGroupId[0], 0x80000000);
    KPA(mp)->flags |= KPA_FLAG_MUTEKI;
}

void kpaMutekiOff(void) {
    MarioWork* mp = marioGetPtr();
    animPoseSetMaterialFlagOff(mp->paperAnimGroupId[0], 0x80000000);
    KPA(mp)->flags &= ~KPA_FLAG_MUTEKI;
}

void kpaMSetMutekiTimer(void) {
    MarioWork* mp = marioGetPtr();
    KPA(mp)->mutekiTimer = sysMsec2Frame(0x2EE0);
    mp->noHitEnemyCt = 0;
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
// then kpaPreInit's kpa_02/kpa_04/kpa_05/kpa_06, matching the target .rodata.
void kpaPreInit(void) {
    MarioWork* mp = marioGetPtr();

    if (KPA(mp) == NULL) {
        KPA(mp) = __memAlloc(0, 0x154);
    }
    memset(KPA(mp), 0, 0x154);
    KPA(mp)->level = 1;
    KPA(mp)->moveRate = 1;
    KPA(mp)->unkE0 = 0;
    KPA(mp)->unkD8 = 1;
    mp = marioGetPtr();
    KPA(mp)->sfxId = KPA_SFX_NONE;
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
    if (mp->noHitEnemyCt != 0) {
        mp->noHitEnemyCt--;
    }
    mp->hitobjHead = NULL;
    mp->hitobjFront = NULL;
    mp->actionHitObj = NULL;
    mp->hitobjPush = NULL;
    mp->hitobjWall = NULL;
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
            KPA(m2)->flags &= ~KPA_FLAG_MUTEKI;
        }
    }
    if (mp->currMotionId == MARIO_MOTION_JUMP_3) {
        prevPos = &mp->prevPosition;
    *prevPos = mp->position;
        mp->riderDelta.x = 0.0f;
        mp->riderDelta.y = 0.0f;
        mp->riderDelta.z = 0.0f;
        marioBottomless();
        marioPreDisp();
        mario_kemuri_80130474();
        marioCamMain();
        mp->field_0x34++;
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
    mp->position.y += mp->riderDelta.y;
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
        if (mp->currMotionId != MARIO_MOTION_HIP && mp->currMotionId != MARIO_MOTION_HIP_2 && mp->airTimer > 5) {
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
    mp->field_0x34++;
    prevPos = &mp->prevPosition;
    *prevPos = mp->position;
    mp->riderDelta.x = 0.0f;
    mp->riderDelta.y = 0.0f;
    mp->riderDelta.z = 0.0f;
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
        mp->hitobjHead = hit;
    }
    if (mp->noHitEnemyCt != 0) {
        mp->noHitEnemyCt--;
    }
    if (mp->currMotionId == MARIO_MOTION_PLANE || mp->currMotionId == MARIO_MOTION_DAMAGE ||
        mp->currMotionId == MARIO_MOTION_DAMAGE_TOGE) {
        marioPreDisp();
        marioBottomless();
        mp->field_0x34++;
        partyMain();
        marioCamMain();
        mp->hitobjHead = NULL;
        mp->hitobjFront = NULL;
        mp->actionHitObj = NULL;
        mp->hitobjPush = NULL;
        mp->hitobjWall = NULL;
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
            KPA(m2)->flags &= ~KPA_FLAG_MUTEKI;
        }
    }
    if (mp->currMotionId == MARIO_MOTION_JUMP_3) {
        prevPos = &mp->prevPosition;
    *prevPos = mp->position;
        mp->riderDelta.x = 0.0f;
        mp->riderDelta.y = 0.0f;
        mp->riderDelta.z = 0.0f;
        marioBottomless();
        marioPreDisp();
        marioBalloonMain();
        mario_kemuri_80130474();
        partyMain();
        marioCamMain();
        mp->field_0x34++;
        return;
    }
    if (mp->flags & 0x40000) {
        marioMove();
        marioChkOverhead();
        if (mp->airTimer > 0x64) {
            marioChgMot(MARIO_MOTION_FALL);
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
        if (mp->currMotionId != MARIO_MOTION_HIP && mp->currMotionId != MARIO_MOTION_HIP_2 && mp->airTimer > 5) {
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
    mp->field_0x34++;
    prevPos = &mp->prevPosition;
    *prevPos = mp->position;
    mp->riderDelta.x = 0.0f;
    mp->riderDelta.y = 0.0f;
    mp->riderDelta.z = 0.0f;
}

f32 kpaSetJumpPara(void) {
    f32* const jump = marioJumpData;
    MarioWork* mp = marioGetPtr();
    mp->jumpVelY = jump[0];
    mp->jumpAccelY = jump[1];
    mp->jumpJerkY = jump[2];
    mp->jumpJerkStepY = jump[3];
    switch (KPA(mp)->level) {
        case 0:
            mp->jumpVelY = float_7_804203dc;
            mp->jumpAccelY = -0.4f;
            mp->jumpJerkY = 0.08f;
            mp->jumpJerkStepY = -0.02f;
            break;
        case 1:
            mp->jumpVelY = 8.1f;
            mp->jumpAccelY = -0.3f;
            mp->jumpJerkY = 0.08f;
            mp->jumpJerkStepY = -0.02f;
            break;
        case 2:
            mp->jumpVelY = 8.4f;
            mp->jumpAccelY = float_neg0p1_80420400;
            mp->jumpJerkY = 0.08f;
            mp->jumpJerkStepY = -0.018f;
            break;
        case 3:
            mp->jumpVelY = 10.5f;
            mp->jumpAccelY = -0.12f;
            mp->jumpJerkY = 0.08f;
            mp->jumpJerkStepY = -0.018f;
            break;
    }
    return mp->jumpVelY;
}

// The original reserves f1 for a float return value (the float temps start at f2),
// so the source declared this returning f32 even though it stores and returns nothing.
f32 kpaSetFallPara(void) {
    MarioWork* mp = marioGetPtr();
    if (mp->buttonHeld & 0x100) {
        mp->jumpVelY = float_neg0p05_804203f0;
        mp->jumpAccelY = float_neg0p08_804203f4;
        mp->jumpJerkY = float_neg0p006_804203f8;
        mp->jumpJerkStepY = float_neg0p002_804203fc;
    } else {
        mp->jumpVelY = float_neg0p1_80420400;
        mp->jumpAccelY = float_neg0p2_80420404;
        mp->jumpJerkY = float_neg0p024_80420408;
        mp->jumpJerkStepY = float_neg0p01_8042040c;
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
        mp->jumpVelY = 2.6f;
        mp->jumpAccelY = -0.06f;
        mp->jumpJerkY = float_neg0p01_8042040c;
        mp->jumpJerkStepY = float_neg0p002_804203fc;
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
        if (mp->jumpVelY <= 0.0f) {
            if (!(mp->flags & 0x2000)) {
                if (mp->characterId == 0) {
                    marioChgPose("M_J_1C");
                }
            }
            marioSetPeakYpos();
            mp->jumpAccelY = -0.06f;
            mp->jumpJerkY = -0.004f;
            mp->jumpJerkStepY = float_neg0p002_804203fc;
            mp->flags &= ~0x10000;
            mp->flags |= 0x20000;
        }
        break;
    }
    mp->position.y += mp->jumpVelY;
    mp->airTimer += 1;
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
            if (mp2->buttonHeld & 0x100) {
                mp2->jumpVelY = float_neg0p05_804203f0;
                mp2->jumpAccelY = float_neg0p08_804203f4;
                mp2->jumpJerkY = float_neg0p006_804203f8;
                mp2->jumpJerkStepY = float_neg0p002_804203fc;
            } else {
                mp2->jumpVelY = float_neg0p1_80420400;
                mp2->jumpAccelY = float_neg0p2_80420404;
                mp2->jumpJerkY = float_neg0p024_80420408;
                mp2->jumpJerkStepY = float_neg0p01_8042040c;
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
        if (mp->currMotionId == MARIO_MOTION_HIP || mp->currMotionId == MARIO_MOTION_HIP_2) {
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
        if (mp->currMotionId != MARIO_MOTION_JUMP_NPC) {
            if (mp->buttonHeld & 0x100) {
                mp->jumpJerkStepY = -0.001f;
            } else {
                mp->jumpJerkStepY = float_neg0p01_8042040c;
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
        marioChgMot(MARIO_MOTION_STAY);
        KPA(mp)->attackMode = 1;
    }
}

void kpaAttackStart2(void) {
    KpaWork* kpa = KPA(marioGetPtr());
    if (kpa->attackMode == 0) {
        kpa->attackMode = 0x1E;
    }
}

void kpaFireAttack(void) {
    MarioWork* mp = marioGetPtr();
    Vec pos16, pos17, pos2A, pos2B;
    Vec pos216, pos217, pos22A, pos22B;

    switch (KPA(mp)->attackMode) {
    case 1:
        mp->flags &= ~0xF0000;
        mp->airTimer = 0;
        KPA(mp)->unkD8 = (KPA(mp)->level == 0) ? 0xC : 8;
        KPA(mp)->unkE0 = 0;
        KPA(mp)->attackTimer = 0;
        KPA(mp)->attackMode = 0x14;
        if (!(mp->flags & 0x20)) {
            mp->baseSpeed = 0.0f;
            mp->kpaSpeed = 0.0f;
        }
        // fallthrough
    case 0x14:
        kpaChgPose("KPA_A_1A", "KPA2_A_1A");
        KPA(mp)->attackTimer = 6;
        KPA(mp)->attackMode = 0x15;
        break;
    case 0x15:
        if (--KPA(mp)->attackTimer <= 0) {
            kpaChgPose("KPA_A_1B", "KPA2_A_1B");
            KPA(mp)->attackTimer = 8;
            KPA(mp)->attackMode = 0x16;
            {
                MarioWork* m = marioGetPtr();
                if (KPA(m)->sfxId == KPA_SFX_NONE) {
                    KPA(m)->sfxId = psndSFXOn_3D((const char*)0x821, &m->position);
                    psndSFXOn_3D((const char*)0x131, &m->position);
                }
            }
        }
        break;
    case 0x16: {
        MarioWork* m;
        pos16 = mp->position;
        m = marioGetPtr();
        pos216 = pos16;
        m = marioGetPtr();
        if (fireefp == NULL) {
            kpaFireBreath(m, &pos216);
        }
        if (--KPA(mp)->attackTimer <= 0) {
            KPA(mp)->attackTimer = sysMsec2Frame(0x190);
            kpaChgPose("KPA_A_1C", "KPA2_A_1C");
            KPA(mp)->attackMode = 0x17;
        }
        break;
    }
    case 0x17: {
        MarioWork* m;
        pos17 = mp->position;
        m = marioGetPtr();
        pos217 = pos17;
        m = marioGetPtr();
        if (fireefp == NULL) {
            kpaFireBreath2(m, &pos217);
        }
        if (--KPA(mp)->attackTimer <= 0) {
            if (!(mp->buttonHeld & 0x200) && KPA(mp)->field_0x144 == 0) {
                MarioWork* m;
                kpaChgPose("KPA_A_1D", "KPA2_A_1D");
                KPA(mp)->attackMode = 0x18;
                KPA(mp)->attackTimer = 6;
                m = marioGetPtr();
                if (KPA(m)->sfxId != KPA_SFX_NONE) {
                    psndSFXOff(KPA(m)->sfxId);
                    KPA(m)->sfxId = KPA_SFX_NONE;
                }
            }
        }
        break;
    }
    case 0x18:
        if (--KPA(mp)->attackTimer <= 0) {
            KPA(mp)->attackMode = 0;
            kpaChgPose("KPA_S_1", "KPA2_S_1");
        }
        break;
    default:
        break;
    }

    switch (KPA(mp)->attackMode) {
    case 0x1E:
        KPA(mp)->unkD8 = (KPA(mp)->level == 0) ? 0xC : 8;
        KPA(mp)->unkE0 = 0;
        KPA(mp)->attackTimer = 0;
        KPA(mp)->attackMode = 0x28;
        if (!(mp->flags & 0x20)) {
            mp->baseSpeed = 0.0f;
            mp->kpaSpeed = 0.0f;
        }
        // fallthrough
    case 0x28:
        kpaChgPose("KPA_A_1A", "KPA2_A_1A");
        KPA(mp)->attackTimer = 6;
        KPA(mp)->attackMode = 0x29;
        break;
    case 0x29:
        if (--KPA(mp)->attackTimer <= 0) {
            kpaChgPose("KPA_A_1B", "KPA2_A_1B");
            KPA(mp)->attackTimer = 8;
            KPA(mp)->attackMode = 0x2A;
            {
                MarioWork* m = marioGetPtr();
                if (KPA(m)->sfxId == KPA_SFX_NONE) {
                    KPA(m)->sfxId = psndSFXOn_3D((const char*)0x821, &m->position);
                    psndSFXOn_3D((const char*)0x131, &m->position);
                }
            }
        }
        break;
    case 0x2A: {
        MarioWork* m;
        pos2A = mp->position;
        m = marioGetPtr();
        pos22A = pos2A;
        m = marioGetPtr();
        if (fireefp == NULL) {
            kpaFireBreath2(m, &pos22A);
        }
        if (--KPA(mp)->attackTimer <= 0) {
            KPA(mp)->attackTimer = sysMsec2Frame(0x190);
            kpaChgPose("KPA_A_1C", "KPA2_A_1C");
            KPA(mp)->attackMode = 0x2B;
        }
        break;
    }
    case 0x2B: {
        MarioWork* m;
        pos2B = mp->position;
        m = marioGetPtr();
        pos22B = pos2B;
        m = marioGetPtr();
        if (fireefp == NULL) {
            kpaFireBreath2(m, &pos22B);
        }
        if (--KPA(mp)->attackTimer <= 0) {
            if (!(mp->buttonHeld & 0x200) && KPA(mp)->field_0x144 == 0) {
                MarioWork* m;
                kpaChgPose("KPA_A_1D", "KPA2_A_1D");
                KPA(mp)->attackMode = 0x2C;
                KPA(mp)->attackTimer = 6;
                m = marioGetPtr();
                if (KPA(m)->sfxId != KPA_SFX_NONE) {
                    psndSFXOff(KPA(m)->sfxId);
                    KPA(m)->sfxId = KPA_SFX_NONE;
                }
            }
        }
        break;
    }
    case 0x2C:
        if (--KPA(mp)->attackTimer <= 0) {
            KPA(mp)->attackMode = 0;
            switch (mp->currMotionId) {
            case MARIO_MOTION_STAY:
                kpaChgPose("KPA_S_1", "KPA2_S_1");
                break;
            case MARIO_MOTION_WALK:
                kpaChgPose("KPA_W_1", "KPA2_W_1");
                break;
            case MARIO_MOTION_DASH:
                kpaChgPose("KPA_R_1", "KPA2_R_1");
                break;
            case MARIO_MOTION_KPA_SWIM:
                kpaChgPose("KPA_S_1", "KPA2_Z_2");
                break;
            }
        }
        break;
    default:
        break;
    }
}

void kpaCancelAttackMode(void) {
    MarioWork* mp;
    mp = marioGetPtr();
    KPA(mp)->attackMode = 0;
    mp = marioGetPtr();
    if (KPA(mp)->sfxId != KPA_SFX_NONE) {
        psndSFXOff(KPA(mp)->sfxId);
        KPA(mp)->sfxId = KPA_SFX_NONE;
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
        if (KPA(mp)->sfxId != KPA_SFX_NONE) {
            psndSFXOff(KPA(mp)->sfxId);
            KPA(mp)->sfxId = KPA_SFX_NONE;
        }
    }
}

// Reusing the work pointer as the node cursor (`root++` before the loop and in the for-increment)
// instead of a separate `node` pointer makes mwcc schedule the preheader `li r28,1` before
// `addi r29,r29,0x50`, as the original does; a separate cursor emits them in the opposite order
// (same shape as kpaEnemyFireHitChk in koopa_hit.c).
void kpaFireMain(void) {
    MarioWork* mp = marioGetPtr();
    f32 sign;
    f32 scale;
    f32 x, y, z;
    f32 f30, f31;
    s32 level;
    EffectEntry* efp;
    KpaFireNode* root;
    s32 i;
    Mtx m2, m0, m1, m3; // declaration order puts the matrices in the original's stack slots
    Vec out;
    Vec v;
    void* hit;
    f32 rootScale;
    f32 py;
    f32 off;
    MarioWork* mp2;

    KPA(mp)->hitFlame = NULL;
    if (fireefp == NULL) {
        goto cleanup;
    }
    sign = (mp->unk1A8 >= 90.0f && mp->unk1A8 <= 270.0f) ? 1.0f : -1.0f;
    level = kpaGetLevel();
    scale = baseScaleTbl[level];
    x = 40.0f * sign * scale + mp->position.x;
    // Taking the flags pointer into a named temp and loading position.y between that call and
    // the ternary keeps the 0x90 load and 20.0f in the pre-branch block, as the original does; the inline
    // form pulls both after the branch.
    mp2 = marioGetPtr();
    py = mp->position.y;
    off = (((MW_S32(mp2, 0x14) & 1) ^ 1) == 1) ? 6.0f : 0.0f;
    y = py + (20.0f * scale + off);
    z = mp->position.z;
    ((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->pos.x = x;
    ((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->pos.y = y;
    ((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->pos.z = z;
    if (((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->nextEfp != NULL) {
        ((KpaFireNode*)((EffectEntry*)((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->nextEfp)->userdata)->pos.x = x;
        ((KpaFireNode*)((EffectEntry*)((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->nextEfp)->userdata)->pos.y = y;
        ((KpaFireNode*)((EffectEntry*)((KpaFireNode*)((EffectEntry*)fireefp)->userdata)->nextEfp)->userdata)->pos.z = z;
    }
    level = kpaGetLevel();
    if (level == 0) {
        goto cleanup;
    }
    efp = fireefp;
    camGetPtr(4);
    root = efp->userdata;
    rootScale = root->radius;
    PSMTXTrans(m0, root->pos.x, root->pos.y, root->pos.z);
    PSMTXRotRad(m1, 0x79, (root->yaw - camGetPtr(4)->viewYaw) * float_deg2rad_804203d8);
    if (root->yaw > 90.0f && root->yaw < 270.0f) {
        PSMTXScale(m2, root->radius, root->radius, -root->radius);
    } else {
        PSMTXScale(m2, root->radius, root->radius, root->radius);
    }
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m3);
    i = 1;
    root++;
    for (; i < efp->count; i++, root++) {
        if ((i - 1) & 3) {
            continue;
        }
        if (root->noHit != 0) {
            continue;
        }
        if (root->unk1C <= 0x10) {
            continue;
        }
        f30 = float_7_804203dc * root->radius;
        f31 = f30 * rootScale;
        if (f31 < float_7_804203dc) {
            continue;
        }
        PSMTXTrans(m0, root->pos.x, root->pos.y, root->pos.z);
        PSMTXScale(m2, f30, f30, f30);
        PSMTXConcat(m0, m2, m0);
        PSMTXConcat(m3, m0, m0);
        v = vec3_802fc920;
        PSMTXMultVec(m0, &v, &out);
        hit = kpaFlameHitCheck(out.x, out.y, out.z, f31);
        if (hit != NULL) {
            KPA(mp)->hitFlame = hit;
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
    if (mp->currMotionId == MARIO_MOTION_HIP_2) {
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
__declspec(section ".sdata2") const f32 float_deg2rad_804203d8 = 0.017453292f;
__declspec(section ".sdata2") const f32 float_7_804203dc = 7.0f;
__declspec(section ".sdata2") const f32 float_0p4_804203e0 = 0.4f;
__declspec(section ".sdata2") const f32 float_0p3_804203e4 = 0.3f;
__declspec(section ".sdata2") const f32 float_1p4_804203e8 = 1.4f;
__declspec(section ".sdata2") const f32 float_1p5_804203ec = 1.5f;
__declspec(section ".sdata2") const f32 float_neg0p05_804203f0 = -0.05f;
__declspec(section ".sdata2") const f32 float_neg0p08_804203f4 = -0.08f;
__declspec(section ".sdata2") const f32 float_neg0p006_804203f8 = -0.006f;
__declspec(section ".sdata2") const f32 float_neg0p002_804203fc = -0.002f;
__declspec(section ".sdata2") const f32 float_neg0p1_80420400 = -0.1f;
__declspec(section ".sdata2") const f32 float_neg0p2_80420404 = -0.2f;
__declspec(section ".sdata2") const f32 float_neg0p024_80420408 = -0.024f;
__declspec(section ".sdata2") const f32 float_neg0p01_8042040c = -0.01f;
#endif

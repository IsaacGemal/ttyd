#include "driver/npcdrv.h"
#include "dolphin/gx/GXStruct.h"
#include "data/mapdata.h"
#include "driver/dispdrv.h"
#include "driver/offscreendrv.h"
#include "driver/animdrv.h"
#include "driver/swdrv.h"
#include "evt/evt_cmd.h"
#include "driver/effdrv.h"
#include "driver/itemdrv.h"
#include "driver/msgdrv.h"
#include "mario/mario.h"
#include "mario/mario_hit.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "party.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "system.h"
#include "manager/fontmgr.h"
#include "driver/windowdrv.h"
#include <string.h>
#include "koopa.h"
#include "koopa_hit.h"
#include "koopa_motion.h"
#include "eff/eff_majinai.h"
#include "kpa_act.h"
#include "party/party_cloud.h"
#include "math_ext.h"
#include "eff/eff_small_star.h"
#include "pmario_sound.h"
#include "mario/mario_sbr.h"
#include "mario/mario_motion.h"
#include "battle/battle.h"
#include "driver/seqdrv.h"
#include "mario/mario_pouch.h"
#include "motion/mot_hammer.h"
#include "motion/mot_jabara.h"
#include "party/party_nokonoko.h"
#include "party/party_vivian.h"
#include "driver/hitdrv.h"
#include "battle/battle_sub.h"
#include "driver/shadowdrv.h"

// This file's radians-to-degrees factor is 180/3.141592 (0x42652EE4), not the SDK's 57.29577951f.
#define NPC_RAD2DEG(r) ((r) * (180.0f / 3.141592f))

extern char _evt_majinai_coin_up_effect[];

extern GlobalWork* gp;
extern NpcTribeInfo npcTribe[];
extern char MarioStayPose[32];
extern char MarioTalkPose[32];
#ifdef MUST_MATCH
// Unit .rodata head (target order); MWCC's generated double pool follows these at +0x1c0.
const f32 vec3_802c2f78[3] = { 0.0f, 0.0f, 0.0f };
const f32 vec3_802c2f84[3] = { 1.0f, 1.0f, 1.0f };
const f32 vec3_802c2f90[3] = { 0.0f, 0.0f, 0.0f };
#endif
// This unit's float rodata (the names are the original .sdata2 symbols). Naming the constants lets MWCC
// hoist the inlined sin/cos coefficient loads once per instance as retail does; the natural literals are
// under the #else.
#ifdef MUST_MATCH
extern const f32 float_0p00761_80419c3c;
extern const f32 float_0p16605_80419c40;
extern const f32 float_3p1416_80419c44;
extern const f32 float_1p5708_80419c48;
extern const f32 float_4p7124_80419c4c;
extern const f32 float_1_80419bb4;
extern const f32 float_0_80419b9c;
extern u32 dat_80419b88;
extern const f32 float_0p03705_80419c50;
extern const f32 float_0p4967_80419c54;
#define NPC_F_SIN0 float_0p00761_80419c3c
#define NPC_F_SIN1 float_0p16605_80419c40
#define NPC_F_COS0 float_0p03705_80419c50
#define NPC_F_COS1 float_0p4967_80419c54
#define NPC_F_PI float_3p1416_80419c44
#define NPC_F_HALFPI float_1p5708_80419c48
#define NPC_F_THREEHALVES float_4p7124_80419c4c
#define NPC_F_ONE float_1_80419bb4
#define NPC_F_ZERO float_0_80419b9c
#define NPC_COLOR_DAT dat_80419b88
#else
#define NPC_F_SIN0 0.00761f
#define NPC_F_SIN1 0.16605f
#define NPC_F_COS0 0.03705f
#define NPC_F_COS1 0.4967f
#define NPC_F_PI 3.1415927f
#define NPC_F_HALFPI 1.5707964f
#define NPC_F_THREEHALVES 4.712389f
#define NPC_F_ONE 1.0f
#define NPC_F_ZERO 0.0f
#define NPC_COLOR_DAT 0xFFFFFF00u
#endif

int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

// Prototypes for the forward references the original function order needs.
static inline f32 npcMain_sqrtf(f32 x, const f64* pool);

static inline s32 __fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_NAN;
            } else {
                return FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
                return FP_ZERO;
            }
            break;
        default:
            return FP_NORMAL;
    }
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

typedef struct NpcWork2 {
    NpcWork field;  // 0x0
    NpcWork battle; // 0x14
} NpcWork2;

//.data
char* luigi_pose[14][2] = { { "Z_1", "Z_1R" }, { "S_1", "S_1R" },   { "W_1", "W_1R" },   { "R_1", "R_1R" },
                            { "T_1", "T_1R" }, { "J_1A", "J_1RR" }, { "J_1B", "J_1BR" }, { "Z_2", "Z_2R" },
                            { "S_2", "S_2R" }, { "T_2", "T_2R" },   { "L_1", "L_1R" },   { "L_2", "L_2R" },
                            { "L_3", "L_3R" }, { "I_1", "I_1R" } };

//.sbss
BOOL dbg_battleskip;
u32 npcMainCount;
NpcFieldEntry* release_wp;
static NpcWork2 work;

// local prototypes

#define npcGetWorkPtr2(inBattle) ((inBattle) ? &work.battle : &work.field)
// TODO: inlining?
#define npcGetWorkPtrInline() (gp->inBattle ? &work.battle : &work.field)

extern const f32 float_365_80419ba8;
extern const f32 float_0_80419b9c;
extern const char str_802C30FC[];
extern const char str_802C3128[];
extern const f32 float_0p5_80419ba0;
extern const f32 float_180_80419bec;
extern const f32 float_90_80419bd0;
extern const f32 float_270_80419be4;
extern const f32 float_0p75_80419bf4;
extern const f32 float_neg1_80419bf8;
extern const f32 float_1_80419bb4;
extern const f32 float_100_80419c18;
extern const f32 float_0p1_80419bfc;
extern const f32 float_0p05_80419c78;
extern const f32 float_0p25_80419c7c;
extern const f32 float_neg2_80419c80;
extern const f32 float_2_80419c5c;
extern const f32 float_5000_80419c84;
extern const f32 float_10000_80419c88;
extern const f32 float_3_80419c08;
extern const f32 float_1p33_80419c8c;
extern const f32 float_neg600_80419c0c;
extern const f32 float_600_80419c10;
extern const f32 float_60_80419ba4;
#ifdef MUST_MATCH
// The original reaches these strings and the 0.75 double through the sqrtf double pool (vec3_802c2f78 + 0x1c0), one
// register for all of them: "kpa_03" and the SFX name sit just before the pool, 0.75 at +0xa0.
#define NPCMAIN_STR_KPA03 ((char*)pool - 0x34)
#define NPCMAIN_STR_FALL ((char*)pool - 0x2c)
#define NPCMAIN_SQRTF(x) npcMain_sqrtf((x), pool)
#define NPCMAIN_0P75 (pool[20])
#else
#define NPCMAIN_STR_KPA03 "kpa_03"
#define NPCMAIN_STR_FALL "SFX_KUPPA_3RD_WATER_FALL1"
#define NPCMAIN_SQRTF(x) sqrtf(x)
#define NPCMAIN_0P75 0.75
#endif
extern const f32 float_neg90_80419bd8;
extern const f32 float_neg180_80419bf0;
extern const f32 float_270_80419be4;
extern const f32 float_neg270_80419be8;
extern const f32 float_90_80419bd0;
extern const f32 float_0p75_80419bf4;
extern const f32 float_neg1_80419bf8;
extern const f32 float_1_80419bb4;
extern const f32 float_1000_80419bc8;
extern const f32 float_10_80419bb8;
extern const f32 float_0p5_80419ba0;
extern const f32 float_0_80419b9c;
extern const f32 float_180_80419bec;
extern const f32 float_360_80419be0;
#ifdef MUST_MATCH
// The original reaches these two strings from the unit's rodata anchor.
#define FBAT_SFX_ENEMY ((char*)vec3_802c2f78 + 0x204)
#define FBAT_SFX_MARIO ((char*)vec3_802c2f78 + 0x220)
#else
#define FBAT_SFX_ENEMY "SFX_BTL_FLD_ENEMY_DAMAGED1"
#define FBAT_SFX_MARIO "SFX_BTL_FLD_MARIO_DAMAGED1"
#endif
#define ENC(p, T, off) (*(T*)((u8*)(p) + (off)))
typedef struct FbatHitInfo {
    s32 kind;           // 0x0, the attack (the fbatSetAttackAnnounce code)
    s32 field_0x4;      // 0x4, the npc's field_2F8, for a stomp that becomes the enemy's first strike
    f32 dist;           // 0x8, to the nearest npc hit so far
    u32 flags;          // 0xC, 1: only a test (no attack announce)
} FbatHitInfo;
extern const f32 float_11_80419bbc;
extern const f32 float_12_80419bc0;
extern const f32 float_15_80419bc4;
extern const f32 float_1p5_80419bcc;
extern const f32 float_30_80419bd4;
typedef struct FbatAngleTable {
    s32 deg[11];
} FbatAngleTable;

// TODO: inlining?
NpcWork* npcGetWorkPtr(void) {
    return gp->inBattle ? &work.battle : &work.field;
}

void npcReleaseFieldNpc(void) {
    NpcWork* wp;
    NpcFieldEntry* field;
    NpcEntry* entry;
    AnimationPose* pose;
    int i;

    wp = &work.field;
    field = release_wp;
    if (wp->flags & 2) {
        entry = wp->entries;
        for (i = 0; i < wp->total; i++, entry++) {
            // entry = &wp->entries[i];
            if (entry->flags & 1 && entry->flags & 2 && animPoseGetAnimPosePtr(entry->poseId)->paperId == -1) {
                if (entry->poseId >= 0) {
                    pose = animPoseGetAnimPosePtr(entry->poseId);
                    field->mFlags = pose->flags;
                    field->wCurrentRenderRotationMinus90Y = pose->field_0x70;
                    field->wRotationY = pose->rotationY;
                    field->wScaleZ_flip = pose->field_0x78;
                    field->wInitToZero4 = pose->field_0x7C;
                    strcpy(field->mModelName, animPoseGetAnimBaseDataPtr(entry->poseId)->fileName);
                    strcpy(field->mAnimName, animPoseGetCurrentAnim(entry->poseId)->name);
                    animPoseRelease(entry->poseId);
                    field++; // next release_wp entry
                    entry->poseId = -1;
                }
            }
        }
    }
}

void npcRecoveryFieldNpc(void) {
    NpcWork* wp;
    NpcFieldEntry* field;
    NpcEntry* entry;
    AnimationPose* pose;
    int i;

    wp = &work.field;
    field = release_wp;
    if (wp->flags & 2) {
        entry = wp->entries;
        for (i = 0; i < wp->total; i++, entry++) {
            if (entry->flags & 1 && entry->flags & 2 && entry->poseId == -1) {
                entry->poseId = animPoseEntry(field->mModelName, 0);
                animPoseSetAnim(entry->poseId, field->mAnimName, TRUE);
                pose = animPoseGetAnimPosePtr(entry->poseId);
                pose->flags = field->mFlags;
                pose->field_0x70 = field->wCurrentRenderRotationMinus90Y;
                pose->rotationY = field->wRotationY;
                pose->field_0x78 = field->wScaleZ_flip;
                pose->field_0x7C = field->wInitToZero4;

                if (pose->flags & 0x40) {
                    pose->flags &= ~0x40;
                    animPoseWorldPositionEvalOn(entry->poseId);
                }
                if (pose->flags & 0x80) {
                    pose->flags &= ~0x80;
                    animPoseWorldMatrixEvalOn(entry->poseId);
                }
                animPoseMain(entry->poseId);
                field++; // next release_wp entry
            }
        }
        wp->flags &= ~2;
    }
}

void mtx_setup(NpcEntry* entry, Mtx mtx, s32 history) {
    f32 rotationAngle;
    f32 scaleSign;
    CameraEntry* camera;
    f32 targetAngle;
    f32 positionAngle;
    f32 v15;
    Mtx mtxX, mtxY, mtxZ;

    rotationAngle = 0.0f;
    scaleSign = 1.0f;
    if (!(entry->flags & 0x2000000) && !(entry->flags & 0x8000000)) {
        camera = camGetCurPtr();
        targetAngle = angleABf(camera->cameraPos.x, camera->cameraPos.z, camera->cameraAt.x, camera->cameraAt.z);
        positionAngle = angleABf(camera->cameraPos.x, camera->cameraPos.z, entry->position.x, entry->position.z);
        rotationAngle = reviseAngle(targetAngle - positionAngle);
    }
    v15 = reviseAngle(reviseAngle(entry->rotation.y));
    if (v15 >= 90.0f && v15 <= 270.0f) {
        scaleSign *= -1.0f;
    }
    MTXIdentity(mtx);
    if (entry->scale.x != 1.0f || entry->scale.y != 1.0f || entry->scale.z != 1.0f || scaleSign != 1.0f) {
        MTXScaleApply(mtx, mtx, entry->scale.x, entry->scale.y, entry->scale.z * scaleSign);
    }
    if (entry->rotationOffset.x || entry->rotationOffset.y || entry->rotationOffset.z) {
        MTXTransApply(mtx, mtx, -entry->rotationOffset.x, -entry->rotationOffset.y, -entry->rotationOffset.z);
    }
    if (entry->rotation.z) {
        MTXRotRad(mtxZ, 'z', entry->rotation.z * 0.017453292f);
        MTXConcat(mtxZ, mtx, mtx);
    }
    if (entry->rotation.x) {
        MTXRotRad(mtxX, 'x', entry->rotation.x * 0.017453292f);
        MTXConcat(mtxX, mtx, mtx);
    }
    if (entry->rotation.y || rotationAngle) {
        MTXRotRad(mtxY, 'y', (entry->rotation.y + rotationAngle) * 0.017453292f);
        MTXConcat(mtxY, mtx, mtx);
    }
    if (entry->rotationOffset.x || entry->rotationOffset.y || entry->rotationOffset.z) {
        MTXTransApply(mtx, mtx, entry->rotationOffset.x, entry->rotationOffset.y, entry->rotationOffset.z);
    }
    if (history == -1) {
        MTXTransApply(mtx, mtx, entry->position.x, entry->position.y, entry->position.z);
    } else {
        MTXTransApply(mtx, mtx, entry->positionHistory[history].x, entry->positionHistory[history].y,
                      entry->positionHistory[history].z);
    }
}

#ifdef MUST_MATCH
// MUST_MATCH: the original does the colour + billboard work through an inlined helper. Inlining the
// whole block makes MWCC put the GXColor local at r1+0x10 and its by-value copy at r1+0x14 (the
// out-of-line shape swaps them), and allocates anim/animPose/i to retail's callee-saved registers
// (r29/r30/r30) instead of emitting the extra `mr r0,r3` save.
static inline void npcOffscreenSetup(NpcEntry* entry) {
    GXColor color;
    AnimTableEntry* anim;
    AnimationPose* animPose;
    CameraEntry* camera;
    f32 dx, dz;
    f32 angle;
    u32 i;

    color = entry->color;
    if (entry->jumpFlags & 0x400000) {
        color.r /= 2;
        color.g /= 2;
        color.b /= 2;
        animPoseSetMaterialEvtColor(entry->poseId, color);
        animPoseSetMaterialFlagOn(entry->poseId, 0x40);
    }

    if (entry->flags & 0x800000) {
        anim = animPoseGetCurrentAnim(entry->poseId); // stale store
        animPose = animPoseGetAnimPosePtr(entry->poseId);
        anim = animPoseGetCurrentAnim(entry->poseId);

        camera = camGetPtr(CAMERA_3D);
        dx = camera->cameraAt.x - camera->cameraPos.x;
        dz = camera->cameraAt.z - camera->cameraPos.z;
        angle = reviseAngle(NPC_RAD2DEG((f32)atan2(-dx, -dz)));
        if (angle < 0.0f) {
            angle += 360.0f;
        }
        angle = reviseAngle((angle - animPose->rotationY));

        if ((angle > 90.0f && angle <= 270.0f) || (angle < -90.0f && angle >= -270.0f)) {
            for (i = 0; i < 14; i++) {
                if (!strcmp(luigi_pose[i][0], anim->name)) {
                    break;
                }
            }
            if (i < 14) {
                animPoseSetAnim(entry->poseId, luigi_pose[i][1], FALSE);
            }
        } else {
            for (i = 0; i < 14; i++) {
                if (!strcmp(luigi_pose[i][1], anim->name)) {
                    break;
                }
            }
            if (i < 14) {
                animPoseSetAnim(entry->poseId, luigi_pose[i][0], FALSE);
            }
        }
    }
}

static inline void npcOffscreenSetup2(NpcEntry* entry, GXColor* colorp) {
    AnimTableEntry* anim;
    AnimationPose* animPose;
    CameraEntry* camera;
    f32 dx, dz;
    f32 angle;
    u32 i;

    *colorp = entry->color;
    if (entry->jumpFlags & 0x400000) {
        colorp->r /= 2;
        colorp->g /= 2;
        colorp->b /= 2;
        animPoseSetMaterialEvtColor(entry->poseId, *colorp);
        animPoseSetMaterialFlagOn(entry->poseId, 0x40);
    }

    if (entry->flags & 0x800000) {
        anim = animPoseGetCurrentAnim(entry->poseId); // stale store
        animPose = animPoseGetAnimPosePtr(entry->poseId);
        anim = animPoseGetCurrentAnim(entry->poseId);

        camera = camGetPtr(CAMERA_3D);
        dx = camera->cameraAt.x - camera->cameraPos.x;
        dz = camera->cameraAt.z - camera->cameraPos.z;
        angle = reviseAngle(NPC_RAD2DEG((f32)atan2(-dx, -dz)));
        if (angle < 0.0f) {
            angle += 360.0f;
        }
        angle = reviseAngle((angle - animPose->rotationY));

        if ((angle > 90.0f && angle <= 270.0f) || (angle < -90.0f && angle >= -270.0f)) {
            for (i = 0; i < 14; i++) {
                if (!strcmp(luigi_pose[i][0], anim->name)) {
                    break;
                }
            }
            if (i < 14) {
                animPoseSetAnim(entry->poseId, luigi_pose[i][1], FALSE);
            }
        } else {
            for (i = 0; i < 14; i++) {
                if (!strcmp(luigi_pose[i][1], anim->name)) {
                    break;
                }
            }
            if (i < 14) {
                animPoseSetAnim(entry->poseId, luigi_pose[i][0], FALSE);
            }
        }
    }
}
#endif

void npcDisp_xlu(CameraId cameraId, void* param) {
    NpcEntry* entry = param; // cast to proper type
    Mtx mtx;
    GXColor color;
    f32 rotY;

    rotY = reviseAngle(-entry->direction);
    mtx_setup(entry, mtx, -1);

#ifdef MUST_MATCH
    npcOffscreenSetup2(entry, &color);
#else
    color = entry->color;
    if (entry->jumpFlags & 0x400000) {
        color.r /= 2;
        color.g /= 2;
        color.b /= 2;
        animPoseSetMaterialEvtColor(entry->poseId, color);
        animPoseSetMaterialFlagOn(entry->poseId, 0x40);
    }

    if (entry->flags & 0x800000) {
        AnimTableEntry* anim;
        AnimationPose* animPose;
        CameraEntry* camera;
        f32 dx, dz;
        f32 angle;
        u32 i;

        anim = animPoseGetCurrentAnim(entry->poseId); //stale store
        animPose = animPoseGetAnimPosePtr(entry->poseId);
        anim = animPoseGetCurrentAnim(entry->poseId);

        camera = camGetPtr(CAMERA_3D);
        dx = camera->cameraAt.x - camera->cameraPos.x;
        dz = camera->cameraAt.z - camera->cameraPos.z;
        angle = reviseAngle(NPC_RAD2DEG((f32)atan2(-dx, -dz)));
        if (angle < 0.0f) {
            angle += 360.0f;
        }
        angle = reviseAngle((angle - animPose->rotationY));

        
        if ((angle > 90.0f && angle <= 270.0f) || (angle < -90.0f && angle >= -270.0f)) {
            for (i = 0; i < 14; i++) {
                if (!strcmp(luigi_pose[i][0], anim->name)) {
                    break;
                }
            }
            if (i < 14) {
                animPoseSetAnim(entry->poseId, luigi_pose[i][1], FALSE);
            }
        } else {
            for (i = 0; i < 14; i++) {
                if (!strcmp(luigi_pose[i][1], anim->name)) {
                    break;
                }
            }
            if (i < 14) {
                animPoseSetAnim(entry->poseId, luigi_pose[i][0], FALSE);
            }
        }
    }
#endif
    animPoseMain(entry->poseId);
    animPoseDrawMtx(entry->poseId, mtx, 1, rotY, 2.0f);
    animPoseDrawMtx(entry->poseId, mtx, 2, rotY, 2.0f);
    animPoseDrawMtx(entry->poseId, mtx, 3, rotY, 2.0f);
}

void npcDisp(CameraId camId, void* param) {
    NpcEntry* entry = param; // cast to proper type
    Mtx mtx;
    GXColor color;
    f32 rotY;
    DispEntry* disp;

    rotY = reviseAngle(-entry->direction);
    mtx_setup(entry, mtx, -1);

    disp = dispGetCurWork();
    switch (disp->renderMode) {
        case 0:
#ifdef MUST_MATCH
            npcOffscreenSetup2(entry, &color);
#else
            color = entry->color;
            if (entry->jumpFlags & 0x400000) {
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
                animPoseSetMaterialEvtColor(entry->poseId, color);
                animPoseSetMaterialFlagOn(entry->poseId, 0x40);
            }
            if (entry->flags & 0x800000) {
                AnimTableEntry* anim;
                AnimationPose* animPose;
                CameraEntry* camera;
                f32 dx, dz;
                f32 angle;
                u32 i;

                anim = animPoseGetCurrentAnim(entry->poseId); // stale store
                animPose = animPoseGetAnimPosePtr(entry->poseId);
                anim = animPoseGetCurrentAnim(entry->poseId);

                camera = camGetPtr(CAMERA_3D);
                dx = camera->cameraAt.x - camera->cameraPos.x;
                dz = camera->cameraAt.z - camera->cameraPos.z;
                angle = reviseAngle(NPC_RAD2DEG((f32)atan2(-dx, -dz)));
                if (angle < 0.0f) {
                    angle += 360.0f;
                }
                angle = reviseAngle((angle - animPose->rotationY));

                if ((angle > 90.0f && angle <= 270.0f) || (angle < -90.0f && angle >= -270.0f)) {
                    for (i = 0; i < 14; i++) {
                        if (!strcmp(luigi_pose[i][0], anim->name)) {
                            break;
                        }
                    }
                    if (i < 14) {
                        animPoseSetAnim(entry->poseId, luigi_pose[i][1], FALSE);
                    }
                } else {
                    for (i = 0; i < 14; i++) {
                        if (!strcmp(luigi_pose[i][1], anim->name)) {
                            break;
                        }
                    }
                    if (i < 14) {
                        animPoseSetAnim(entry->poseId, luigi_pose[i][0], FALSE);
                    }
                }
            }
#endif
            animPoseMain(entry->poseId);
            animPoseDrawMtx(entry->poseId, mtx, 1, rotY, 2.0f);
            break;
        case 1:
            animPoseDrawMtx(entry->poseId, mtx, 2, rotY, 2.0f);
            break;
        case 2:
            animPoseDrawMtx(entry->poseId, mtx, 3, rotY, 2.0f);
            break;
    }
}

// 1:1: npcDisp_xlu with a sysWaitDrawSync/GXClearBoundingBox prologue and a
// GXReadBoundingBox + offscreenAddBoundingBox(entry->field_110, ...) epilogue.
void npcDisp_offscreen_xlu(CameraId camId, void* param) {
    NpcEntry* entry = param; // cast to proper type
    Mtx mtx;
    GXColor color;
    f32 rotY;
    u16 left, top, right, bottom;

    sysWaitDrawSync();
    GXClearBoundingBox();
    rotY = reviseAngle(-entry->direction);
    mtx_setup(entry, mtx, -1);

#ifdef MUST_MATCH
    npcOffscreenSetup(entry);
#else
    color = entry->color;
    if (entry->jumpFlags & 0x400000) {
        color.r /= 2;
        color.g /= 2;
        color.b /= 2;
        animPoseSetMaterialEvtColor(entry->poseId, color);
        animPoseSetMaterialFlagOn(entry->poseId, 0x40);
    }
    if (entry->flags & 0x800000) {
        AnimTableEntry* anim;
        AnimationPose* animPose;
        CameraEntry* camera;
        f32 dx, dz;
        f32 angle;
        u32 i;

        anim = animPoseGetCurrentAnim(entry->poseId); // stale store
        animPose = animPoseGetAnimPosePtr(entry->poseId);
        anim = animPoseGetCurrentAnim(entry->poseId);

        camera = camGetPtr(CAMERA_3D);
        dx = camera->cameraAt.x - camera->cameraPos.x;
        dz = camera->cameraAt.z - camera->cameraPos.z;
        angle = reviseAngle(NPC_RAD2DEG((f32)atan2(-dx, -dz)));
        if (angle < 0.0f) {
            angle += 360.0f;
        }
        angle = reviseAngle((angle - animPose->rotationY));

        if ((angle > 90.0f && angle <= 270.0f) || (angle < -90.0f && angle >= -270.0f)) {
            for (i = 0; i < 14; i++) {
                if (!strcmp(luigi_pose[i][0], anim->name)) {
                    break;
                }
            }
            if (i < 14) {
                animPoseSetAnim(entry->poseId, luigi_pose[i][1], FALSE);
            }
        } else {
            for (i = 0; i < 14; i++) {
                if (!strcmp(luigi_pose[i][1], anim->name)) {
                    break;
                }
            }
            if (i < 14) {
                animPoseSetAnim(entry->poseId, luigi_pose[i][0], FALSE);
            }
        }
    }
#endif
    animPoseMain(entry->poseId);
    animPoseDrawMtx(entry->poseId, mtx, 1, rotY, 2.0f);
    animPoseDrawMtx(entry->poseId, mtx, 2, rotY, 2.0f);
    animPoseDrawMtx(entry->poseId, mtx, 3, rotY, 2.0f);
    sysWaitDrawSync();
    GXReadBoundingBox(&left, &top, &right, &bottom);
    offscreenAddBoundingBox(entry->field_110, left, top, right, bottom);
}

// 1:1: npcDisp with the offscreen bounding-box prologue/epilogue.
void npcDisp_offscreen(CameraId camId, void* param) {
    NpcEntry* entry = param; // cast to proper type
    Mtx mtx;
    GXColor color;
    f32 rotY;
    DispEntry* disp;
    u16 left, top, right, bottom;

    sysWaitDrawSync();
    GXClearBoundingBox();
    rotY = reviseAngle(-entry->direction);
    mtx_setup(entry, mtx, -1);

    disp = dispGetCurWork();
    switch (disp->renderMode) {
        case 0:
#ifdef MUST_MATCH
            npcOffscreenSetup(entry);
#else
            color = entry->color;
            if (entry->jumpFlags & 0x400000) {
                color.r /= 2;
                color.g /= 2;
                color.b /= 2;
                animPoseSetMaterialEvtColor(entry->poseId, color);
                animPoseSetMaterialFlagOn(entry->poseId, 0x40);
            }
            if (entry->flags & 0x800000) {
                AnimTableEntry* anim;
                AnimationPose* animPose;
                CameraEntry* camera;
                f32 dx, dz;
                f32 angle;
                u32 i;

                anim = animPoseGetCurrentAnim(entry->poseId); // stale store
                animPose = animPoseGetAnimPosePtr(entry->poseId);
                anim = animPoseGetCurrentAnim(entry->poseId);

                camera = camGetPtr(CAMERA_3D);
                dx = camera->cameraAt.x - camera->cameraPos.x;
                dz = camera->cameraAt.z - camera->cameraPos.z;
                angle = reviseAngle(NPC_RAD2DEG((f32)atan2(-dx, -dz)));
                if (angle < 0.0f) {
                    angle += 360.0f;
                }
                angle = reviseAngle((angle - animPose->rotationY));

                if ((angle > 90.0f && angle <= 270.0f) || (angle < -90.0f && angle >= -270.0f)) {
                    for (i = 0; i < 14; i++) {
                        if (!strcmp(luigi_pose[i][0], anim->name)) {
                            break;
                        }
                    }
                    if (i < 14) {
                        animPoseSetAnim(entry->poseId, luigi_pose[i][1], FALSE);
                    }
                } else {
                    for (i = 0; i < 14; i++) {
                        if (!strcmp(luigi_pose[i][1], anim->name)) {
                            break;
                        }
                    }
                    if (i < 14) {
                        animPoseSetAnim(entry->poseId, luigi_pose[i][0], FALSE);
                    }
                }
            }
#endif
            animPoseMain(entry->poseId);
            animPoseDrawMtx(entry->poseId, mtx, 1, rotY, 2.0f);
            break;
        case 1:
            animPoseDrawMtx(entry->poseId, mtx, 2, rotY, 2.0f);
            break;
        case 2:
            animPoseDrawMtx(entry->poseId, mtx, 3, rotY, 2.0f);
            break;
    }
    sysWaitDrawSync();
    GXReadBoundingBox(&left, &top, &right, &bottom);
    offscreenAddBoundingBox(entry->field_110, left, top, right, bottom);
}

// Inline degree/radian sine and cosine approximations from the original blur path; only used
// to keep npcDisp_blur's arithmetic in one place.
static inline f32 npcSinf(f32 rad) {
    f32 f0, f1;
    f32 sinv;
    f32 c1, c2, pi;

    sinv = NPC_F_SIN0;
    c1 = NPC_F_SIN1;
    c2 = NPC_F_ONE;
    pi = NPC_F_PI;
    if (rad <= pi) {
        pi = NPC_F_HALFPI;
        if (rad <= pi) {
            f0 = rad * rad;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * rad;
        } else {
            f1 = rad - pi;
            rad = pi - f1;
            f0 = rad * rad;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * rad;
        }
    } else if (rad < NPC_F_THREEHALVES) {
        rad = rad - pi;
        f0 = rad * rad;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * rad;
        sinv = -sinv;
    } else {
        f1 = rad - NPC_F_THREEHALVES;
        pi = NPC_F_HALFPI;
        rad = pi - f1;
        f0 = rad * rad;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * rad;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 npcCosf(f32 rad) {
    f32 cosv;
    f32 f0;
    f32 c1, c2, pi;

    cosv = NPC_F_COS0;
    c1 = NPC_F_COS1;
    c2 = NPC_F_ONE;
    pi = NPC_F_PI;
    if (rad <= pi) {
        pi = NPC_F_HALFPI;
        if (rad <= pi) {
            f0 = rad * rad;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
        } else {
            f0 = rad - pi;
            rad = pi - f0;
            f0 = rad * rad;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < NPC_F_THREEHALVES) {
        rad = rad - pi;
        f0 = rad * rad;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        f0 = rad - NPC_F_THREEHALVES;
        pi = NPC_F_HALFPI;
        rad = pi - f0;
        f0 = rad * rad;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

// Blur recovered (2*sin/-(2*cos) offsets from camGetPtr(camId)->viewYaw, 4 draws
// with history 4..1, alpha 0x80-100..0x80-25, flag save/restore). The named unit-rodata constants hoist the
// coefficient loads before the branches, `t1`/`t2` make MWCC put the product in f2 and the angle in f5, each
// branch reuses `rad` as the folded angle, and declaring `alpha` first (with its init/decrement in the for
// header) and putting `i` in the leading block give retail's callee-saved map. Left: the sin/cos
// accumulator lands in f1 (an extern constant does not coalesce like a literal, so halfpi then takes f0), and
// the colour/argument stack slots are 0x8/0xc swapped. The colour base is dat_80419b88 (not an li).
// NONMATCHING (97.73%): sin/cos accumulator FPR (halfpi lands in f0, retail f1) and the colour/arg
// stack slots only. The npcSinf square temp must be `f0` (not `f1`) to match retail's `fmuls f0,f5,f5`.
void npcDisp_blur(CameraId camId, void* param) {
    s32 alpha;
    NpcEntry* entry = param; // cast to proper type
    Mtx mtx;
    GXColor color;
    f32 rotY;
    f32 offX, offZ;
    f32 t1, t2;
    s32 i;
    u32 flag;

    rotY = reviseAngle(-entry->direction);
    flag = animPoseGetMaterialFlag(entry->poseId);
    t1 = 6.2831855f * camGetPtr(camId)->viewYaw;
    offX = 2.0f * npcSinf(t1 / 360.0f);
    t2 = 6.2831855f * camGetPtr(camId)->viewYaw;
    offZ = 2.0f * -npcCosf(t2 / 360.0f);
    animPoseMain(entry->poseId);
    for (i = 4, alpha = 100; i > 0; i--, alpha -= 25) {
        mtx_setup(entry, mtx, i);
        PSMTXTransApply(mtx, mtx, offX, 0.0f, offZ);
        animPoseSetMaterialFlagOn(entry->poseId, 0x40);
        *(u32*)&color = NPC_COLOR_DAT;
        color.a = 0x80 - alpha;
        animPoseSetMaterialEvtColor(entry->poseId, color);        animPoseDrawMtx(entry->poseId, mtx, 1, rotY, 2.0f);
        animPoseDrawMtx(entry->poseId, mtx, 2, rotY, 2.0f);
        animPoseDrawMtx(entry->poseId, mtx, 3, rotY, 2.0f);
    }
    animPoseSetMaterialFlagOff(entry->poseId, 0x40);
    animPoseSetMaterialFlagOn(entry->poseId, flag);
}

void npcInit(void) { // 1:1
    FieldBattleData* data;
    NpcWork* wp;

    wp = &work.field;
    wp->total = NPC_FIELD_MAX_COUNT;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(NpcEntry) * wp->total);
    memset(wp->entries, 0, sizeof(NpcEntry) * wp->total);

    release_wp = __memAlloc(HEAP_DEFAULT, sizeof(NpcFieldEntry) * wp->total);

    wp = &work.battle;
    wp->total = NPC_BATTLE_MAX_COUNT;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(NpcEntry) * wp->total);
    memset(wp->entries, 0, sizeof(NpcEntry) * wp->total);

    data = __memAlloc(HEAP_DEFAULT, sizeof(FieldBattleData));
    gp->fbatData = data;
    memset(gp->fbatData, 0, sizeof(FieldBattleData));
    data->mode = 0;
    npcMainCount = 0;
}

void npcReset(BOOL inBattle) {
    NpcWork* wp = npcGetWorkPtr2(inBattle);
    memset(wp->entries, 0, sizeof(NpcEntry) * wp->total);
    wp->count = 0;
    if (!inBattle) {
        gp->fbatData->mode = 0;
    }
    npcMainCount = 0;
}

s32 npcGetReactionOfLivingBody(BOOL inBattle) {
    return npcGetWorkPtr2(inBattle)->count;
}

// TODO: fix entry->position delayed set-Vec with r6/r30
// TODO: a1 is desc/ription?
// NONMATCHING (93.21%): the two search loops use a cached `total` and an incrementing `entry` pointer,
// and the position/scale zero/one vectors are this file's rodata vec3_802c2f78/vec3_802c2f84.  Still register
// rotation (a1 in r26 vs r25, so `stmw r26` instead of `stmw r25`) and the vec3 base is scheduled later.
s32 npcEntry(const char* a1, const char* animName) {
    NpcWork* wp;
    NpcEntry* entry;
    s32 total;
    s32 i;

    wp = gp->inBattle ? &work.battle : &work.field;
    // wp = npcGetWorkPtr(); //inlined
    total = wp->total;

    // unused ------------------------------------------------
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->description, a1)) {
            break; // found an entry, unused
        }
    }
    // unused ------------------------------------------------

    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (!(entry->flags & 1)) {
            break; // unused entry
        }
    }
    memset(entry, 0, sizeof(NpcEntry));
    entry->flags = 3;
    strcpy(entry->description, a1);
    entry->poseId = animPoseEntry(animName, gp->inBattle != 0);
    if (entry->poseId == -2) {
        entry->poseId = animPoseEntry("hoshi", gp->inBattle != 0);
    }
    animPosePeraOn(entry->poseId);
#ifdef MUST_MATCH
    // vec3_802c2f78 (0,0,0) and vec3_802c2f84 (1,1,1) are this file's own rodata, which the original reaches from one
    // base register; indexing past the first object reproduces that until the file defines its rodata
    entry->position = *(Vec*)&vec3_802c2f78;
#else
    entry->position.x = 0.0f;
    entry->position.y = 0.0f;
    entry->position.z = 0.0f;
#endif
    entry->prevPosition = entry->position;
    entry->width = animPoseGetHeight(entry->poseId);
    entry->height = animPoseGetRadius(entry->poseId);
#ifdef MUST_MATCH
    entry->scale = *(Vec*)&vec3_802c2f84;
#else
    entry->scale.x = 1.0f;
    entry->scale.y = 1.0f;
    entry->scale.z = 1.0f;
#endif
    entry->field_154 = NPC_F_ONE;
    entry->field_108 = 0;
    entry->field_10C = "M_I_2";
    entry->field_198 = gp->renderTime;
    entry->field_188 = 0;
    entry->jumpGravity = NPC_F_ONE;
    entry->field_1D0 = NPC_F_ZERO;
    entry->jumpYScale = NPC_F_ONE;
    entry->field_110 = -1;
    entry->color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    entry->cameraId = CAMERA_3D;
    entry->field_2F8 = 0;
    entry->balloonType = 1;
    if (animPoseGetVivianType(entry->poseId)) {
        entry->flags |= 0x4000000;
    }
    if (!strcmp(animName, "c_luigi")) {
        entry->flags |= 0x800000u;
    }
    animPoseSetMaterialLightFlagOn(entry->poseId, 2);
    wp->count++;
    return i;
}

NpcTribeInfo* npcGetTribe(const char* tribeName) { // 1:1
    NpcTribeInfo* tribe;

    for (tribe = npcTribe; tribe->nameJp; tribe++) {
        if (!strcmp(tribe->nameJp, tribeName)) {
            return tribe;
        }
    }
    return NULL;
}

void npcDelete(NpcEntry* entry) {
    int i;

    for (i = 0; i < 4; i++) {
        if (entry->slaves[i]) {
            entry->slaves[i]->master = NULL;
        }
        entry->slaves[i] = NULL;
    }

    if (entry->prev) {
        if (entry->next) {
            entry->prev->next = entry->next;
            entry->next->prev = entry->prev;
        } else {
            entry->prev->next = NULL;
        }
    } else {
        if (entry->next) {
            entry->next->prev = NULL;
        }
    }
    entry->flags &= ~2;
    if (entry->poseId >= 0) {
        animPoseRelease(entry->poseId);
    }
    entry->poseId = -1;
    entry->flags &= ~1;
    npcGetWorkPtrInline()->count--;
}

// Every instruction matches. A `NpcEntry* slave` local (used for both the condition and the npcDelete
// argument) recovers the entry/next r27/r31 colouring and the first inlined npcDelete; the second slave
// loop still has the inlined npcDelete's hoisted work pointer in r30 and the slave in r29 (target r29/r30),
// and the final npcDelete swaps its r3/r4 scratch.
// NONMATCHING (98.28%): second slave-loop work/slave colouring and final npcDelete r3/r4 scratch.
void _npcDeleteGroup(NpcEntry* entry) { // recursively inlined, warning
    NpcEntry* slave;
    int i;

    if (entry->next) {
        _npcDeleteGroup(entry->next);
    }

    for (i = 0; i < 4; i++) {
        slave = entry->slaves[i];
        if (slave) {
            npcDelete(slave);
            entry->slaves[i] = NULL;
        }
    }
    npcDelete(entry);
}

void npcDeleteGroup(NpcEntry* entry) {
    if (entry->master) {
        entry = entry->master;
    }
    while (entry->prev) {
        entry = entry->prev;
    }
    _npcDeleteGroup(entry);
}

#ifdef MUST_MATCH
static inline f32 npcMain_sqrtf(f32 x, const f64* pool) {
    if (x > float_0_80419b9c) {
        f64 guess = __frsqrte(x);
        f64 half = pool[0];
        f64 three = pool[1];
        guess = half * guess * (three - guess * guess * x);
        guess = half * guess * (three - guess * guess * x);
        guess = half * guess * (three - guess * guess * x);
        return x * guess;
    } else if (x < pool[2]) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}
#endif

// Mario can be noticed by a field npc: not in a cutscene, menu or talk, and not hidden by Vivian.
static inline BOOL npcMarioNoticeable(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & 0x82000000) {
        return FALSE;
    }
    if (mario->flags & 0xA) {
        return FALSE;
    }
    if ((s32)gp->field_0x164 != 0) {
        return FALSE;
    }
    if (seqGetSeq() != 2) {
        return FALSE;
    }
    if (mario->currMotionId == 0x19) {
        return FALSE;
    }
    if (vivianGetStatus() == 0 || vivianGetStatus() == 1) {
    } else {
        return FALSE;
    }
    return TRUE;
}

// Flurrie's breath reaches the npc at its feet, middle or head.
static inline BOOL npcBlown(NpcEntry* entry) {
    Vec pos;
    Vec at;

    if (entry->flags & 0x20000000) {
        return FALSE;
    }
    if (entry->flags & 0x20) {
        return FALSE;
    }
    pos = entry->position;
    at = pos;
    if (cloudGetBreathPower(entry->width, &at) > float_0_80419b9c) {
        return TRUE;
    }
    pos = entry->position;
    pos.y -= entry->height * float_0p5_80419ba0;
    at = pos;
    if (cloudGetBreathPower(entry->width, &at) > float_0_80419b9c) {
        return TRUE;
    }
    pos = entry->position;
    pos.y -= entry->height;
    at = pos;
    if (cloudGetBreathPower(entry->width, &at) > float_0_80419b9c) {
        return TRUE;
    }
    return FALSE;
}

// Per frame for each live npc: advance its clocks, apply its movement and walk/run animation, raise its event's
// reactions (interrupt flags: 1 found Mario, 2 lost him, 4 left its territory, 8 blown by Flurrie), and queue its
// draw, shadow and offscreen passes; then run the first strike banner and the field battle mode.
// keep loop-invariant string addresses where they are written, as the original does
#pragma opt_loop_invariants off
// keep the constants and addresses at their uses rather than propagated into the hot loop's registers
#pragma opt_propagation off
// NONMATCHING (98.60%): retail keeps a redundant `fcmpo f30, 0`/`lfs float_0` before PSVECSubtract that ours
// folds into the later sqrtf guard; `fire = jump = 0` is two `li`s where retail has `li`+`mr`; the state == 1/4
// FPR set is one callee-saved register low; fbat and state sit in each other's GPR. All calls and constants match.
void npcMain(void) {
    s32 fire;
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    const char* fallSfx = NPCMAIN_STR_FALL;
    NpcWork* npcwork;
    NpcEntry* entry;
    FieldBattleData* fbat;
    FirstStrikeInfo* strike;
    EventEntry* evt;
    MarioWork* mario;
    s32 total;
    s32 i;
    s32 state;
    BOOL hit;
    s32 shadowId;
    f32 dx, dy, dz;
    f32 dist;
    f32 dir;
    f32 runScale;
    f32 angle;
    f32 z;
    Vec v;
    Vec back;
    Vec moved;
    Vec shadow;
    Vec feet;

    npcwork = gp->inBattle ? &work.battle : &work.field;
    marioGetPtr()->field_0x1D8 = 0;
    total = npcwork->total;
    npcMainCount++;

    for (i = 0, entry = npcwork->entries; i < total; i++, entry++) {
        if (!(entry->flags & 1) || !(entry->flags & 2)) {
            continue;
        }
        if (entry->flags & 0x8000) {
            animPoseGetAnimPosePtr(entry->poseId)->flags |= 0x100;
        } else {
            animPoseGetAnimPosePtr(entry->poseId)->flags &= ~0x100;
        }
        if (!(entry->flags & 0x100000)) {
            entry->jumpFlags |= 0x10;
        }
        if (entry->jumpFlags & 0x10000) {
            entry->jumpFlags &= ~0x10;
        }
        if (*(u16*)&entry->wFbatHitCheckRelated) {
            (*(u16*)&entry->wFbatHitCheckRelated)--;
        }

        if (entry->jumpFlags & 0x10) {
            entry->field_190 = gp->renderTime - entry->field_198;
            if (OSTicksToMilliseconds(entry->field_190) > 500) {
                entry->field_190 = OSMillisecondsToTicks(16);
            }
            if (entry->flags & 0x20000) {
                entry->field_178 += entry->field_190;
            } else {
                entry->field_178 = 0;
            }
            entry->field_188 += entry->field_190;
            entry->field_198 = gp->renderTime;
            entry->field_1A0 = (f32)(OSTicksToMicroseconds(entry->field_188)) / 1000000.0f;
            entry->field_180 = (f32)(OSTicksToMicroseconds(entry->field_178)) / 1000000.0f;
            entry->moveDt = (f32)(OSTicksToMicroseconds(entry->field_190)) / 1000000.0f;
            entry->field_1A8 = (f32)(OSTicksToMicroseconds(entry->field_198)) / 1000000.0f;
        } else {
            entry->field_198 = gp->renderTime;
        }

        if (entry->flags & 2) {
            // In Bowser's water level: splash on entering the water and move at half speed while in it.
            if (entry->jumpFlags & 0x100000) {
                entry->jumpFlags |= 0x200000;
            } else {
                entry->jumpFlags &= ~0x200000;
            }
            entry->jumpFlags &= ~0x100000;
            if (!strcmp(gp->currentMapName, NPCMAIN_STR_KPA03) && !(entry->position.y > float_365_80419ba8)) {
                entry->jumpFlags |= 0x100000;
                if (!(entry->jumpFlags & 0x200000) && (entry->jumpFlags & 0x100000)) {
                    psndSFXOn_3D(fallSfx, &entry->position);
                }
            }
            if ((entry->jumpFlags & 0x100000) && !(entry->jumpFlags & 0x10000000)) {
                entry->field_1D0 = float_100_80419c18;
                runScale = float_0p5_80419ba0;
            } else {
                runScale = float_1_80419bb4;
            }
            if (entry->jumpFlags & 0x100000) {
                *(u16*)entry->field_31E |= 1;
            }

            if (entry->jumpFlags & 0x10) {
                if (entry->moveDt > float_0_80419b9c) {
                    f32 mx = entry->moveOffset.x;
                    f32 mz = entry->moveOffset.z;

                    if ((entry->jumpFlags & 0x100000) && !(entry->jumpFlags & 0x10000000)) {
                        mx *= float_0p5_80419ba0;
                        mz *= float_0p5_80419ba0;
                    }
                    if (entry->flags & 0x08000000) {
                        mz = 0.0f;
                    }
                    if (entry->flags & 0x40000000) {
                        entry->position.x += mx;
                        entry->position.z += mz;
                    } else {
                        entry->field_2FC = (u32)npcHitCheckSide(entry, &entry->position.x, &entry->position.z,
                                                                &entry->moveDir, mx, mz);
                    }
                    entry->moveOffset.x = float_0_80419b9c;
                    entry->moveOffset.z = float_0_80419b9c;
                    npcMoveG(entry);
                }
                if (!(entry->flags & 8) && !(entry->flags & 0x20) && !(entry->flags & 0x4000)) {
                    npcCheckHitMarioSide(entry);
                }

                // Pick the stay/walk/run animation from the distance moved this frame.
                PSVECSubtract(&entry->position, &entry->prevPosition, &v);
                if ((entry->flags & 0x400) && entry->moveDt > float_0_80419b9c) {
                    f32 speed = NPCMAIN_SQRTF(v.x * v.x + v.z * v.z) / entry->moveDt;

                    if (speed < float_0p1_80419bfc) {
                        if (entry->field_108 != 1) {
                            animPoseSetAnim(entry->poseId, entry->tribe->stayAnim, 1);
                            entry->field_108 = 1;
                        }
                    } else if (speed < entry->tribe->runStartSpeed * runScale) {
                        if (entry->field_108 != 2) {
                            animPoseSetAnim(entry->poseId, entry->tribe->walkAnimation, 1);
                            entry->field_108 = 2;
                        }
                    } else if (entry->field_108 != 3) {
                        animPoseSetAnim(entry->poseId, entry->tribe->runAnimation, 1);
                        entry->field_108 = 3;
                    }
                }
            }

            // Raise the npc's reactions for its event: blown (8), out of its territory (4), lost Mario (2),
            // found Mario (1).
            if (entry->regularEvtId != 0) {
                evt = evtGetPtrID(entry->regularEvtId);
                if (evt != NULL && !(evt->flags & 2)) {
                    state = evt->wNpcEventType;
                    if (entry->blowEvent != NULL && state != 5 && state != 8 && npcBlown(entry)) {
                        *(s32*)entry->field_140 |= 8;
                        goto interrupt;
                    }

                    if (entry->territoryType != NPC_TERRITORY_TYPE_NOTHING && (state == 1 || state == 2)) {
                        hit = FALSE;
                        if (entry->territoryType == NPC_TERRITORY_TYPE_CIRCLE) {
                            dx = entry->territoryBase.x - entry->position.x;
                            dz = entry->territoryBase.z - entry->position.z;
                            dist = NPCMAIN_SQRTF(dx * dx + dz * dz);
                            if (dist > (state == 2 ? entry->territoryHoming.x : entry->territoryHoming.x)) {
                                hit = TRUE;
                            }
                        } else if (entry->territoryType == NPC_TERRITORY_TYPE_SQUARE) {
                            if (state == 2) {
                                if (entry->position.x < entry->territoryBase.x - entry->territoryHoming.x ||
                                    entry->position.x > entry->territoryBase.x + entry->territoryHoming.x ||
                                    entry->position.z < entry->territoryBase.z - entry->territoryHoming.z ||
                                    entry->position.z > entry->territoryBase.z + entry->territoryHoming.z) {
                                    hit = TRUE;
                                }
                            } else if (entry->position.x < entry->territoryBase.x - entry->territoryLoiter.x ||
                                       entry->position.x > entry->territoryBase.x + entry->territoryLoiter.x ||
                                       entry->position.z < entry->territoryBase.z - entry->territoryLoiter.z ||
                                       entry->position.z > entry->territoryBase.z + entry->territoryLoiter.z) {
                                hit = TRUE;
                            }
                        }
                        if (hit) {
                            *(s32*)entry->field_140 |= 4;
                            goto interrupt;
                        }
                    }

                    if (state == 2) {
                        mario = marioGetPtr();
                        dx = mario->position.x - entry->position.x;
                        dy = mario->position.y - entry->position.y;
                        dz = mario->position.z - entry->position.z;
                        PSVECSubtract(&entry->prevPosition, &entry->position, &moved);
                        dir = entry->direction;
                        if ((entry->jumpFlags & 2) && NPCMAIN_SQRTF(moved.x * moved.x + moved.z * moved.z) > float_0p05_80419c78) {
                            dir = entry->moveDir;
                        }
                        hit = FALSE;
                        if (!npcMarioNoticeable()) {
                            hit = TRUE;
                        } else {
                            dist = NPCMAIN_SQRTF(dx * dx + dz * dz);
                            if (dist > float_0p25_80419c7c * entry->homingRange) {
                                if (dy > entry->territoryLoiter.y) {
                                    hit = TRUE;
                                } else if (-dy > entry->territoryHoming.y) {
                                    hit = TRUE;
                                } else if (dist > entry->homingRange) {
                                    hit = TRUE;
                                } else if (angle = angleABf(float_0_80419b9c, float_0_80419b9c, dx, dz),
                                           __fabsf(compAngle(dir, angle)) > float_0p5_80419ba0 * entry->homingAngle) {
                                    hit = TRUE;
                                }
                            }
                        }
                        if (hit) {
                            *(s32*)entry->field_140 |= 2;
                            goto interrupt;
                        }
                    }

                    if (state == 1 || state == 4) {
                        mario = marioGetPtr();
                        dz = mario->position.z - entry->position.z;
                        dx = mario->position.x - entry->position.x;
                        dy = mario->position.y - entry->position.y;
                        dist = dx * dx + dz * dz;
                        PSVECSubtract(&entry->prevPosition, &entry->position, &back);
                        dir = reviseAngle(entry->direction) < float_180_80419bec ? float_90_80419bd0 : float_270_80419be4;
                        hit = TRUE;
                        if (!npcMarioNoticeable()) {
                            hit = FALSE;
                        } else {
                            dist = NPCMAIN_SQRTF(dist);
                            if (dist > float_0p25_80419c7c * entry->searchRange) {
                                if (dist > entry->searchRange) {
                                    hit = FALSE;
                                    goto found;
                                }
                                angle = angleABf(float_0_80419b9c, float_0_80419b9c, dx, dz);
                                if (__fabsf(compAngle(dir, angle)) > float_0p5_80419ba0 * entry->searchAngle) {
                                    hit = FALSE;
                                    goto found;
                                }
                            }
                            if (dy > entry->territoryLoiter.y) {
                                hit = FALSE;
                            } else if (-dy > entry->territoryHoming.y) {
                                hit = FALSE;
                            } else if (state == 4) {
                                if (entry->territoryType == NPC_TERRITORY_TYPE_CIRCLE) {
                                    dx = entry->territoryBase.x - entry->position.x;
                                    dz = entry->territoryBase.z - entry->position.z;
                                    if (NPCMAIN_SQRTF(dx * dx + dz * dz) > NPCMAIN_0P75 * entry->territoryHoming.x) {
                                        hit = FALSE;
                                    }
                                } else if (entry->position.x < entry->territoryBase.x - 0.75f * entry->territoryHoming.x ||
                                           entry->position.x > entry->territoryBase.x + 0.75f * entry->territoryHoming.x ||
                                           entry->position.z < entry->territoryBase.z - 0.75f * entry->territoryHoming.z ||
                                           entry->position.z > entry->territoryBase.z + 0.75f * entry->territoryHoming.z) {
                                    hit = FALSE;
                                }
                            }
                        }
                    found:
                        if (hit) {
                            *(s32*)entry->field_140 |= 1;
                        }
                    }
                }
            interrupt:
                npcCheckInterrupt(entry);
            }

            // Draw only what is inside the view volume (flag 0x40 draws regardless).
            if (!(entry->flags & 0x800)) {
                PSMTXMultVec(camGetPtr(CAMERA_3D)->view, &entry->position, &v);
                PSMTX44MultVec(camGetPtr(CAMERA_3D)->projection, &v, &v);
                if ((v.z > float_neg1_80419bf8 && v.z < float_0_80419b9c && v.x >= float_neg2_80419c80 &&
                     v.x <= float_2_80419c5c && v.y >= float_neg2_80419c80 && v.y <= float_2_80419c5c) ||
                    (entry->flags & 0x40)) {
                    if (entry->color.r != 0xFF || entry->color.g != 0xFF || entry->color.b != 0xFF ||
                        entry->color.a != 0xFF) {
                        animPoseSetMaterialFlagOn(entry->poseId, 0x40);
                        animPoseSetMaterialEvtColor(entry->poseId, entry->color);
                    } else {
                        animPoseSetMaterialFlagOff(entry->poseId, 0x40);
                    }

                    z = float_5000_80419c84 * v.z + float_5000_80419c84;
                    if (z < float_0_80419b9c) {
                        z = float_0_80419b9c;
                    } else if (z > float_10000_80419c88) {
                        z = float_10000_80419c88;
                    }
                    z *= float_neg1_80419bf8;

                    if (!(entry->flags & 0x80) &&
                        (!(*(u16*)&entry->wFbatHitCheckRelated & 2) || (entry->jumpFlags & 0x10000))) {
                        if (entry->color.a != 0xFF) {
                            dispEntry(entry->cameraId, z, 2, npcDisp_xlu, entry);
                        } else {
                            dispEntry(entry->cameraId, z, 0, npcDisp, entry);
                            dispEntry(entry->cameraId, z, 1, npcDisp, entry);
                            dispEntry(entry->cameraId, z, 2, npcDisp, entry);
                        }

                        if (!(entry->flags & 0x04000000)) {
                            if (!(entry->flags & 0x40000)) {
                                shadow = entry->position;
                                if (entry->tribe != NULL) {
                                    shadow.x += entry->tribe->shadowOffsetX * entry->scale.x *
                                                sinfd(reviseAngle(entry->direction) < float_180_80419bec
                                                          ? float_90_80419bd0
                                                          : float_270_80419be4);
                                    shadow.y += entry->tribe->shadowOffsetY * entry->scale.y;
                                    shadow.z += entry->tribe->shadowOffsetZ * entry->scale.z;
                                }
                                shadowId = shadowEntry(shadow.x, shadow.y, shadow.z,
                                                  float_1p33_80419c8c * (entry->field_154 *
                                                                         (entry->width / 3.0f * entry->scale.x)));
                                if (entry->flags & 0x80000) {
                                    shadowSetCollision(shadowId, 0);
                                }
                            }
                        } else {
                            f32 hx, hy, hz, hdist, nx, ny, nz;

                            feet = entry->position;
                            hdist = float_100_80419c18;
                            if (hitCheckFilter(NULL, &hx, &hy, &hz, &hdist, &nx, &ny, &nz, feet.x,
                                               float_1_80419bb4 + feet.y, feet.z, float_0_80419b9c,
                                               float_neg1_80419bf8, float_0_80419b9c) != NULL) {
                                feet.y = hy;
                            }
                            animPoseVivianMain(entry->poseId, &feet);
                            shadowId = shadowEntry(entry->position.x, entry->position.y, entry->position.z,
                                              entry->field_154 * (entry->width / 3.0f * entry->scale.x));
                            if (entry->flags & 0x80000) {
                                shadowSetCollision(shadowId, 0);
                            }
                        }

                        if (entry->flags & 0x100) {
                            dispEntry(entry->cameraId, z, 2, npcDisp_blur, entry);
                        }
                    }

                    if (entry->field_110 != -1) {
                        if (entry->color.a != 0xFF) {
                            dispEntry(CAMERA_OFFSCREEN2, z, 2, npcDisp_offscreen_xlu, entry);
                        } else {
                            dispEntry(CAMERA_OFFSCREEN2, z, 0, npcDisp_offscreen, entry);
                            dispEntry(CAMERA_OFFSCREEN2, z, 1, npcDisp_offscreen, entry);
                            dispEntry(CAMERA_OFFSCREEN2, z, 2, npcDisp_offscreen, entry);
                        }
                    }
                }
            }

            entry->prevPosition = entry->position;
            if ((entry->flags & 0x100) && !(npcMainCount & 1)) {
                entry->positionHistory[4] = entry->positionHistory[3];
                entry->positionHistory[3] = entry->positionHistory[2];
                entry->positionHistory[2] = entry->positionHistory[1];
                entry->positionHistory[1] = entry->positionHistory[0];
                entry->positionHistory[0] = entry->position;
            }
        }
        entry->jumpFlags &= ~0x10;
    }

    // The first strike announcement: slide in, hold, slide out (type 2); or a fixed 60-frame banner (type 1).
    fbat = gp->fbatData;
    strike = &fbat->firstStrike;
    if (strike->enabled) {
        switch (strike->type) {
            case 2:
                strike->field_0xC++;
                if ((s32)strike->field_0xC < 20) {
                    strike->field_0x10 = intplGetValue(5, strike->field_0xC, 20, float_neg600_80419c0c, float_0_80419b9c);
                } else if ((s32)strike->field_0xC < 80) {
                    strike->field_0x10 = float_0_80419b9c;
                } else if ((s32)strike->field_0xC < 100) {
                    strike->field_0x10 =
                        intplGetValue(1, strike->field_0xC - 80, 20, float_0_80419b9c, float_600_80419c10);
                } else {
                    strike->field_0x4 = 0;
                    break;
                }
                strike->field_0x14 = float_0_80419b9c;
                dispEntry(CAMERA_FADE, float_0_80419b9c, 2, _fbatFirstAttackAnnouceDisp, strike);
                break;
            case 1:
                strike->field_0xC++;
                if ((s32)strike->field_0xC > 60) {
                    strike->field_0x4 = 0;
                } else {
                    strike->field_0x10 = float_0_80419b9c;
                    strike->field_0x14 = float_0_80419b9c;
                    dispEntry(CAMERA_FADE, float_0_80419b9c, 2, _fbatFirstAttackAnnouceDisp, strike);
                }
                break;
        }
    }

    switch (fbat->mode) {
        case 1: {
            FieldBattleData* data;
            s32 jump;

            fbatEncountCheck();
            data = gp->fbatData;
            mario = marioGetPtr();
            fire = jump = 0;
            if (mario->characterId == 2) {
                jump = kpaJumpChk();
            }
            if (mario->characterId == 2) {
                fire = kpaFireAttackCheck();
            }
            if (jump) {
                if (data->field_57C > 0 && data->field_578 >= 2) {
                    Vec pos = mario->position;

                    pos.y += float_60_80419ba4;
                    kpaAddScorePos(data->field_57C, &pos);
                }
                data->field_578 = 0;
                data->field_57C = 0;
            }
            if (!fire) {
                data->field_574 = 0;
            }
            break;
        }
        case 2:
        case 3:
            fbatTalkMode();
            break;
        case 4:
            break;
        case 5:
            fbatBattleMode();
            break;
        case 6:
            break;
    }
    fbat->field_54C[1] = 0;
}
#pragma opt_loop_invariants reset
#pragma opt_propagation reset

NpcEntry* npcNameToPtr(const char* name) {
    NpcWork* wp = npcGetWorkPtrInline();
    int i;
    s32 total;
    NpcEntry* entry;

    total = wp->total;
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->description, name)) {
            break;
        }
    }
    return entry;
}

NpcEntry* npcNameToPtr_NoAssert(const char* name) {
    NpcWork* wp = npcGetWorkPtrInline();
    int i;
    s32 total;
    NpcEntry* entry;

    total = wp->total;
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->description, name)) {
            break;
        }
    }
    if (i >= total) {
        return NULL;
    } else {
        return entry;
    }
}

void npcSetMarioAutoTalkPose(const char* stayPose, const char* talkPose) {
    strcpy(MarioStayPose, stayPose);
    strcpy(MarioTalkPose, talkPose);
}

void npcSetTalkPose(const char* name) {
    NpcWork* wp;
    NpcEntry* entry;
    s32 total;
    int i;

    if ((s8)name[0] == 0) {
        return;
    }
#ifdef MUST_MATCH
    // The sign-extend round trip on the argument leaves the dead value that pins retail's parameter
    // save (`mr r28, r3`) before the `name[0]` load; the backend folds it away.
    if (!strcmp((const char*)(s64)(s32)name, "(^x^)party")) {
#else
    if (!strcmp(name, "(^x^)party")) {
#endif
        return;
    }
    if (!strcmp(name, "party")) {
        PartyEntry* party = partyGetPtr(marioGetPartyId());
        if (party == NULL) {
            return;
        }
        partyChgPoseId(party, 7);
        return;
    }
    if (!strcmp(name, "extparty")) {
        PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
        if (party == NULL) {
            return;
        }
        partyChgPoseId(party, 7);
        return;
    }
    if (!strcmp(name, "mario")) {
        marioChgPose(MarioTalkPose);
        return;
    }

    wp = gp->inBattle ? &work.battle : &work.field;
    total = wp->total;
    entry = wp->entries;
    for (i = 0; i < total; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->description, name)) {
            break;
        }
    }
    if (entry->flags & 0x200) {
        animPoseSetAnim(entry->poseId, entry->talkAnim, TRUE);
    }
}

void npcSetStayPose(const char* name) {
    NpcWork* wp;
    NpcEntry* entry;
    s32 total;
    int i;

    if ((s8)name[0] == 0) {
        return;
    }
#ifdef MUST_MATCH
    // The sign-extend round trip on the argument leaves the dead value that pins retail's parameter
    // save (`mr r28, r3`) before the `name[0]` load; the backend folds it away.
    if (!strcmp((const char*)(s64)(s32)name, "(^x^)party")) {
#else
    if (!strcmp(name, "(^x^)party")) {
#endif
        return;
    }
    if (!strcmp(name, "party")) {
        PartyEntry* party = partyGetPtr(marioGetPartyId());
        if (party == NULL) {
            return;
        }
        partyChgPoseId(party, 1);
        return;
    }
    if (!strcmp(name, "extparty")) {
        PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
        if (party == NULL) {
            return;
        }
        partyChgPoseId(party, 1);
        return;
    }
    if (!strcmp(name, "mario")) {
        marioChgPose(MarioStayPose);
        return;
    }

    wp = gp->inBattle ? &work.battle : &work.field;
    total = wp->total;
    entry = wp->entries;
    for (i = 0; i < total; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->description, name)) {
            break;
        }
    }
    if (entry->flags & 0x200) {
        animPoseSetAnim(entry->poseId, entry->stayAnim, TRUE);
    }
}

// Inline degree/radian sine and cosine approximations (same polynomials as the npc blur path).
// Assigning the coefficients to non-const locals first stops MWCC rematerialising them in every
// branch, which recovers the target's early coefficient loads.
static inline f32 npcSideSinf(f32 rad) {
    f32 sinv;
    f32 f0, f1;
    f32 c0, c1, c2, pi;

    c0 = 0.00761f;
    c1 = 0.16605f;
    c2 = 1.0f;
    pi = 3.1415927f;
    if (rad <= pi) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            sinv = c0 * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * rad;
        } else {
            f0 = rad - 1.5707964f;
            f1 = 1.5707964f - f0;
            f0 = f1 * f1;
            sinv = c0 * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * f1;
        }
    } else if (rad < 4.712389f) {
        f1 = rad - pi;
        f0 = f1 * f1;
        sinv = c0 * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    } else {
        f0 = rad - 4.712389f;
        f1 = 1.5707964f - f0;
        f0 = f1 * f1;
        sinv = c0 * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 npcSideCosf(f32 rad) {
    f32 cosv;
    f32 f0;
    f32 c0, c1, c2, pi;

    c0 = 0.03705f;
    c1 = 0.4967f;
    c2 = 1.0f;
    pi = 3.1415927f;
    if (rad <= pi) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            cosv = c0 * f0 - c1;
            cosv = cosv * f0 + c2;
        } else {
            f0 = rad - 1.5707964f;
            f0 = 1.5707964f - f0;
            f0 = f0 * f0;
            cosv = c0 * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < 4.712389f) {
        f0 = rad - pi;
        f0 = f0 * f0;
        cosv = c0 * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        f0 = rad - 4.712389f;
        f0 = 1.5707964f - f0;
        f0 = f0 * f0;
        cosv = c0 * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

static inline f32 npcHitSideSinf(f32 rad) {
    f32 f0, f1;
    f32 sinv;
    f32 c1, c2, pi;

    sinv = 0.00761f;
    c1 = 0.16605f;
    c2 = 1.0f;
    pi = 3.1415927f;
    if (rad <= pi) {
        pi = 1.5707964f;
        if (rad <= pi) {
            f1 = rad * rad;
            sinv = sinv * f1 - c1;
            sinv = sinv * f1 + c2;
            sinv = sinv * rad;
        } else {
            f1 = rad - pi;
            f0 = pi - f1;
            f1 = f0 * f0;
            sinv = sinv * f1 - c1;
            sinv = sinv * f1 + c2;
            sinv = sinv * f0;
        }
    } else if (rad < 4.712389f) {
        f0 = rad - pi;
        f1 = f0 * f0;
        sinv = sinv * f1 - c1;
        sinv = sinv * f1 + c2;
        sinv = sinv * f0;
        sinv = -sinv;
    } else {
        f1 = rad - 4.712389f;
        pi = 1.5707964f;
        f0 = pi - f1;
        f1 = f0 * f0;
        sinv = sinv * f1 - c1;
        sinv = sinv * f1 + c2;
        sinv = sinv * f0;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 npcHitSideCosf(f32 rad) {
    f32 cosv;
    f32 f0;
    f32 c1, c2, pi;

    cosv = 0.03705f;
    c1 = 0.4967f;
    c2 = 1.0f;
    pi = 3.1415927f;
    if (rad <= pi) {
        pi = 1.5707964f;
        if (rad <= pi) {
            f0 = rad * rad;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
        } else {
            f0 = rad - pi;
            f0 = pi - f0;
            f0 = f0 * f0;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < 4.712389f) {
        f0 = rad - pi;
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        f0 = rad - 4.712389f;
        pi = 1.5707964f;
        f0 = pi - f0;
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

// Match-only shape: folding the radius product through this one-line inline makes MWCC keep the
// `box.x + width + 5.0f` accumulator in the target's f2 instead of f0. The natural `* 0.5f` is under
// the #else at the call site (rule 7).
#ifdef MUST_MATCH
static inline f32 npcHitSideMul(f32 a, f32 b) {
    return a * b;
}
#endif

// `opt_propagation off` stops MWCC rematerialising the sin/cos coefficients into every branch;
// `opt_lifetimes off` is what puts the accumulator/pivot/scratch in the target's FPRs.
#pragma opt_propagation off
#pragma opt_lifetimes off
// NONMATCHING (99.59%): the sin/cos blocks and the radius match; only `offZ` (target f4, ours f6) and the
// inlined sqrtf expression that reads it (target f6, ours f5) still colour into the other scratch FPR.
// The remaining symbol-only pool displacements resolve when the unit links.
s32 npcCheckHitMarioSide(NpcEntry* entry) {
    MarioWork* mario = marioGetPtr();
    Vec pos;
    f32 d1, d2;
    f32 radius;
    f32 angle;
    f32 offX, offZ;
    f32 rad;
    f32 dist;

    if (entry->position.y > mario->position.y + mario->wPlayerCollisionBox.y) {
        return 0;
    }
    if (mario->position.y > entry->position.y + entry->height) {
        return 0;
    }
    pos.x = mario->position.x;
    pos.z = mario->position.z;
    pos.y = entry->position.y;
    d1 = PSVECDistance(&pos, &entry->position);
#ifdef MUST_MATCH
    radius = npcHitSideMul(mario->wPlayerCollisionBox.x + entry->width + 5.0f, 0.5f);
#else
    radius = (mario->wPlayerCollisionBox.x + entry->width + 5.0f) * 0.5f;
#endif
    if (d1 > radius) {
        return 0;
    }
    entry->position.x = entry->prevPosition.x;
    entry->position.z = entry->prevPosition.z;
    d2 = PSVECDistance(&pos, &entry->position);
    if (d2 > radius) {
        return 0;
    }
    mario->field_0x1D8 = (s32)entry;
    angle = angleABf(mario->position.x, mario->position.z, entry->position.x, entry->position.z);
    if (__fabsf(compAngle(angle, mario->directionView)) < 40.0f) {
        {
            f32 t = 6.2831855f * mario->directionView;
            offX = (radius - d2) * -npcHitSideSinf(t / 360.0f);
            offZ = (radius - d2) * npcHitSideCosf(t / 360.0f);
        }
    } else {
        {
            f32 t = 6.2831855f * angle;
            offX = (entry->position.x - radius * npcHitSideSinf(t / 360.0f)) - mario->position.x;
            offZ = (entry->position.z + radius * npcHitSideCosf(t / 360.0f)) - mario->position.z;
        }
    }
    dist = sqrtf(offX * offX + offZ * offZ);
    {
        f32 limit = mario->wPlayerCollisionBox.z > 4.0f ? 3.0f : 1.5f;
        if (dist > limit) {
            f32 scale = 2.0f / dist;
            offX *= scale;
            offZ *= scale;
        }
    }
    mario->position.x += offX;
    mario->position.z += offZ;
    marioChkWallAround(&mario->position, 0, 0.0f, 160.0f + angle, 20.0f, mario->wPlayerCollisionBox.y);
    return 1;
}
#pragma opt_lifetimes reset
#pragma opt_propagation reset

// 100%: testing `unkFlag` directly (not `jflags & 0x100000`) recovers the jflags/unkFlag colouring. In the
// jump branch, declaring `f64 dabs` for `__fabs(moveY)` and casting `(f32)dabs` into `absY` makes MWCC round
// out-of-place (`frsp f0, f2`) instead of in place, and declaring `absY` before `yOffset` gets f3=yOffset.
void npcMoveG(NpcEntry* entry) {
    f32 moveY;
    f32 force;
    u32 jflags;
    u32 unkFlag;
    u32 flags;

    moveY = entry->moveOffset.y;
    entry->moveOffset.y = 0.0f;
    jflags = entry->jumpFlags;
    unkFlag = jflags & 0x100000;
    if (unkFlag && !(jflags & 0x10000000)) {
        moveY *= 0.5f;
    }
    force = 0.0f;
    flags = entry->flags;
    if (flags & 0x20000) {
        f32 d = entry->field_180 - entry->moveDt;
        if (unkFlag == 0 || (jflags & 0x10000000)) {
            force = entry->jumpVelY * entry->moveDt +
                    (-980.0f * entry->jumpGravity) * (entry->field_180 * entry->field_180 - d * d);
        } else {
            force = 0.4f * (entry->jumpVelY * entry->moveDt) +
                    (0.15f * (-980.0f * entry->jumpGravity)) * (entry->field_180 * entry->field_180 - d * d);
        }
        force *= entry->jumpYScale;
        if (entry->field_1D0 > 0.0f) {
            f32 limit;
            if (unkFlag != 0) {
                limit = 0.5f * (-entry->field_1D0 * entry->moveDt);
            } else {
                limit = -entry->field_1D0 * entry->moveDt;
            }
            if (force < limit) {
                force = limit;
            }
        }
    }
    moveY += force;
    if (!(flags & 0x10)) {
        entry->position.y += moveY;
        if (entry->position.y < -1000.0f) {
            entry->position.y = -1000.0f;
        }
    } else if (moveY != 0.0f) {
        if (moveY > 0.0f) {
            entry->jumpFlags |= 1;
            entry->position.y += moveY;
        } else {
            f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;
            f64 dabs;
            f32 absY;
            f32 yOffset = 0.8f * entry->height;
            HitEntry* hit;

            dabs = __fabs(moveY);
            absY = (f32)dabs;
            distance = yOffset + absY;
            hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, entry->position.x,
                                 entry->position.y + yOffset, entry->position.z, 0.0f, -1.0f, 0.0f);
            *(HitEntry**)entry->field_300 = hit;
            if (hit == NULL) {
                entry->jumpFlags |= 1;
                entry->position.y += moveY;
            } else {
                entry->jumpFlags &= ~1;
                entry->position.y = hitY;
                entry->field_178 = 0;
            }
        }
    }
}

void npcTuningRy(NpcEntry* npc, f32 angle) {
    npc->direction = reviseAngle(angle);
}

f32 npcTransRytoFaceDir(NpcEntry* npc) {
    if (reviseAngle(npc->direction) < 180.0f) {
        return 90.0f;
    }
    return 270.0f;
}

// The unit's .rodata head is defined above (vec3 blob and the SFX/msg string objects). Their
// presence places MWCC's generated sqrtf (0.5/3.0/0.0) and int->float 2^52 double pool at
// vec3_802c2f78 + 0x1c0, where this function's target reads it; without them the pool landed at
// ...rodata.0 + 0x30. Note `a_magu`/`mjef_coinup` also have a later codegen copy from the
// functions that still use the bare literals; that tail is a separate function-order issue.
HitEntry* npcHitCheckSide(NpcEntry* entry, f32* outX, f32* outZ, f32* outAngle, f32 dx, f32 dz) {
    HitEntry* hit;
    f32 hx, hy, hz, d, nx, ny, nz;
    f32 sinv, cosv;
    f32 dist2;
    f32 y, dot, ox, oz;
    f32 angA, angB, ndist;

    *outX = entry->position.x;
    *outZ = entry->position.z;
    if (dx == 0.0f && dz == 0.0f) {
        return NULL;
    }
    sincosf(&sinv, &cosv, angleABf(0.0f, 0.0f, dx, dz));
    dist2 = dx * dx + dz * dz;

    d = 0.75f * entry->width + sqrtf(dx * dx + dz * dz);
    y = 0.2f * entry->height;
    if (y > 10.0f) {
        y = 10.0f;
    }
    if (entry->flags & 0x2000) {
        hit = hitCheckAttr(4, &hx, &hy, &hz, &d, &nx, &ny, &nz, entry->position.x, entry->position.y + y,
                           entry->position.z, sinv, 0.0f, cosv);
    } else {
        hit = hitCheckFilter(NULL, &hx, &hy, &hz, &d, &nx, &ny, &nz, entry->position.x, entry->position.y + y,
                             entry->position.z, sinv, 0.0f, cosv);
    }
    if (hit != NULL) {
        dot = dx * nx + dz * nz;
        ox = 0.5f * (dx - dot * nx);
        oz = 0.5f * (dz - dot * nz);
        *outX += ox;
        *outZ += oz;
        if (entry->flags & 0x10000) {
            angA = reviseAngle(180.0f + *outAngle);
            angB = reviseAngle(angleABf(0.0f, 0.0f, 100.0f * nx, 100.0f * nz));
            *outAngle = (s32)reviseAngle(angB + reviseAngle(angB - angA));
        }
        return hit;
    }

    d = 0.75f * entry->width + sqrtf(dx * dx + dz * dz);
    y = 0.4f * entry->height;
    if (entry->flags & 0x2000) {
        hit = hitCheckAttr(4, &hx, &hy, &hz, &d, &nx, &ny, &nz, entry->position.x, entry->position.y + y,
                           entry->position.z, sinv, 0.0f, cosv);
    } else {
        hit = hitCheckFilter(NULL, &hx, &hy, &hz, &d, &nx, &ny, &nz, entry->position.x, entry->position.y + y,
                             entry->position.z, sinv, 0.0f, cosv);
    }
    if (hit != NULL) {
        ndist = sqrtf(nx * nx + nz * nz);
        if (angleABf(0.0f, 0.0f, ndist, 0.0f - ny) >= 45.0f) {
            dot = dx * nx + dz * nz;
            ox = 0.5f * (dx - dot * nx);
            oz = 0.5f * (dz - dot * nz);
            *outX += ox;
            *outZ += oz;
            if (entry->flags & 0x10000) {
                angA = reviseAngle(180.0f + *outAngle);
                angB = reviseAngle(angleABf(0.0f, 0.0f, 100.0f * nx, 100.0f * nz));
                *outAngle = (s32)reviseAngle(angB + reviseAngle(angB - angA));
            }
            return hit;
        }
    }

    dist2 = sqrtf(dx * dx + dz * dz);
    d = 0.75f * entry->width + dist2;
    y = 0.75f * entry->height;
    if (entry->flags & 0x2000) {
        hit = hitCheckAttr(4, &hx, &hy, &hz, &d, &nx, &ny, &nz, entry->position.x, entry->position.y + y,
                           entry->position.z, sinv, 0.0f, cosv);
    } else {
        hit = hitCheckFilter(NULL, &hx, &hy, &hz, &d, &nx, &ny, &nz, entry->position.x, entry->position.y + y,
                             entry->position.z, sinv, 0.0f, cosv);
    }
    if (hit != NULL) {
        dot = dx * nx + dz * nz;
        ox = 0.5f * (dx - dot * nx);
        oz = 0.5f * (dz - dot * nz);
        *outX += ox;
        *outZ += oz;
        if (entry->flags & 0x10000) {
            angA = reviseAngle(180.0f + *outAngle);
            angB = reviseAngle(angleABf(0.0f, 0.0f, 100.0f * nx, 100.0f * nz));
            *outAngle = (s32)reviseAngle(angB + reviseAngle(angB - angA));
        }
        return hit;
    }
    *outX += dx;
    *outZ += dz;
    return NULL;
}

// The original evaluates `dx2 + dy*dy + dz2 > 0.0f` into a dead `fcmpo` (no branch); a discarded
// `sqrtf(dx2 + dy*dy + dz2)` statement reproduces that, and declaring dy after dx2/dz2 gets dx2/dz2/dy
// into f9/f10/f11.
// 100%: the inline sqrtf's double pool is at vec3_802c2f78+0x1c0, so the MUST_MATCH helper reads the
// doubles through the unit's base register (0x1c0/0x1c8/0x1d0) and declares `half`/`three` inside the
// `x > 0.0f` branch after `frsqrte` so the loads schedule where the target has them.
#ifdef MUST_MATCH
static inline f32 npcNearDistCheck_sqrtf(f32 x, const f64* pool) {
    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        f64 half = pool[0];
        f64 three = pool[1];
        guess = half * guess * (three - guess * guess * x);
        guess = half * guess * (three - guess * guess * x);
        guess = half * guess * (three - guess * guess * x);
        return x * guess;
    } else if (x < pool[2]) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}
#endif

NpcEntry* npcNearDistCheck(f32 x, f32 y, f32 z, f32 radius) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    NpcEntry* entry;
    NpcEntry* found = NULL;
    s32 total = wp->total;
    s32 i;

    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (entry->flags & 1) {
            f32 dx = x - entry->position.x;
            f32 dz = z - entry->position.z;
            f32 dx2 = dx * dx;
            f32 dz2 = dz * dz;
            f32 dy = y - entry->position.y;
#ifdef MUST_MATCH
            f32 dist = npcNearDistCheck_sqrtf(dx2 + dz2, pool);
#else
            f32 dist = sqrtf(dx2 + dz2);
#endif
            sqrtf(dx2 + dy * dy + dz2);
            if (dist < radius) {
                found = entry;
            }
        }
    }
    return found;
}

FieldBattleData* fbatGetPointer(void) {
    return gp->fbatData;
}

void fbatChangeMode(s16 mode) {
    gp->fbatData->mode = mode;
}

void fbatSetAttackAnnounce(s32 a1) {
    FirstStrikeInfo* firstStrike = &gp->fbatData->firstStrike;

    firstStrike->enabled = FALSE;
    firstStrike->field_0x4 = a1;
    firstStrike->field_0xC = 0;
    switch (firstStrike->field_0x4) {
        case 0x20000:
        case 0x40000:
        case 0x80000:
        case 0x100000:
        case 0x200000:
        case 0x400000:
        case 0x800000:
        case 0x1000000:
            firstStrike->msg_tag = "fb_sensei_shita"; // You struck first!
            firstStrike->type = 2;
            firstStrike->color = (GXColor){ 0xFF, 0x80, 0x80, 0xFF };
            break;
        case 0x10000000:
            firstStrike->msg_tag = "fb_sensei_sareta"; // Your foe struck first!
            firstStrike->type = 1;
            firstStrike->color = (GXColor){ 0x80, 0x80, 0xFF, 0xFF };
            break;
        default:
            firstStrike->type = 0;
            break;
    }
}

void fbatSetAttackAnnounceEnable(void) {
    gp->fbatData->firstStrike.enabled = TRUE;
}

// The first-strike banner: a frame jittered by up to 4 units, with the message drawn inside it.
void _fbatFirstAttackAnnouceDisp(CameraId camId, void* param) {
    FirstStrikeInfo* firstStrike = param;
    const char* message;
    f32 x;
    f32 y;
    f32 width;
    f32 frameX;
    f32 frameY;
#ifdef MUST_MATCH
    // MWCC canonicalises `value * 0.1f` to `0.1f * value`; a named temp keeps the original's
    // value-first operand order, and `width / 2.0f` does the same for the 0.5 multiply.
    f32 k;
#endif

#ifdef MUST_MATCH
    k = 0.1f;
#endif
    message = msgSearch(firstStrike->msg_tag);
    x = firstStrike->field_0x10;
    y = firstStrike->field_0x14;
    width = FontGetMessageWidth(message);
    frameX = x + (f32)(irand(10000) % 5);
    frameY = y + (f32)(irand(10000) % 5);
#ifdef MUST_MATCH
    windowDispGX_Waku_col(0, firstStrike->color, 0.1f * (frameX - 10.0f - width / 2.0f), frameY * k,
                          (20.0f + width) * k, 3.2f, 1.0f);
#else
    windowDispGX_Waku_col(0, firstStrike->color, 0.1f * (frameX - 10.0f - width * 0.5f), frameY * 0.1f,
                          (20.0f + width) * 0.1f, 3.2f, 1.0f);
#endif
    FontDrawStart();
    FontDrawEdge();
    // Passing the colour as a compound literal makes MWCC place it in the original's stack slot.
    FontDrawColor(&(GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    FontDrawScale(0.1f);
#ifdef MUST_MATCH
    FontDrawStringShake(message, 0.1f * (x - width / 2.0f), (y - 3.0f) * k);
#else
    FontDrawStringShake(message, 0.1f * (x - width * 0.5f), 0.1f * (y - 3.0f));
#endif
}

// 100%: control flow and the `mode == 3` test match. The two `regularEvtId` reads go through a fresh
// `*(NpcEntry**)(fbatData->field_2 + 2)` load (as in the original), which also fixes `entry`/`mario` to r30/r29.

// Same 57 calls and immediates, no structural difference. The three decision trees match (the first switch carries the empty case 0x10000 the
// original probes; the side scan is a `switch` on `blocked`), `wk` is the ternary form, the OffChk pair is one
// `||` test, the first reviseAngle result uses its own local, and `h180` keeps the blocked-loop threshold in
// the target's f29 (an inline constant put the whole talk-loop FPR set one register low). The two SFX strings
// and the inlined sqrtf's double pool are read through the unit's base register, so the sqrtf uses
// npcNearDistCheck_sqrtf with the explicit pool and the strings come off `sfxBase`; that keeps MWCC from
// re-materialising the section base (`addi r3, r28, 0`) before each string offset. The two `angleABf` zero
// constants before dx/dz are literals so their loads schedule as retail does. Naming the `fb + 0x10` out-address
// (`fbatOut`) makes MWCC keep `total` in r27 and that temp in r26, as retail does; the second fbatHitCheck block
// gets a block-scoped `fbatOut2` so its temp takes r22 and its `fb` r25 (a function-scope reuse kept both in r26).
// Initialising the talk loop as `best = NULL; i = 0;` matches the two `li` order. Seven scratch-FPR rotations
// remain in the second wrap loop (diff/rot/const swap f1/f2/f3); no source form tested moves them.
// NONMATCHING (99.93%): every instruction matches; only callee-saved/scratch register numbers differ.
void fbatEncountCheck(void) {
    NpcWork* wk;
    s32 total;
    s32* fbatOut;
    s32 i;
    NpcEntry* npc;
    NpcEntry* best;
    NpcEntry* hit;
    FieldBattleData* fb;
    MarioWork* mario;
    AnimationPose* pose;
    CameraEntry* cam;
    f32 camAngle;
    f32 rel;
    f32 nearest;
    f32 dist;
    f32 sx;
    f32 sz;
    f32 dx;
    f32 dz;
    f32 d;
    f32 diff;
    f32 h180;
    f32 hitX, hitY, hitZ, hitNX, hitNY, hitNZ;
    f32 vec[3];
    s32 side;
    s32 found;
    s32 blocked;
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
    const char* sfxBase = (const char*)vec3_802c2f78;
#endif

    wk = gp->inBattle != 0 ? &work.battle : &work.field;
    total = wk->total;
    mario = marioGetPtr();
    wk->field_0x10 = NULL;
    if ((u32)marioStGetSystemLevel() != 0) {
        return;
    }
    if (!(wk->flags & 1)) {
        if (marioKeyOffChk() != 0 || marioCtrlOffChk() != 0) {
            return;
        }
    }
    if (N_itemPickUpFromFieldCheck() != 0) {
        return;
    }
    fb = gp->fbatData;
    fbatOut = (s32*)((u8*)fb + 0x10);
    hit = (NpcEntry*)fbatHitCheck(0x13F1, (s32)fbatOut);
    if (hit != NULL && fb->mode == 1) {
        FieldBattleData* fb2 = gp->fbatData;
        s32 ok;

        if (fb2->mode != 1) {
            ok = 0;
        } else {
            fb2->mode = 5;
            ok = 1;
            *(s8*)((u8*)fb2 + 2) = 0;
            *(NpcEntry**)((u8*)fb2 + 4) = hit;
            fb2->target = hit;
        }
        if (ok != 0) {
            switch (*fbatOut) {
                case 0x40000:
                case 0x80000:
                case 0x20000:
                    marioChgMotJump2();
                    if (dbg_battleskip != 1) {
                        animPoseSetAnim(hit->poseId, ENC(hit->tribe, const char*, 0x20), 1);
                    }
                    break;
                case 0x100000:
                case 0x400000:
                case 0x200000:
                    if (dbg_battleskip != 1) {
                        animPoseSetAnim(hit->poseId, ENC(hit->tribe, const char*, 0x20), 1);
                    }
                    break;
                case 0x800000:
                    if (dbg_battleskip != 1) {
                        animPoseSetAnim(hit->poseId, ENC(hit->tribe, const char*, 0x20), 1);
                    }
                    break;
                case 0x1000000:
                    if (dbg_battleskip != 1) {
                        animPoseSetAnim(hit->poseId, ENC(hit->tribe, const char*, 0x20), 1);
                    }
                    break;
                case 0x10000:
                    break;
            }
            pose = animPoseGetAnimPosePtr(hit->poseId);
            ENC(hit, f32, 0x148) = hit->direction;
            hit->direction = angleABf(hit->position.x, hit->position.z, mario->position.x, mario->position.z);
#ifdef MUST_MATCH
            {
                f32 r0 = reviseAngle(-hit->direction);
                rel = r0 + float_neg90_80419bd8;
            }
#else
            rel = reviseAngle(-hit->direction) + float_neg90_80419bd8;
#endif
            cam = camGetPtr(CAMERA_3D);
            camAngle = reviseAngle(NPC_RAD2DEG((f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z))));
            if (camAngle < float_0_80419b9c) {
                camAngle += float_360_80419be0;
            }
            diff = reviseAngle(camAngle - rel);
            if ((diff > float_90_80419bd0 && diff <= float_270_80419be4) || (diff < float_neg90_80419bd8 && diff >= float_neg270_80419be8)) {
                cam = camGetPtr(CAMERA_3D);
                camAngle = reviseAngle(NPC_RAD2DEG((f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z))));
                if (camAngle < float_0_80419b9c) {
                    camAngle += float_360_80419be0;
                }
                pose->field_0x7C = reviseAngle(float_180_80419bec + camAngle);
            } else {
                cam = camGetPtr(CAMERA_3D);
                camAngle = reviseAngle(NPC_RAD2DEG((f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z))));
                if (camAngle < float_0_80419b9c) {
                    camAngle += float_360_80419be0;
                }
                pose->field_0x7C = camAngle;
            }
            while (pose->field_0x7C - pose->rotationY > float_180_80419bec) {
                pose->rotationY += float_360_80419be0;
            }
            while (pose->field_0x7C - pose->rotationY < float_neg180_80419bf0) {
                pose->rotationY -= float_360_80419be0;
            }
            pose->rotationY = float_0p75_80419bf4 * (pose->field_0x7C - pose->rotationY) + pose->rotationY;
            switch (*fbatOut) {
                case 0x200000:
                case 0x1000000:
                case 0x800000:
                case 0x400000:
                case 0x80000:
                case 0x100000:
                case 0x40000:
                case 0x20000:
                    effSmallStarEntry(3, 3, hit->position.x, hit->position.y + hit->height, hit->position.z, float_0_80419b9c, float_neg1_80419bf8, float_0_80419b9c);
                    break;
                case 0x10000000:
                    effSmallStarEntry(3, 3, mario->position.x, mario->position.y + mario->wPlayerCollisionBox.y, mario->position.z, float_0_80419b9c, float_neg1_80419bf8, float_0_80419b9c);
                    break;
            }
            switch (*fbatOut) {
                case 0x200000:
                case 0x1000000:
                case 0x800000:
                case 0x400000:
                case 0x80000:
                case 0x100000:
                case 0x40000:
                case 0x20000:
                    if (hit != NULL) {
#ifdef MUST_MATCH
                        psndSFXOn_3D(sfxBase + 0x204, &hit->position);
#else
                        psndSFXOn_3D(FBAT_SFX_ENEMY, &hit->position);
#endif
                        return;
                    }
                    break;
                case 0x10000000:
#ifdef MUST_MATCH
                    psndSFXOn_3D(sfxBase + 0x220, &mario->position);
#else
                    psndSFXOn_3D(FBAT_SFX_MARIO, &mario->position);
#endif
                    break;
            }
        }
        return;
    }
    nearest = float_1000_80419bc8;
    if (marioChkTalkable() != 0 && *(s8*)((u8*)mario + 0x39) == 0) {
        npc = wk->entries;
        best = NULL;
        i = 0;
        for (; i < total; i++, npc++) {
            if ((npc->flags & 2) && !(npc->flags & 8) && ENC(npc, u32, 0x128) != 0) {
                found = 0;
                if (!(npc->position.y > mario->position.y + mario->wPlayerCollisionBox.y) &&
                    !(mario->position.y > npc->position.y + npc->height)) {
                    vec[0] = mario->position.x;
                    vec[2] = mario->position.z;
                    vec[1] = npc->position.y;
                    dist = PSVECDistance((Vec*)vec, &npc->position);
                    if (dist <= float_0p5_80419ba0 * (mario->wPlayerCollisionBox.x + npc->width) + float_10_80419bb8 && dist < nearest) {
                        best = npc;
                    }
                    side = 0;
                    camAngle = camGetPtr(CAMERA_3D)->viewYaw;
                    rel = reviseAngle(marioGetDispDir() - camAngle);
                    h180 = float_180_80419bec;
                    for (blocked = 0; blocked < 3; blocked++) {
                        f32 ang;

                        switch (blocked) {
                            case 0:
                                ang = reviseAngle(angleABf(mario->position.x, mario->position.z, npc->position.x, npc->position.z) - camGetPtr(CAMERA_3D)->viewYaw);
                                break;
                            case 1:
                                ang = reviseAngle(angleABf(mario->position.x, mario->position.z, float_0p5_80419ba0 * npc->width + npc->position.x, npc->position.z) - camGetPtr(CAMERA_3D)->viewYaw);
                                break;
                            default:
                                ang = reviseAngle(angleABf(mario->position.x, mario->position.z, -(float_0p5_80419ba0 * npc->width - npc->position.x), npc->position.z) - camGetPtr(CAMERA_3D)->viewYaw);
                                break;
                        }
                        if (ang < h180) {
                            if (!(rel > h180)) {
                                side = 1;
                                break;
                            }
                        } else if (!(rel < h180)) {
                            side = 1;
                            break;
                        }
                    }
                    if (side != 0) {
                        vec[0] = mario->position.x;
                        vec[2] = mario->position.z;
                        vec[1] = npc->position.y;
                        dist = PSVECDistance((Vec*)vec, &npc->position);
                        if (!(dist > float_0p5_80419ba0 * (mario->wPlayerCollisionBox.x + npc->width) + float_10_80419bb8)) {
                            dx = npc->position.x - mario->position.x;
                            dz = npc->position.z - mario->position.z;
                            sincosf(&sx, &sz, angleABf(0.0f, 0.0f, dx, dz));
#ifdef MUST_MATCH
                            d = npcNearDistCheck_sqrtf(dx * dx + dz * dz, pool);
#else
                            d = sqrtf(dx * dx + dz * dz);
#endif
                            if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &d, &hitNX, &hitNY, &hitNZ, mario->position.x,
                                               float_0p75_80419bf4 * mario->wPlayerCollisionBox.y + mario->position.y,
                                               mario->position.z, sx, float_0_80419b9c, sz) == NULL) {
                                found = 1;
                                if (dist < nearest) {
                                    nearest = dist;
                                    wk->field_0x10 = npc;
                                }
                            }
                        }
                    }
                }
                if (found != 0) {
                    if (!(npc->jumpFlags & 0x20000000)) {
                        MarioWork* m2 = marioGetPtr();

                        npc->jumpFlags |= 0x20000000;
                        ENC(npc, f32, 0x148) = npc->direction;
                        evtStopID(npc->regularEvtId);
                        if (!(npc->flags & 0x1000000)) {
                            npc->direction = reviseAngle(angleABf(npc->position.x, npc->position.z, m2->position.x, m2->position.z));
                        }
                    }
                } else if (npc->jumpFlags & 0x20000000) {
                    npc->jumpFlags &= ~0x20000000;
                    if (npc->flags & 0x400000) {
                        npc->direction = ENC(npc, f32, 0x148);
                        evtStartID(npc->regularEvtId);
                    } else {
                        ENC(npc, s32, 0x140) |= 0x10000000;
                    }
                }
            }
        }
        ENC(mario, NpcEntry*, 0x1D4) = best;
        if (wk->field_0x10 != NULL && (keyGetButtonTrg(0) & 0x100)) {
            NpcEntry* tgt = wk->field_0x10;
            FieldBattleData* fb3 = gp->fbatData;

            if (fb3->mode == 1) {
                fb3->mode = 2;
                *(s8*)((u8*)fb3 + 2) = 0;
                *(NpcEntry**)((u8*)fb3 + 4) = tgt;
                ENC(tgt, s32, 0x318) = 0;
            }
            return;
        }
    }
    fb = gp->fbatData;
    if (fb->mode == 1) {
        s32* fbatOut2 = (s32*)((u8*)fb + 0x10);

        hit = (NpcEntry*)fbatHitCheck(0x2361, (s32)fbatOut2);
        if (hit != NULL && fb->mode == 1) {
            s32 fval;
            FieldBattleData* fb2;

            fval = *fbatOut2;
            fb2 = gp->fbatData;
            if (fb2->mode == 1) {
                fb2->mode = 3;
                *(s8*)((u8*)fb2 + 2) = 0;
                *(NpcEntry**)((u8*)fb2 + 4) = hit;
                ENC(hit, s32, 0x318) = fval;
            }
            switch (*fbatOut2) {
                case 0x40000:
                case 0x20000:
                case 0x80000:
                    marioChgMotJump2();
                    break;
            }
            effSmallStarEntry(3, 3, hit->position.x, hit->position.y + hit->height, hit->position.z, float_0_80419b9c, float_neg1_80419bf8, float_0_80419b9c);
            return;
        }
    }
    npcKoopaModeEncountCheck();
}

void fbatTalkMode(void) {
    FieldBattleData* fbatData = gp->fbatData;
    NpcEntry* entry = *(NpcEntry**)(fbatData->field_2 + 2);
    MarioWork* mario = marioGetPtr();
    NpcWork* wp;
    EventEntry* evt;

    switch (fbatData->field_2[0]) {
        case 0:
            evtStopID((*(NpcEntry**)(fbatData->field_2 + 2))->regularEvtId);
            if (!(entry->flags & 0x1000000)) {
                entry->direction = reviseAngle(angleABf(entry->position.x, entry->position.z, mario->position.x,
                                                          mario->position.z));
            }
            marioKeyOff();
            partyKeyOff();
            switch (fbatData->mode) {
                case 3:
                    evt = evtEntry(entry->deadEvent, 0, 0);
                    evt->wNpcEventType = 6;
                    break;
                default:
                    evt = evtEntry(entry->talkEvt, 0, 0);
                    evt->wNpcEventType = 6;
                    marioChgTalkMotion();
                    break;
            }
            evt->ownerNPC = entry;
            *(s32*)fbatData->field_C = evt->eventId;
            fbatData->field_2[0]++;
            break;
        case 1:
            if (!evtCheckID(*(s32*)fbatData->field_C)) {
                if (!(entry->jumpFlags & 0x20000000)) {
                    evtStartID((*(NpcEntry**)(fbatData->field_2 + 2))->regularEvtId);
                    entry->direction = *(f32*)entry->field_148;
                }
                if (entry->flags & 0x400) {
                    entry->field_108 = 0;
                }
                if (entry->flags & 0x200) {
                    animPoseSetAnim(entry->poseId, entry->currentAnim, 1);
                }
                marioKeyOn();
                partyKeyOn();
                marioChgStayMotion();
                fbatData->mode = 1;
                wp = gp->inBattle ? &work.battle : &work.field;
                wp->field_0x10 = NULL;
            }
            break;
    }
}

// Scoping `info` and each list loop to its own case (ordinary per-block locals) recovered the case-2/3/4
// temporaries (98.86->99.23). The four function-wide pointers cannot be recoloured by declaration order,
// initializer form, expression form or opt_lifetimes/opt_common_subs/opt_propagation/optimize_for_size.
// Confirmed by a re-run this session: all 720 declaration orders of the six live function-wides are either
// 99.23 or 98.66, and pragma/flag/splitting/aliasing/helper-inline variants keep the same colouring.
// NONMATCHING (99.23%): every instruction matches; only callee-saved register numbers differ. The original
// keeps fbatData=r30, entry=r29, pouch=r31, mario=r27; MWCC here picks fbatData=r28, entry=r29, pouch=r30, mario=r31.
void fbatBattleMode(void) {
    PouchData* pouch;
    NpcEntry* entry;
    FieldBattleData* fbatData;
    s32 i;
    s32 total;
    s32 result;
    NpcEntry* npc;
    NpcWork* wp;
    EventEntry* evt;
    MarioWork* mario;

    fbatData = gp->fbatData;
    entry = fbatData->target;
    pouch = pouchGetPtr();
    mario = marioGetPtr();
    entry->rotation.y = 0.0f;

    switch (fbatData->field_2[0]) {
        case 0:
            if (dbg_battleskip == 1) {
                fbatData->mode = 1;
            } else if (entry->battleInfo.groupSetup == NULL) {
                fbatData->mode = 1;
            } else if (gp->systemLevelFlags & 0x10) {
                fbatData->mode = 1;
            } else if (entry->deadEvent == NULL) {
                fbatData->mode = 1;
            } else {
                s32 flagA;
                s32 flagB;
                FieldBattleInfo* info0;

                *(s32*)fbatData->target->field_140 &= 0xF00;
                evtStopID(fbatData->target->regularEvtId);
                info0 = (FieldBattleInfo*)((u8*)fbatData + 0x20);
                if (!(entry->battleInfo.field_4 & 0x10000000)) {
                    if (pouchEquipCheckBadge(ITEM_BUMP_ATTACK) &&
                        entry->battleInfo.groupSetup->enemyData->mUnitKindParams->mLevel < pouch->level) {
                        flagA = TRUE;
                        flagB = TRUE;
                        goto afterStrike;
                    }
                    if (pouchEquipCheckBadge(ITEM_FIRST_ATTACK) &&
                        (*(s32*)((u8*)fbatData + 0x10) & 0x01FE0000) &&
                        entry->battleInfo.groupSetup->enemyData->mUnitKindParams->mLevel < pouch->level) {
                        flagA = TRUE;
                        flagB = TRUE;
                        goto afterStrike;
                    }
                }
                if (dbg_battleskip == 2) {
                    flagA = TRUE;
                    flagB = TRUE;
                } else {
                    flagA = FALSE;
                    flagB = FALSE;
                }
            afterStrike:
                if (flagA == 0) {
                    if (flagB == 0) {
                        gp->fbatData->firstStrike.enabled = TRUE;
                    }
                    marioFBattlePrepare();
                    seqSetSeq(SEQ_BATTLE, NULL, NULL);
                    if (*(s32*)((u8*)fbatData + 0x10) == 0x10000000) {
                        unk_80058374();
                    }
                    fbatData->field_2[0] = 1;
                } else {
                    if (flagB) {
                        BattleInformationSetResult(info0, 2);
                    } else {
                        BattleInformationSetResult(info0, 5);
                    }
                    fbatData->field_2[0] = 4;
                }
            }
            break;
        case 1:
            if (gp->inBattle) {
                fbatData->field_2[0]++;
            }
            break;
        case 2:
            if (!gp->inBattle) {
                FieldBattleInfo* info2;
                NpcWork* wp2;
                NpcEntry* npc2;
                s32 total2;
                s32 i2;
                fbatData->field_54C[0] = 0;
                info2 = (FieldBattleInfo*)((u8*)fbatData + 0x20);
                result = BattleInformationGetResult(info2);
                if (pouch->mMerleeNextCurseType == 4 && result == 1) {
                    fbatData->field_2[0] = 3;
                    fbatData->field_54C[0] = 1;
                    evt = evtEntry(_evt_majinai_coin_up_effect, 0, 0);
                    *(s32*)((u8*)fbatData + 0x48) = evt->eventId;
                    BattleMajinaiDone();
                    marioKeyOff();
                    wp2 = gp->inBattle ? &work.battle : &work.field;
                    total2 = wp2->total;
                    for (i2 = 0, npc2 = wp2->entries; i2 < total2; i2++, npc2++) {
                        if (npc2 != NULL && (npc2->flags & 2) && (npc2->flags & 0x10000000)) {
                            npc2->jumpFlags |= 0x10000;
                            evtStopID(npc2->regularEvtId);
                            animPoseSetLocalTimeRate(npc2->poseId, 0.0f);
                        }
                    }
                } else {
                    marioFBattlePost();
                    fbatData->field_2[0] = 4;
                }
            }
            break;
        case 3:
            if (!evtCheckID(*(s32*)((u8*)fbatData + 0x48))) {
                NpcWork* wp3;
                NpcEntry* npc3;
                s32 total3;
                s32 i3;
                marioKeyOn();
                marioFBattlePost();
                *(s32*)((u8*)fbatData + 0x48) = 0;
                fbatData->field_2[0] = 4;
                wp3 = gp->inBattle ? &work.battle : &work.field;
                total3 = wp3->total;
                for (i3 = 0, npc3 = wp3->entries; i3 < total3; i3++, npc3++) {
                    if (npc3 != NULL && (npc3->flags & 2) && (npc3->flags & 0x10000000) && (npc3->jumpFlags & 0x10000)) {
                        npc3->jumpFlags &= ~0x10000;
                        evtStartID(npc3->regularEvtId);
                        animPoseSetLocalTimeRate(npc3->poseId, 1.0f);
                    }
                }
            }
            break;
        case 4:
            marioChgMoveMotion();
            if (!gp->inBattle) {
                s32 hasStolen;
                s32 mult;
                FieldBattleInfo* info4;

                info4 = (FieldBattleInfo*)((u8*)fbatData + 0x20);
                result = BattleInformationGetResult(info4);
                hasStolen = FALSE;
                if (entry->battleInfo.stolenItems[0] != 0) {
                    hasStolen = TRUE;
                } else if (entry->battleInfo.stolenItems[1] != 0) {
                    hasStolen = TRUE;
                } else if (entry->battleInfo.stolenItems[2] != 0) {
                    hasStolen = TRUE;
                } else if (entry->battleInfo.stolenItems[3] != 0) {
                    hasStolen = TRUE;
                } else if (entry->battleInfo.stolenItems[4] != 0) {
                    hasStolen = TRUE;
                } else if (entry->battleInfo.stolenItems[5] != 0) {
                    hasStolen = TRUE;
                } else if (entry->battleInfo.stolenItems[6] != 0) {
                    hasStolen = TRUE;
                } else if (entry->battleInfo.stolenItems[7] != 0) {
                    hasStolen = TRUE;
                }
                fbatData->mode = 1;
                if (result == 1 || result == 2) {
                    mult = 1;
                    if ((s32)BattleInformationGetResult(info4) == 1) {
                        entry->btlCoinDropCount = *(u32*)&info4->field_0x14[0];
                    } else if (result == 1) {
                        entry->btlCoinDropCount = entry->field_316;
                    } else {
                        mult = 0;
                        entry->btlCoinDropCount = 0;
                        goto after;
                    }
                    if (fbatData->field_54C[0]) {
                        mult = 3;
                    }
                    if (pouchEquipCheckBadge(ITEM_MONEY_MONEY)) {
                        mult += pouchEquipCheckBadge(ITEM_MONEY_MONEY);
                    }
                    if (entry->battleInfo.field_4 & 0x100) {
                        mult++;
                    }
                after:
                    entry->btlCoinDropCount *= mult;
                    if (!hasStolen) {
                        npcGroupDead(entry, 0);
                    } else {
                        if (entry->flags & 0x400) {
                            entry->field_108 = 0;
                        }
                        entry->wFbatHitCheckRelated = 0x5A;
                        *(s32*)entry->field_140 &= ~0x100;
                        *(s32*)entry->field_140 |= 0x10000000;
                        if (entry->flags & 0x200) {
                            animPoseSetAnim(entry->poseId, entry->currentAnim, TRUE);
                        }
                        npcGetBackItemEntry(entry);
                    }
                } else if (result == 4) {
                    if (entry->flags & 0x400) {
                        entry->field_108 = 0;
                    }
                    *(s32*)entry->field_140 |= 0x10000000;
                    marioSetMutekiTime(3000);
                    *(s16*)((u8*)mario + 0x2C0) = sysMsec2Frame(3000);
                    if (entry->flags & 0x200) {
                        animPoseSetAnim(entry->poseId, entry->currentAnim, TRUE);
                    }
                }
            }
            break;
    }
}

void _npcDead(NpcEntry* entry, s32 koopaEncounterType) {
    *(s32*)entry->field_140 |= 0x10;
    entry->jumpFlags |= 4;
    entry->koopaEncounterType = koopaEncounterType;
    if (entry->flags & 4) {
        fbatGetPointer()->deadInfo[0].mask |= (1 << entry->field_314);
    }
}

// The walk, the recursive inlining depth and the inlined _npcDead copies all match; the only diffs
// are the deepest slave loop's callee-saved registers. The original keeps the shift's `1` in r27
// (loaded slave in r24, array pointer in r25, counter in r26); ours lets the deepest copy
// rematerialise `1` into a volatile, so slave/pointer/counter land in r25/r26/r27 and the prologue
// is `stmw r25` instead of `stmw r24`. This requires _npcDead to call fbatGetPointer(), which is
// what makes the deepest inlined copy emit a real `bl fbatGetPointer`.
// NONMATCHING (97.46%): deepest slave-loop register allocation (see above).
void _npcGroupDead(NpcEntry* entry, s32 type) {
    int i;

    if (entry->next) {
        _npcGroupDead(entry->next, type);
    }
    for (i = 0; i < 4; i++) {
        if (entry->slaves[i]) {
            _npcDead(entry->slaves[i], type);
        }
    }
    _npcDead(entry, type);
}

// dont_inline keeps npcGroupDead out of line: the original bot callers emit a real `bl npcGroupDead`.
#pragma dont_inline on
void npcGroupDead(NpcEntry* entry, s32 type) {
    if (entry->master) {
        entry = entry->master;
    }
    while (entry->prev) {
        entry = entry->prev;
    }
    _npcGroupDead(entry, type);
}
#pragma dont_inline reset

NpcEntry* fbatNpcTalkCheck(void) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;

    if (wp->field_0x10 && marioChkTalkable()) {
        return wp->field_0x10;
    }
    return NULL;
}

void fbatHitCheckAll(void) {
    fbatHitCheck(0x13F1, 0);
}

// What fbatHitCheck reports through its second argument (fbatEncountCheck reads it at fbatData + 0x10).


// Mario can attack a field npc: not in a cutscene, menu or talk, not riding Vivian's shadow or a pipe.
static inline BOOL fbatAttackEnabled(NpcEntry* npc) {
    MarioWork* mario = marioGetPtr();
    NpcWork* npcWork = npcGetWorkPtrInline();

    if (mario->flags & 0x82000000) {
        return FALSE;
    }
    if (!(npcWork->flags & 1) && (mario->flags & 0xA)) {
        return FALSE;
    }
    if ((s32)gp->field_0x164 != 0) {
        return FALSE;
    }
    if (seqGetSeq() != 2) {
        return FALSE;
    }
    if (mario->currMotionId == 0x19) {
        return FALSE;
    }
    if (vivianGetStatus() != 0) {
        return FALSE;
    }
    if (mario->currMotionId == 0x1F || mario->currMotionId == 0x20) {
        return FALSE;
    }
    if (*(u16*)&npc->wFbatHitCheckRelated != 0) {
        return FALSE;
    }
    return TRUE;
}

// The hammer's elevation per swing frame, in degrees.

// The npc's own event has reached state 5.
static inline BOOL fbatNpcEventDone(NpcEntry* npc) {
    if (evtGetPtrID(npc->regularEvtId) != NULL) {
        return evtGetPtrID(npc->regularEvtId)->wNpcEventType == 5;
    }
    return FALSE;
}

// Finds the nearest field npc Mario's attacks hit, for each kind of attack `flags` enables in turn: jumps
// (0x30), the hammer (0x40), the spin hammer (0x80), Koops' shell (0x100) and partner 3 (0x200). Flag 0x2000
// only tests (no attack announce); 0x1000 looks at npcs with flag 8 instead of the others.
// NONMATCHING (99.3%): every call, branch and constant matches; one redundant `addi r22,r29,0` copies the
// hammer angle table's rodata base instead of reusing r29, and the callee-saved GPR colouring then differs.
void* fbatHitCheck(s32 flags, s32 out) {
    NpcEntry* hit = NULL;
    NpcEntry* npc = NULL;
    NpcWork* npcWork = npcGetWorkPtrInline();
    MarioWork* mario;
    FbatHitInfo info;
    s32 count;
    u32 dryRun;
    s32 i;
    f32 dist;

    mario = marioGetPtr();
    info.dist = float_1000_80419bc8;
    info.kind = 0;
    info.flags &= ~1;
    dryRun = flags & 0x2000;
    if (dryRun) {
        info.flags |= 1;
    }

    // jumps: the npc under Mario's feet
    if (flags & 0x30) {
        u32 stomp = flags & 0x10;

        count = npcWork->total; // retail reads the total here, and only here
        npc = npcWork->entries;
        if (!(stomp && mario->unk4C > 0) && fbatAttackEnabled(npc)) {
            u32 flag8 = flags & 0x1000;
            u32 jump = flags & 0x20;

            for (i = 0; i < count; i++, npc++) {
                if (npc == NULL || !(npc->flags & 2)) {
                    continue;
                }
                if (flag8) {
                    if (!(npc->flags & 8)) {
                        continue;
                    }
                } else if (npc->flags & 8) {
                    continue;
                }
                if (dryRun && !(npc->reactionFlags & 1)) {
                    continue;
                }
                if ((npc->flags & 0x20) || (npc->jumpFlags & 4) || (npc->flags & 0x08000000)) {
                    continue;
                }
                if (mario->position.y >= npc->position.y + npc->height) {
                    continue;
                }
                if (mario->position.y + mario->wPlayerCollisionBox.y <= npc->position.y) {
                    continue;
                }
                {
                    f32 dz = mario->position.z - npc->position.z;
                    f32 dx = mario->position.x - npc->position.x;

                    dist = sqrtf(dx * dx + dz * dz);
                }
                if (dist > float_0p5_80419ba0 * (mario->wPlayerCollisionBox.x + npc->width)) {
                    continue;
                }
                if (dist >= info.dist) {
                    continue;
                }
                if (stomp && !(npc->jumpFlags & 0x10000) && mario->unk4C <= 0) {
                    if (npc->field_2F8 == 0 || fbatNpcEventDone(npc)) {
                        if (npc->flags & 0x20000000) {
                            continue;
                        }
                        info.kind = 0x10000;
                        hit = npc;
                        info.dist = dist;
                        if (!dryRun) {
                            fbatSetAttackAnnounce(info.kind);
                        }
                    } else if (pouchEquipCheckBadge(0x140)) {
                        // Spike Shield: the stomp lands anyway
                        info.kind = 0x10000;
                        hit = npc;
                        info.dist = dist;
                        if (!dryRun) {
                            fbatSetAttackAnnounce(info.kind);
                        }
                    } else {
                        // a spiked enemy: it strikes first
                        info.kind = 0x10000000;
                        hit = npc;
                        info.dist = dist;
                        if (!dryRun) {
                            fbatSetAttackAnnounce(info.kind);
                        }
                        info.field_0x4 = npc->field_2F8;
                    }
                }
                if (!jump) {
                    continue;
                }
                if ((mario->flags & 0x20000) && !(mario->flags & 0x10000) && !(npc->flags & 0x20000000)) {
                    hit = npc;
                    info.dist = dist;
                    if (marioGetJabaraState() == 6) {
                        info.kind = 0x20000;
                        if (!dryRun) {
                            fbatSetAttackAnnounce(info.kind);
                        }
                    } else if (mario->currMotionId == 0x10) {
                        info.kind = 0x40000;
                        if (!dryRun) {
                            fbatSetAttackAnnounce(info.kind);
                        }
                    } else {
                        info.kind = 0x20000;
                        if (!dryRun) {
                            fbatSetAttackAnnounce(info.kind);
                        }
                    }
                } else if (marioGetJabaraState() == 6 && !(npc->flags & 0x20000000)) {
                    hit = npc;
                    info.dist = dist;
                    info.kind = 0x20000;
                    if (!dryRun) {
                        fbatSetAttackAnnounce(info.kind);
                    }
                }
            }
        }
    }

    // the hammer
    if (flags & 0x40) {
        while (hit == NULL) {
            if ((flags & 0x10) && mario->unk4C > 0) {
                break;
            }
            if (!fbatAttackEnabled(npc)) {
                break;
            }
            if (flags & 1) {
                f32 radius;
                f32 angle;
                f32 frame;
                f32 s, c, s2, c2;
                f32 x, y, z;
                s32 frameNo;
                u32 flag8;
                MarioWork* pose;
                f32 yaw;

                if (mario->currMotionId != 0x12) {
                    break;
                }
                radius = float_1p5_80419bcc * mario->wPlayerCollisionBox.x;

                yaw = reviseAngle((mario->unk1A8 - float_90_80419bd0) + camGetPtr(CAMERA_3D)->viewYaw);
                angle = yaw;
                pose = marioGetPtr();
                animPoseGetAnimPosePtr(pose->paperAnimGroupId[marioAnimeId()]);
                frameNo = pose->multiTimer;
                if (frameNo > 20) {
                    frameNo = 10;
                }
                frame = frameNo;
                pose = marioGetPtr();
                {
#ifdef MUST_MATCH
                    // The original has one copy of this table (dat_802c2f9c), shared with
                    // _npcHitCheckHammerAllMotion's (whose initialiser is where it pools); read it through the
                    // unit's rodata base.
                    FbatAngleTable frameAngle = *(FbatAngleTable*)((u8*)vec3_802c2f78 + 0x24); // dat_802c2f9c
#else
                    FbatAngleTable frameAngle = {{0, 0, 0, 0, 45, 60, 90, 100, 101, 101, 101}};
#endif

                    if (frame < float_0_80419b9c) {
                        frame = float_0_80419b9c;
                    }
                    if (frame >= float_11_80419bbc) {
                        frame = float_10_80419bb8;
                    }
                    sincosf(&s, &c, angle);
                    sincosf(&s2, &c2, frameAngle.deg[(s32)frame]);
                }
                x = s2 * (s * radius) + pose->position.x;
                z = s2 * (c * radius) + pose->position.z;
                y = radius * c2 + pose->position.y;
                flag8 = flags & 0x1000;
                npc = npcWork->entries;
                for (i = 0; i < count; i++, npc++) {
                    if (npc == NULL || !(npc->flags & 2)) {
                        continue;
                    }
                    if (flag8) {
                        if (!(npc->flags & 8)) {
                            continue;
                        }
                    } else if (npc->flags & 8) {
                        continue;
                    }
                    if (dryRun && !(npc->reactionFlags & 2)) {
                        continue;
                    }
                    if ((npc->flags & 0x20000000) || (npc->flags & 0x20) || (npc->flags & 0x08000000)
                        || (npc->jumpFlags & 4)) {
                        continue;
                    }
                    if (_npcHitCheckSphere(npc, &dist, x, y, z, float_12_80419bc0) == NULL || dist >= info.dist) {
                        continue;
                    }
                    info.dist = dist;
                    hit = npc;
                    info.kind = 0x100000;
                    if (!dryRun) {
                        fbatSetAttackAnnounce(info.kind);
                    }
                    break;
                }
            } else {
                f32 radius = float_1p5_80419bcc * mario->wPlayerCollisionBox.x;
                f32 angle = reviseAngle((mario->unk1A8 - float_90_80419bd0) + camGetPtr(CAMERA_3D)->viewYaw);
                u32 flag8 = flags & 0x1000;

                npc = npcWork->entries;
                for (i = 0; i < count; i++, npc++) {
                    if (npc == NULL || !(npc->flags & 2)) {
                        continue;
                    }
                    if (flag8) {
                        if (!(npc->flags & 8)) {
                            continue;
                        }
                    } else if (npc->flags & 8) {
                        continue;
                    }
                    if (dryRun && !(npc->reactionFlags & 2)) {
                        continue;
                    }
                    if ((npc->flags & 0x20000000) || (npc->flags & 0x20) || (npc->jumpFlags & 4)
                        || (npc->flags & 0x08000000)) {
                        continue;
                    }
                    if (_npcHitCheckHammerAllMotion(npc, &dist, radius, angle) == NULL || dist >= info.dist) {
                        continue;
                    }
                    info.dist = dist;
                    hit = npc;
                    info.kind = 0x100000;
                    if (!dryRun) {
                        fbatSetAttackAnnounce(info.kind);
                    }
                    break;
                }
            }
            break;
        }
    }

    // the spin hammer
    if (flags & 0x80) {
        while (hit == NULL) {
            if ((flags & 0x10) && mario->unk4C > 0) {
                break;
            }
            if (!fbatAttackEnabled(npc)) {
                break;
            }
            if (flags & 1) {
                f32 angle;
                f32 radius;
                f32 s, c;
                f32 x, y, z;
                u32 flag8;
                MarioWork* m;

                if (mario->currMotionId != 0x13) {
                    break;
                }
                angle = marioGetAngleKaitenHammer();
                if (angle < float_0_80419b9c) {
                    break;
                }
                radius = float_1p5_80419bcc * mario->wPlayerCollisionBox.x;
                m = marioGetPtr();
                sincosf(&s, &c, angle);
                flag8 = flags & 0x1000;
                x = s * radius + m->position.x;
                z = c * radius + m->position.z;
                y = float_15_80419bc4 + m->position.y;
                npc = npcWork->entries;
                for (i = 0; i < count; i++, npc++) {
                    if (npc == NULL || !(npc->flags & 2)) {
                        continue;
                    }
                    if (flag8) {
                        if (!(npc->flags & 8)) {
                            continue;
                        }
                    } else if (npc->flags & 8) {
                        continue;
                    }
                    if (dryRun && !(npc->reactionFlags & 2)) {
                        continue;
                    }
                    if ((npc->flags & 0x20000000) || (npc->flags & 0x20) || (npc->jumpFlags & 4)
                        || (npc->flags & 0x08000000)) {
                        continue;
                    }
                    if (_npcHitCheckSphere(npc, &dist, x, y, z, float_12_80419bc0) == NULL || dist >= info.dist) {
                        continue;
                    }
                    info.dist = dist;
                    hit = npc;
#ifdef MUST_MATCH
                    // The two arms write the same field through syntactically different lvalues so the
                    // frontend keeps the if/else; with the plain form MWCC if-converts it to a select.
                    if (pouchGetHammerLv() < 3) {
                        info.kind = 0x200000;
                    } else {
                        ENC(&info, s32, 0) = 0x400000;
                    }
#else
                    if (pouchGetHammerLv() < 3) {
                        info.kind = 0x200000;
                    } else {
                        info.kind = 0x400000;
                    }
#endif
                    if (!dryRun) {
                        fbatSetAttackAnnounce(info.kind);
                    }
                    break;
                }
            } else {
                f32 radius = float_1p5_80419bcc * mario->wPlayerCollisionBox.x;
                f32 angle = reviseAngle((mario->unk1A8 - float_90_80419bd0) + camGetPtr(CAMERA_3D)->viewYaw);
                u32 flag8 = flags & 0x1000;

                npc = npcWork->entries;
                for (i = 0; i < count; i++, npc++) {
                    if (npc == NULL || !(npc->flags & 2)) {
                        continue;
                    }
                    if (flag8) {
                        if (!(npc->flags & 8)) {
                            continue;
                        }
                    } else if (npc->flags & 8) {
                        continue;
                    }
                    if (dryRun && !(npc->reactionFlags & 2)) {
                        continue;
                    }
                    if ((npc->flags & 0x20000000) || (npc->flags & 0x20) || (npc->jumpFlags & 4)
                        || (npc->flags & 0x08000000)) {
                        continue;
                    }
                    if (_npcHitCheckHammerAllMotion(npc, &dist, radius, angle) == NULL || dist >= info.dist) {
                        continue;
                    }
                    info.dist = dist;
                    hit = npc;
#ifdef MUST_MATCH
                    // The two arms write the same field through syntactically different lvalues so the
                    // frontend keeps the if/else; with the plain form MWCC if-converts it to a select.
                    if (pouchGetHammerLv() < 3) {
                        info.kind = 0x200000;
                    } else {
                        ENC(&info, s32, 0) = 0x400000;
                    }
#else
                    if (pouchGetHammerLv() < 3) {
                        info.kind = 0x200000;
                    } else {
                        info.kind = 0x400000;
                    }
#endif
                    if (!dryRun) {
                        fbatSetAttackAnnounce(info.kind);
                    }
                    break;
                }
            }
            break;
        }
    }

    // Koops' shell
    if (flags & 0x100) {
        PartyEntry* party = partyGetPtr(marioGetPartyId());

        while (hit == NULL) {
            u32 attack;

            if ((flags & 0x10) && mario->unk4C > 0) {
                break;
            }
            if (!fbatAttackEnabled(npc)) {
                break;
            }
            attack = flags & 1;
            if (attack) {
                s32 status;

                if (party == NULL || party->field_33 != 2 || !(party->flags & 0x100)) {
                    break;
                }
                status = nokonokoGetStatus(party);
                if (status < 2 || status > 5) {
                    break;
                }
            }
            if (attack) {
                f32 radius = float_0p5_80419ba0 * party->field_F4;
                f32 y = float_0p5_80419ba0 * *(f32*)((u8*)party + 0xF0) + party->position.y; // 0xF0: the shell's height
                f32 z = party->position.z;
                f32 x = party->position.x;
                u32 flag8 = flags & 0x1000;

                npc = npcWork->entries;
                for (i = 0; i < count; i++, npc++) {
                    if (npc == NULL || !(npc->flags & 2)) {
                        continue;
                    }
                    if (flag8) {
                        if (!(npc->flags & 8)) {
                            continue;
                        }
                    } else if (npc->flags & 8) {
                        continue;
                    }
                    if (dryRun && !(npc->reactionFlags & 4)) {
                        continue;
                    }
                    if ((npc->flags & 0x20000000) || (npc->flags & 0x20) || (npc->jumpFlags & 4)
                        || (npc->flags & 0x08000000)) {
                        continue;
                    }
                    if (_npcHitCheckSphere(npc, &dist, x, y, z, radius) == NULL || dist >= info.dist) {
                        continue;
                    }
                    info.dist = dist;
                    hit = npc;
                    info.kind = 0x800000;
                    if (!dryRun) {
                        fbatSetAttackAnnounce(info.kind);
                    }
                    break;
                }
            }
            break;
        }
    }

    // partner 3
    if (flags & 0x200) {
        PartyEntry* party = partyGetPtr(marioGetPartyId());

        while (hit == NULL) {
            NpcEntry* other;
            u32 flag8;

            if ((flags & 0x10) && mario->unk4C > 0) {
                break;
            }
            if (!fbatAttackEnabled(npc)) {
                break;
            }
            if (!(flags & 1) || party == NULL || party->field_33 != 3
                || *((u8*)gp->fbatData + 0x54D) == 0) { // fbatData 0x54D: no name yet
                break;
            }
            flag8 = flags & 0x1000;
            other = npcWork->entries;
            for (i = 0; i < count; i++, other++) {
                if (other == NULL || !(other->flags & 2)) {
                    continue;
                }
                if (flag8) {
                    if (!(other->flags & 8)) {
                        continue;
                    }
                } else if (other->flags & 8) {
                    continue;
                }
                if (dryRun && !(other->reactionFlags & 8)) {
                    continue;
                }
                if ((other->flags & 0x20000000) || (other->flags & 0x20) || (other->jumpFlags & 4)
                    || (other->flags & 0x08000000)) {
                    continue;
                }
                if (_npcHitCheckSphere(other, &dist, party->position.x,
                                       float_0p5_80419ba0 * *(f32*)((u8*)party + 0xF0) + party->position.y, party->position.z,
                                       float_30_80419bd4) == NULL
                    || dist >= info.dist) {
                    continue;
                }
                info.dist = dist;
                hit = other;
                info.kind = 0x1000000;
                if (!dryRun) {
                    fbatSetAttackAnnounce(info.kind);
                }
                break;
            }
            break;
        }
    }

    if ((FbatHitInfo*)out != NULL) {
        *(FbatHitInfo*)out = info;
    }
    return hit;
}

#pragma force_active on
#pragma force_active reset

// Match-only sqrtf for _npcHitCheckSphere: the original reaches the shared sqrtf double pool
// (0.5 / 3.0 / 0.0 at vec3_802c2f78 + 0x1c0) with vec3_802c2f78 as the base register, so reproduce
// that access instead of letting the compiler emit its own .rodata pool (rule 7).
#ifdef MUST_MATCH
static inline f32 npcHitCheckSphere_sqrtf(f32 x, const f64* pool) {
    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        f64 half = pool[0];
        f64 three = pool[1];
        guess = half * guess * (three - guess * guess * x);
        guess = half * guess * (three - guess * guess * x);
        guess = half * guess * (three - guess * guess * x);
        return x * guess;
    } else if (x < pool[2]) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}
#endif

// dont_inline: retail calls _npcHitCheckSphere from fbatHitCheck (4 sites) instead of inlining it.
#pragma dont_inline on
NpcEntry* _npcHitCheckSphere(NpcEntry* entry, f32* outDist, f32 x, f32 y, f32 z, f32 radius) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((u8*)vec3_802c2f78 + 0x1c0);
#endif
    f32 dx = x - entry->position.x;
    f32 dy = y - entry->position.y;
    f32 dz = z - entry->position.z;

    if (y != entry->position.y) {
        if (y >= entry->position.y && y <= entry->position.y + entry->height) {
            dy = 0.0f;
        } else if (y > entry->position.y + entry->height) {
            dy = y - (entry->position.y + entry->height);
        }
    }
#ifdef MUST_MATCH
    *outDist = npcHitCheckSphere_sqrtf(dx * dx + dy * dy + dz * dz, pool);
#else
    *outDist = sqrtf(dx * dx + dy * dy + dz * dz);
#endif
    if (*outDist < radius + entry->width * 0.5f) {
        return entry;
    }
    return NULL;
}
#pragma dont_inline reset

// Hammer sweep over 12 elevation angles placing a point radius from Mario along (sin/cos of angle,
// elevation from the shared table), then the _npcHitCheckSphere test. The local angle table
// materialises as a per-iteration stack copy (correct) but from this unit's own `.rodata` template
// instead of dat_802c2f9c (vec3_802c2f78 + 0x24), and the compiler's double pool (int->float 2^52,
// sqrtf 0.5/3.0) lands at 0x48 instead of vec3_802c2f78 + 0x1d8/0x1c0, because this unit's rodata
// data is not written yet.
// Calling marioGetPtr() before the angle-table declaration (which needs a nested block, declarations
// precede statements) and declaring c2,s2,c1,s1 reproduces the target's call order and stack slots.
// Removing the `dist`/`npc` initialisers (the loop always assigns them) and declaring dx/dy/dz as dy,dx,dz
// recovered the prologue; computing the point into a `Vec pos` (pos.x, pos.z, pos.y) and copying to px/pz/py
// gets the target's px,pz,py FP order (was py,px,pz). The angle-table and sqrtf double-pool offsets resolve
// once the unit's rodata is complete.
// The allocator coalesces pos.y into the register of its dead addend here; the original instead reuses the
// register that held s2. No declaration order or expression order in the source moves that choice.
// NONMATCHING (99.78%): only the FP register holding pos.y differs (ours f3, target f6; dx then follows into f5).
NpcEntry* _npcHitCheckHammerAllMotion(NpcEntry* entry, f32* outDist, f32 radius, f32 angle) {
    MarioWork* mario;
    f32 i = 0.0f;
    f32 dist;
    NpcEntry* npc;
    NpcEntry* hit;

    for (; i <= 11.0f; i += 1.0f) {
        f32 idx = i;
        f32 c2, s2, c1, s1;
        Vec pos;
        f32 px, py, pz;
        f32 dy, dx, dz;

        mario = marioGetPtr();
        {
        s32 angles[] = { 0, 0, 0, 0, 45, 60, 90, 100, 101, 101, 101 };

        if (i < 0.0f) {
            idx = 0.0f;
        }
        if (idx >= 11.0f) {
            idx = 10.0f;
        }
        sincosf(&s1, &c1, angle);
        sincosf(&s2, &c2, (f32)angles[(s32)idx]);
        pos.x = s2 * (s1 * radius) + mario->position.x;
        pos.z = s2 * (c1 * radius) + mario->position.z;
        pos.y = radius * c2 + mario->position.y;
        px = pos.x;
        pz = pos.z;
        py = pos.y;
        dx = px - entry->position.x;
        dy = py - entry->position.y;
        dz = pz - entry->position.z;
        if (py != entry->position.y) {
            if (py >= entry->position.y && py <= entry->position.y + entry->height) {
                dy = 0.0f;
            } else if (py > entry->position.y + entry->height) {
                dy = py - (entry->position.y + entry->height);
            }
        }
        dist = sqrtf(dx * dx + dy * dy + dz * dz);
        npc = (dist < entry->width * 0.5f + 12.0f) ? entry : NULL;
        hit = npc;
        if (npc != NULL) {
            break;
        }
        }
    }
    *outDist = dist;
    return hit;
}

BOOL npcCheckInterrupt(NpcEntry* npc) {
    EventEntry* evt;

    evtGetPtrID(npc->regularEvtId);
    if (*(s32*)npc->field_140 & ~0xF00) {
        evt = NULL;
        if ((*(s32*)npc->field_140 & 0x10) && npc->deadEvent) {
            evt = evtEntry(npc->deadEvent, 0, 0x20);
            evt->wNpcEventType = 8;
            evt->ownerNPC = npc;
        } else if ((*(s32*)npc->field_140 & 0x10000000) && npc->moveEvent) {
            evt = evtEntry(npc->moveEvent, 0, 0x20);
            evt->wNpcEventType = 1;
            evt->ownerNPC = npc;
        } else if ((*(s32*)npc->field_140 & 0x8) && npc->blowEvent) {
            evt = evtEntry(npc->blowEvent, 0, 0x20);
            evt->wNpcEventType = 5;
            evt->ownerNPC = npc;
        } else if ((*(s32*)npc->field_140 & 1) && npc->findEvent && !(npc->jumpFlags & 0x1000) &&
                   !(npc->flags & 0x200000) && !(*(s32*)npc->field_140 & 0x100)) {
            evt = evtEntry(npc->findEvent, 0, 0x20);
            evt->wNpcEventType = 2;
            evt->ownerNPC = npc;
        } else if ((*(s32*)npc->field_140 & 2) && npc->lostEvent && !(npc->jumpFlags & 0x1000) &&
                   !(npc->flags & 0x200000)) {
            evt = evtEntry(npc->lostEvent, 0, 0x20);
            evt->wNpcEventType = 3;
            evt->ownerNPC = npc;
        } else if ((*(s32*)npc->field_140 & 4) && npc->returnEvent && !(npc->jumpFlags & 0x1000) &&
                   !(npc->flags & 0x200000)) {
            evt = evtEntry(npc->returnEvent, 0, 0x20);
            evt->wNpcEventType = 4;
            evt->ownerNPC = npc;
        }
        if (evt != NULL) {
            evtDeleteID(npc->regularEvtId);
            npc->regularEvtId = evt->eventId;
            *(s32*)npc->field_140 &= 0xF00;
            npc->jumpFlags &= ~0x1000;
            npc->field_188 = 0;
            npc->moveSpeed = 0.0f;
            return TRUE;
        }
    }
    *(s32*)npc->field_140 &= 0xF00;
    npc->jumpFlags &= ~0x1000;
    return FALSE;
}

// Solved by using the declared `field_9C` for the 0xB0 roll store (MWCC then leaves it
// indexed with `stbx` while the heldItems loop strength-reduces, as retail does) and by declaring `i` before
// `u8* p`. The first roll loop now matches. Also: declaring `num` before `enemy` puts the heldItems loop's
// num/enemy/walk in r27/r26/r30 (retail r30/r26/r27, still swapped). The level test is a switch (levels 1 and 2
// skip the roll; 0 and everything else roll), whose lowering is retail's `beq/blt; cmpwi 3; bge roll; b body`.
// NONMATCHING (95.48%): the heldItems loop's num/walk register pairing and its increment-block placement.
void npcSetupBattleInfo(NpcEntry* npc, BattleSetupData* setup) {
    s32 i;
    u8* p;
    NpcBattleInfo* info = &npc->battleInfo;
    NpcBattleSetupEntry* sel;
    NpcBattleSetupEntry* entry;
    s32 num;
    BattleUnitSetup* enemy;
    ItemDropData* list;
    ItemDropData* d;
    s32 evt;
    s32 pick;
    s32 sum;

    memset(info, 0, 0xC8);
    memset(info->recoveredItems, 0, 0x20);
    if (setup == NULL) {
        return;
    }
    if ((u32)setup->name == 0) {
        return;
    }
    *(s32*)&info->field_9C[0x10] = setup->field_0x1C;
    switch (*(s32*)&info->field_9C[0x10]) {
    case 1:
    case 2:
        break;
    case 0:
    default:
        for (i = 0, p = (u8*)setup; i < 0x10; i++) {
            info->field_9C[0x14 + i] = p[0x20] + irand((s8)p[0x21] - (s8)p[0x20] + 1);
            p += 2;
        }
        break;
    }
    info->field_4 = setup->field_0x18;
    *(s32*)info->field_8 = setup->field_0x8;
    info->itemDropped = 0;
    info->heartsDropped = 0;
    info->flowersDropped = 0;
    *(s32*)&info->field_9C[0x24] = (s32)setup->music;
    evt = setup->flag;
    if (evt >= GSWF(0) && evt <= EVTDAT_GSWF_MAX) {
        if (swGet(evt + EVTDAT_GSWF_BASE)) {
            sel = setup->onLoadouts;
        } else {
            sel = setup->offLoadouts;
        }
    } else {
        sel = setup->offLoadouts;
    }
    info->groupSetup = NULL;
    if (sel != NULL) {
        entry = sel;
        sum = 0;
        while (entry->weight > 0) {
            sum += entry->weight;
            entry++;
        }
        if (sum > 0) {
            pick = irand(sum);
            for (;;) {
                pick -= sel->weight;
                if (pick < 0) {
                    break;
                }
                sel++;
            }
            info->groupSetup = sel->setup;
            *(s32*)info = (s32)sel->stageData;
        }
    }
    if (info->groupSetup != NULL) {
        num = info->groupSetup->numEnemies;
        enemy = info->groupSetup->enemyData;
        memset(info->heldItems, 0, 0x20);
        for (i = 0; i < num; i++, enemy++) {
            list = enemy->itemDropTable;
            if (list != NULL) {
                d = list;
                sum = 0;
                while (d->holdWeight > 0 || d->type != 0) {
                    sum += d->holdWeight;
                    d++;
                }
                if (sum > 0) {
                    pick = irand(sum);
                    while ((pick -= list->holdWeight) >= 0) {
                        list++;
                    }
                    info->heldItems[i] = list->type;
                    info->itemDropLists[i] = enemy->itemDropTable;
                }
            }
        }
    }
    npc->field_316 = 0;
    if (info->groupSetup != NULL) {
        npc->field_316 = *(u8*)((u8*)info->groupSetup->enemyData->mUnitKindParams + 0x12);
    }
    npc->flags |= 0x10000000;
}

void npcSetBattleInfo(NpcEntry* npc, s32 battleInfoId) {
    BattleSetupData* setup;
    s32 i;

    if (battleInfoId == -1) {
        setup = NULL;
    } else {
        char areaName[4];
        AreaData* area;

        areaName[0] = gp->currentMapName[0];
        areaName[1] = gp->currentMapName[1];
        areaName[2] = gp->currentMapName[2];
        areaName[3] = 0;
        area = areaDataPtr(areaName);
        setup = (BattleSetupData*)area->field_8;
        while (battleInfoId > 0) {
            setup++;
            battleInfoId--;
        }
    }
    npcSetupBattleInfo(npc, setup);
    for (i = 0; i < 4; i++) {
        if (npc->slaves[i] != NULL) {
            npc->slaves[i]->battleInfo = npc->battleInfo;
        }
    }
}

void npcStopForEvent(void) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    int i;
    s32 total;
    NpcEntry* entry;

    total = wp->total;
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        npcStopForOneEvent(entry);
    }
}

void npcStopForOneEvent(NpcEntry* entry) {
    if (entry && (entry->flags & 2) && (entry->flags & 0x10000000)) {
        entry->jumpFlags |= 0x10000;
        evtStopID(entry->regularEvtId);
        animPoseSetLocalTimeRate(entry->poseId, 0.0f);
    }
}

void npcStartForEvent(void) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    int i;
    s32 total;
    NpcEntry* entry;

    total = wp->total;
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        npcStartForOneEvent(entry);
    }
}

void npcStartForOneEvent(NpcEntry* entry) {
    if (entry && (entry->flags & 2) && (entry->flags & 0x10000000) && (entry->jumpFlags & 0x10000)) {
        entry->jumpFlags &= ~0x10000;
        evtStartID(entry->regularEvtId);
        animPoseSetLocalTimeRate(entry->poseId, 1.0f);
    }
}

void npcSetColor(const char* name, GXColor* color) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    NpcEntry* entry;
    s32 total;
    s32 i;

    total = wp->total;
    entry = wp->entries;
    for (i = 0; i < total; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->description, name)) {
            break;
        }
    }
    entry->color = *color;
}

void npcBlurOn(const char* name) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    NpcEntry* entry;
    s32 total;
    int i;

    total = wp->total;
    entry = wp->entries;
    for (i = 0; i < total; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->description, name)) {
            break;
        }
    }
    entry->flags |= 0x100;
    entry->positionHistory[0] = entry->position;
    entry->positionHistory[1] = entry->position;
    entry->positionHistory[2] = entry->position;
    entry->positionHistory[3] = entry->position;
    entry->positionHistory[4] = entry->position;
}

void npcBlurOff(const char* name) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    NpcEntry* entry;
    s32 total;
    int i;

    total = wp->total;
    entry = wp->entries;
    for (i = 0; i < total; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->description, name)) {
            break;
        }
    }
    entry->flags &= ~0x100;
}

void* npcGetBtlSetupWork(NpcEntry* npc, s32 index) {
    return ((void**)((u8*)npc + 0x2CC))[index];
}

void npcSetBtlSetupWork(NpcEntry* npc, s32 index, void* work) {
    ((void**)((u8*)npc + 0x2CC))[index] = work;
}

void npcSetSlave(NpcEntry* npc, NpcEntry* slave, s32 id) {
    slave->master = npc;
    npc->slaves[id] = slave;
}

void npcSetLink(NpcEntry* npc, NpcEntry* link) {
    npc->next = link;
    link->prev = npc;
}

NpcTerritoryType dbGetDefData(NpcTerritoryInfo* info, const char* description) {
    NpcTerritoryType type;

    type = NPC_TERRITORY_TYPE_NOTHING;
    for (;;) {
        if (info->description == NULL) {
            break;
        }
        if (strcmp(info->description, description) == 0) {
            type = info->type;
            break;
        }
        info++;
    }
    return type;
}

void npcExecAllInitEvt(void) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    int i;
    s32 total;
    NpcEntry* entry;

    total = wp->total;
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (entry != NULL && (entry->flags & 1)) {
            if (entry->initEvent) {
                EventEntry* evt;

                if (entry->initEvtId) {
                    evtDeleteID(entry->initEvtId);
                }
                evt = evtEntry(entry->initEvent, 0, 0);
                evt->wNpcEventType = 0;
                evt->ownerNPC = entry;
                entry->initEvtId = evt->eventId;
            } else {
                entry->initEvtId = 0;
            }
        }
    }
}

BOOL npcWaitAllInitEvtEnd(void) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    BOOL result = TRUE;
    int i;
    s32 total;
    NpcEntry* entry;

    total = wp->total;
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (entry != NULL && (entry->flags & 1) && !(entry->flags & 2)) {
            if (entry->initEvtId) {
                result = FALSE;
                if (!evtCheckID(entry->initEvtId)) {
                    entry->initEvtId = 0;
                    entry->flags |= 2;
                }
            } else {
                entry->flags |= 2;
            }
        }
    }
    return result;
}

void npcExecAllReglEvt(void) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    int i;
    s32 total;
    NpcEntry* entry;

    total = wp->total;
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (entry != NULL && (entry->flags & 1) && entry->moveEvent) {
            EventEntry* evt;

            if (entry->regularEvtId) {
                evtDeleteID(entry->regularEvtId);
            }
            evt = evtEntry(entry->moveEvent, 0, 0x20);
            evt->wNpcEventType = 1;
            evt->ownerNPC = entry;
            entry->regularEvtId = evt->eventId;
        }
    }
}

s32 _check(EventEntry* evt) {
    s32* args = evt->args;
    s32 arg0 = args[0];

    if (pouchGetPtr()->mMerleeCurseUsesRemaining) {
        evtSetValue(evt, arg0, 0);
    } else {
        evtSetValue(evt, arg0, 1);
    }
    return 2;
}

// 100%: scoping the effect->userdata deref to a block lets the string argument materialise as
// `lis r3, str@ha; addi r4, r3, str@l`, and the reload of evt->userdata[0] for effSetName matches.
s32 _majinai_effect(EventEntry* evt, s32 a2) {
    MarioWork* mario = marioGetPtr();

    if (a2 != 0) {
        EffectEntry* effect;

        effect = effMajinaiEntry(0, mario->position.x,
                                 mario->position.y + mario->wPlayerCollisionBox.y + 16.0f, mario->position.z);
        evt->userdata[0] = (s32)effect;
        {
        EffectEntry* e = (EffectEntry*)evt->userdata[0];
        ((MajinaiWork*)e->userdata)->field_0x10 = 0.8f;
        }
        effSetName((EffectEntry*)evt->userdata[0], str_802C3128);
    }
    if (evt->userdata[0] != 0 && effNameToPtr(str_802C3128) != NULL) {
        return 0;
    }
    return 2;
}

void npcGetBackItemEntry(NpcEntry* entry) {
    NpcBattleInfo* info = &entry->battleInfo;
    s32 i;

    for (i = 0; i < 8; i++) {
        ItemType item = info->recoveredItems[i];
        if (item) {
            ItemEntry* itemEntryPtr = itemEntry(NULL, item, 0xB, -1, 0, entry->position.x, entry->position.y, entry->position.z);
            itemFlagOn(itemEntryPtr, 0x100);
            info->recoveredItems[i] = 0;
        }
    }
}

const char str_802C2FC8[] = "SFX_EVT_FLD_INCANT_CUBE1";
const char str_802C2FE4[] = "SFX_EVT_FLD_INCANT_CUBE2";
const char str_802C3000[] = "SFX_EVT_FLD_INCANT_CUBE3";
const char str_802C301C[] = "SFX_EVT_FLD_INCANT_CUBE4";
const char str_802C3038[] = "SFX_EVT_FLD_INCANT_CUBE5";
const char str_802C3054[] = "SFX_EVT_BTL_INCANT_CUBE1";
const char str_802C3070[] = "SFX_EVT_BTL_INCANT_CUBE2";
const char str_802C308C[] = "SFX_EVT_BTL_INCANT_CUBE3";
const char str_802C30A8[] = "SFX_EVT_BTL_INCANT_CUBE4";
const char str_802C30C4[] = "SFX_EVT_BTL_INCANT_CUBE5";
const char str_802C30E0[] = "msg_mj_coin_up";
const char str_802C30F0[] = "msg_mj_last";
const char str_802C30FC[] = "a_magu";
const char str_802C3104[] = "kpa_03";
const char str_802C310C[] = "SFX_KUPPA_3RD_WATER_FALL1";
const char str_802C3128[] = "mjef_coinup";

void npcClearDeadInfo(void) {
    memset(gp->fbatData->deadInfo, 0, 0x500);
}

void npcKoopaModeEncountCheck(void) {
    FieldBattleData* fbat = gp->fbatData;
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    s32 i;
    s32 total;
    NpcEntry* entry;

    total = wp->total;
    if (fbat->mode != 1) {
        return;
    }
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (entry != NULL && (entry->flags & 1) && (entry->flags & 0x8000000) && !(entry->jumpFlags & 4)) {
            switch (kpaEnemyHitChk(entry->position.x, entry->position.y, entry->position.z, entry->height,
                                   entry->width)) {
                case 1:
                    if (!(*(u16*)entry->field_31E & 1)) {
                        npcGroupDead(entry, 1);
                    }
                    break;
                case 2:
                    if (!(*(u16*)entry->field_31E & 2)) {
                        npcGroupDead(entry, 2);
                    }
                    break;
                case 3:
                    if (!(*(u16*)entry->field_31E & 4)) {
                        npcGroupDead(entry, 4);
                    }
                    break;
                case 4:
                    kpaPowDown();
                    break;
                default:
                    if (!(*(u16*)entry->field_31E & 0x10) && *(u32*)entry->field_300 != 0 &&
                        !strcmp(hitGetName((HitEntry*)(*(u32*)entry->field_300)), str_802C30FC)) {
                        npcGroupDead(entry, 0x10);
                    }
                    break;
            }
        }
    }
}

s32 npcKoopaModeMobjBoundDeadCheck(HitEntry* mobj) {
#ifdef MUST_MATCH
    const f64* pool = (const f64*)((const u8*)vec3_802c2f78 + 0x1c0);
#endif
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;
    int i;
    s32 total;
    NpcEntry* entry;
    s32 count = 0;
    total = wp->total;
    if (mobj == NULL) {
        return 0;
    }
    for (i = 0, entry = wp->entries; i < total; i++, entry++) {
        if (entry != NULL && (entry->flags & 1) && (entry->flags & 0x8000000) && !(entry->jumpFlags & 4) &&
            !(*(u16*)entry->field_31E & 8) && *(u32*)entry->field_300 == (u32)mobj) {
            npcGroupDead(entry, 8);
            count++;
        }
    }
    return count;
}

void fbatSandersBombTriggerOn(void) {
    gp->fbatData->field_54C[1] = TRUE;
}

s32 npcCalcScore(NpcEntry* entry) {
    FieldBattleData* fbatData = gp->fbatData;
    s32 score = 0x12C;
    Vec pos;

    pos = entry->position;
    pos.y += entry->tribe->height;

    if (entry->koopaEncounterType == 0x10) {
        score = 0;
    } else {
        if (entry->koopaEncounterType == 8) {
            score = 0x190;
        }
        if (kpaMutekiCheck()) {
            score += 0x64;
        }
        if (entry->koopaEncounterType == 2) {
            (*(s32*)&fbatData->field_570[4])++;
            score *= *(s32*)&fbatData->field_570[4];
        }
        if (entry->koopaEncounterType == 1) {
            (*(s32*)&fbatData->field_570[8])++;
            *(s32*)&fbatData->field_570[0xC] += score;
        }
        if (score > 0x270F) {
            score = 0x2710;
        }
    }
    if (score > 0) {
        kpaAddScorePos(score, &pos);
    }
    return score;
}

// The original copies entry->position into a shared `position` temp and from there into one Vec per
// cloudGetBreathPower call (the temp sits at 0x2C, the call Vecs at 0x20/0x14/0x8); writing the copies
// this way is what reproduces those stack slots. `half` keeps the fnmsubs operands (f2=height, f1=0.5).
s32 npcCheckBlow(NpcEntry* entry) {
    Vec position;
    Vec pos1;
    Vec pos2;
    Vec pos3;
    f32 half;

    if (entry->flags & 0x20000000) {
        return FALSE;
    }
    if (entry->flags & 0x20) {
        return FALSE;
    }
    position = entry->position;
    pos1 = position;
    if (cloudGetBreathPower(entry->width, &pos1) > 0.0f) {
        return TRUE;
    }
    position = entry->position;
    half = 0.5f;
    position.y = position.y - entry->height * half;
    pos2 = position;
    if (cloudGetBreathPower(entry->width, &pos2) > 0.0f) {
        return TRUE;
    }
    position = entry->position;
    position.y = position.y - entry->height;
    pos3 = position;
    return cloudGetBreathPower(entry->width, &pos3) > 0.0f;
}

s32 N_fbatPreventMarioEventChk(void) {
    s32 mode = gp->fbatData->mode;

    if (mode != 0 && mode != 1 && mode != 8) {
        return TRUE;
    }
    return FALSE;
}

#include "driver/npcdrv.h"
#include "dolphin/gx/GXStruct.h"
#include "data/mapdata.h"
#include "driver/dispdrv.h"
#include "driver/offscreendrv.h"
#include "driver/animdrv.h"
#include "driver/swdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_snd.h"
#include "battle/battle_event_cmd.h"
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
void FontDrawColor(GXColor* color);
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

extern s32 _evt_majinai_coin_up_effect[];

extern GlobalWork* gp;
extern NpcTribeInfo npcTribe[];

int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

// Prototypes for the forward references the original function order needs.
static inline void npcHammerPos(f32* x, f32* y, f32* z, f32 frame, f32 angle, f32 radius);
static void npcSpinHammerPos(f32* x, f32* y, f32* z, f32 angle, f32 radius);
static void fbatFirstAttackAnnounceMain(FieldBattleData* fbat);

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
#ifdef MUST_MATCH
        // The last refinement step stays an expression, as tik_05's sqrtf (Matching) has it: under
        // npcCheckHitMarioSide's opt_lifetimes off, a third `guess =` would keep guess's register.
        x = x * (_half * guess * (_three - guess * guess * x));
        return x;
#else
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
#endif
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

char MarioStayPose[32] = "KPA_S_1";
char MarioTalkPose[32] = "KPA_T_1";

//.sbss
// MUST_MATCH: MWCC emits uninitialised globals in reverse declaration order.
#ifdef MUST_MATCH
// Keep the original section tail even though no code references it.
#pragma force_active on
static u32 gap_08_804186C4_sbss;
#pragma force_active reset
#endif
static NpcWork2 work;
NpcFieldEntry* release_wp;
u32 npcMainCount;
BOOL dbg_battleskip;

// local prototypes

#define npcGetWorkPtr2(inBattle) ((inBattle) ? &work.battle : &work.field)
// TODO: inlining?
#define npcGetWorkPtrInline() (gp->inBattle ? &work.battle : &work.field)

#define ENC(p, T, off) (*(T*)((u8*)(p) + (off)))
typedef struct FbatHitInfo {
    s32 kind;           // 0x0, the attack (the fbatSetAttackAnnounce code)
    s32 field_0x4;      // 0x4, the npc's field_2F8, for a stomp that becomes the enemy's first strike
    f32 dist;           // 0x8, to the nearest npc hit so far
    u32 flags;          // 0xC, 1: only a test (no attack announce)
} FbatHitInfo;

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

    sinv = 0.00761f;
    c1 = 0.16605f;
    c2 = 1.0f;
    pi = 3.1415927f;
    if (rad <= pi) {
        pi = 1.5707964f;
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
    } else if (rad < 4.712389f) {
        rad = rad - pi;
        f0 = rad * rad;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * rad;
        sinv = -sinv;
    } else {
        f1 = rad - 4.712389f;
        pi = 1.5707964f;
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
    f32 f0, f1;
    f32 cosv;
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
            f1 = rad - pi;
            rad = pi - f1;
            f0 = rad * rad;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < 4.712389f) {
        rad = rad - pi;
        f0 = rad * rad;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        f1 = rad - 4.712389f;
        pi = 1.5707964f;
        rad = pi - f1;
        f0 = rad * rad;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

// Blur recovered (2*sin/-(2*cos) offsets from camGetPtr(camId)->viewYaw, 4 draws
// with history 4..1, alpha 0x80-100..0x80-25, flag save/restore). `t1`/`t2` make MWCC put the product in f2
// and the angle in f5, each branch reuses `rad` as the folded angle, and declaring `alpha` first and putting
// `i` in the leading block give retail's callee-saved map. The colour is a compound literal argument, which also
// gives retail's stack slots for it and its by-value copy. At optimization level 2 the sin/cos coefficient
// locals stay variables (at -O4 they are copied into every branch) and the colour copy's address is not
// hoisted out of the loop; the do-while is the loop retail has (no entry test).
#pragma push
#pragma optimization_level 2
void npcDisp_blur(CameraId camId, void* param) {
    s32 alpha;
    NpcEntry* entry = param; // cast to proper type
    Mtx mtx;
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
    i = 4;
    alpha = 100;
    do {
        mtx_setup(entry, mtx, i);
        PSMTXTransApply(mtx, mtx, offX, 0.0f, offZ);
        animPoseSetMaterialFlagOn(entry->poseId, 0x40);
        animPoseSetMaterialEvtColor(entry->poseId, (GXColor){ 0xFF, 0xFF, 0xFF, 0x80 - alpha });
        animPoseDrawMtx(entry->poseId, mtx, 1, rotY, 2.0f);
        animPoseDrawMtx(entry->poseId, mtx, 2, rotY, 2.0f);
        animPoseDrawMtx(entry->poseId, mtx, 3, rotY, 2.0f);
        i--;
        alpha -= 25;
    } while (i > 0);
    animPoseSetMaterialFlagOff(entry->poseId, 0x40);
    animPoseSetMaterialFlagOn(entry->poseId, flag);
}
#pragma pop


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
// 100%: the two search loops use a cached `total` and an incrementing `entry` pointer (`i` declared before
// `entry`), and the position/scale vectors are compound literals, the head of the unit's .rodata.
s32 npcEntry(const char* a1, const char* animName) {
    NpcWork* wp;
    s32 total;
    s32 i;
    NpcEntry* entry;

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
    entry->position = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->prevPosition = entry->position;
    entry->width = animPoseGetHeight(entry->poseId);
    entry->height = animPoseGetRadius(entry->poseId);
    entry->scale = (Vec){ 1.0f, 1.0f, 1.0f };
    entry->field_154 = 1.0f;
    entry->gait = 0;
    entry->field_10C = "M_I_2";
    entry->field_198 = gp->renderTime;
    entry->field_188 = 0;
    entry->jumpGravity = 1.0f;
    entry->field_1D0 = 0.0f;
    entry->jumpYScale = 1.0f;
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
    NpcWork* wp;

    for (i = 0; i < 4; i++) {
        NpcEntry* slave = entry->slaves[i];
        if (slave) {
            slave->master = NULL;
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
    wp = npcGetWorkPtrInline();
    wp->count--;
}

// npcDelete's `slave` and `wp` locals give its inlined copies here retail's temporaries (the slave loads
// and the hoisted work pointer); with them, the plain `entry->slaves[i]` form matches.
void _npcDeleteGroup(NpcEntry* entry) { // recursively inlined, warning
    int i;

    if (entry->next) {
        _npcDeleteGroup(entry->next);
    }

    for (i = 0; i < 4; i++) {
        if (entry->slaves[i]) {
            npcDelete(entry->slaves[i]);
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


// Mario can be noticed by a field npc: not in a cutscene, menu or talk, and not hidden by Vivian.
static inline BOOL npcMarioNoticeable(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & 0x82000000) {
        return FALSE;
    }
    if (mario->flags & 0xA) {
        return FALSE;
    }
    if ((s32)gp->mapInitEvtId != 0) {
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

// Bowser's field attacks while the field battle mode is idle: score a stomp, and clear the fire flag when he is
// not breathing fire. `jump` and `fire` start as two separate zeroes, which MWCC merges into retail's `li`+`mr`.
static inline void npcKoopaAttackCheck(void) {
    s32 jump;
    FieldBattleData* data;
    MarioWork* mario;
    s32 fire;

    data = gp->fbatData;
    mario = marioGetPtr();
    jump = 0;
    fire = 0;
    if (mario->characterId == 2) {
        jump = kpaJumpChk();
    }
    if (mario->characterId == 2) {
        fire = kpaFireAttackCheck();
    }
    if (jump) {
        npcKoopaJumpScore(data, mario);
    }
    if (!fire) {
        data->field_574 = 0;
    }
}

// Per frame for each live npc: advance its clocks, apply its movement and walk/run animation, raise its event's
// reactions (interrupt flags: 1 found Mario, 2 lost him, 4 left its territory, 8 blown by Flurrie), and queue its
// draw, shadow and offscreen passes; then run the first strike banner and the field battle mode.
// The Mario-distance blocks (state 2, states 1/4) and the circle territory test declare their own dx/dy/dz,
// `hit` and (1/4) dist/angle, which gives retail's callee-saved registers. In states 1/4 the distance is taken
// once before PSVECSubtract and again once Mario is noticeable: the first sqrtf is dead, and its leftover compare
// is retail's lone `fcmpo f30, 0`.
void npcMain(void) {
#ifdef MUST_MATCH
    const char* fallSfx;
#endif
    NpcWork* npcwork;
    NpcEntry* entry;
    FieldBattleData* fbat;
    EventEntry* evt;
    MarioWork* mario;
    s32 total;
    s32 i;
    s32 state;
    BOOL hit;
    s32 shadowId;
    f32 dist;
    f32 dir;
    f32 runScale;
    f32 angle;
    f32 z;
    Vec v;
    Vec moved;
    Vec back;
    Vec shadow;
    Vec feet;

    npcwork = gp->inBattle ? &work.battle : &work.field;
    marioGetPtr()->field_0x1D8 = 0;
    total = npcwork->total;
    npcMainCount++;
#ifdef MUST_MATCH
    fallSfx = "SFX_KUPPA_3RD_WATER_FALL1";
#endif

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
        if (entry->wFbatHitCheckRelated) {
            entry->wFbatHitCheckRelated--;
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
#ifdef MUST_MATCH
            // npcKoopaWaterCheck by hand: retail loads the splash SFX name once before the npc loop
            if (entry->jumpFlags & 0x100000) {
                entry->jumpFlags |= 0x200000;
            } else {
                entry->jumpFlags &= ~0x200000;
            }
            entry->jumpFlags &= ~0x100000;
            if (!strcmp(gp->currentMapName, "kpa_03") && !(entry->position.y > 365.0f)) {
                entry->jumpFlags |= 0x100000;
                if (!(entry->jumpFlags & 0x200000) && (entry->jumpFlags & 0x100000)) {
                    psndSFXOn_3D(fallSfx, &entry->position);
                }
            }
#else
            npcKoopaWaterCheck(entry);
#endif
            if ((entry->jumpFlags & 0x100000) && !(entry->jumpFlags & 0x10000000)) {
                entry->field_1D0 = 100.0f;
                runScale = 0.5f;
            } else {
                runScale = 1.0f;
            }
            if (entry->jumpFlags & 0x100000) {
                entry->hazardImmuneFlags |= 1;
            }

            if (entry->jumpFlags & 0x10) {
                if (entry->moveDt > 0.0f) {
                    f32 mx = entry->moveOffset.x;
                    f32 mz = entry->moveOffset.z;

                    if ((entry->jumpFlags & 0x100000) && !(entry->jumpFlags & 0x10000000)) {
                        mx *= 0.5f;
                        mz *= 0.5f;
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
                    entry->moveOffset.x = 0.0f;
                    entry->moveOffset.z = 0.0f;
                    npcMoveG(entry);
                }
                if (!(entry->flags & 8) && !(entry->flags & 0x20) && !(entry->flags & 0x4000)) {
                    npcCheckHitMarioSide(entry);
                }

                // Pick the stay/walk/run animation from the distance moved this frame.
                PSVECSubtract(&entry->position, &entry->prevPosition, &v);
                if ((entry->flags & 0x400) && entry->moveDt > 0.0f) {
                    f32 speed = sqrtf(v.x * v.x + v.z * v.z) / entry->moveDt;

                    if (speed < 0.1f) {
                        if (entry->gait != 1) {
                            animPoseSetAnim(entry->poseId, entry->tribe->stayAnim, 1);
                            entry->gait = 1;
                        }
                    } else if (speed < entry->tribe->runStartSpeed * runScale) {
                        if (entry->gait != 2) {
                            animPoseSetAnim(entry->poseId, entry->tribe->walkAnimation, 1);
                            entry->gait = 2;
                        }
                    } else if (entry->gait != 3) {
                        animPoseSetAnim(entry->poseId, entry->tribe->runAnimation, 1);
                        entry->gait = 3;
                    }
                }
            }

            // Raise the npc's reactions for its event: blown (8), out of its territory (4), lost Mario (2),
            // found Mario (1).
            if (entry->regularEvtId != 0) {
                evt = evtGetPtrID(entry->regularEvtId);
                if (evt != NULL && !(evt->flags & 2)) {
                    state = evt->wNpcEventType;
                    if (entry->blowEvent != NULL && state != NPC_EVENT_TYPE_BLOW && state != NPC_EVENT_TYPE_DEAD && npcCheckBlow(entry)) {
                        entry->interruptFlags |= 8;
                        goto interrupt;
                    }

                    if (entry->territoryType != NPC_TERRITORY_TYPE_NOTHING && (state == NPC_EVENT_TYPE_MOVE || state == NPC_EVENT_TYPE_FIND)) {
                        hit = FALSE;
                        if (entry->territoryType == NPC_TERRITORY_TYPE_CIRCLE) {
                            f32 dx;
                            f32 dz;

                            dx = entry->territoryBase.x - entry->position.x;
                            dz = entry->territoryBase.z - entry->position.z;
                            dist = sqrtf(dx * dx + dz * dz);
                            if (dist > (state == NPC_EVENT_TYPE_FIND ? entry->territoryHoming.x : entry->territoryHoming.x)) {
                                hit = TRUE;
                            }
                        } else if (entry->territoryType == NPC_TERRITORY_TYPE_SQUARE) {
                            if (state == NPC_EVENT_TYPE_FIND) {
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
                            entry->interruptFlags |= 4;
                            goto interrupt;
                        }
                    }

                    if (state == NPC_EVENT_TYPE_FIND) {
                        f32 dx;
                        f32 dy;
                        f32 dz;
                        BOOL hit;

                        mario = marioGetPtr();
                        dx = mario->position.x - entry->position.x;
                        dy = mario->position.y - entry->position.y;
                        dz = mario->position.z - entry->position.z;
                        PSVECSubtract(&entry->prevPosition, &entry->position, &moved);
                        dir = entry->direction;
                        if ((entry->jumpFlags & 2) && sqrtf(moved.x * moved.x + moved.z * moved.z) > 0.05f) {
                            dir = entry->moveDir;
                        }
                        hit = FALSE;
                        if (!npcMarioNoticeable()) {
                            hit = TRUE;
                        } else {
                            dist = sqrtf(dx * dx + dz * dz);
                            if (dist > 0.25f * entry->homingRange) {
                                if (dy > entry->territoryLoiter.y) {
                                    hit = TRUE;
                                } else if (-dy > entry->territoryHoming.y) {
                                    hit = TRUE;
                                } else if (dist > entry->homingRange) {
                                    hit = TRUE;
                                } else if (angle = angleABf(0.0f, 0.0f, dx, dz),
                                           __fabsf(compAngle(dir, angle)) > 0.5f * entry->homingAngle) {
                                    hit = TRUE;
                                }
                            }
                        }
                        if (hit) {
                            entry->interruptFlags |= 2;
                            goto interrupt;
                        }
                    }

                    if (state == NPC_EVENT_TYPE_MOVE || state == NPC_EVENT_TYPE_RETURN) {
                        f32 dx;
                        f32 dist;
                        f32 dy;
                        f32 dz;
                        f32 angle;
                        BOOL hit;

                        mario = marioGetPtr();
                        dx = mario->position.x - entry->position.x;
                        dy = mario->position.y - entry->position.y;
                        dz = mario->position.z - entry->position.z;
                        dist = sqrtf(dx * dx + dz * dz);
                        PSVECSubtract(&entry->prevPosition, &entry->position, &back);
                        dir = reviseAngle(entry->direction) < 180.0f ? 90.0f : 270.0f;
                        hit = TRUE;
                        if (!npcMarioNoticeable()) {
                            hit = FALSE;
                        } else {
                            dist = sqrtf(dx * dx + dz * dz);
                            if (dist > 0.25f * entry->searchRange) {
                                if (dist > entry->searchRange) {
                                    hit = FALSE;
                                    goto found;
                                }
                                angle = angleABf(0.0f, 0.0f, dx, dz);
                                if (__fabsf(compAngle(dir, angle)) > 0.5f * entry->searchAngle) {
                                    hit = FALSE;
                                    goto found;
                                }
                            }
                            if (dy > entry->territoryLoiter.y) {
                                hit = FALSE;
                            } else if (-dy > entry->territoryHoming.y) {
                                hit = FALSE;
                            } else if (state == NPC_EVENT_TYPE_RETURN) {
                                if (entry->territoryType == NPC_TERRITORY_TYPE_CIRCLE) {
                                    dx = entry->territoryBase.x - entry->position.x;
                                    dz = entry->territoryBase.z - entry->position.z;
                                    if (sqrtf(dx * dx + dz * dz) > 0.75 * entry->territoryHoming.x) {
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
                            entry->interruptFlags |= 1;
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
                if ((v.z > (-1.0f) && v.z < 0.0f && v.x >= (-2.0f) &&
                     v.x <= 2.0f && v.y >= (-2.0f) && v.y <= 2.0f) ||
                    (entry->flags & 0x40)) {
                    if (entry->color.r != 0xFF || entry->color.g != 0xFF || entry->color.b != 0xFF ||
                        entry->color.a != 0xFF) {
                        animPoseSetMaterialFlagOn(entry->poseId, 0x40);
                        animPoseSetMaterialEvtColor(entry->poseId, entry->color);
                    } else {
                        animPoseSetMaterialFlagOff(entry->poseId, 0x40);
                    }

                    z = 5000.0f * v.z + 5000.0f;
                    if (z < 0.0f) {
                        z = 0.0f;
                    } else if (z > 10000.0f) {
                        z = 10000.0f;
                    }
                    z *= (-1.0f);

                    if (!(entry->flags & 0x80) &&
                        (!(entry->wFbatHitCheckRelated & 2) || (entry->jumpFlags & 0x10000))) {
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
                                                sinfd(reviseAngle(entry->direction) < 180.0f
                                                          ? 90.0f
                                                          : 270.0f);
                                    shadow.y += entry->tribe->shadowOffsetY * entry->scale.y;
                                    shadow.z += entry->tribe->shadowOffsetZ * entry->scale.z;
                                }
                                shadowId = shadowEntry(shadow.x, shadow.y, shadow.z,
                                                  1.33f * (entry->field_154 *
                                                                         (entry->width / 3.0f * entry->scale.x)));
                                if (entry->flags & 0x80000) {
                                    shadowSetCollision(shadowId, 0);
                                }
                            }
                        } else {
                            f32 hx, hy, hz, hdist, nx, ny, nz;

                            feet = entry->position;
                            hdist = 100.0f;
                            if (hitCheckFilter(NULL, feet.x, 1.0f + feet.y, feet.z, 0.0f, (-1.0f), 0.0f, &hx, &hy,
                                               &hz, &hdist, &nx,
                                               &ny, &nz) != NULL) {
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

    fbat = gp->fbatData;
    fbatFirstAttackAnnounceMain(fbat);

    switch (fbat->mode) {
        case 1:
            fbatEncountCheck();
            npcKoopaAttackCheck();
            break;
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
// All locals at function scope in the order a rasim search gave for offZ/the sqrt argument/the 0.5 constant.
s32 npcCheckHitMarioSide(NpcEntry* entry) {
    MarioWork* mario = marioGetPtr();
    Vec pos;
    f32 viewT;
    f32 offX;
    f32 radius;
    f32 angle;
    f32 scale;
    f32 d1;
    f32 dist;
    f32 limit;
    f32 hitT;
    f32 offZ;
    f32 d2;

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
        viewT = 6.2831855f * mario->directionView;
        offX = (radius - d2) * -npcHitSideSinf(viewT / 360.0f);
        offZ = (radius - d2) * npcHitSideCosf(viewT / 360.0f);
    } else {
        hitT = 6.2831855f * angle;
        offX = (entry->position.x - radius * npcHitSideSinf(hitT / 360.0f)) - mario->position.x;
        offZ = (entry->position.z + radius * npcHitSideCosf(hitT / 360.0f)) - mario->position.z;
    }
    dist = sqrtf(offX * offX + offZ * offZ);
    limit = mario->wPlayerCollisionBox.z > 4.0f ? 3.0f : 1.5f;
    if (dist > limit) {
        scale = 2.0f / dist;
        offX *= scale;
        offZ *= scale;
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
            hit = hitCheckFilter(NULL, entry->position.x, entry->position.y + yOffset, entry->position.z, 0.0f, -1.0f, 0.0f, &hitX, &hitY,
                                 &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
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
        hit = hitCheckFilter(NULL, entry->position.x, entry->position.y + y, entry->position.z, sinv, 0.0f, cosv, &hx, &hy, &hz,
                             &d, &nx, &ny, &nz);
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
        hit = hitCheckFilter(NULL, entry->position.x, entry->position.y + y, entry->position.z, sinv, 0.0f, cosv, &hx, &hy, &hz,
                             &d, &nx, &ny, &nz);
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
        hit = hitCheckFilter(NULL, entry->position.x, entry->position.y + y, entry->position.z, sinv, 0.0f, cosv, &hx, &hy, &hz,
                             &d, &nx, &ny, &nz);
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

NpcEntry* npcNearDistCheck(f32 x, f32 y, f32 z, f32 radius) {
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
            f32 dist = sqrtf(dx2 + dz2);
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

// The first strike announcement: slide in, hold, slide out (type 2); or a fixed 60-frame banner (type 1).
static void fbatFirstAttackAnnounceMain(FieldBattleData* fbat) {
    FirstStrikeInfo* strike = &fbat->firstStrike;

    if (strike->enabled) {
        switch (strike->type) {
            case 2:
                strike->field_0xC++;
                if ((s32)strike->field_0xC < 20) {
                    strike->field_0x10 = intplGetValue(5, strike->field_0xC, 20, (-600.0f), 0.0f);
                } else if ((s32)strike->field_0xC < 80) {
                    strike->field_0x10 = 0.0f;
                } else if ((s32)strike->field_0xC < 100) {
                    strike->field_0x10 =
                        intplGetValue(1, strike->field_0xC - 80, 20, 0.0f, 600.0f);
                } else {
                    strike->field_0x4 = 0;
                    break;
                }
                strike->field_0x14 = 0.0f;
                dispEntry(CAMERA_FADE, 0.0f, 2, _fbatFirstAttackAnnouceDisp, strike);
                break;
            case 1:
                strike->field_0xC++;
                if ((s32)strike->field_0xC > 60) {
                    strike->field_0x4 = 0;
                } else {
                    strike->field_0x10 = 0.0f;
                    strike->field_0x14 = 0.0f;
                    dispEntry(CAMERA_FADE, 0.0f, 2, _fbatFirstAttackAnnouceDisp, strike);
                }
                break;
        }
    }
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
// `fbatData->talkNpc` load (as in the original), which also fixes `entry`/`mario` to r30/r29.

// Same 57 calls and immediates, no structural difference. The three decision trees match (the first switch carries the empty case 0x10000 the
// original probes; the side scan is a `switch` on `blocked`), `wk` is the ternary form, the OffChk pair is one
// `||` test, the first reviseAngle result uses its own local, and `h180` keeps the blocked-loop threshold in
// the target's f29 (an inline constant put the whole talk-loop FPR set one register low). The two `angleABf` zero
// constants before dx/dz are literals so their loads schedule as retail does. Naming the `fb + 0x10` out-address
// (`fbatOut`) makes MWCC keep `total` in r27 and that temp in r26, as retail does; the second fbatHitCheck block
// gets a block-scoped `fbatOut2` so its temp takes r22 and its `fb` r25 (a function-scope reuse kept both in r26).
// Initialising the talk loop as `best = NULL; i = 0;` matches the two `li` order. The camera-relative angle is
// `rel = reviseAngle(...); rel += -90.0f;` (a single `+ -90.0f` expression puts the constant first in the
// fadds), and the final turn is `rotationY += 0.75f * (...)`, which gives the second wrap loop retail's f1/f2/f3.
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
    fbatOut = &fb->field_10;
    hit = (NpcEntry*)fbatHitCheck(0x13F1, (s32)fbatOut);
    if (hit != NULL && fb->mode == 1) {
        FieldBattleData* fb2 = gp->fbatData;
        s32 ok;

        if (fb2->mode != 1) {
            ok = 0;
        } else {
            fb2->mode = 5;
            ok = 1;
            fb2->state = 0;
            fb2->talkNpc = hit;
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
            rel = reviseAngle(-hit->direction);
            rel += -90.0f;
            cam = camGetPtr(CAMERA_3D);
            camAngle = reviseAngle(NPC_RAD2DEG((f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z))));
            if (camAngle < 0.0f) {
                camAngle += 360.0f;
            }
            diff = reviseAngle(camAngle - rel);
            if ((diff > 90.0f && diff <= 270.0f) || (diff < (-90.0f) && diff >= (-270.0f))) {
                cam = camGetPtr(CAMERA_3D);
                camAngle = reviseAngle(NPC_RAD2DEG((f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z))));
                if (camAngle < 0.0f) {
                    camAngle += 360.0f;
                }
                pose->field_0x7C = reviseAngle(180.0f + camAngle);
            } else {
                cam = camGetPtr(CAMERA_3D);
                camAngle = reviseAngle(NPC_RAD2DEG((f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z))));
                if (camAngle < 0.0f) {
                    camAngle += 360.0f;
                }
                pose->field_0x7C = camAngle;
            }
            while (pose->field_0x7C - pose->rotationY > 180.0f) {
                pose->rotationY += 360.0f;
            }
            while (pose->field_0x7C - pose->rotationY < (-180.0f)) {
                pose->rotationY -= 360.0f;
            }
            pose->rotationY += 0.75f * (pose->field_0x7C - pose->rotationY);
            switch (*fbatOut) {
                case 0x200000:
                case 0x1000000:
                case 0x800000:
                case 0x400000:
                case 0x80000:
                case 0x100000:
                case 0x40000:
                case 0x20000:
                    effSmallStarEntry(3, 3, hit->position.x, hit->position.y + hit->height, hit->position.z, 0.0f, (-1.0f), 0.0f);
                    break;
                case 0x10000000:
                    effSmallStarEntry(3, 3, mario->position.x, mario->position.y + mario->wPlayerCollisionBox.y, mario->position.z, 0.0f, (-1.0f), 0.0f);
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
                        psndSFXOn_3D("SFX_BTL_FLD_ENEMY_DAMAGED1", &hit->position);
                        return;
                    }
                    break;
                case 0x10000000:
                    psndSFXOn_3D("SFX_BTL_FLD_MARIO_DAMAGED1", &mario->position);
                    break;
            }
        }
        return;
    }
    nearest = 1000.0f;
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
                    if (dist <= 0.5f * (mario->wPlayerCollisionBox.x + npc->width) + 10.0f && dist < nearest) {
                        best = npc;
                    }
                    side = 0;
                    camAngle = camGetPtr(CAMERA_3D)->viewYaw;
                    rel = reviseAngle(marioGetDispDir() - camAngle);
                    h180 = 180.0f;
                    for (blocked = 0; blocked < 3; blocked++) {
                        f32 ang;

                        switch (blocked) {
                            case 0:
                                ang = reviseAngle(angleABf(mario->position.x, mario->position.z, npc->position.x, npc->position.z) - camGetPtr(CAMERA_3D)->viewYaw);
                                break;
                            case 1:
                                ang = reviseAngle(angleABf(mario->position.x, mario->position.z, 0.5f * npc->width + npc->position.x, npc->position.z) - camGetPtr(CAMERA_3D)->viewYaw);
                                break;
                            default:
                                ang = reviseAngle(angleABf(mario->position.x, mario->position.z, -(0.5f * npc->width - npc->position.x), npc->position.z) - camGetPtr(CAMERA_3D)->viewYaw);
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
                        if (!(dist > 0.5f * (mario->wPlayerCollisionBox.x + npc->width) + 10.0f)) {
                            dx = npc->position.x - mario->position.x;
                            dz = npc->position.z - mario->position.z;
                            sincosf(&sx, &sz, angleABf(0.0f, 0.0f, dx, dz));
                            d = sqrtf(dx * dx + dz * dz);
                            if (hitCheckFilter(NULL, mario->position.x, 0.75f * mario->wPlayerCollisionBox.y + mario->position.y, mario->position.z, sx, 0.0f, sz, &hitX, &hitY,
                                               &hitZ,
                                               &d, &hitNX, &hitNY, &hitNZ) == NULL) {
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
                fb3->state = 0;
                fb3->talkNpc = tgt;
                ENC(tgt, s32, 0x318) = 0;
            }
            return;
        }
    }
    fb = gp->fbatData;
    if (fb->mode == 1) {
        s32* fbatOut2 = &fb->field_10;

        hit = (NpcEntry*)fbatHitCheck(0x2361, (s32)fbatOut2);
        if (hit != NULL && fb->mode == 1) {
            s32 fval;
            FieldBattleData* fb2;

            fval = *fbatOut2;
            fb2 = gp->fbatData;
            if (fb2->mode == 1) {
                fb2->mode = 3;
                fb2->state = 0;
                fb2->talkNpc = hit;
                ENC(hit, s32, 0x318) = fval;
            }
            switch (*fbatOut2) {
                case 0x40000:
                case 0x20000:
                case 0x80000:
                    marioChgMotJump2();
                    break;
            }
            effSmallStarEntry(3, 3, hit->position.x, hit->position.y + hit->height, hit->position.z, 0.0f, (-1.0f), 0.0f);
            return;
        }
    }
    npcKoopaModeEncountCheck();
}

void fbatTalkMode(void) {
    FieldBattleData* fbatData = gp->fbatData;
    NpcEntry* entry = fbatData->talkNpc;
    MarioWork* mario = marioGetPtr();
    NpcWork* wp;
    EventEntry* evt;

    switch (fbatData->state) {
        case 0:
            evtStopID(fbatData->talkNpc->regularEvtId);
            if (!(entry->flags & 0x1000000)) {
                entry->direction = reviseAngle(angleABf(entry->position.x, entry->position.z, mario->position.x,
                                                          mario->position.z));
            }
            marioKeyOff();
            partyKeyOff();
            switch (fbatData->mode) {
                case 3:
                    evt = evtEntry(entry->deadEvent, 0, 0);
                    evt->wNpcEventType = NPC_EVENT_TYPE_TALK;
                    break;
                default:
                    evt = evtEntry(entry->talkEvt, 0, 0);
                    evt->wNpcEventType = NPC_EVENT_TYPE_TALK;
                    marioChgTalkMotion();
                    break;
            }
            evt->ownerNPC = entry;
            fbatData->talkEvtId = evt->eventId;
            fbatData->state++;
            break;
        case 1:
            if (!evtCheckID(fbatData->talkEvtId)) {
                if (!(entry->jumpFlags & 0x20000000)) {
                    evtStartID(fbatData->talkNpc->regularEvtId);
                    entry->direction = *(f32*)entry->field_148;
                }
                if (entry->flags & 0x400) {
                    entry->gait = 0;
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
// temporaries. The stolen-item check is a loop that MWCC fully unrolls into retail's eight-test chain; it steps a
// byte offset, as evt_nannpc's entry loop does, so the frontend does not turn the index into a walking pointer
// (`stolenItems[i]` leaves an `addi` in the unrolled chain). One load register shared by the unrolled tests is
// what lets hasStolen/info4/result/mario take retail's registers.
void fbatBattleMode(void) {
    PouchData* pouch;
    FieldBattleData* fbatData;
    NpcEntry* entry;
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

    switch (fbatData->state) {
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

                fbatData->target->interruptFlags &= 0xF00;
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
                        (fbatData->field_10 & 0x01FE0000) &&
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
                    if (fbatData->field_10 == 0x10000000) {
                        unk_80058374();
                    }
                    fbatData->state = 1;
                } else {
                    if (flagB) {
                        BattleInformationSetResult(info0, 2);
                    } else {
                        BattleInformationSetResult(info0, 5);
                    }
                    fbatData->state = 4;
                }
            }
            break;
        case 1:
            if (gp->inBattle) {
                fbatData->state++;
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
                    fbatData->state = 3;
                    fbatData->field_54C[0] = 1;
                    evt = evtEntry(_evt_majinai_coin_up_effect, 0, 0);
                    fbatData->majinaiEvtId = evt->eventId;
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
                    fbatData->state = 4;
                }
            }
            break;
        case 3:
            if (!evtCheckID(fbatData->majinaiEvtId)) {
                NpcWork* wp3;
                NpcEntry* npc3;
                s32 total3;
                s32 i3;
                marioKeyOn();
                marioFBattlePost();
                fbatData->majinaiEvtId = 0;
                fbatData->state = 4;
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
                FieldBattleInfo* info4;
                s32 hasStolen;
                s32 mult;

                info4 = (FieldBattleInfo*)((u8*)fbatData + 0x20);
                result = BattleInformationGetResult(info4);
                hasStolen = FALSE;
                for (i = 0; i < sizeof(entry->battleInfo.stolenItems); i += sizeof(ItemType)) {
                    if (*(ItemType*)((u8*)entry->battleInfo.stolenItems + i) != 0) {
                        hasStolen = TRUE;
                        break;
                    }
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
                            entry->gait = 0;
                        }
                        entry->wFbatHitCheckRelated = 0x5A;
                        entry->interruptFlags &= ~0x100;
                        entry->interruptFlags |= 0x10000000;
                        if (entry->flags & 0x200) {
                            animPoseSetAnim(entry->poseId, entry->currentAnim, TRUE);
                        }
                        npcGetBackItemEntry(entry);
                    }
                } else if (result == 4) {
                    if (entry->flags & 0x400) {
                        entry->gait = 0;
                    }
                    entry->interruptFlags |= 0x10000000;
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

void _npcDead(NpcEntry* entry, u16 koopaEncounterType) {
    entry->interruptFlags |= 0x10;
    entry->jumpFlags |= 4;
    entry->koopaEncounterType = koopaEncounterType;
    if (entry->flags & 4) {
        fbatGetPointer()->deadInfo[0].mask |= (1 << entry->field_314);
    }
}

// _npcDead calling fbatGetPointer() is what makes the deepest inlined copy emit a real `bl fbatGetPointer`.
// `type` is a u16 like the koopaEncounterType it is stored to: with an s32 the inlined copies carry a (dead)
// truncation, the deepest slave loop gets too long for MWCC to hoist the shift's `1` out of it, and the
// callee-saved registers move (`stmw r25` instead of retail's `stmw r24`).
void _npcGroupDead(NpcEntry* entry, u16 type) {
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
void npcGroupDead(NpcEntry* entry, u16 type) {
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
    if ((s32)gp->mapInitEvtId != 0) {
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
    if (npc->wFbatHitCheckRelated != 0) {
        return FALSE;
    }
    return TRUE;
}

// The hammer's elevation per swing frame, in degrees.

// The npc's own event has reached state 5.
static inline BOOL fbatNpcEventDone(NpcEntry* npc) {
    if (evtGetPtrID(npc->regularEvtId) != NULL) {
        return evtGetPtrID(npc->regularEvtId)->wNpcEventType == NPC_EVENT_TYPE_BLOW;
    }
    return FALSE;
}

// Finds the nearest field npc Mario's attacks hit, for each kind of attack `flags` enables in turn: jumps
// (0x30), the hammer (0x40), the spin hammer (0x80), Koops' shell (0x100) and partner 3 (0x200). Flag 0x2000
// only tests (no attack announce); 0x1000 looks at npcs with flag 8 instead of the others.
// dont_inline: retail calls _npcHitCheckSphere (4 sites) and _npcHitCheckHammerAllMotion (2) from here instead
// of inlining them; the static inline helpers are still inlined.
// The attack flags (flag8/jump/stomp) and dryRun/mario are function-wide and declared first, one `flag8`
// and one `npc` serve every attack kind, the hammer frame number goes straight into npcHammerPos, and both
// hammer helpers return the head position through x/y/z pointers: together they give retail's callee-saved
// GPR map. The hammer and spin blocks share one `angle`/`radius` pair between their two branches, and the
// shell block declares x, y, z before radius: that gives the FPRs.
#pragma dont_inline on
void* fbatHitCheck(s32 flags, s32 out) {
    u32 flag8;
    u32 jump;
    u32 stomp;
    u32 dryRun;
    MarioWork* mario;
    NpcEntry* hit = NULL;
    NpcEntry* npc = NULL;
    NpcWork* npcWork = npcGetWorkPtrInline();
    FbatHitInfo info;
    s32 i;
    s32 count;
    f32 dist;

    mario = marioGetPtr();
    info.dist = 1000.0f;
    info.kind = 0;
    info.flags &= ~1;
    dryRun = flags & 0x2000;
    if (dryRun) {
        info.flags |= 1;
    }

    // jumps: the npc under Mario's feet
    if (flags & 0x30) {
        stomp = flags & 0x10;
        count = npcWork->total; // retail reads the total here, and only here
        npc = npcWork->entries;
        if (!(stomp && mario->unk4C > 0) && fbatAttackEnabled(npc)) {
            flag8 = flags & 0x1000;
            jump = flags & 0x20;
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
                if (dist > 0.5f * (mario->wPlayerCollisionBox.x + npc->width)) {
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
        f32 angle;
        f32 radius;

        while (hit == NULL) {
            if ((flags & 0x10) && mario->unk4C > 0) {
                break;
            }
            if (!fbatAttackEnabled(npc)) {
                break;
            }
            if (flags & 1) {
                f32 x, y, z;
                s32 frameNo;
                MarioWork* pose;
                f32 yaw;

                if (mario->currMotionId != 0x12) {
                    break;
                }
                radius = 1.5f * mario->wPlayerCollisionBox.x;

                yaw = reviseAngle((mario->unk1A8 - 90.0f) + camGetPtr(CAMERA_3D)->viewYaw);
                angle = yaw;
                pose = marioGetPtr();
                animPoseGetAnimPosePtr(pose->paperAnimGroupId[marioAnimeId()]);
                frameNo = pose->multiTimer;
                if (frameNo > 20) {
                    frameNo = 10;
                }
                npcHammerPos(&x, &y, &z, frameNo, angle, radius);
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
                    if (_npcHitCheckSphere(npc, &dist, x, y, z, 12.0f) == NULL || dist >= info.dist) {
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
                radius = 1.5f * mario->wPlayerCollisionBox.x;
                angle = reviseAngle((mario->unk1A8 - 90.0f) + camGetPtr(CAMERA_3D)->viewYaw);
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
        f32 angle;
        f32 radius;

        while (hit == NULL) {
            if ((flags & 0x10) && mario->unk4C > 0) {
                break;
            }
            if (!fbatAttackEnabled(npc)) {
                break;
            }
            if (flags & 1) {
                f32 x;
                f32 y;
                f32 z;

                if (mario->currMotionId != 0x13) {
                    break;
                }
                angle = marioGetAngleKaitenHammer();
                if (angle < 0.0f) {
                    break;
                }
                radius = 1.5f * mario->wPlayerCollisionBox.x;
                npcSpinHammerPos(&x, &y, &z, angle, radius);
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
                    if ((npc->flags & 0x20000000) || (npc->flags & 0x20) || (npc->jumpFlags & 4)
                        || (npc->flags & 0x08000000)) {
                        continue;
                    }
                    if (_npcHitCheckSphere(npc, &dist, x, y, z, 12.0f) == NULL || dist >= info.dist) {
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
                radius = 1.5f * mario->wPlayerCollisionBox.x;
                angle = reviseAngle((mario->unk1A8 - 90.0f) + camGetPtr(CAMERA_3D)->viewYaw);
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

                if (party == NULL || party->field_33 != 2 || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
                    break;
                }
                status = nokonokoGetStatus(party);
                if (status < 2 || status > 5) {
                    break;
                }
            }
            if (attack) {
                f32 x;
                f32 y;
                f32 z;
                f32 radius = 0.5f * party->radius;
                x = party->position.x;
                y = 0.5f * party->height + party->position.y; // 0xF0: the shell's height
                z = party->position.z;
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
                if (dryRun && !(npc->reactionFlags & 8)) {
                    continue;
                }
                if ((npc->flags & 0x20000000) || (npc->flags & 0x20) || (npc->jumpFlags & 4)
                    || (npc->flags & 0x08000000)) {
                    continue;
                }
                if (_npcHitCheckSphere(npc, &dist, party->position.x,
                                       0.5f * party->height + party->position.y, party->position.z,
                                       30.0f) == NULL
                    || dist >= info.dist) {
                    continue;
                }
                info.dist = dist;
                hit = npc;
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

#pragma dont_inline reset

// The spin hammer's head: `radius` out from Mario at `angle`, 15 units up.
static void npcSpinHammerPos(f32* x, f32* y, f32* z, f32 angle, f32 radius) {
    MarioWork* m = marioGetPtr();
    f32 s, c;

    sincosf(&s, &c, angle);
    *x = s * radius + m->position.x;
    *z = c * radius + m->position.z;
    *y = 15.0f + m->position.y;
}

NpcEntry* _npcHitCheckSphere(NpcEntry* entry, f32* outDist, f32 x, f32 y, f32 z, f32 radius) {
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
    *outDist = sqrtf(dx * dx + dy * dy + dz * dz);
    if (*outDist < radius + entry->width * 0.5f) {
        return entry;
    }
    return NULL;
}

// Hammer sweep over 12 elevation angles: npcHammerPos places the hammer head (its angle table is the one
// template, dat_802c2f9c, that fbatHitCheck's inlined copy also reads), then _npcHitCheckSphere, inlined
// here. npcHammerPos returns the head through x/y/z pointers: with a Vec the scalarised pos.y outranks the
// sphere test's entry->position.y temp, while the named `y` ranks below it as in the original. The unused
// `zero` vector is the original's: its template sits in .rodata between npcEntry's vectors and the angle table.
NpcEntry* _npcHitCheckHammerAllMotion(NpcEntry* entry, f32* outDist, f32 radius, f32 angle) {
    Vec zero = { 0.0f, 0.0f, 0.0f };
    f32 i = 0.0f;
    f32 dist;
    NpcEntry* npc;
    NpcEntry* hit;

    for (; i <= 11.0f; i += 1.0f) {
        f32 x, y, z;

        npcHammerPos(&x, &y, &z, i, angle, radius);
        npc = _npcHitCheckSphere(entry, &dist, x, y, z, 12.0f);
        hit = npc;
        if (npc != NULL) {
            break;
        }
    }
    *outDist = dist;
    return hit;
}

// The hammer head's position: `angle` around Mario, raised by the swing angle of animation frame `frame`.
static inline void npcHammerPos(f32* x, f32* y, f32* z, f32 frame, f32 angle, f32 radius) {
    MarioWork* mario = marioGetPtr();
    s32 angles[] = { 0, 0, 0, 0, 45, 60, 90, 100, 101, 101, 101 };
    f32 s1, c1, s2, c2;

    if (frame < 0.0f) {
        frame = 0.0f;
    }
    if (frame >= 11.0f) {
        frame = 10.0f;
    }
    sincosf(&s1, &c1, angle);
    sincosf(&s2, &c2, (f32)angles[(s32)frame]);
    *x = s2 * (s1 * radius) + mario->position.x;
    *z = s2 * (c1 * radius) + mario->position.z;
    *y = radius * c2 + mario->position.y;
}

BOOL npcCheckInterrupt(NpcEntry* npc) {
    EventEntry* evt;

    evtGetPtrID(npc->regularEvtId);
    if (npc->interruptFlags & ~0xF00) {
        evt = NULL;
        if ((npc->interruptFlags & 0x10) && npc->deadEvent) {
            evt = evtEntry(npc->deadEvent, 0, 0x20);
            evt->wNpcEventType = NPC_EVENT_TYPE_DEAD;
            evt->ownerNPC = npc;
        } else if ((npc->interruptFlags & 0x10000000) && npc->moveEvent) {
            evt = evtEntry(npc->moveEvent, 0, 0x20);
            evt->wNpcEventType = NPC_EVENT_TYPE_MOVE;
            evt->ownerNPC = npc;
        } else if ((npc->interruptFlags & 0x8) && npc->blowEvent) {
            evt = evtEntry(npc->blowEvent, 0, 0x20);
            evt->wNpcEventType = NPC_EVENT_TYPE_BLOW;
            evt->ownerNPC = npc;
        } else if ((npc->interruptFlags & 1) && npc->findEvent && !(npc->jumpFlags & 0x1000) &&
                   !(npc->flags & 0x200000) && !(npc->interruptFlags & 0x100)) {
            evt = evtEntry(npc->findEvent, 0, 0x20);
            evt->wNpcEventType = NPC_EVENT_TYPE_FIND;
            evt->ownerNPC = npc;
        } else if ((npc->interruptFlags & 2) && npc->lostEvent && !(npc->jumpFlags & 0x1000) &&
                   !(npc->flags & 0x200000)) {
            evt = evtEntry(npc->lostEvent, 0, 0x20);
            evt->wNpcEventType = NPC_EVENT_TYPE_LOST;
            evt->ownerNPC = npc;
        } else if ((npc->interruptFlags & 4) && npc->returnEvent && !(npc->jumpFlags & 0x1000) &&
                   !(npc->flags & 0x200000)) {
            evt = evtEntry(npc->returnEvent, 0, 0x20);
            evt->wNpcEventType = NPC_EVENT_TYPE_RETURN;
            evt->ownerNPC = npc;
        }
        if (evt != NULL) {
            evtDeleteID(npc->regularEvtId);
            npc->regularEvtId = evt->eventId;
            npc->interruptFlags &= 0xF00;
            npc->jumpFlags &= ~0x1000;
            npc->field_188 = 0;
            npc->moveSpeed = 0.0f;
            return TRUE;
        }
    }
    npc->interruptFlags &= 0xF00;
    npc->jumpFlags &= ~0x1000;
    return FALSE;
}

// Rolls each of the setup's 16 (min, max) byte pairs into the battle info.
static inline void npcRollBattleParams(NpcBattleInfo* info, BattleSetupData* setup) {
    u8* p;
    s32 i;

    for (i = 0, p = (u8*)setup; i < 0x10; i++) {
        info->field_9C[0x14 + i] = p[0x20] + irand((s8)p[0x21] - (s8)p[0x20] + 1);
        p += 2;
    }
}

// The 0xB0 roll store uses the declared `field_9C` (MWCC then leaves it indexed with `stbx`). The level test is a
// switch (levels 1 and 2 skip the roll; 0 and everything else roll), whose lowering is retail's
// `beq/blt; cmpwi 3; bge roll; b body`. The held-item pick stores and breaks inside its `for (;;)` (retail's
// bcc->b shape). The loadout weight sum keeps the weight it tests in `weight` (retail adds the loaded value, no
// reload). The roll loop is its own inline (its i/p are inliner locals, which number above the held-items loop's
// induction pointer and leave num, i and that pointer in retail's r30/r28/r27).
void npcSetupBattleInfo(NpcEntry* npc, BattleSetupData* setup) {
    s32 i;
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
    s32 weight;

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
        npcRollBattleParams(info, setup);
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
        while ((weight = entry->weight) > 0) {
            sum += weight;
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
        for (i = 0; i < num; enemy++, i++) {
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
                    for (;;) {
                        pick -= list->holdWeight;
                        if (pick < 0) {
                            info->heldItems[i] = list->type;
                            info->itemDropLists[i] = enemy->itemDropTable;
                            break;
                        }
                        list++;
                    }
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
                evt->wNpcEventType = NPC_EVENT_TYPE_INIT;
                evt->ownerNPC = entry;
                entry->initEvtId = evt->eventId;
            } else {
                entry->initEvtId = 0;
            }
        }
    }
}

BOOL npcWaitAllInitEvtEnd(void) {
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
            evt->wNpcEventType = NPC_EVENT_TYPE_MOVE;
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
        effSetName((EffectEntry*)evt->userdata[0], "mjef_coinup");
    }
    if (evt->userdata[0] != 0 && effNameToPtr("mjef_coinup") != NULL) {
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

EVT_BEGIN(evt_majinai_effect_se_fld)
    CALL(btlevtcmd_ftomsec, 29, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_FLD_INCANT_CUBE1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftomsec, 200, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_FLD_INCANT_CUBE2"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftomsec, 90, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_FLD_INCANT_CUBE3"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftomsec, 80, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_FLD_INCANT_CUBE4"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftomsec, 5, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_FLD_INCANT_CUBE5"), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_majinai_effect_se_btl)
    CALL(btlevtcmd_ftomsec, 29, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_BTL_INCANT_CUBE1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftomsec, 200, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_BTL_INCANT_CUBE2"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftomsec, 90, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_BTL_INCANT_CUBE3"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftomsec, 80, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_BTL_INCANT_CUBE4"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftomsec, 5, LW(10))
    WAIT_MS(LW(10))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_BTL_INCANT_CUBE5"), LW(0), LW(1), LW(2), 0)
    RETURN
EVT_END()

EVT_BEGIN(_evt_majinai_coin_up_effect)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    SPAWN_EVENT(evt_majinai_effect_se_fld)
    CALL(_majinai_effect)
    CALL(evt_msg_print, 0, STRING("msg_mj_coin_up"), 0, 0)
    CALL(_check, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_msg_print, 0, STRING("msg_mj_last"), 0, 0)
    END_IF
    RETURN
EVT_END()

// The 4-byte zero word that ends the unit's .data: an all-zero object needs explicit_zero_data to be emitted and
// force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_8030543C_data = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

void npcClearDeadInfo(void) {
    memset(gp->fbatData->deadInfo, 0, 0x500);
}

// Bowser's water level: splash on entering the water; npcMain halves the npc's speed while it is in it.
void npcKoopaWaterCheck(NpcEntry* entry) {
    if (entry->jumpFlags & 0x100000) {
        entry->jumpFlags |= 0x200000;
    } else {
        entry->jumpFlags &= ~0x200000;
    }
    entry->jumpFlags &= ~0x100000;
    if (!strcmp(gp->currentMapName, "kpa_03") && !(entry->position.y > 365.0f)) {
        entry->jumpFlags |= 0x100000;
        if (!(entry->jumpFlags & 0x200000) && (entry->jumpFlags & 0x100000)) {
            psndSFXOn_3D("SFX_KUPPA_3RD_WATER_FALL1", &entry->position);
        }
    }
}

void npcKoopaModeEncountCheck(void) {
    FieldBattleData* fbat = gp->fbatData;
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
                    if (!(entry->hazardImmuneFlags & 1)) {
                        npcGroupDead(entry, 1);
                    }
                    break;
                case 2:
                    if (!(entry->hazardImmuneFlags & 2)) {
                        npcGroupDead(entry, 2);
                    }
                    break;
                case 3:
                    if (!(entry->hazardImmuneFlags & 4)) {
                        npcGroupDead(entry, 4);
                    }
                    break;
                case 4:
                    kpaPowDown();
                    break;
                default:
                    if (!(entry->hazardImmuneFlags & 0x10) && *(u32*)entry->field_300 != 0 &&
                        !strcmp(hitGetName((HitEntry*)(*(u32*)entry->field_300)), "a_magu")) {
                        npcGroupDead(entry, 0x10);
                    }
                    break;
            }
        }
    }
}

s32 npcKoopaModeMobjBoundDeadCheck(HitEntry* mobj) {
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
            !(entry->hazardImmuneFlags & 8) && *(u32*)entry->field_300 == (u32)mobj) {
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

void npcKoopaJumpScore(FieldBattleData* data, MarioWork* mario) {
    if (data->field_57C > 0 && data->field_578 >= 2) {
        Vec pos = mario->position;

        pos.y += 60.0f;
        kpaAddScorePos(data->field_57C, &pos);
    }
    data->field_578 = 0;
    data->field_57C = 0;
}

// The original copies entry->position into a shared `position` temp and from there into one Vec per
// cloudGetBreathPower call (the temp sits at 0x2C, the call Vecs at 0x20/0x14/0x8); writing the copies
// this way is what reproduces those stack slots. npcMain inlines this function.
s32 npcCheckBlow(NpcEntry* entry) {
    Vec position;
    Vec pos1;
    Vec pos2;
    Vec pos3;

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
    position.y -= entry->height / 2.0f;
    pos2 = position;
    if (cloudGetBreathPower(entry->width, &pos2) > 0.0f) {
        return TRUE;
    }
    position = entry->position;
    position.y = position.y - entry->height;
    pos3 = position;
    if (cloudGetBreathPower(entry->width, &pos3) > 0.0f) {
        return TRUE;
    }
    return FALSE;
}

s32 N_fbatPreventMarioEventChk(void) {
    s32 mode = gp->fbatData->mode;

    if (mode != 0 && mode != 1 && mode != 8) {
        return TRUE;
    }
    return FALSE;
}

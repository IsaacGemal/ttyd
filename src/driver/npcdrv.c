#include "driver/npcdrv.h"
#include "dolphin/gx/GXStruct.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/itemdrv.h"
#include "driver/msgdrv.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "party.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "system.h"
#include <string.h>
#include "koopa.h"
#include "eff/eff_majinai.h"
#include "kpa_act.h"
#include "party/party_cloud.h"
#include "math_ext.h"
#include "mario/mario_motion.h"

extern GlobalWork* gp;
extern NpcTribeInfo npcTribe[];
extern char MarioStayPose[32];
extern char MarioTalkPose[32];
#ifdef MUST_MATCH
extern f32 vec3_802c2f78[];
#endif

int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

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


// NONMATCHING (98.72%): 1 extra, plus register differences.
void npcDisp_xlu(CameraId cameraId, void* param) {
    NpcEntry* entry = param; // cast to proper type
    Mtx mtx;
    GXColor color;
    f32 rotY;

    rotY = reviseAngle(-entry->direction);
    mtx_setup(entry, mtx, -1);

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
        angle = reviseAngle(MTXRadToDeg((f32)atan2(-dx, -dz)));
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
    animPoseMain(entry->poseId);
    animPoseDrawMtx(entry->poseId, mtx, 1, rotY, 2.0f);
    animPoseDrawMtx(entry->poseId, mtx, 2, rotY, 2.0f);
    animPoseDrawMtx(entry->poseId, mtx, 3, rotY, 2.0f);
}

// NONMATCHING (0.55%): 180 missing.
void npcDisp(CameraId camId, void* param) {
    NpcEntry* entry = param; // cast to proper type
}

// NONMATCHING (0.55%): 181 missing.
void npcDisp_offscreen_xlu(CameraId camId, void* param) {
    NpcEntry* entry = param; // cast to proper type
}

// NONMATCHING (0.51%): 194 missing.
void npcDisp_offscreen(CameraId camId, void* param) {
    NpcEntry* entry = param; // cast to proper type
}

// NONMATCHING (0.53%): 186 missing.
void npcDisp_blur(CameraId camId, void* param) {
    NpcEntry* entry = param; // cast to proper type
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
// NONMATCHING (81.76%): the two search loops use a cached `total` and an incrementing `entry` pointer,
// and the position/scale zero/one vectors are the shared rodata blobs vec3_802c2f78[0]/[3].  Still register
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

    if (total > 0) {
        for (i = 0, entry = wp->entries; i < total; i++, entry++) {
            if (!(entry->flags & 1)) {
                break; // unused entry
            }
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
    entry->prevPosition.x = entry->position.x;
    entry->prevPosition.y = entry->position.y;
    entry->prevPosition.z = entry->position.z;
    entry->width = animPoseGetHeight(entry->poseId);
    entry->height = animPoseGetRadius(entry->poseId);
#ifdef MUST_MATCH
    entry->scale = *(Vec*)&vec3_802c2f78[3];
#else
    entry->scale.x = 1.0f;
    entry->scale.y = 1.0f;
    entry->scale.z = 1.0f;
#endif
    entry->field_154 = 1.0f;
    entry->field_108 = 0;
    entry->field_10C = "M_I_2";
    entry->field_198 = gp->renderTime;
    entry->field_188 = 0;
    entry->jumpGravity = 1.0f;
    entry->field_1D0 = 0.0f;
    entry->jumpYScale = 1.0f;
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

void npcDeleteGroup(NpcEntry* entry) {
    if (entry->master) {
        entry = entry->master;
    }
    while (entry->prev) {
        entry = entry->prev;
    }
    _npcDeleteGroup(entry);
}

// NONMATCHING (97.10%): same instructions; only register numbers or operands differ.
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

// NONMATCHING (12.08%): 8 instructions differ, 1395 missing, 3 extra, plus register differences.
void npcMain(void) {
    NpcWork* npcwork;
    NpcEntry* entry;
    int i;

    npcwork = gp->inBattle ? &work.battle : &work.field;
    // npcwork = npcGetWorkPtr();
    marioGetPtr()->field_0x1D8 = 0;
    npcMainCount++;

    for (i = 0; i < npcwork->total; i++) {
        entry = &npcwork->entries[i];
        if (entry->flags & 1 && entry->flags & 2) {
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
        }
    }
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

void fbatHitCheckAll(void) {
    fbatHitCheck(0x13F1, 0);
}

void _fbatFirstAttackAnnounceDisp(CameraId camId, void* param) {
    FirstStrikeInfo* firstStrike = param;
    const char* message;

    message = msgSearch(firstStrike->msg_tag);
}

void npcSetSlave(NpcEntry* npc, NpcEntry* slave, s32 id) {
    slave->master = npc;
    npc->slaves[id] = slave;
}

void npcSetLink(NpcEntry* npc, NpcEntry* link) {
    npc->next = link;
    link->prev = npc;
}

void npcSetBtlSetupWork(NpcEntry* npc, s32 index, void* work) {
    ((void**)((u8*)npc + 0x2CC))[index] = work;
}

void* npcGetBtlSetupWork(NpcEntry* npc, s32 index) {
    return ((void**)((u8*)npc + 0x2CC))[index];
}

void fbatSandersBombTriggerOn(void) {
    gp->fbatData->field_54C[1] = TRUE;
}

void npcClearDeadInfo(void) {
    memset(gp->fbatData->deadInfo, 0, 0x500);
}

s32 N_fbatPreventMarioEventChk(void) {
    s32 mode = gp->fbatData->mode;

    if (mode != 0 && mode != 1 && mode != 8) {
        return TRUE;
    }
    return FALSE;
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

void npcSetMarioAutoTalkPose(const char* stayPose, const char* talkPose) {
    strcpy(MarioStayPose, stayPose);
    strcpy(MarioTalkPose, talkPose);
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

void npcStartForOneEvent(NpcEntry* entry) {
    if (entry && (entry->flags & 2) && (entry->flags & 0x10000000) && (entry->jumpFlags & 0x10000)) {
        entry->jumpFlags &= ~0x10000;
        evtStartID(entry->regularEvtId);
        animPoseSetLocalTimeRate(entry->poseId, 1.0f);
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

NpcEntry* fbatNpcTalkCheck(void) {
    NpcWork* wp = gp->inBattle ? &work.battle : &work.field;

    if (wp->field_0x10 && marioChkTalkable()) {
        return wp->field_0x10;
    }
    return NULL;
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
            !(*(u16*)entry->field_31E & 8) && *(u32*)entry->field_300 == (u32)mobj) {
            npcGroupDead(entry, 8);
            count++;
        }
    }
    return count;
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

void _npcDead(NpcEntry* entry, s32 koopaEncounterType) {
    *(s32*)entry->field_140 |= 0x10;
    entry->jumpFlags |= 4;
    entry->koopaEncounterType = koopaEncounterType;
    if (entry->flags & 4) {
        gp->fbatData->deadInfo[0].mask |= (1 << entry->field_314);
    }
}

// NONMATCHING (1.61%): empty placeholder, not decompiled yet.
void npcSetBattleInfo(NpcEntry* npc, s32 battleInfoId) {
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

// NONMATCHING (96.41%): 1 instruction differs, 1 missing, plus register differences.
s32 _majinai_effect(EventEntry* evt, s32 a2) {
    MarioWork* mario = marioGetPtr();

    if (a2 != 0) {
        EffectEntry* effect;

        effect = effMajinaiEntry(0, mario->position.x,
                                 mario->position.y + mario->wPlayerCollisionBox.y + 16.0f, mario->position.z);
        evt->userdata[0] = (s32)effect;
        *(f32*)((u8*)effect->userdata + 0x10) = 0.8f;
        effSetName((EffectEntry*)evt->userdata[0], "mjef_coinup");
    }
    if (evt->userdata[0] != 0 && effNameToPtr("mjef_coinup") != NULL) {
        return 0;
    }
    return 2;
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
            evt->wNpcEventType = 1;
            evt->ownerNPC = entry;
            entry->regularEvtId = evt->eventId;
        }
    }
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

// NONMATCHING (88.37%): 5 missing, 5 extra, plus register differences.
void npcSetStayPose(const char* name) {
    NpcWork* wp;
    NpcEntry* entry;
    s32 total;
    int i;

    if ((s8)name[0] == 0) {
        return;
    }
    if (!strcmp(name, "(^x^)party")) {
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

// NONMATCHING (88.35%): same signed-byte check and same prologue-scheduling diff as npcSetStayPose.
void npcSetTalkPose(const char* name) {
    NpcWork* wp;
    NpcEntry* entry;
    s32 total;
    int i;

    if ((s8)name[0] == 0) {
        return;
    }
    if (!strcmp(name, "(^x^)party")) {
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

// NONMATCHING (65.88%): 3 instructions differ, 24 missing, 1 extra, plus register differences.
s32 npcCheckBlow(NpcEntry* entry) {
    Vec pos;

    if (entry->flags & 0x20000000) {
        return FALSE;
    }
    if (entry->flags & 0x20) {
        return FALSE;
    }
    pos = entry->position;
    if (cloudGetBreathPower(entry->width, &pos) > 0.0f) {
        return TRUE;
    }
    pos.y = entry->position.y - entry->height * 0.5f;
    if (cloudGetBreathPower(entry->width, &pos) > 0.0f) {
        return TRUE;
    }
    pos.y = entry->position.y - entry->height;
    return cloudGetBreathPower(entry->width, &pos) > 0.0f;
}

// NONMATCHING (99.97%): control flow and registers match; only the inline-sqrtf double pool
// (`double_0p5_802c3138`/`double_3_802c3140`) resolves to a different address than the original.
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

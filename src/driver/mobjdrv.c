#include "driver/mobjdrv.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "driver/offscreendrv.h"
#include "driver/itemdrv.h"
#include "manager/evtmgr.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"
#include <string.h>
#include "manager/evtmgr_cmd.h"
#include "driver/shadowdrv.h"
#include "koopa.h"
#include "koopa_hit.h"
#include "motion/mot_hip.h"
#include "party.h"
#include "mario/mario_party.h"

#define MAX_MOBJ_WORK 3

extern GlobalWork* gp;

// .rodata constants referenced by this file's original.
extern const f32 float_0_8041a0c8;
extern const f32 float_0p5_8041a100;
extern const f32 float_2p5_8041a0e8;
extern const f32 float_deg2rad_8041a0dc;
extern const f32 float_10_8041a0cc;
extern char str_8041A0E0[8];
extern char str_8041A104[8];
extern char str_8041A10C[8];

// .data joint templates used by mobjHitEntry to build the embedded hit joints.
extern MapFileJoint cubeDJ;
extern MapFileJoint headDJ;
extern MapFileJoint frontDJ;
extern MapFileJoint front2DJ;

#ifdef MUST_MATCH
// MUST_MATCH: the original reaches the joint templates through the .data section base symbol (the
// compiler-generated negone_one pair); this local is the same anchor at the same section offset.
__declspec(section ".data") static f32 negone_one_374[2] = { -1.0f, 1.0f };
#endif

//.sbss
BOOL koopaRunFlag;

//.bss
static MapObjectWork work[MAX_MOBJ_WORK];

#define mobjGetWorkFlag(flag) (flag ? &work[1] : &work[0])

void mobjKoopaOn(void) {
    if (!koopaRunFlag) {
        MapObjectWork* wp = &work[2];

        koopaRunFlag = TRUE;
        wp->count = 128;
        wp->entries = _mapAlloc(sizeof(MapObjectEntry) * wp->count);
        memset(wp->entries, 0, sizeof(MapObjectEntry) * wp->count);
    }
}

inline MapObjectWork* mobjGetWork(void) {
    if (koopaRunFlag) {
        return &work[2];
    } else {
        return mobjGetWorkFlag(gp->inBattle);
    }
}

inline void calcMtx(MapObjectEntry* entry, Mtx dest, Vec position) { // guessing, always inlined
    Mtx trans, scale, xrot, yrot, zrot;

    MTXTrans(trans, position.x, position.y, position.z);
    MTXScale(scale, entry->scale2.x, entry->scale2.y, entry->scale2.z);
    MTXRotRad(xrot, 'x', MTXDegToRad(entry->rotation.x));
    MTXRotRad(yrot, 'y', MTXDegToRad(entry->rotation.y));
    MTXRotRad(zrot, 'z', MTXDegToRad(entry->rotation.z));
    MTXConcat(trans, zrot, trans);
    MTXConcat(trans, yrot, trans);
    MTXConcat(trans, xrot, trans);
    MTXConcat(trans, scale, dest);
}

void mobjDispXLU(CameraId camId, void* param) {
    MapObjectEntry* entry = param; // cast to correct type

    if (entry->flags & 0x400) {
        animSetPaperTexMtx(entry->paperTexMtx, NULL, NULL);
        animSetPaperTexObj(&entry->paperTexObj, NULL, NULL, entry->unk208, NULL, NULL, 1, 1);
    }
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 10.0f * entry->scale.x, 1);
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 10.0f * entry->scale.x, 2);
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 10.0f * entry->scale.x, 3);
    if (entry->flags & 0x400) {
        animSetPaperTexMtx(NULL, NULL, NULL);
        animSetPaperTexObj(NULL, NULL, NULL, NULL, NULL, NULL, 1, 1);
    }
}

void mobjDisp(CameraId camId, void* param) {
    MapObjectEntry* entry = param; // cast to correct type
    DispEntry* disp;

    if (entry->flags & 0x400) {
        animSetPaperTexMtx(entry->paperTexMtx, NULL, NULL);
        animSetPaperTexObj(&entry->paperTexObj, NULL, NULL, entry->unk208, NULL, NULL, 1, 1);
    }
    disp = dispGetCurWork();
    switch (disp->renderMode) {
        case 0:
            animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                         10.0f * entry->scale.x, 1);
            break;
        case 1:
            animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                         10.0f * entry->scale.x, 2);
            break;
        case 2:
        default:
            animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                         10.0f * entry->scale.x, 3);
            break;
    }
    if (entry->flags & 0x400) {
        animSetPaperTexMtx(NULL, NULL, NULL);
        animSetPaperTexObj(NULL, NULL, NULL, NULL, NULL, NULL, 1, 1);
    }
}

void mobjDisp_OffscreenXLU(CameraId camId, void* param) {
    MapObjectEntry* entry = param; // cast to correct type
    u16 left, top, right, bottom;

    sysWaitDrawSync();
    GXClearBoundingBox();
    if (entry->flags & 0x400) {
        animSetPaperTexMtx(entry->paperTexMtx, NULL, NULL);
        animSetPaperTexObj(&entry->paperTexObj, NULL, NULL, entry->unk208, NULL, NULL, 1, 1);
    }
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 10.0f * entry->scale.x, 1);
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 10.0f * entry->scale.x, 2);
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 10.0f * entry->scale.x, 3);
    if (entry->flags & 0x400) {
        animSetPaperTexMtx(NULL, NULL, NULL);
        animSetPaperTexObj(NULL, NULL, NULL, NULL, NULL, NULL, 1, 1);
    }
    sysWaitDrawSync();
    GXReadBoundingBox(&left, &top, &right, &bottom);
    offscreenAddBoundingBox(entry->offscreenId, left, top, right, bottom);
}

void mobjInit(void) {
    MapObjectWork* wp = &work[0];

    wp->count = 16;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(MapObjectEntry) * wp->count);
    memset(wp->entries, 0, sizeof(MapObjectEntry) * wp->count);

    wp = &work[1];
    wp->count = 8;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(MapObjectEntry) * wp->count);
    memset(wp->entries, 0, sizeof(MapObjectEntry) * wp->count);

    koopaRunFlag = FALSE;
}

void mobjReset(BOOL inBattle) {
    MapObjectWork* wp = mobjGetWorkFlag(inBattle);
    memset(wp->entries, 0, sizeof(MapObjectEntry) * wp->count);
    koopaRunFlag = FALSE;
}

// NONMATCHING (99.89%): only the f-register choice in the scale2.x/scale2.z computation differs
// (the original holds bboxMax in f2 and scale in f1; ours holds bboxMax in f1 and scale in f2).
void mobjHitEntry(MapObjectEntry* entry, s32 type) {
    AnimationPoseData* animData;
    Vec bboxMin;
    Vec bboxMax;
    Vec position;
    Vec minTmp;
    Vec maxTmp;
    Mtx mtx;
    s32 i;
#ifdef MUST_MATCH
    u8* base = (u8*)negone_one_374;
#endif

    animData = animPoseGetAnimBaseDataPtr(entry->poseId);
    minTmp = (Vec){ 0.0f, 0.0f, 0.0f };
    minTmp.x = animData->bboxMin.x;
    minTmp.y = animData->bboxMin.y;
    minTmp.z = animData->bboxMin.z;
    bboxMin = minTmp;
    maxTmp = (Vec){ 0.0f, 0.0f, 0.0f };
    maxTmp.x = animData->bboxMax.x;
    maxTmp.y = animData->bboxMax.y;
    maxTmp.z = animData->bboxMax.z;
    bboxMax = maxTmp;

    for (i = 0; i < 2; i++) {
        if (entry->hitObj[i].hitObject != NULL)
            hitDelete(entry->hitObj[i].name);
    }

    PSVECAdd(&bboxMax, &bboxMin, &entry->translation);
    PSVECScale(&entry->translation, &entry->translation, float_0p5_8041a100);
    PSVECScale(&entry->translation, &entry->translation, 10.0f);
    entry->translation.y = float_0_8041a0c8;
    entry->scale2.x = float_10_8041a0cc * (entry->scale.x * (f32)__fabs(bboxMax.x - bboxMin.x));
    entry->scale2.y = float_10_8041a0cc * (entry->scale.y * (f32)__fabs(bboxMax.y));
    entry->scale2.z = float_10_8041a0cc * (entry->scale.z * (f32)__fabs(bboxMax.z - bboxMin.z));

    if (strncmp(animData->fileName, str_8041A0E0, strlen(str_8041A0E0)) == 0) {
        entry->translation.z = float_0_8041a0c8;
        entry->scale2.z = float_10_8041a0cc * (float_2p5_8041a0e8 * entry->scale.z);
    }

    switch (type) {
        case 0: {
            Mtx zrot;
            Mtx yrot;
            Mtx xrot;
            Mtx scale;
            Mtx trans;

            PSVECAdd(&entry->position, &entry->translation, &position);
            MTXTrans(trans, position.x, position.y, position.z);
            MTXScale(scale, entry->scale2.x, entry->scale2.y, entry->scale2.z);
            MTXRotRad(xrot, 'x', (float_deg2rad_8041a0dc * entry->rotation.x));
            MTXRotRad(yrot, 'y', (float_deg2rad_8041a0dc * entry->rotation.y));
            MTXRotRad(zrot, 'z', (float_deg2rad_8041a0dc * entry->rotation.z));
            MTXConcat(trans, zrot, trans);
            MTXConcat(trans, yrot, trans);
            MTXConcat(trans, xrot, trans);
            MTXConcat(trans, scale, mtx);
#ifdef MUST_MATCH
            memcpy(&entry->hitObj[0].unk4[8], (MapFileJoint*)(base + 0x16a4), 0x68);
#else
            memcpy(&entry->hitObj[0].unk4[8], &cubeDJ, 0x68);
#endif
            strcpy(entry->hitObj[0].name, str_8041A104);
            strcat(entry->hitObj[0].name, entry->name);
            ((MapFileJoint*)&entry->hitObj[0].unk4[8])->name = entry->hitObj[0].name;
            entry->hitObj[0].hitObject = hitEntryMOBJ((MapFileJoint*)&entry->hitObj[0].unk4[8], mtx);
            entry->hitObj[0].hitObject->mapObj = (MapObject*)entry;
            break;
        }
        case 1: {
            Mtx zrot;
            Mtx yrot;
            Mtx xrot;
            Mtx scale;
            Mtx trans;

            PSVECAdd(&entry->position, &entry->translation, &position);
            MTXTrans(trans, position.x, position.y, position.z);
            MTXScale(scale, entry->scale2.x, entry->scale2.y, entry->scale2.z);
            MTXRotRad(xrot, 'x', (float_deg2rad_8041a0dc * entry->rotation.x));
            MTXRotRad(yrot, 'y', (float_deg2rad_8041a0dc * entry->rotation.y));
            MTXRotRad(zrot, 'z', (float_deg2rad_8041a0dc * entry->rotation.z));
            MTXConcat(trans, zrot, trans);
            MTXConcat(trans, yrot, trans);
            MTXConcat(trans, xrot, trans);
            MTXConcat(trans, scale, mtx);
#ifdef MUST_MATCH
            memcpy(&entry->hitObj[0].unk4[8], (MapFileJoint*)(base + 0x1758), 0x68);
#else
            memcpy(&entry->hitObj[0].unk4[8], &headDJ, 0x68);
#endif
            strcpy(entry->hitObj[0].name, str_8041A104);
            strcat(entry->hitObj[0].name, entry->name);
            ((MapFileJoint*)&entry->hitObj[0].unk4[8])->name = entry->hitObj[0].name;
            entry->hitObj[0].hitObject = hitEntryMOBJ((MapFileJoint*)&entry->hitObj[0].unk4[8], mtx);
            entry->hitObj[0].hitObject->mapObj = (MapObject*)entry;
            break;
        }
        case 2: {
            Mtx zrot;
            Mtx yrot;
            Mtx xrot;
            Mtx scale;
            Mtx trans;

            PSVECAdd(&entry->position, &entry->translation, &position);
            MTXTrans(trans, position.x, position.y, position.z);
            MTXScale(scale, entry->scale2.x, entry->scale2.y, entry->scale2.z);
            MTXRotRad(xrot, 'x', (float_deg2rad_8041a0dc * entry->rotation.x));
            MTXRotRad(yrot, 'y', (float_deg2rad_8041a0dc * entry->rotation.y));
            MTXRotRad(zrot, 'z', (float_deg2rad_8041a0dc * entry->rotation.z));
            MTXConcat(trans, zrot, trans);
            MTXConcat(trans, yrot, trans);
            MTXConcat(trans, xrot, trans);
            MTXConcat(trans, scale, mtx);
#ifdef MUST_MATCH
            memcpy(&entry->hitObj[0].unk4[8], (MapFileJoint*)(base + 0x180c), 0x68);
#else
            memcpy(&entry->hitObj[0].unk4[8], &frontDJ, 0x68);
#endif
            strcpy(entry->hitObj[0].name, str_8041A104);
            strcat(entry->hitObj[0].name, entry->name);
            ((MapFileJoint*)&entry->hitObj[0].unk4[8])->name = entry->hitObj[0].name;
            entry->hitObj[0].hitObject = hitEntryMOBJ((MapFileJoint*)&entry->hitObj[0].unk4[8], mtx);
            entry->hitObj[0].hitObject->mapObj = (MapObject*)entry;
#ifdef MUST_MATCH
            memcpy(&entry->hitObj[1].unk4[8], (MapFileJoint*)(base + 0x18c0), 0x68);
#else
            memcpy(&entry->hitObj[1].unk4[8], &front2DJ, 0x68);
#endif
            strcpy(entry->hitObj[1].name, str_8041A10C);
            strcat(entry->hitObj[1].name, entry->name);
            ((MapFileJoint*)&entry->hitObj[1].unk4[8])->name = entry->hitObj[1].name;
            entry->hitObj[1].hitObject = hitEntryMOBJ((MapFileJoint*)&entry->hitObj[1].unk4[8], mtx);
            entry->hitObj[1].hitObject->mapObj = (MapObject*)entry;
            break;
        }
    }
}

s32 mobjEntry(const char* name, const char* animPoseName) {
    MapObjectWork* wp = mobjGetWork();
    s32 entryCount;
    s32 idx;
    MapObjectEntry* entry;

    entryCount = wp->count;
    for (idx = 0, entry = wp->entries; idx < entryCount; idx++, entry++) {
        if ((entry->flags & 1) && (strcmp(entry->name, name) == 0))
            break;
    }
    // SPM uses this to assert the name isn't duplicate, here it's just pointless

    for (idx = 0, entry = wp->entries; idx < entryCount; idx++, entry++) {
        if ((entry->flags & 1) == 0)
            break;
    }

    memset(entry, 0, sizeof(*entry));
    entry->flags |= 1;
    strcpy(entry->name, name);

    entry->position = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->scale = (Vec){ 1.0f, 1.0f, 1.0f };
    entry->rotation = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->unk5C = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->unk68 = 1.0f;
    entry->unk6C = 1.0f;
    entry->camId = CAMERA_3D;
    entry->poseId = animPoseEntry(animPoseName, gp->inBattle != 0);
    animPosePeraOff(entry->poseId);
    strcpy(entry->animName, animPoseName);
    animPoseSetAnim(entry->poseId, "S_1", 1);
    animPoseSetMaterialLightFlagOn(entry->poseId, 2);
    mobjHitEntry(entry, 0);
    entry->offscreenId = -1;
    return idx;
}

void mobjDelete(const char* name) {
    MapObjectWork* wp = mobjGetWork();
    s32 entryCount;
    int i;
    MapObjectEntry* entry;

    entryCount = wp->count;
    entry = wp->entries;
    for (i = 0; i < entryCount; i++, entry++) {
        if ((entry->flags & 1) && strcmp(entry->name, name) == 0) {
            for (i = 0; i < 2; i++) {
                if ((entry->hitObj[i].name) != NULL) {
                    if (!gp->inBattle) {
                        marioResetHitObj(entry->hitObj[i].name);
                    }
                    hitDelete(entry->hitObj[i].name);
                }
            }
            entry->flags &= ~1;
            break;
        }
    }
}

void mobjSetPosition(const char* name, f32 x, f32 y, f32 z) {
    MapObjectWork* wp = mobjGetWork();
    MapObjectEntry* entry;
    s32 entryCount;
    int i;

    entryCount = wp->count;
    entry = wp->entries;
    for (i = 0; i < entryCount; i++, entry++) {
        if ((entry->flags & 1) && strcmp(entry->name, name) == 0) {
            break;
        }
    }

    entry->position.x = x;
    entry->position.y = y + 0.1f;
    entry->position.z = z;
}

// Static inline duplicates of mobjCalcMtx/mobjCalcMtx2 so the compiler always inlines them into
// mobjMain (the original's mobjMain has both bodies inlined).
static inline void mobjMainCalcMtx(MapObjectEntry* entry, Mtx mtx, BOOL calcBBox) {
    Vec position;
    Mtx trans;
    Mtx scale;
    Mtx xRot;
    Mtx yRot;
    Mtx zRot;
    AnimationPoseData* animData;
    Vec bboxMin;
    Vec bboxMax;
    Vec minTmp;
    Vec maxTmp;
    s32 i;

    PSVECAdd(&entry->position, &entry->translation, &position);

    if (calcBBox) {
        animData = animPoseGetAnimBaseDataPtr(entry->poseId);

        minTmp = (Vec){ 0.0f, 0.0f, 0.0f };
        minTmp.x = animData->bboxMin.x;
        minTmp.y = animData->bboxMin.y;
        minTmp.z = animData->bboxMin.z;
        bboxMin = minTmp;
        maxTmp = (Vec){ 0.0f, 0.0f, 0.0f };
        maxTmp.x = animData->bboxMax.x;
        maxTmp.y = animData->bboxMax.y;
        maxTmp.z = animData->bboxMax.z;
        bboxMax = maxTmp;

        entry->scale2.x = 10.0f * (entry->scale.x * (f32)__fabs(bboxMax.x - bboxMin.x));
        entry->scale2.y = 10.0f * (entry->scale.y * (f32)__fabs(bboxMax.y));
        entry->scale2.z = 10.0f * (entry->scale.z * (f32)__fabs(bboxMax.z - bboxMin.z));

        if (strncmp(animData->fileName, str_8041A0E0, strlen(str_8041A0E0)) == 0) {
            entry->translation.z = 0.0f;
            entry->scale2.z = 10.0f * (2.5f * entry->scale.z);
        }
    }

    MTXTrans(trans, position.x, position.y, position.z);
    MTXScale(scale, entry->scale2.x, entry->scale2.y, entry->scale2.z);
    MTXRotRad(xRot, 'x', MTXDegToRad(entry->rotation.x));
    MTXRotRad(yRot, 'y', MTXDegToRad(entry->rotation.y));
    MTXRotRad(zRot, 'z', MTXDegToRad(entry->rotation.z));
    MTXConcat(trans, zRot, trans);
    MTXConcat(trans, yRot, trans);
    MTXConcat(trans, xRot, trans);
    MTXConcat(trans, scale, mtx);

    for (i = 0; i < 2; i++) {
        if (entry->hitObj[i].hitObject != 0)
            hitReCalcMatrix(entry->hitObj[i].hitObject, mtx);
    }
}

// NONMATCHING (99.38%): 3 instructions differ, 1 missing, 4 extra, plus register differences. Known issue: register allocation and stack-slot differences; see REPORT.md
void mobjMain(void) {
    MapObjectWork* wp;
    s32 count;
    MapObjectEntry* entry;
    MarioWork* mario;
    PartyEntry* party;
    FieldBattleData* fbat;
    s16 mode;
    void* v24;
    s32 v25;
    s32 i;
    Mtx mtx1, mtx2, mtx3;

    wp = mobjGetWork();
    count = wp->count;
    entry = wp->entries;

    mario = marioGetPtr();
    party = partyGetPtr(marioGetPartyId());
    fbat = fbatGetPointer();
    mode = fbat->mode;
    v24 = NULL;
    v25 = 0;
    if (party != NULL && (party->flags & 0x100)) {
        v25 = 1;
    }

    if (gp->inBattle == 0) {
        if (*(void**)((u8*)mario + 0x1FC) != NULL) {
            if (((void**)wp)[2] == NULL ||
                strcmp(hitGetName(((HitEntry**)wp)[2]), hitGetName(*(HitEntry**)((u8*)mario + 0x1FC))) != 0) {
                ((void**)wp)[2] = *(void**)((u8*)mario + 0x1FC);
            }
        } else {
            ((void**)wp)[2] = NULL;
        }

        if (*(void**)((u8*)mario + 0x1F4) != NULL) {
            if (((void**)wp)[3] == NULL ||
                strcmp(hitGetName(((HitEntry**)wp)[3]), hitGetName(*(HitEntry**)((u8*)mario + 0x1F4))) != 0) {
                void* tmp = *(void**)((u8*)mario + 0x1F4);

                ((void**)wp)[3] = tmp;
                v24 = tmp;
            }
        } else {
            ((void**)wp)[3] = NULL;
        }

        if (*(void**)((u8*)mario + 0x1E4) != NULL) {
            if (((void**)wp)[4] == NULL ||
                strcmp(hitGetName(((HitEntry**)wp)[4]), hitGetName(*(HitEntry**)((u8*)mario + 0x1E4))) != 0) {
                ((void**)wp)[4] = *(void**)((u8*)mario + 0x1E4);
            }
        } else {
            ((void**)wp)[4] = NULL;
        }

        if (*(void**)((u8*)mario + 0x1E0) != NULL) {
            if (((void**)wp)[5] == NULL ||
                strcmp(hitGetName(((HitEntry**)wp)[5]), hitGetName(*(HitEntry**)((u8*)mario + 0x1E0))) != 0) {
                ((void**)wp)[5] = *(void**)((u8*)mario + 0x1E0);
            }
        } else {
            ((void**)wp)[5] = NULL;
        }

        if (party != NULL && *(void**)((u8*)party + 0x130) != NULL) {
            if (((void**)wp)[6] == NULL ||
                strcmp(hitGetName(((HitEntry**)wp)[6]), hitGetName(*(HitEntry**)((u8*)party + 0x130))) != 0) {
                ((void**)wp)[6] = *(void**)((u8*)party + 0x130);
            }
        } else {
            ((void**)wp)[6] = NULL;
        }
    }

    for (i = 0; i < count; i++, entry++) {
        if (!(entry->flags & 1))
            continue;

        if (gp->inBattle != 0 || mode <= 1u || mode == 8) {
            s32 v22;
            HitEntry* hitObj;

            if (entry->flags & 4) {
                if (mobjCheckItemboxOpen(entry) == 2) {
                    mobjMainCalcMtx(entry, mtx1, FALSE);
                } else {
                    mobjMainCalcMtx(entry, mtx2, TRUE);
                }
                entry->flags &= ~4;
            }

            if (entry->flags & 0x40) {
                if (entry->unk5C.y >= 0.0f) {
                    entry->position.y += entry->unk5C.y;
                    entry->unk5C.y -= entry->unk68;
                } else {
                    f32 nz, ny, nx, dist, hitZ, hitY, hitX;

                    dist = 10.0f + (1.0f + (f32)__fabs(entry->unk5C.y));
                    if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &dist, &nx, &ny, &nz, entry->position.x,
                                       10.0f + entry->position.y, entry->position.z, 0.0f, -1.0f, 0.0f) != NULL) {
                        entry->position.y = hitY;
                        entry->unk5C.y = (-0.5f * entry->unk6C) * entry->unk5C.y;
                        if (__fabs(entry->unk5C.y) <= 2.0) {
                            entry->unk5C.y = 0.0f;
                        }
                    } else {
                        entry->position.y += entry->unk5C.y;
                        entry->unk5C.y -= entry->unk68;
                    }
                }
                mobjMainCalcMtx(entry, mtx3, TRUE);
            }

            if (!(entry->flags & 8)) {
                v22 = 0;
                hitObj = entry->hitObj[0].hitObject;
                if (mario->characterId != 2) {
                    if ((entry->flags & 0x10000) && *(void**)((u8*)mario + 0x1FC) == (void*)hitObj) {
                        if (marioGetPtr()->position.y <= 5.0f + entry->position.y) {
                            v22 = 1;
                            entry->unk1E0 = 0x10000;
                            goto apply;
                        }
                    }
                    if ((entry->flags & 0x400000) && v24 == (void*)hitObj) {
                        v22 = 1;
                        entry->unk1E0 = 0x400000;
                        goto apply;
                    }
                    if ((entry->flags & 0x200000) && *(void**)((u8*)mario + 0x1E4) == (void*)hitObj) {
                        v22 = 1;
                        entry->unk1E0 = 0x200000;
                        goto apply;
                    }
                    if ((entry->flags & 0x1000000) && *(void**)((u8*)mario + 0x1E4) == (void*)hitObj &&
                        marioChkHipBump() != 0) {
                        v22 = 1;
                        entry->unk1E0 = 0x1000000;
                        goto apply;
                    }
                    if (unk_8005bb80() != 0 && (entry->flags & 0x40000) &&
                        *(void**)(mario->field_0x1DC + 4) == (void*)hitObj && (*(u16*)(mario->field_0x245 + 3) & 0x100) &&
                        v25 == 0) {
                        v22 = 1;
                        entry->unk1E0 = 0x40000;
                        goto apply;
                    }
                    if ((entry->flags & 0x800000) && party != NULL) {
                        if (*(void**)((u8*)party + 0x130) == (void*)hitObj) {
                            v22 = 1;
                            entry->unk1E0 = 0x800000;
                            goto apply;
                        }
                        if (*(void**)((u8*)party + 0x130) != NULL && party->currentMemberId == 3 &&
                            PSVECDistance(&party->position, &entry->position) < entry->hitObj[0].hitObject->radius + party->field_F4) {
                            v22 = 1;
                            entry->unk1E0 = 0x800000;
                            goto apply;
                        }
                    }
                    if ((entry->flags & 0x2000000) && party != NULL && party->currentMemberId == 3) {
                        if (*(void**)((u8*)party + 0x130) == (void*)hitObj) {
                            v22 = 1;
                            entry->unk1E0 = 0x2000000;
                            goto apply;
                        }
                        if (*(void**)((u8*)party + 0x130) != NULL &&
                            PSVECDistance(&party->position, &entry->position) < entry->hitObj[0].hitObject->radius + party->field_F4) {
                            v22 = 1;
                            entry->unk1E0 = 0x800000;
                            goto apply;
                        }
                    }
                } else {
                    s32 j;

                    if (entry->flags & 0x4000000) {
                        for (j = 0; j < 10; j++) {
                            if (kpaGetHitobjHead(j) == (void*)hitObj) {
                                v22 = 1;
                                entry->unk1E0 = 0x4000000;
                                break;
                            }
                        }
                    }
                    if (entry->flags & 0x10000000) {
                        for (j = 0; j < 10 && v22 == 0; j++) {
                            if (kpaGetHitobjRide(j) == (void*)hitObj) {
                                if (kpaHipAttackCheck() != 0) {
                                    v22 = 1;
                                    entry->unk1E0 = 0x10000000;
                                    break;
                                }
                            }
                        }
                    }
                    if ((entry->flags & 0x8000000) && kpaGetHitobj() == (void*)hitObj && kpaGetLevel() == 3) {
                        v22 = 1;
                        entry->unk1E0 = 0x8000000;
                        goto apply;
                    }
                    if ((entry->flags & 0x20000000) && kpaGetHitFlame() == (void*)hitObj) {
                        v22 = 1;
                        entry->unk1E0 = 0x20000000;
                        goto apply;
                    }
                    if ((entry->flags & 0x80000) && *(void**)((u8*)mario + 0x1E0) == (void*)hitObj) {
                        v22 = 1;
                        entry->unk1E0 = 0x80000;
                        goto apply;
                    }
                    if ((entry->flags & 0x200000) && *(void**)((u8*)mario + 0x1E4) == (void*)hitObj) {
                        v22 = 1;
                        entry->unk1E0 = 0x200000;
                    }
                }
            apply:
                if (v22 != 0) {
                    entry->flags |= 8;
                }
            }

            if (entry->flags & 0x100) {
                entry->flags &= ~8;
            }
            if ((u32)marioStGetSystemLevel() != 0) {
                entry->flags &= ~8;
            }
            {
                MarioWork* m = marioGetPtr();

                if (m->currMotionId == 0x1F || m->currMotionId == 0x20) {
                    entry->flags &= ~8;
                }
            }
            if (entry->unk1D0 != NULL) {
                if (entry->unk1D0(entry) == -1)
                    continue;
            }
        }

        if (!(entry->flags & 1))
            continue;

        if (koopaRunFlag == 0 && (entry->flags & 0x802) == 0) {
            GXColor blend;
            GXColor blend2;
            MapEntry* mapEntry;
            s32 shadow;

            mapEntry = &mapGetWork()->entries[0];
            mapGetBlend(&blend);
            mapGetBlend2(&blend2);
            if ((mapEntry->flags & 2) && !(animPoseGetMaterialFlag(entry->poseId) & 0x800) && blend.a != 0xff) {
                goto noShadow;
            }
            if ((mapEntry->flags & 4) && !(animPoseGetMaterialFlag(entry->poseId) & 0x1000) && blend2.a != 0xff) {
                goto noShadow;
            }
            shadow = shadowEntry(entry->position.x, entry->position.y, entry->position.z,
                                 10.0f * entry->scale.x * entry->scale2.x / 25.0f);
            if (entry->flags & 0x400000) {
                shadowSetType(shadow, 1);
                shadowSetSize(shadow, 10.0f * entry->scale.x * entry->scale2.x / 20.0f);
            }
        noShadow:;
        }

        animPoseMain(entry->poseId);

        {
            GXColor color;
            *(u32*)&color = animPoseGetMaterialEvtColor(entry->poseId);

            if (entry->flags & 0x4000)
                continue;
            if ((entry->flags & 0x80) == 0) {
                Vec pos;

                PSMTXMultVec(camGetPtr(CAMERA_3D)->view, &entry->position, &pos);
                PSMTX44MultVec(camGetPtr(CAMERA_3D)->projection, &pos, &pos);
                if (__fabs(pos.x) > 1.5)
                    continue;
                if (__fabs(pos.y) > 1.5)
                    continue;
                if (__fabs(pos.z) > 1.0)
                    continue;
            }
            if ((entry->flags & 0x2000) && ((gp->retraceCount >> 3) & 1))
                continue;

            if (entry->offscreenId == -1) {
                if ((animPoseGetMaterialFlag(entry->poseId) & 0x40) && ((u8*)&color)[3] != 0xff) {
                    dispEntry(entry->camId, dispCalcZ(&entry->position), 2, mobjDispXLU, entry);
                } else {
                    dispEntry(entry->camId, dispCalcZ(&entry->position), 0, mobjDisp, entry);
                    dispEntry(entry->camId, dispCalcZ(&entry->position), 1, mobjDisp, entry);
                    dispEntry(entry->camId, dispCalcZ(&entry->position), 2, mobjDisp, entry);
                }
            } else {
                if ((animPoseGetMaterialFlag(entry->poseId) & 0x40) && ((u8*)&color)[3] != 0xff) {
                    dispEntry(CAMERA_OFFSCREEN2, dispCalcZ(&entry->position), 2, mobjDisp_OffscreenXLU, entry);
                } else {
                    dispEntry(CAMERA_OFFSCREEN2, dispCalcZ(&entry->position), 0, mobjDisp_OffscreenXLU, entry);
                    dispEntry(CAMERA_OFFSCREEN2, dispCalcZ(&entry->position), 1, mobjDisp_OffscreenXLU, entry);
                    dispEntry(CAMERA_OFFSCREEN2, dispCalcZ(&entry->position), 2, mobjDisp_OffscreenXLU, entry);
                }
            }
        }
    }
}

MapObjectEntry* mobjNameToPtr(const char* name) {
    int i;
    s32 entryCount;
    MapObjectEntry* entry;
    MapObjectWork* wp = mobjGetWork();

    entryCount = wp->count;
    entry = wp->entries;
    for (i = 0; i < entryCount; i++, entry++) {
        if ((entry->flags & 1) && strcmp(entry->name, name) == 0) {
            break;
        }
    }

    return entry;
}

MapObjectEntry* mobjNameToPtrNoAssert(const char* name) {
    int i;
    s32 entryCount;
    MapObjectEntry* entry;
    MapObjectWork* wp = mobjGetWork();

    entryCount = wp->count;
    entry = wp->entries;
    for (i = 0; i < entryCount; i++, entry++) {
        if ((entry->flags & 1) && strcmp(entry->name, name) == 0) {
            break;
        }
    }

    if (i >= entryCount) {
        return NULL;
    }

    return entry;
}

MapObjectEntry* mobjHitObjPtrToPtr(HitEntry* hitObj) {
    // for mobj hits, HitEntry.mapObj holds the MapObjectEntry
    if (hitObj->attributes & (1 << 31)) {
        return (MapObjectEntry*)hitObj->mapObj;
    }

    return NULL;
}

EventEntry* mobjRunEvent(MapObjectEntry* entry, s32* eventCode) {
    EventEntry* event;

    if (eventCode == NULL) {
        return NULL;
    }
    if (evtCheckID(entry->eventId)) {
        return NULL;
    }

    entry->eventId = 0;
    event = evtEntryType(eventCode, 0x1E, 0, 0x1A);
    event->thisMapObj = entry;
    entry->eventId = event->eventId;

    return event;
}

void mobjCalcMtx(MapObjectEntry* entry) {
    Mtx mtx;
    Mtx zRot;
    Mtx yRot;
    Mtx xRot;
    Mtx scale;
    Mtx trans;
    Vec position;
    AnimationPoseData* animData;
    Vec bboxMin;
    Vec bboxMax;
    Vec minTmp;
    Vec maxTmp;
    s32 i;

    PSVECAdd(&entry->position, &entry->translation, &position);
    animData = animPoseGetAnimBaseDataPtr(entry->poseId);

    minTmp = (Vec){ 0.0f, 0.0f, 0.0f };
    minTmp.x = animData->bboxMin.x;
    minTmp.y = animData->bboxMin.y;
    minTmp.z = animData->bboxMin.z;
    bboxMin = minTmp;
    maxTmp = (Vec){ 0.0f, 0.0f, 0.0f };
    maxTmp.x = animData->bboxMax.x;
    maxTmp.y = animData->bboxMax.y;
    maxTmp.z = animData->bboxMax.z;
    bboxMax = maxTmp;

    entry->scale2.x = 10.0f * (entry->scale.x * (f32)__fabs(bboxMax.x - bboxMin.x));
    entry->scale2.y = 10.0f * (entry->scale.y * (f32)__fabs(bboxMax.y));
    entry->scale2.z = 10.0f * (entry->scale.z * (f32)__fabs(bboxMax.z - bboxMin.z));

    if (strncmp(animData->fileName, "HOBJ_", strlen("HOBJ_")) == 0) {
        entry->translation.z = 0.0f;
        entry->scale2.z = 10.0f * (2.5f * entry->scale.z);
    }

    MTXTrans(trans, position.x, position.y, position.z);
    MTXScale(scale, entry->scale2.x, entry->scale2.y, entry->scale2.z);
    MTXRotRad(xRot, 'x', MTXDegToRad(entry->rotation.x));
    MTXRotRad(yRot, 'y', MTXDegToRad(entry->rotation.y));
    MTXRotRad(zRot, 'z', MTXDegToRad(entry->rotation.z));
    MTXConcat(trans, zRot, trans);
    MTXConcat(trans, yRot, trans);
    MTXConcat(trans, xRot, trans);
    MTXConcat(trans, scale, mtx);

    for (i = 0; i < 2; i++) {
        if (entry->hitObj[i].hitObject != 0)
            hitReCalcMatrix(entry->hitObj[i].hitObject, mtx);
    }
}

void mobjCalcMtx2(MapObjectEntry* entry) {
    Mtx mtx;
    Mtx zRot;
    Mtx yRot;
    Mtx xRot;
    Mtx scale;
    Mtx trans;
    Vec position;
    s32 i;

    PSVECAdd(&entry->position, &entry->translation, &position);

    // TODO: Try matching using calcMtx inline function - had issues with lining up the stack offsets
    // Matches fine here
    MTXTrans(trans, position.x, position.y, position.z);
    MTXScale(scale, entry->scale2.x, entry->scale2.y, entry->scale2.z);
    MTXRotRad(xRot, 'x', MTXDegToRad(entry->rotation.x));
    MTXRotRad(yRot, 'y', MTXDegToRad(entry->rotation.y));
    MTXRotRad(zRot, 'z', MTXDegToRad(entry->rotation.z));
    MTXConcat(trans, zRot, trans);
    MTXConcat(trans, yRot, trans);
    MTXConcat(trans, xRot, trans);
    MTXConcat(trans, scale, mtx);

    for (i = 0; i < 2; i++) {
        if (entry->hitObj[i].hitObject != 0)
            hitReCalcMatrix(entry->hitObj[i].hitObject, mtx);
    }
}

BOOL mobjGetHint(MapObjectEntry* entry) {
    if (entry == NULL) {
        return 0;
    }
    return (entry->flags >> 5) & 1;
}

BOOL mobjCheckExec() {
    MapObjectWork* wp = mobjGetWork();
    int i;
    MapObjectEntry* entry;

    for (i = 0, entry = wp->entries; i < wp->count; entry++, i++) {
        if (entry->flags & (1 << 9))
            return TRUE;
    }
    return FALSE;
}

s32 mobjCheckItemboxOpen(MapObjectEntry* entry) {
    if (strcmp(entry->animName, "MOBJ_TreasureBox") == 0 || strcmp(entry->animName, "MOBJ_BigTreasureBox") == 0 ||
        strcmp(entry->animName, "MOBJ_GrayTreasureBox") == 0 || strcmp(entry->animName, "MOBJ_BlackTreasureBox") == 0) {
        if (entry->unk1DC == 99)
            return 2;
        else
            return 1;
    } else {
        return 0;
    }
}

// Unreferenced in the original, but its pool slot shifts the offsets mobjNearDistCheck2 reads.
#pragma force_active on
const Vec vec3_802c3d28 = { 0.0f, 0.0f, 0.0f };
#pragma force_active reset

static int mobjCheckKururingFloorItem(MapObjectEntry* entry);

MapObjectEntry* mobjNearDistCheck2(f32 x, f32 y, f32 z, f32 minMagnitude, char** names) {
    int entryCount;
    Vec pos;
    Vec dist;
    Vec _pos;
    MapObjectEntry* entry;
    MapObjectWork* wp = mobjGetWork();
    MapObjectEntry* ret;
    int i;
    float mag;
    char** animName;

    entryCount = wp->count;
    entry = wp->entries;
    ret = NULL;

    _pos = (Vec){ 0.0f, 0.0f, 0.0f };
    _pos.x = x;
    _pos.y = y;
    _pos.z = z;
    pos = _pos;

    for (i = 0; i < entryCount; i++, entry++) {
        if ((entry->flags & 1) == 0)
            continue;

        animName = names;
        while (*animName != NULL) {
            if (strcmp(entry->animName, *animName) == 0) {
                if (strcmp(entry->animName, "MOBJ_TreasureBox") == 0 ||
                    strcmp(entry->animName, "MOBJ_BigTreasureBox") == 0 ||
                    strcmp(entry->animName, "MOBJ_GrayTreasureBox") == 0 ||
                    strcmp(entry->animName, "MOBJ_BlackTreasureBox") == 0) {
                    if (mobjCheckItemboxOpen(entry) != 1) {
                        animName++;
                        continue;
                    }
                } else if (strcmp(entry->animName, "MOBJ_KururinFloor") == 0) {
                    if (mobjCheckKururingFloorItem(entry) != 2) {
                        animName++;
                        continue;
                    }
                }

                VECSubtract(&entry->position, &pos, &dist);
                mag = VECMag(&dist);
                if (mag < minMagnitude) {
                    minMagnitude = mag;
                    ret = entry;
                    break;
                }
                break;
            }
            animName++;
        }
    }

    return ret;
}

static int mobjCheckKururingFloorItem(MapObjectEntry* entry) { // was always inlined in retail but not in demo
    if (!strcmp(entry->animName, "MOBJ_KururinFloor")) {
        ItemEntry* item = itemNameToPtr(entry->name);
        if ((item != NULL && (item->flags & 1) && evtGetValue(0, entry->unk1E4) == 0))
            return 1;
        else if ((evtGetValue(0, entry->unk1E4) != 0))
            return 1;
        else
            return 2;
    } else {
        return 0;
    }
}

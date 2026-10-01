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

// NONMATCHING (0.10%): empty placeholder, not decompiled yet.
void mobjMain(void) {
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

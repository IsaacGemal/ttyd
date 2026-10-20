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

extern u32 cubeDJ[26];
extern u32 headDJ[26];
extern u32 frontDJ[26];
extern u32 front2DJ[26];

#define MAX_MOBJ_WORK 3

extern GlobalWork* gp;

// .sdata2 pool constants this unit owns, defined after the code (real objects, in the original's address order).
extern const f32 float_0_8041a0c8;
extern const f32 float_10_8041a0cc;
extern const f32 float_1_8041a0d0;
extern const f32 float_neg1_8041a0d4;
extern const f32 float_neg0p5_8041a0d8;
extern const f32 float_deg2rad_8041a0dc;
extern const char str_8041A0E0[8];
extern const f32 float_2p5_8041a0e8;
extern const f32 float_5_8041a0ec;
extern const f32 float_25_8041a0f0;
extern const f32 float_20_8041a0f4;
extern const f32 float_0p1_8041a0f8;
extern const char str_8041A0FC[4];
extern const f32 float_0p5_8041a100;
extern const char str_8041A104[8];
extern const char str_8041A10C[8];

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
    MTXRotRad(xrot, 'x', (float_deg2rad_8041a0dc * (entry->rotation.x)));
    MTXRotRad(yrot, 'y', (float_deg2rad_8041a0dc * (entry->rotation.y)));
    MTXRotRad(zrot, 'z', (float_deg2rad_8041a0dc * (entry->rotation.z)));
    MTXConcat(trans, zrot, trans);
    MTXConcat(trans, yrot, trans);
    MTXConcat(trans, xrot, trans);
    MTXConcat(trans, scale, dest);
}

void mobjDispXLU(CameraId camId, void* param) {
    MapObjectEntry* entry = param; // cast to correct type

    if (entry->flags & MOBJ_FLAG_PAPER_TEX) {
        animSetPaperTexMtx(entry->paperTexMtx, NULL, NULL);
        animSetPaperTexObj(&entry->paperTexObj, NULL, NULL, entry->paperTex, NULL, NULL, 1, 1);
    }
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 float_10_8041a0cc * entry->scale.x, 1);
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 float_10_8041a0cc * entry->scale.x, 2);
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 float_10_8041a0cc * entry->scale.x, 3);
    if (entry->flags & MOBJ_FLAG_PAPER_TEX) {
        animSetPaperTexMtx(NULL, NULL, NULL);
        animSetPaperTexObj(NULL, NULL, NULL, NULL, NULL, NULL, 1, 1);
    }
}

void mobjDisp(CameraId camId, void* param) {
    MapObjectEntry* entry = param; // cast to correct type
    DispEntry* disp;

    if (entry->flags & MOBJ_FLAG_PAPER_TEX) {
        animSetPaperTexMtx(entry->paperTexMtx, NULL, NULL);
        animSetPaperTexObj(&entry->paperTexObj, NULL, NULL, entry->paperTex, NULL, NULL, 1, 1);
    }
    disp = dispGetCurWork();
    switch (disp->renderMode) {
        case 0:
            animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                         float_10_8041a0cc * entry->scale.x, 1);
            break;
        case 1:
            animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                         float_10_8041a0cc * entry->scale.x, 2);
            break;
        case 2:
        default:
            animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                         float_10_8041a0cc * entry->scale.x, 3);
            break;
    }
    if (entry->flags & MOBJ_FLAG_PAPER_TEX) {
        animSetPaperTexMtx(NULL, NULL, NULL);
        animSetPaperTexObj(NULL, NULL, NULL, NULL, NULL, NULL, 1, 1);
    }
}

void mobjDisp_OffscreenXLU(CameraId camId, void* param) {
    MapObjectEntry* entry = param; // cast to correct type
    u16 left, top, right, bottom;

    sysWaitDrawSync();
    GXClearBoundingBox();
    if (entry->flags & MOBJ_FLAG_PAPER_TEX) {
        animSetPaperTexMtx(entry->paperTexMtx, NULL, NULL);
        animSetPaperTexObj(&entry->paperTexObj, NULL, NULL, entry->paperTex, NULL, NULL, 1, 1);
    }
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 float_10_8041a0cc * entry->scale.x, 1);
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 float_10_8041a0cc * entry->scale.x, 2);
    animPoseDraw(entry->poseId, entry->position.x, entry->position.y, entry->position.z, entry->rotation.y,
                 float_10_8041a0cc * entry->scale.x, 3);
    if (entry->flags & MOBJ_FLAG_PAPER_TEX) {
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
    PSVECScale(&entry->translation, &entry->translation, (float_0p5_8041a100));
    PSVECScale(&entry->translation, &entry->translation, float_10_8041a0cc);
    entry->translation.y = (float_0_8041a0c8);
#ifdef MUST_MATCH
    // MUST_MATCH: holding __fabs()'s double result in an f64 local (its natural return type) makes MWCC
    // colour bboxMax to f2 and scale to f1 as the target does; the direct form swaps the two.
    {
        f64 dx = __fabs(bboxMax.x - bboxMin.x);
        entry->scale2.x = (float_10_8041a0cc) * (entry->scale.x * (f32)dx);
    }
    entry->scale2.y = (float_10_8041a0cc) * (entry->scale.y * (f32)__fabs(bboxMax.y));
    {
        f64 dz = __fabs(bboxMax.z - bboxMin.z);
        entry->scale2.z = (float_10_8041a0cc) * (entry->scale.z * (f32)dz);
    }
#else
    entry->scale2.x = (float_10_8041a0cc) * (entry->scale.x * (f32)__fabs(bboxMax.x - bboxMin.x));
    entry->scale2.y = (float_10_8041a0cc) * (entry->scale.y * (f32)__fabs(bboxMax.y));
    entry->scale2.z = (float_10_8041a0cc) * (entry->scale.z * (f32)__fabs(bboxMax.z - bboxMin.z));
#endif

    if (strncmp(animData->fileName, str_8041A0E0, strlen(str_8041A0E0)) == 0) {
        entry->translation.z = (float_0_8041a0c8);
        entry->scale2.z = (float_10_8041a0cc) * ((float_2p5_8041a0e8) * entry->scale.z);
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
            MTXRotRad(xrot, 'x', ((float_deg2rad_8041a0dc) * entry->rotation.x));
            MTXRotRad(yrot, 'y', ((float_deg2rad_8041a0dc) * entry->rotation.y));
            MTXRotRad(zrot, 'z', ((float_deg2rad_8041a0dc) * entry->rotation.z));
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
            MTXRotRad(xrot, 'x', ((float_deg2rad_8041a0dc) * entry->rotation.x));
            MTXRotRad(yrot, 'y', ((float_deg2rad_8041a0dc) * entry->rotation.y));
            MTXRotRad(zrot, 'z', ((float_deg2rad_8041a0dc) * entry->rotation.z));
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
            MTXRotRad(xrot, 'x', ((float_deg2rad_8041a0dc) * entry->rotation.x));
            MTXRotRad(yrot, 'y', ((float_deg2rad_8041a0dc) * entry->rotation.y));
            MTXRotRad(zrot, 'z', ((float_deg2rad_8041a0dc) * entry->rotation.z));
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
        if ((entry->flags & MOBJ_FLAG_ACTIVE) && (strcmp(entry->name, name) == 0))
            break;
    }
    // SPM uses this to assert the name isn't duplicate, here it's just pointless

    for (idx = 0, entry = wp->entries; idx < entryCount; idx++, entry++) {
        if ((entry->flags & MOBJ_FLAG_ACTIVE) == 0)
            break;
    }

    memset(entry, 0, sizeof(*entry));
    entry->flags |= MOBJ_FLAG_ACTIVE;
    strcpy(entry->name, name);

    entry->position = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->scale = (Vec){ 1.0f, 1.0f, 1.0f };
    entry->rotation = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->velocity = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->gravity = float_1_8041a0d0;
    entry->bounce = float_1_8041a0d0;
    entry->camId = CAMERA_3D;
    entry->poseId = animPoseEntry(animPoseName, gp->inBattle != 0);
    animPosePeraOff(entry->poseId);
    strcpy(entry->animName, animPoseName);
    animPoseSetAnim(entry->poseId, str_8041A0FC, 1);
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
        if ((entry->flags & MOBJ_FLAG_ACTIVE) && strcmp(entry->name, name) == 0) {
            for (i = 0; i < 2; i++) {
                if ((entry->hitObj[i].name) != NULL) {
                    if (!gp->inBattle) {
                        marioResetHitObj(entry->hitObj[i].name);
                    }
                    hitDelete(entry->hitObj[i].name);
                }
            }
            entry->flags &= ~MOBJ_FLAG_ACTIVE;
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
        if ((entry->flags & MOBJ_FLAG_ACTIVE) && strcmp(entry->name, name) == 0) {
            break;
        }
    }

    entry->position.x = x;
    entry->position.y = float_0p1_8041a0f8 + y;
    entry->position.z = z;
}

// The two zero Vecs the bbox code reads. Declared non-const but forced into .rodata: MWCC's
// scheduler/register allocator reproduce the original's scale2.x codegen only for this form, and
// both mobjCalcMtx and the inlined mobjMainCalcMtx must reference these same objects so the unit
// does not emit two extra Vecs and shift the rest of .rodata by 0x18.
__declspec(section ".rodata") Vec vec3_802c3d10 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_802c3d1c = { 0.0f, 0.0f, 0.0f };

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

        minTmp = vec3_802c3d10;
        minTmp.x = animData->bboxMin.x;
        minTmp.y = animData->bboxMin.y;
        minTmp.z = animData->bboxMin.z;
        bboxMin = minTmp;
        maxTmp = vec3_802c3d1c;
        maxTmp.x = animData->bboxMax.x;
        maxTmp.y = animData->bboxMax.y;
        maxTmp.z = animData->bboxMax.z;
        bboxMax = maxTmp;

        entry->scale2.x = float_10_8041a0cc * (entry->scale.x * (f32)__fabs(bboxMax.x - bboxMin.x));
        entry->scale2.y = float_10_8041a0cc * (entry->scale.y * (f32)__fabs(bboxMax.y));
        entry->scale2.z = float_10_8041a0cc * (entry->scale.z * (f32)__fabs(bboxMax.z - bboxMin.z));

        if (strncmp(animData->fileName, str_8041A0E0, strlen(str_8041A0E0)) == 0) {
            entry->translation.z = float_0_8041a0c8;
            entry->scale2.z = float_10_8041a0cc * (float_2p5_8041a0e8 * entry->scale.z);
        }
    }

    MTXTrans(trans, position.x, position.y, position.z);
    MTXScale(scale, entry->scale2.x, entry->scale2.y, entry->scale2.z);
    MTXRotRad(xRot, 'x', (float_deg2rad_8041a0dc * (entry->rotation.x)));
    MTXRotRad(yRot, 'y', (float_deg2rad_8041a0dc * (entry->rotation.y)));
    MTXRotRad(zRot, 'z', (float_deg2rad_8041a0dc * (entry->rotation.z)));
    MTXConcat(trans, zRot, trans);
    MTXConcat(trans, yRot, trans);
    MTXConcat(trans, xRot, trans);
    MTXConcat(trans, scale, mtx);

    for (i = 0; i < 2; i++) {
        if (entry->hitObj[i].hitObject != 0)
            hitReCalcMatrix(entry->hitObj[i].hitObject, mtx);
    }
}

// mobjMain's inlined mobjMainCalcMtx and the out-of-line mobjCalcMtx must reference the SAME zero Vecs
// (vec3_802c3d10/vec3_802c3d1c, .rodata +0x48/+0x54). Written as two separate compound literals each, MWCC
// emits two extra Vecs and every later .rodata offset (the MOBJ_* strings and doubles) shifts by 0x18. Sharing
// the named globals fixes mobjMain, mobjCalcMtx, mobjNearDistCheck2 and mobjCheckItemboxOpen.
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
    // These five groups are declared here, in this order, so their stack slots land where the original's do
    // (the compiler assigns the lowest slot to the last-declared local).
    GXColor blend;
    GXColor blend2;
    GXColor color;
    f32 nz, ny, nx, dist, hitZ, hitY, hitX;
    Vec pos;

    wp = mobjGetWork();
    count = wp->count;
    entry = wp->entries;

    mario = marioGetPtr();
    party = partyGetPtr(marioGetPartyId());
    fbat = fbatGetPointer();
    mode = fbat->mode;
    v24 = NULL;
    v25 = 0;
    if (party != NULL && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        v25 = 1;
    }

    if (gp->inBattle == 0) {
        HitEntry** hp = (HitEntry**)wp;

        if (*(HitEntry**)((u8*)mario + 0x1FC) != NULL) {
            if (hp[2] == NULL ||
                strcmp(hitGetName(hp[2]), hitGetName(*(HitEntry**)((u8*)mario + 0x1FC))) != 0) {
                hp[2] = *(HitEntry**)((u8*)mario + 0x1FC);
            }
        } else {
            hp[2] = NULL;
        }

        if (*(HitEntry**)((u8*)mario + 0x1F4) != NULL) {
            if (hp[3] == NULL ||
                strcmp(hitGetName(hp[3]), hitGetName(*(HitEntry**)((u8*)mario + 0x1F4))) != 0) {
                HitEntry* tmp = *(HitEntry**)((u8*)mario + 0x1F4);

                hp[3] = tmp;
                v24 = tmp;
            }
        } else {
            hp[3] = NULL;
        }

        if (mario->hitobjStandOn != NULL) {
            if (hp[4] == NULL ||
                strcmp(hitGetName(hp[4]), hitGetName(mario->hitobjStandOn)) != 0) {
                hp[4] = mario->hitobjStandOn;
            }
        } else {
            hp[4] = NULL;
        }

        if (mario->hitobjFront != NULL) {
            if (hp[5] == NULL ||
                strcmp(hitGetName(hp[5]), hitGetName(mario->hitobjFront)) != 0) {
                hp[5] = mario->hitobjFront;
            }
        } else {
            hp[5] = NULL;
        }

        if (party != NULL && party->frontHit != NULL) {
            if (hp[6] == NULL ||
                strcmp(hitGetName(hp[6]), hitGetName(party->frontHit)) != 0) {
                hp[6] = party->frontHit;
            }
        } else {
            hp[6] = NULL;
        }
    }

    for (i = 0; i < count; i++, entry++) {
        if (!(entry->flags & MOBJ_FLAG_ACTIVE))
            continue;

#ifdef MUST_MATCH
        // MUST_MATCH: the original tests gp->inBattle a second time under the short-circuit; the compiler
        // then emits the unreachable second conditional branch (to the non-body path) the target has.
        if (gp->inBattle != 0 || (gp->inBattle == 0 && (mode <= 1u || mode == 8))) {
#else
        if (gp->inBattle != 0 || mode <= 1u || mode == 8) {
#endif
            s32 v22;
            HitEntry* hitObj;

            if (entry->flags & MOBJ_FLAG_RECALC_MTX) {
                if (mobjCheckItemboxOpen(entry) == 2) {
                    mobjMainCalcMtx(entry, mtx1, FALSE);
                } else {
                    mobjMainCalcMtx(entry, mtx2, TRUE);
                }
                entry->flags &= ~MOBJ_FLAG_RECALC_MTX;
            }

            if (entry->flags & MOBJ_FLAG_GRAVITY) {
                if (entry->velocity.y >= float_0_8041a0c8) {
                    entry->position.y += entry->velocity.y;
                    entry->velocity.y -= entry->gravity;
                } else {
                    dist = float_10_8041a0cc + (float_1_8041a0d0 + (f32)__fabs(entry->velocity.y));
                    if (hitCheckFilter(NULL, entry->position.x, float_10_8041a0cc + entry->position.y, entry->position.z, float_0_8041a0c8, float_neg1_8041a0d4, float_0_8041a0c8, &hitX, &hitY,
                                       &hitZ, &dist, &nx, &ny, &nz) != NULL) {
                        entry->position.y = hitY;
                        { f32 tmpk = float_neg0p5_8041a0d8 * entry->bounce; entry->velocity.y = tmpk * entry->velocity.y; }
                        if (__fabs(entry->velocity.y) <= 2.0) {
                            entry->velocity.y = float_0_8041a0c8;
                        }
                    } else {
                        entry->position.y += entry->velocity.y;
                        entry->velocity.y -= entry->gravity;
                    }
                }
                mobjMainCalcMtx(entry, mtx3, TRUE);
            }

            if (!(entry->flags & MOBJ_FLAG_TRIGGERED)) {
                v22 = 0;
                hitObj = entry->hitObj[0].hitObject;
                if (mario->characterId != 2) {
                    if ((entry->flags & 0x10000) && *(void**)((u8*)mario + 0x1FC) == (void*)hitObj) {
                        if (marioGetPtr()->position.y <= float_5_8041a0ec + entry->position.y) {
                            v22 = 1;
                            entry->triggerFlag = 0x10000;
                            goto apply;
                        }
                    }
                    if ((entry->flags & MOBJ_FLAG_TRIGGER_HEAD) && v24 == (void*)hitObj) {
                        v22 = 1;
                        entry->triggerFlag = MOBJ_FLAG_TRIGGER_HEAD;
                        goto apply;
                    }
                    if ((entry->flags & MOBJ_FLAG_TRIGGER_STAND_ON) && mario->hitobjStandOn == hitObj) {
                        v22 = 1;
                        entry->triggerFlag = MOBJ_FLAG_TRIGGER_STAND_ON;
                        goto apply;
                    }
                    if ((entry->flags & MOBJ_FLAG_TRIGGER_HIP_DROP) && mario->hitobjStandOn == hitObj &&
                        marioChkHipBump() != 0) {
                        v22 = 1;
                        entry->triggerFlag = MOBJ_FLAG_TRIGGER_HIP_DROP;
                        goto apply;
                    }
                    if (unk_8005bb80() != 0 && (entry->flags & 0x40000) &&
                        *(void**)(mario->field_0x1DC + 4) == (void*)hitObj && (mario->buttonNew & 0x100) &&
                        v25 == 0) {
                        v22 = 1;
                        entry->triggerFlag = 0x40000;
                        goto apply;
                    }
                    if ((entry->flags & MOBJ_FLAG_TRIGGER_PARTY) && party != NULL) {
                        if (party->frontHit == hitObj) {
                            v22 = 1;
                            entry->triggerFlag = MOBJ_FLAG_TRIGGER_PARTY;
                            goto apply;
                        }
                        if (party->frontHit != NULL && party->currentMemberId == PARTY_MEMBER_BOBBERY &&
                            PSVECDistance(&party->position, &entry->position) < entry->hitObj[0].hitObject->radius + party->radius) {
                            v22 = 1;
                            entry->triggerFlag = MOBJ_FLAG_TRIGGER_PARTY;
                            goto apply;
                        }
                    }
                    if ((entry->flags & MOBJ_FLAG_TRIGGER_BOBBERY) && party != NULL && party->currentMemberId == PARTY_MEMBER_BOBBERY) {
                        if (party->frontHit == hitObj) {
                            v22 = 1;
                            entry->triggerFlag = MOBJ_FLAG_TRIGGER_BOBBERY;
                            goto apply;
                        }
                        if (party->frontHit != NULL &&
                            PSVECDistance(&party->position, &entry->position) < entry->hitObj[0].hitObject->radius + party->radius) {
                            v22 = 1;
                            entry->triggerFlag = MOBJ_FLAG_TRIGGER_PARTY;
                            goto apply;
                        }
                    }
                } else {
                    s32 j;

                    if (entry->flags & MOBJ_FLAG_TRIGGER_KPA_HEAD) {
                        for (j = 0; j < 10; j++) {
                            if (kpaGetHitobjHead(j) == (void*)hitObj) {
                                v22 = 1;
                                entry->triggerFlag = MOBJ_FLAG_TRIGGER_KPA_HEAD;
                                break;
                            }
                        }
                    }
                    if (entry->flags & MOBJ_FLAG_TRIGGER_KPA_HIP) {
                        for (j = 0; j < 10 && v22 == 0; j++) {
                            if (kpaGetHitobjRide(j) == (void*)hitObj) {
                                if (kpaHipAttackCheck() != 0) {
                                    v22 = 1;
                                    entry->triggerFlag = MOBJ_FLAG_TRIGGER_KPA_HIP;
                                    break;
                                }
                            }
                        }
                    }
                    if ((entry->flags & MOBJ_FLAG_TRIGGER_KPA_LV3) && kpaGetHitobj() == (void*)hitObj && kpaGetLevel() == 3) {
                        v22 = 1;
                        entry->triggerFlag = MOBJ_FLAG_TRIGGER_KPA_LV3;
                        goto apply;
                    }
                    if ((entry->flags & MOBJ_FLAG_TRIGGER_KPA_FLAME) && kpaGetHitFlame() == (void*)hitObj) {
                        v22 = 1;
                        entry->triggerFlag = MOBJ_FLAG_TRIGGER_KPA_FLAME;
                        goto apply;
                    }
                    if ((entry->flags & 0x80000) && mario->hitobjFront == (void*)hitObj) {
                        v22 = 1;
                        entry->triggerFlag = 0x80000;
                        goto apply;
                    }
                    if ((entry->flags & MOBJ_FLAG_TRIGGER_STAND_ON) && mario->hitobjStandOn == hitObj) {
                        v22 = 1;
                        entry->triggerFlag = MOBJ_FLAG_TRIGGER_STAND_ON;
                    }
                }
            apply:
                if (v22 != 0) {
                    entry->flags |= MOBJ_FLAG_TRIGGERED;
                }
            }

            if (entry->flags & MOBJ_FLAG_RETRIGGER) {
                entry->flags &= ~MOBJ_FLAG_TRIGGERED;
            }
            if ((u32)marioStGetSystemLevel() != 0) {
                entry->flags &= ~MOBJ_FLAG_TRIGGERED;
            }
            {
                MarioWork* m = marioGetPtr();

                if (m->currMotionId == 0x1F || m->currMotionId == 0x20) {
                    entry->flags &= ~MOBJ_FLAG_TRIGGERED;
                }
            }
            if (entry->update != NULL) {
                if (entry->update(entry) == -1)
                    continue;
            }
        }

        if (!(entry->flags & MOBJ_FLAG_ACTIVE))
            continue;

        if (koopaRunFlag == 0 && (entry->flags & 0x802) == 0) {
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
            { f32 sz = float_10_8041a0cc * entry->scale.x; shadow = shadowEntry(entry->position.x, entry->position.y, entry->position.z, sz * entry->scale2.x / float_25_8041a0f0); }
            if (entry->flags & MOBJ_FLAG_TRIGGER_HEAD) {
                shadowSetType(shadow, 1);
                { f32 ts = float_10_8041a0cc * entry->scale.x; shadowSetSize(shadow, ts * entry->scale2.x / float_20_8041a0f4); }
            }
        noShadow:;
        }

        animPoseMain(entry->poseId);

        {
            *(u32*)&color = animPoseGetMaterialEvtColor(entry->poseId);

            if (entry->flags & MOBJ_FLAG_NO_DISP)
                continue;
            if ((entry->flags & MOBJ_FLAG_NO_CULL) == 0) {
                PSMTXMultVec(camGetPtr(CAMERA_3D)->view, &entry->position, &pos);
                PSMTX44MultVec(camGetPtr(CAMERA_3D)->projection, &pos, &pos);
                if (__fabs(pos.x) > 1.5)
                    continue;
                if (__fabs(pos.y) > 1.5)
                    continue;
                if (__fabs(pos.z) > 1.0)
                    continue;
            }
            if ((entry->flags & MOBJ_FLAG_BLINK) && ((gp->retraceCount >> 3) & 1))
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
        if ((entry->flags & MOBJ_FLAG_ACTIVE) && strcmp(entry->name, name) == 0) {
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
        if ((entry->flags & MOBJ_FLAG_ACTIVE) && strcmp(entry->name, name) == 0) {
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

    minTmp = vec3_802c3d10;
    minTmp.x = animData->bboxMin.x;
    minTmp.y = animData->bboxMin.y;
    minTmp.z = animData->bboxMin.z;
    bboxMin = minTmp;
    maxTmp = vec3_802c3d1c;
    maxTmp.x = animData->bboxMax.x;
    maxTmp.y = animData->bboxMax.y;
    maxTmp.z = animData->bboxMax.z;
    bboxMax = maxTmp;

    entry->scale2.x = float_10_8041a0cc * (entry->scale.x * (f32)__fabs(bboxMax.x - bboxMin.x));
    entry->scale2.y = float_10_8041a0cc * (entry->scale.y * (f32)__fabs(bboxMax.y));
    entry->scale2.z = float_10_8041a0cc * (entry->scale.z * (f32)__fabs(bboxMax.z - bboxMin.z));

    if (strncmp(animData->fileName, str_8041A0E0, strlen(str_8041A0E0)) == 0) {
        entry->translation.z = float_0_8041a0c8;
        entry->scale2.z = float_10_8041a0cc * (float_2p5_8041a0e8 * entry->scale.z);
    }

    MTXTrans(trans, position.x, position.y, position.z);
    MTXScale(scale, entry->scale2.x, entry->scale2.y, entry->scale2.z);
    MTXRotRad(xRot, 'x', (float_deg2rad_8041a0dc * (entry->rotation.x)));
    MTXRotRad(yRot, 'y', (float_deg2rad_8041a0dc * (entry->rotation.y)));
    MTXRotRad(zRot, 'z', (float_deg2rad_8041a0dc * (entry->rotation.z)));
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
    MTXRotRad(xRot, 'x', (float_deg2rad_8041a0dc * (entry->rotation.x)));
    MTXRotRad(yRot, 'y', (float_deg2rad_8041a0dc * (entry->rotation.y)));
    MTXRotRad(zRot, 'z', (float_deg2rad_8041a0dc * (entry->rotation.z)));
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
        if (entry->state == 99)
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
        if ((entry->flags & MOBJ_FLAG_ACTIVE) == 0)
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
        if ((item != NULL && (item->flags & 1) && evtGetValue(0, entry->flagVar) == 0))
            return 1;
        else if ((evtGetValue(0, entry->flagVar) != 0))
            return 1;
        else
            return 2;
    } else {
        return 0;
    }
}

#pragma force_active on
// ---- .sdata2 prefix (named, in original address order) ----
__declspec(section ".sdata2") const f32 float_0_8041a0c8 = 0.0f;
__declspec(section ".sdata2") const f32 float_10_8041a0cc = 10.0f;
__declspec(section ".sdata2") const f32 float_1_8041a0d0 = 1.0f;
__declspec(section ".sdata2") const f32 float_neg1_8041a0d4 = -1.0f;
__declspec(section ".sdata2") const f32 float_neg0p5_8041a0d8 = -0.5f;
__declspec(section ".sdata2") const f32 float_deg2rad_8041a0dc = 0.017453292f;
__declspec(section ".sdata2") const char str_8041A0E0[8] = "HOBJ_";
__declspec(section ".sdata2") const f32 float_2p5_8041a0e8 = 2.5f;
__declspec(section ".sdata2") const f32 float_5_8041a0ec = 5.0f;
__declspec(section ".sdata2") const f32 float_25_8041a0f0 = 25.0f;
__declspec(section ".sdata2") const f32 float_20_8041a0f4 = 20.0f;
__declspec(section ".sdata2") const f32 float_0p1_8041a0f8 = 0.1f;
__declspec(section ".sdata2") const char str_8041A0FC[4] = "S_1";
__declspec(section ".sdata2") const f32 float_0p5_8041a100 = 0.5f;
__declspec(section ".sdata2") const char str_8041A104[8] = "MOBJ_";
__declspec(section ".sdata2") const char str_8041A10C[8] = "MOBJ~";
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") u32 gap_09_8041A114_sdata2[1] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

// ---- .sdata ----
__declspec(section ".sdata") u32 cubeMSJ[5] = {
    0x00010000, 0x00000000, 0x80000000, 0x00000000, 0x00000000
};

__declspec(section ".sdata") u32 front2MSJ[5] = {
    0x00010000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};


// ---- .data (after negone_one_374 above) ----
__declspec(section ".data") u32 lbl_803083C8[2] = {
    0xBF800000, 0x3F800000
};

static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000
};

__declspec(section ".data") u32 lbl_803083D8[2] = {
    0x3F800000, 0xBF800000
};

static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000
};

__declspec(section ".data") u32 lbl_803083E8[2] = {
    0xBF800000, 0x3F800000
};

static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000
};

__declspec(section ".data") u32 lbl_803083F8[2] = {
    0x3F800000, 0xBF800000
};

u32 cubeDVP[103] = {
    0x00000008, 0xBF000000, 0x00000000, 0x3F000000, 0x3F000000, 0x00000000, 0x3F000000, 0xBF000000,
    0x3F800000, 0x3F000000, 0x3F000000, 0x3F800000, 0x3F000000, 0xBF000000, 0x3F800000, 0xBF000000,
    0x3F000000, 0x3F800000, 0xBF000000, 0xBF000000, 0x00000000, 0xBF000000, 0x3F000000, 0x00000000,
    0xBF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

u32 cubeDVN[55] = {
    0x00000006, 0x00000000, 0x00000000, 0x3F800000, 0x00000000, 0x3F800000, 0x00000000, 0x00000000,
    0x00000000, 0xBF800000, 0x00000000, 0xBF800000, 0x00000000, 0x3F800000, 0x00000000, 0x00000000,
    0xBF800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

u32 cubeDDT[26] = {
    (u32)(void*)cubeDVP, (u32)(void*)cubeDVN, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000
};

u32 cubeDN[1230] = {
    0x00000004, 0x00000005, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00060005,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00020005, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00040005, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000004, 0x00070004, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00010004, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0x00050004, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0x00030004, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000004, 0x00070003, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0x00060003, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00010003,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000003, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000004,
    0x00050002, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00040002, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00070002, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0x00060002, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000004, 0x00030001, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00020001, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0x00050001, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00040001,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000004, 0x00010000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00030000, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00020000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

u32 cubeDP[19] = {
    0x00000000, 0x00000006, 0x00000000, (u32)(void*)cubeDDT, (u32)(void*)cubeDN, (u32)((u8*)cubeDN +820), (u32)((u8*)cubeDN +1640), (u32)((u8*)cubeDN +2460),
    (u32)((u8*)cubeDN +3280), (u32)((u8*)cubeDN +4100), 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000
};

u32 cubeDJ[26] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xBF000000,
    0x00000000, 0xBF000000, 0x3F000000, 0x3F800000, 0x3F000000, 0x00000000, (u32)(void*)cubeMSJ, 0x00000001,
    0x00000000, (u32)(void*)cubeDP
};

u32 headDP[19] = {
    0x00000000, 0x00000001, 0x00000000, (u32)(void*)cubeDDT, (u32)((u8*)cubeDN +1640), 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000
};

u32 headDJ[26] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xBF000000,
    0x00000000, 0xBF000000, 0x3F000000, 0x00000000, 0x3F000000, 0x00000000, (u32)(void*)cubeMSJ, 0x00000001,
    0x00000000, (u32)(void*)headDP
};

u32 frontDP[19] = {
    0x00000000, 0x00000001, 0x00000000, (u32)(void*)cubeDDT, (u32)((u8*)cubeDN +4100), 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000
};

u32 frontDJ[26] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xBF000000,
    0x00000000, 0xBF000000, 0x3F000000, 0x3F800000, 0x3F000000, 0x00000000, (u32)(void*)cubeMSJ, 0x00000001,
    0x00000000, (u32)(void*)frontDP
};

u32 front2DP[19] = {
    0x00000000, 0x00000005, 0x00000000, (u32)(void*)cubeDDT, (u32)(void*)cubeDN, (u32)((u8*)cubeDN +820), (u32)((u8*)cubeDN +1640), (u32)((u8*)cubeDN +2460),
    (u32)((u8*)cubeDN +3280), 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000
};

u32 front2DJ[26] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xBF000000,
    0x00000000, 0xBF000000, 0x3F000000, 0x3F800000, 0x3F000000, 0x00000000, (u32)(void*)front2MSJ, 0x00000001,
    0x00000000, (u32)(void*)front2DP
};
#pragma force_active reset

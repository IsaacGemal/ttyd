/*
 * Warning, inlining hell
 */
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <math.h>

#include <stdio.h>

#include <string.h>

#define TESTHEAP_SIZE (u32)(1.5 * 1024 * 1024)
#define PI 3.14159265358979323846264338327950288419716939937510f

extern GlobalWork* gp;
extern int sprintf(char* str, const char* format, ...);
extern double tan(double arg);

// The header's AnimationPoseData padding at 0x1A0 is 4 bytes short: the original
// binary has groups at 0x1A8 and mpAnims at 0x1AC. Work around it locally.
#define ANIM_POSE_DATA_GROUPS(data) (*(AnimPoseGroup**)((u8*)(data) + 0x1A8))
#define ANIM_POSE_DATA_ANIMS(data) (*(AnimTableEntry**)((u8*)(data) + 0x1AC))

//.data
u8 _vivihimoData[0xC00]; // TODO: embed the actual data? gets overwritten by a/vivian.bin in animInit()

//.bss
static AnimationWork work;
f32 tanfTbl[0xB2];

//.sdata
s32 vivihimoTailGroupNo[4] = { 0, 2, -1, -1 };
void* vivihimoData[4] = {
    NULL,
    &_vivihimoData,
    &_vivihimoData + 0x400,
    &_vivihimoData + 0x800,
};
static AnimationWork* wp = &work;
s32 _dummy = -1;
static s32 tbl[4] = { 2, 1, 3, 0 };

//.sbss
u32 anim_ext_tex_size;
Mtx mtx; // TODO: make sure it's put in .sbss
s32 g_modeling_mtx_lv;
Mtx* g_modeling_mtx;
Mtx g_modeling_mtx_stack[32];
extern Vec vec3_802c2dd4[];
u32 zero_80419a50 = 0;

void pushGXModelMtx_JointNode__(void* node, f32 x, f32 y, f32 z);
void pushGXModelMtx_TransformNode__(void* node);

// local declarations
void animPoseRefresh(void);
void animPaperPoseDisp(CameraId camId, void* param);
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);
void animPaperPoseDispSub(s32 unused, AnimationPose* pose);

AnimationWork* animGetPtr(void) {
    return wp;
}

OSTime animTimeGetTime(BOOL globalTime) {
    if (globalTime == FALSE) {
        return OSTicksToMilliseconds(gp->renderFieldTime);
    }
    return OSTicksToMilliseconds(gp->renderTime);
}

void initTestHeap(void) {
    if (!wp->testHeap) { // allocate 1.5MiB test heap
        wp->testHeap = __memAlloc(HEAP_DEFAULT, TESTHEAP_SIZE);
    }
    wp->testAlloc = (u32)wp->testHeap;
}

// heavily inlined
void* testAlloc(u32 size) {
    void* alloc = (void*)wp->testAlloc;

    if (size & 31) {
        size += 32 - (size & 31);
    }
    wp->testAlloc += size;
    if ((u32)wp->testAlloc >= (u32)wp->testHeap + TESTHEAP_SIZE) { // need to allocate more
        animPoseRefresh();
        alloc = testAlloc(size);
    }
    return alloc;
}

void animInit(void) {
    int i;
    FileEntry* file;

    wp->totalFiles = 64; // TODO: #define these?
    wp->totalTextures = 64;
    wp->totalPoses = 256;
    wp->mFloatScratchRP = 1024;
    wp->mFloatScratchWP = 0;
    wp->files = __memAlloc(HEAP_DEFAULT, sizeof(AnimationPoseFile) * wp->totalFiles);
    wp->textures = __memAlloc(HEAP_DEFAULT, sizeof(AnimationTextureFile) * wp->totalTextures);
    wp->poses = __memAlloc(HEAP_DEFAULT, sizeof(AnimationPose) * wp->totalPoses);
    wp->mbUseFloatScratch = 0;
    wp->mpFloatScratch = __memAlloc(HEAP_DEFAULT, sizeof(Vec) * wp->mFloatScratchRP);

    wp->mpTexObjs[0] = NULL;
    wp->mpTexObjs[1] = NULL;
    wp->mpCurTexObj = NULL;

    wp->mpTexDatas[0] = NULL;
    wp->mpTexDatas[1] = NULL;
    wp->mpCurTexData = NULL;

    wp->paperMtxPtr[0] = NULL;
    wp->paperMtxPtr[1] = NULL;
    wp->paperMtxPtr[2] = NULL;

    wp->field_0xD8 = 1;
    wp->field_0xDC = 1;

    wp->testHeap = NULL;
    wp->inBattle = FALSE;
    initTestHeap(); // inlined

    for (i = 0; i < 0xB2; i++) {
        tanfTbl[i] = (f32)tan(MTXDegToRad(i - 0x59));
    }
    file = fileAllocf(0, "a/vivian.bin");
    if (file) {
        memcpy_as4(vivihimoData[1], *file->data, sizeof(_vivihimoData));
        memcpy_as4(vivihimoTailGroupNo, (void*)((u32)*file->data + sizeof(_vivihimoData)), sizeof(vivihimoTailGroupNo));
        fileFree(file);
    }
    fileSetCurrentArchiveType(0);
    wp->mp_ag2tg = fileAllocf(0, "a/ag2tg.bin");
}

void animMain(void) {
    dispEntry(CAMERA_OFFSCREEN, 0.0f, 1, animPaperPoseDisp, NULL);
}

inline AnimationPoseData* animPoseGetBaseData(AnimationPose* pose) {
    return *wp->files[pose->fileId].handle->data;
}

#define animPoseGetBaseData(pose) (*(AnimationPoseData**)(wp->files[(pose)->fileId].handle->data))

// heavily inlined, TODO helper function for that AnimPoseData* data, copy of animPoseGetAnimBaseDataPtr with AnimPose*
// arg
void animPose_AllocBuffer(AnimationPose* pose) {
    pose->mpBufferVtxPos = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferPosNum);
    pose->mpVtxArrayPos = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferPosNum);

    pose->mpBufferVtxNrm = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferNrmNum);
    pose->mpVtxArrayNrm = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferNrmNum);

    pose->mpBufferGroupVisibility = testAlloc(sizeof(u8) * animPoseGetBaseData(pose)->mBufferVisibilityNum);
    pose->mpGroupVisibility = pose->mpBufferGroupVisibility;

    pose->mpBufferNode = testAlloc(sizeof(AnimPoseNode*) * animPoseGetBaseData(pose)->mBufferNodeNum);
    pose->mpNodes = testAlloc(sizeof(AnimPoseNode*) * animPoseGetBaseData(pose)->mBufferNodeNum);

    pose->mpBufferTexAnimEntries = testAlloc(sizeof(AnimTexMtx) * animPoseGetBaseData(pose)->mBufferTexAnimNum);
    pose->mpTexAnimEntries = pose->mpBufferTexAnimEntries;

    pose->lastFrameZero = -1;
}

// heavily inlined
void animPoseRefresh(void) {
    int i;

    initTestHeap(); // inline
    for (i = 0; i < wp->totalPoses; i++) {
        AnimationPose* pose = &wp->poses[i]; // inline
        if (pose->flags) {
            // TODO: cleanup in rewrite?
            if ((!wp->inBattle || pose->type != 0) && (wp->inBattle || pose->type != 1)) {
                animPose_AllocBuffer(pose);
                pose->lastFrameZero = -1; // TODO: redundant?
            }
        }
    }
}

void animPoseBattleInit(void) {
    wp->inBattle = TRUE;
    animPoseRefresh();
}

// NONMATCHING (94.61%): 13 instructions differ, 2 missing, 7 extra, plus register differences.
s32 animPoseEntry(const char* animName, s32 type) {
    AnimationPose *pose, *temp;
    AnimationPoseFile *file, *tempfile;
    AnimationTextureFile *texture, *temptex;
    AnimationPoseData* data;
    s32 poseId, fileId, texId;
    s32 check, i;
    char textureName[256];

    switch (type) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }

    for (i = 0, poseId = 0; i < wp->totalPoses; i++, poseId++) {
        temp = &wp->poses[i];
        if (!temp->flags) {
            pose = temp;
            goto label_1;
        }
    }
    pose = NULL;
    poseId = -1;
label_1:
    pose->flags |= 1;
    pose->typeFlags = 0;
    pose->references = 0;

    check = wp->totalFiles;
    for (i = 0; i < check; i++) {
        file = &wp->files[i];
        if (!file->active) {
            data = *file->handle->data;
            if (!strcmp(data->fileName, animName)) {
                file->references++;
                goto label_5;
            }
        }
    }

    for (i = 0, fileId = 0; i < wp->totalFiles; i++, fileId++) {
        tempfile = &wp->files[i];
        if (!tempfile->active) {
            file = tempfile;
            goto label_2;
        }
    }
    file = NULL;
    fileId = -1;
label_2:
    file->active = TRUE;
    file->references++;
    file->handle = fileAllocf(5, "%s/%s", "a", animName);

    if (file->handle) {
        fileId = -2;
        file->active = FALSE;
        file->references--;
    } else {
        data = *file->handle->data;
        if (!data) {
            fileFree(file->handle);
            fileId = -2;
            file->active = FALSE;
            file->references--;
        } else {
            sprintf(textureName, "%s/%s-", "a", data->textureName);
            check = wp->totalTextures;
            for (i = 0; i < check; i++) {
                texture = &wp->textures[i];
                if ((u32)texture->active != 0) {
                    if (texture->handle) {
                        if (!strcmp(texture->handle->filename, textureName)) {
                            texture->references++;
                            goto label_4;
                        }
                    }
                }
            }

            for (i = 0, texId = 0; i < wp->totalTextures; i++, texId++) {
                temptex = &wp->textures[i];
                if ((u32)temptex->active == 0) {
                    texture = temptex;
                    goto label_3;
                }
            }
            texture = NULL;
            texId = -1;
        label_3:
            texture->active = TRUE;
            texture->references++;
            texture->handle = fileAlloc(textureName, 4);
        label_4:
            file->textureId = texId;
        }
    }
label_5:
    pose->fileId = fileId;
    if (pose->fileId == -2) {
        pose->flags = 0;
        return -2;
    }
    pose->animId = 0;
    pose->localTime = animTimeGetTime(type); // inline
    pose->lastFrameZero = -1;
    pose->lastFrameTime = 0.0f;
    pose->field_0x80 = 0;
    pose->loopTime = 0.0f;
    pose->field_0x70 = 0.0f;
    pose->rotationY = 0.0f;
    pose->field_0x7C = 0.0f;
    pose->type = type;
    pose->paperId = -1;
    animPose_AllocBuffer(pose);
    pose->gxCallback = NULL;
    pose->disableDraw = 0;
    pose->materialFlag = 0;
    pose->materialLightFlag = 0;
    pose->materialColor = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    pose->materialColor2 = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    pose->field_0xFC = 1.0f;
    pose->field_0xF8 = 1.0f;
    pose->field_0x100 = 0.0f;
    pose->localTimeRate = 1.0f;
    MTXIdentity(pose->matrix);
    if (!strcmp(animName, "tst_vivi")) {
        pose->vivianType = 1;
        pose->vivianGroupNo = &vivihimoTailGroupNo[0];
    } else if (!strcmp(animName, "c_vivian")) {
        pose->vivianType = 1;
        pose->vivianGroupNo = &vivihimoTailGroupNo[1];
    } else if (!strcmp(animName, "c_maririn")) {
        pose->vivianType = 2;
        pose->vivianGroupNo = &vivihimoTailGroupNo[2];
    } else if (!strcmp(animName, "c_majyorin")) {
        pose->vivianType = 3;
        pose->vivianGroupNo = &vivihimoTailGroupNo[3];
    } else { // if none match
        pose->vivianType = 0;
        pose->vivianGroupNo = &_dummy;
    }
    pose->dispCallback = NULL;
    pose->dispUserDataCallback = NULL;
    return poseId;
}

// NONMATCHING (76.80%): 5 instructions differ, 8 missing, 8 extra, plus register differences.
s32 animPaperPoseEntry(const char* animName, s32 group) {
    AnimationPose* pose;
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if (pose->flags & 1) {
            if (pose->typeFlags & 1 && pose->typeFlags & 2 && pose->type == group) {
                if (!strcmp(animPoseGetBaseData(pose)->fileName, animName)) {
                    break;
                }
            }
        }
    }
    if (i == wp->totalPoses) {
        i = -1;
    }
    if (i < 0) {
        i = animPoseEntry(animName, group);
        if (i != -2) {
            pose = &wp->poses[i];
            pose->typeFlags = 3;
            pose->references = 0;
            pose->flags |= 1;
        }
    } else {
        wp->poses[i].references++;
    }
    return i;
}

// NONMATCHING (95.13%): 3 instructions differ, 2 missing, plus register differences.
BOOL animEffectAsync(const char* animName, u32 group) {
    s32 fileId;
    s32 other;
    void** data;
    u8* entry;
    u8* result;
    s32 offset;
    s32 size;

    switch (group) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }
    fileId = fileAsyncf(5, NULL, "%s/%s", "a", animName);
    data = wp->mp_ag2tg->data;
    size = (s32)data[1];
    entry = (u8*)data[0];
    for (offset = 0; offset < size; offset += 0x20) {
        if (!strcmp((char*)entry, animName)) {
            result = entry + 0x20;
            goto found;
        }
        entry += 0x20;
    }
    result = (u8*)&zero_80419a50;
found:
    other = fileAsyncf(4, NULL, "%s/%s-", "a", result);

    if (fileId == -1) {
        return TRUE;
    }
    if ((u32)fileId == 0) {
        return FALSE;
    }
    if (other == -1) {
        return TRUE;
    }
    if ((u32)other == 0) {
        return FALSE;
    }
    return TRUE;
}

void animPosePeraOn(s32 poseId) {
    wp->poses[poseId].flags |= 8;
}

void animPosePeraOff(s32 poseId) {
    wp->poses[poseId].flags &= ~8;
}

void animPosePaperPeraOn(s32 poseId) {
    AnimationPose *src, *dst;

    src = animPoseGetAnimPosePtr(poseId);
    dst = &wp->poses[src->paperId];
    dst->flags |= 8;
    dst->field_0x70 = src->field_0x70;
    dst->rotationY = src->rotationY;
    dst->field_0x7C = src->field_0x7C;
}

void animPosePaperPeraOff(s32 poseId) {
    AnimationPose* src;

    src = animPoseGetAnimPosePtr(poseId);
    wp->poses[src->paperId].flags &= ~8;
}

void animPoseSetLocalTimeRate(s32 poseId, f32 localTimeRate) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->localTimeRate = localTimeRate;
}

// TODO: get animTimeGetTime to inline here
void animPoseSetLocalTime(s32 poseId, f32 localTimeFrames) {
    AnimationPose *pose, *pera;
    OSTime time;

    pose = &wp->poses[poseId];
    time = (localTimeFrames * 16.666666f);
    pose->localTime = animTimeGetTime(pose->type) - time;
    pose->mLocalTimeInit = pose->localTime;
    if (pose->paperId != -1) {
        pera = &wp->poses[pose->paperId];
        if (!(pera->typeFlags & 2)) {
            pera->localTime = pose->localTime;
        }
    }
}

void animPoseSetStartTime(s32 poseId, OSTime startTime) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->localTime = startTime;
}

// NONMATCHING (95.47%): 4 instructions differ, 3 missing, plus register differences.
void animPoseSetAnim(s32 poseId, const char* animName, BOOL reset) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = *wp->files[pose->fileId].handle->data;
    s32 top = data->mAnimCount;
    s32 animId = (s32)animName;

    if (animId != 0) {
        for (animId = 0; animId < top; animId++) {
            if (!strcmp(ANIM_POSE_DATA_ANIMS(data)[animId].name, animName)) {
                break;
            }
        }
        if (animId >= top) {
            animId = 0;
        }
    }
    if (!reset && pose->animId == animId) {
        return;
    }
    pose->animId = animId;
    if (pose->paperId != -1 && (pose->flags & 2)) {
        pose->flags |= 4;
    }
    pose->localTime = animTimeGetTime(pose->type);
    pose->lastFrameZero = -1;
    pose->lastFrameTime = 0.0f;
    pose->field_0x80 = 0;
    pose->loopTime = 0.0f;
}

s32 animPaperPoseGetId(const char* animName, s32 group) {
    AnimationPose* pose;
    AnimationPoseData* data;
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if ((pose->flags & 1) && (pose->typeFlags & 1) && (pose->typeFlags & 2) && pose->type == group) {
            data = *wp->files[pose->fileId].handle->data;
            if (!strcmp(data->fileName, animName)) {
                break;
            }
        }
    }
    if (i == wp->totalPoses) {
        i = -1;
    }
    return i;
}

// NONMATCHING (0.62%): empty placeholder, not decompiled yet.
void animPoseSetPaperAnimGroup(s32 poseId, const char* animName, BOOL someflag) {
}

// NONMATCHING (81.02%): 6 instructions differ, 14 missing, plus register differences.
void animPoseSetPaperAnim(s32 poseId, const char* animName) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = *wp->files[wp->poses[pose->paperId].fileId].handle->data;
    s32 top = data->mAnimCount;
    s32 animId = (s32)animName;

    if (animId != 0) {
        for (animId = 0; animId < top; animId++) {
            if (!strcmp(ANIM_POSE_DATA_ANIMS(data)[animId].name, animName)) {
                break;
            }
        }
        if (animId >= top) {
            animId = 0;
        }
    }
    *(s32*)((u8*)pose + 0x94) = animId;
    pose->mLocalTimeInit = animTimeGetTime(pose->type);
    if (pose->paperId != -1 && !(wp->poses[pose->paperId].typeFlags & 2)) {
        animPoseMain(pose->paperId);
    }
}

// NONMATCHING (0.63%): empty placeholder, not decompiled yet.
void animPoseSetEffect(s32 poseId, const char* animName, BOOL someflag) {
}

// NONMATCHING (95.10%): 5 instructions differ, 2 missing, 1 extra, plus register differences.
void animPoseSetEffectAnim(s32 poseId, const char* animName, BOOL reset) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data;
    s32 top;
    s32 i;

    if (pose->paperId == -1) {
        return;
    }
    pose = &wp->poses[pose->paperId];
    data = *wp->files[pose->fileId].handle->data;
    top = data->mAnimCount;
    if (animName != NULL) {
        for (i = 0; i < top; i++) {
            if (!strcmp(ANIM_POSE_DATA_ANIMS(data)[i].name, animName)) {
                break;
            }
        }
        if (i == top) {
            i = 0;
        }
    } else {
        i = 0;
    }
    if (!reset && pose->animId == i) {
        return;
    }
    pose->animId = i;
    if (pose->paperId != -1 && (pose->flags & 2)) {
        pose->flags |= 4;
    }
    pose->localTime = animTimeGetTime(pose->type);
    pose->lastFrameZero = -1;
    pose->lastFrameTime = 0.0f;
    pose->field_0x80 = 0;
    pose->loopTime = 0.0f;
}

void animPoseSetGXFunc(s32 poseId, void (*callback)(s32 wXluStage), BOOL disableDraw) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->gxCallback = callback;
    pose->disableDraw = disableDraw;
}

f32 animPoseGetRadius(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return (f32)*(s32*)((u8*)data + 0xC8) * 2.0f;
}

f32 animPoseGetHeight(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return (f32)*(s32*)((u8*)data + 0xCC) * 2.0f;
}

f32 animPoseGetLoopTimes(s32 id) {
    AnimationPose* pose = &wp->poses[id];
    return pose->loopTime;
}

// NONMATCHING (78.42%): 1 instruction differs, 2 missing, 2 extra, plus register differences.
BOOL animPoseGetPeraEnd(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];

    if (pose->flags & 8) {
        goto compute;
    }
    return TRUE;
compute:
    return fabs(pose->field_0x7C - pose->rotationY) < 10.0;
}

void animPoseSetMaterialFlagOn(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialFlag |= mask;
}

void animPoseSetMaterialFlagOff(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialFlag &= ~mask;
}

void animPoseSetMaterialLightFlagOn(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialLightFlag |= mask;
}

void animPoseSetMaterialLightFlagOff(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialLightFlag &= ~mask;
}

void animPoseSetMaterialEvtColor(s32 poseId, GXColor color) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialColor = color;
}

u32 animPoseGetMaterialFlag(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->materialFlag;
}

u32 animPoseGetMaterialLightFlag(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->materialLightFlag;
}

GXColor* animPoseGetMaterialEvtColor(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return (GXColor*)*(u32*)&pose->materialColor;
}

// NONMATCHING (0.06%): empty placeholder, not decompiled yet.
void animPoseMain(s32 poseId) {
}

// pushGXModelMtx_TransformNode__
// pushGXModelMtx_JointNode__

// NONMATCHING (0.28%): empty placeholder, not decompiled yet.
void animSetMaterial_Texture(s32 texCount, s32* texIdRemap, AnimPoseTexEntry* pTexEntries, AnimTexMtx* animEntries,
                             s32 texBindsCapacity, AnimTexBind* texBinds, AnimationTextureFile* texFile) {
}

inline BOOL animSetMaterial_ChangeTexture(AnimationPoseData* data, AnimPoseShapeDraw* draw, AnimationPose* pose,
                                          Mtx mtx, void** texData, GXTexObj* texObj, s32 texType) {
    AnimPoseTexEntry* texEntry;
    AnimTexMtx* texAnim;
    AnimTexBind* bind;
    s32 bindCount, texCount;
    s32 dataId, bindId, frameExt;
    GXTexObj* smartObj;
    int i;

    bind = data->texBinds;
    bindCount = data->texBindCount;
    texAnim = pose->mpTexAnimEntries;
    texEntry = data->texData;
    texCount = draw->texCount;

    for (i = 0; i < texCount; i++) {
        dataId = draw->texDataIds[texCount - i - 1];
        frameExt = texAnim[dataId].mTexBindIdx;
        bindId = texEntry[dataId].bindId + frameExt;
        if (bindId < 0 || bindId >= bindCount) {
            bindId = 0;
            //"Error: animSetMaterial_ChangeTexture: テクスチャがありません frameExt=%d\n"
            printf("Error: animSetMaterial_ChangeTexture: "
                   "\x83\x65\x83\x4E\x83\x58\x83\x60\x83\x83\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1 "
                   "frameExt=%d\n",
                   frameExt);
        }
        if (bind[bindId].texType == texType) {
            if (texObj) {
                smartObj = smartTexObj(texObj, texData);
                GXLoadTexObj(smartObj, i);
                if (mtx) {
                    GXLoadTexMtxImm(mtx, GX_TEXMTX0 + (u32)(i * 3), GX_MTX2x4);
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_TEXMTX0 + (u32)(i * 3));
                } else {
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_IDENTITY);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

// materialProc

// I'm so sorry, whoever gets tasked with matching this function
// note: I detect at least 8 inlined functions here, good luck finding the separation
// NONMATCHING (68.93%): 38 instructions differ, 63 missing, 159 extra, plus register differences.
void renderProc(s32 shapeId) {
    CameraEntry* camera;
    AnimationPoseData* data;
    AnimPoseShape *shapes, *shape;
    AnimPoseShapeDraw *draws, *draw;
    s32 dispMode, drawId;
    s32 posFirstVtx, nrmFirstVtx, clr0FirstVtx;

    f32* buffer;
    s32 nrmVtxSize;
    s32 texCount;
    AnimationPose* pose;
    Mtx* mtx;
    GXTexObj* texObj;
    BOOL texChange1, check1, texChange2, check2;
    GXTexObj* currTexObj;
    u8* texIds;
    s32* firstId;
    AnimDrawCallEntry* drawCall;
    s32 drawCallCount;
    GXColor* clr0VtxPtr;
    s32 posFirstId, nrmFirstId, clr0FirstId;
    Vec *nrmVtxPtr, *posVtxPtr;

    s32 count;
    u16 *posIndexPtr, *nrmIndexPtr, *clr0IndexPtr, *texIndexPtr;
    s32 firstDrawId;
    s32 index;
    s32 texIndex;

    int i, j, k, l;

    // stack variables
    s32 clr0VtxSize, posVtxSize; // TODO: make sure stack, +0xB0, +0xAC
    s32 drawCount;               // TODO: make sure stack, +0xA8
    Mtx camMtx, invCamMtx;
    f32* texVtxIndices[8];
    Vec lightPos, lightPos2; // TODO: rename? 2 is probably implicit stack copy

    if (shapeId != -1) {
        camera = camGetCurPtr();
        MTXConcat(camera->view, *g_modeling_mtx, camMtx);
        GXLoadPosMtxImm(camMtx, 0);
        MTXInvXpose(camMtx, invCamMtx);
        GXLoadNrmMtxImm(invCamMtx, 0);
        GXSetCurrentMtx(0);

        data = wp->mpCurPoseData;
        shapes = data->shapes;
        draws = data->draws;
        shape = &shapes[shapeId];
        dispMode = shape->dispMode;
        drawId = shape->shapeDrawStart;
        posFirstVtx = shape->vtxPos.first;
        nrmFirstVtx = shape->vtxNrm.first;
        clr0FirstVtx = shape->vtxClr0.first;
        drawCount = shape->shapeDrawCount;
        draw = &draws[drawId];

        if ((dispMode == 0 && wp->currDispMode == 2) || dispMode == wp->currDispMode ||
            wp->mpCurPose->materialColor.a != 0xFF || wp->mpCurPose->materialColor2.a != 0xFF) {
            GXSetCullMode(tbl[shape->cullMode]);
            data = wp->mpCurPoseData;
            posVtxSize = (s32)sizeof(Vec) * posFirstVtx;
            clr0VtxSize = (s32)sizeof(GXColor) * clr0FirstVtx;
            nrmVtxSize = nrmFirstVtx;
            buffer = data->texVtxBuffer;
            texVtxIndices[0] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[0].first];
            texVtxIndices[1] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[1].first];
            texVtxIndices[2] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[2].first];
            texVtxIndices[3] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[3].first];
            texVtxIndices[4] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[4].first];
            texVtxIndices[5] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[5].first];
            texVtxIndices[6] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[6].first];
            texVtxIndices[7] = &buffer[(sizeof(f32) * 2) * shape->vtxTex[7].first];

            for (i = 0; i < drawCount; i++, draw++) {
                texCount = draw->texCount;
                if (!wp->mpCurPose->disableDraw) {
                    lightPos = (Vec){ 0.0f, 0.0f, 0.0f };
                    MTXMultVec(*g_modeling_mtx, &lightPos, &lightPos);
                    lightPos2 = lightPos;
                    mapSetMaterialLight(wp->mpCurPose->materialLightFlag, lightPos2);
                    animSetMaterial_Texture(draw->texCount, draw->texDataIds, wp->mpCurPoseData->texData,
                                            wp->mpCurPose->mpTexAnimEntries, wp->mpCurPoseData->texBindCount,
                                            wp->mpCurPoseData->texBinds,
                                            &wp->textures[wp->files[wp->mpCurPose->fileId].textureId]);
                    mapSetMaterialLastStageBlend(wp->mpCurPose->materialFlag, wp->mpCurPose->materialColor,
                                                 wp->mpCurPose->materialColor2);
                    camera = camGetPtr(CAMERA_3D);
                    if (camGetCurPtr() == camera) {
                        mapSetMaterialFog();
                    }
                    mapSetMaterialTev(texCount, draw->drawMode, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                }
                pose = wp->mpCurPose;
                if (pose->typeFlags) {
                    texChange1 = animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, *wp->paperMtxPtr[0],
                                                               wp->mpTexDatas[0], wp->mpTexObjs[0], 1);
                    check1 = texChange1;
                    if (!texChange1) {
                        texChange2 = animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, *wp->paperMtxPtr[1],
                                                                   wp->mpTexDatas[1], wp->mpTexObjs[1], 2);
                        check1 = texChange2;
                        check2 = texChange2;
                    } else {
                        check2 = FALSE;
                    }

                    if (check1 && (texChange1 && !wp->field_0xD8 || check2 && !wp->field_0xDC) && !draw->drawMode) {
                        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
                    }
                    if (check1 && !draw->drawMode) {
                        currTexObj = wp->mpCurTexObj;
                        if (currTexObj) {
                            smartTexObj(currTexObj, wp->mpCurTexData);
                            if (wp->currDispMode != 3) {
                                GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
                                GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                                GXSetZCompLoc(GX_FALSE);
                                GXSetAlphaCompare(GX_GEQUAL, 0x80u, GX_AOP_OR, GX_NEVER, 0);
                            }
                            texObj = wp->mpTexObjs[texChange1 == 0];
                            if (texObj) {
                                GXLoadTexObj(texObj, GX_TEXMAP0);
                                GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE,
                                                  GX_PTIDENTITY);
                                GXLoadTexMtxImm(*wp->paperMtxPtr[texChange1 == 0], GX_TEXMTX0, GX_MTX2x4);
                            }
                            GXLoadTexObj(currTexObj, GX_TEXMAP1);
                            GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE,
                                              GX_PTIDENTITY);
                            GXLoadTexMtxImm(*wp->paperMtxPtr[2], GX_TEXMTX1, GX_MTX2x4);
                            mapSetMaterialTev(2, 12, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                        }
                    }
                }

                pose = wp->mpCurPose;
                if (pose->flags & 0x10) {
                    texObj = wp->mpTexObjs[0];
                    if (texObj) {
                        mtx = wp->paperMtxPtr[0];
                        if (mtx) {
                            animSetMaterial_ChangeTexture(wp->mpCurPoseData, draw, pose, *mtx, wp->mpTexDatas[0],
                                                          texObj, 1);
                        }
                    }
                }

                texIds = draw->texArrayIds;
                firstId = draw->firstIdTex;
                data = wp->mpCurPoseData; // inlined function here
                drawCall = &data->drawCalls[draw->firstDrawCall];
                texCount = draw->texCount;
                drawCallCount = draw->drawCallCount;
                clr0VtxPtr = &data->clr0VtxBuffer[clr0VtxSize / 4];
                posFirstId = draw->firstIdPos;
                nrmFirstId = draw->firstIdNrm;
                clr0FirstId = draw->firstIdClr0;

                if (wp->mbUseFloatScratch) {
                    nrmVtxPtr = &wp->vtxArrayNrm[nrmVtxSize];
                    posVtxPtr = &wp->vtxArrayPos[posVtxSize / sizeof(Vec)];
                } else {
                    pose = wp->mpCurPose;
                    posVtxPtr = &pose->mpVtxArrayPos[posVtxSize / sizeof(Vec)];
                    nrmVtxPtr = &pose->mpVtxArrayNrm[nrmVtxSize];
                }
                GXClearVtxDesc();
                GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
                GXSetArray(GX_VA_POS, posVtxPtr, sizeof(Vec));
                GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
                GXSetArray(GX_VA_NRM, nrmVtxPtr, sizeof(Vec));
                GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
                GXSetArray(GX_VA_CLR0, clr0VtxPtr, sizeof(GXColor));

                if (texCount == 1) { // just use texIndex[0]
                    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
                    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
                    GXSetArray(GX_VA_TEX0, texVtxIndices[texIds[0]], sizeof(f32) * 2);
                    for (j = 0; j < drawCallCount; j++, drawCall++) {
                        data = wp->mpCurPoseData; // doubly inlined function here
                        count = drawCall->count;
                        // indices are stored as u32, has to do math to load the lower 2 bytes
                        posIndexPtr = &data->posIndex[((drawCall->first + posFirstId) * 2) + 1];
                        nrmIndexPtr = &data->nrmIndex[((drawCall->first + nrmFirstId) * 2) + 1];
                        clr0IndexPtr = &data->clr0Index[((drawCall->first + clr0FirstId) * 2) + 1];
                        texIndexPtr = &data->texIndex[texIds[0]][((drawCall->first + firstId[texIds[0]]) * 2) + 1];
                        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, (u16)drawCall->count);
                        for (k = 0; k < count; k++) { // unrolled, warning
                            GXPosition1x16(*posIndexPtr);
                            posIndexPtr += 2;
                            GXNormal1x16(*nrmIndexPtr);
                            nrmIndexPtr += 2;
                            GXColor1x16(*clr0IndexPtr);
                            clr0IndexPtr += 2;
                            GXTexCoord1x16(*texIndexPtr);
                            texIndexPtr += 2;
                        }
                    }
                } else { // enumerate texIndex
                    for (j = 0; j < texCount; j++) {
                        GXSetVtxDesc(GX_VA_TEX0 + j, GX_INDEX16);
                        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + j, GX_TEX_ST, GX_F32, 0);
                        GXSetArray(GX_VA_TEX0 + j, texVtxIndices[texIds[texCount - j - 1]], sizeof(f32) * 2);
                    }
                    for (j = 0; j < drawCallCount; j++, drawCall++) {
                        firstDrawId = drawCall->first;
                        data = wp->mpCurPoseData; // doubly inlined function here
                        count = drawCall->count;
                        // indices are stored as u32, has to do math to load the lower 2 bytes
                        posIndexPtr = &data->posIndex[((drawCall->first + posFirstId) * 2) + 1];
                        nrmIndexPtr = &data->nrmIndex[((drawCall->first + nrmFirstId) * 2) + 1];
                        clr0IndexPtr = &data->clr0Index[((drawCall->first + clr0FirstId) * 2) + 1];

                        for (k = 0, texIndex = 0; k < count; k++) {
                            GXPosition1x16(*posIndexPtr);
                            GXNormal1x16(*nrmIndexPtr);
                            GXColor1x16(*clr0IndexPtr);
                            for (l = 0; l < texCount; l++) {
                                index = texIds[texCount - l - 1];
                                GXTexCoord1x16(wp->mpCurPoseData
                                                   ->texIndex[index][((firstDrawId + firstId[index]) * 2) + texIndex]);
                            }
                            texIndex += 2;
                            posIndexPtr += 2;
                            nrmIndexPtr += 2;
                            clr0IndexPtr += 2;
                        }
                    }
                }
            }
        }
    }
}

// dispProc

// NONMATCHING (32.81%): 18 missing, plus register differences.
void animPoseDraw(s32 poseId, f32 x, f32 y, f32 z, f32 rotY, f32 scale, s32 xluMode) {
    Mtx matrix;

    MTXTrans(matrix, x, y, z);
    animPoseDrawMtx(poseId, matrix, xluMode, rotY, scale);
}

// _animPoseDrawMtx

// NONMATCHING (0.21%): empty placeholder, not decompiled yet.
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale) {
}

void animSetPaperTexObj(GXTexObj* a1, GXTexObj* a2, GXTexObj* a3, void* a4, void* a5, void* a6, s32 a7, s32 a8) {
    wp->mpTexObjs[0] = a4 ? a1 : NULL;
    wp->mpTexObjs[1] = a5 ? a2 : NULL;
    wp->mpCurTexObj = a6 ? a3 : NULL;
    wp->mpTexDatas[0] = a4;
    wp->mpTexDatas[1] = a5;
    wp->mpCurTexData = a6;
    wp->field_0xD8 = a7;
    wp->field_0xDC = a7;
}

// NONMATCHING (99.36%): same instructions; only register numbers or operands differ.
void animPoseRelease(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationTextureFile* texture;
    AnimationPoseFile* file;

    if ((u32)pose->typeFlags != 0) {
        pose->references--;
        if (pose->references < 0) {
            file = &wp->files[pose->fileId];
            file->references--;
            if (file->references <= 0) {
                file->active = 0;
                texture = &wp->textures[file->textureId];
                texture->references--;
                if (texture->references <= 0) {
                    fileFree(texture->handle);
                    texture->active = 0;
                }
                fileFree(file->handle);
            }
            pose->flags = 0;
        }
    } else {
        if (pose->flags & 0x40) {
            smartFree(*(smartEntry**)((u8*)pose + 0x15C));
        }
        if (pose->flags & 0x80) {
            smartFree(*(smartEntry**)((u8*)pose + 0x164));
        }
        file = &wp->files[pose->fileId];
        file->references--;
        if (file->references <= 0) {
            file->active = 0;
            texture = &wp->textures[file->textureId];
            texture->references--;
            if (texture->references <= 0) {
                fileFree(texture->handle);
                texture->active = 0;
            }
            fileFree(file->handle);
        }
        pose->flags = 0;
    }
}

void animPaperPoseRelease(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationTextureFile* texture;
    AnimationPoseFile* file;

    pose->references--;
    if (pose->references < 0) {
        file = &wp->files[pose->fileId];
        file->references--;
        if (file->references <= 0) {
            file->active = 0;
            texture = &wp->textures[file->textureId];
            texture->references--;
            if (texture->references <= 0) {
                fileFree(texture->handle);
                texture->active = 0;
            }
            fileFree(file->handle);
        }
        pose->flags = 0;
    }
}

// animPoseAutoRelease
// NONMATCHING (99.43%): same instructions; only register numbers or operands differ.
void animPoseAutoRelease(s32 type) {
    AnimationPose* pose;
    AnimationTextureFile* texture;
    AnimationPoseFile* file;
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if ((pose->flags & 1) && pose->type == type) {
            if ((u32)pose->typeFlags != 0) {
                pose->references--;
                if (pose->references < 0) {
                    file = &wp->files[pose->fileId];
                    file->references--;
                    if (file->references <= 0) {
                        file->active = 0;
                        texture = &wp->textures[file->textureId];
                        texture->references--;
                        if (texture->references <= 0) {
                            fileFree(texture->handle);
                            texture->active = 0;
                        }
                        fileFree(file->handle);
                    }
                    pose->flags = 0;
                }
            } else {
                if (pose->flags & 0x40) {
                    smartFree(*(smartEntry**)((u8*)pose + 0x15C));
                }
                if (pose->flags & 0x80) {
                    smartFree(*(smartEntry**)((u8*)pose + 0x164));
                }
                file = &wp->files[pose->fileId];
                file->references--;
                if (file->references <= 0) {
                    file->active = 0;
                    texture = &wp->textures[file->textureId];
                    texture->references--;
                    if (texture->references <= 0) {
                        fileFree(texture->handle);
                        texture->active = 0;
                    }
                    fileFree(file->handle);
                }
                pose->flags = 0;
            }
        }
    }
    if (type == 1) {
        wp->inBattle = FALSE;
    }
    animPoseRefresh();
}

// NONMATCHING (5.47%): 3 instructions differ, 25 missing, 2 extra, plus register differences.
void animPaperPoseDisp(CameraId camId, void* param) {
    AnimationPose* pose;
    int i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if (pose->flags & 1 && pose->paperId != -1) {
            animPaperPoseDispSub(0, pose);
        }
    }
}

// NONMATCHING (0.17%): empty placeholder, not decompiled yet.
void animPaperPoseDispSub(s32 unused, AnimationPose* pose) {
}

// animPoseDisp_MakeExtTexture

void animSetPaperTexMtx(Mtx mtx1, Mtx mtx2, Mtx mtx3) {
    if (mtx1 == NULL) {
        wp->paperMtxPtr[0] = NULL;
    } else {
        PSMTXCopy(mtx1, wp->paperMtx[0]);
        wp->paperMtxPtr[0] = &wp->paperMtx[0];
    }
    if (mtx2 == NULL) {
        wp->paperMtxPtr[1] = NULL;
    } else {
        PSMTXCopy(mtx2, wp->paperMtx[1]);
        wp->paperMtxPtr[1] = &wp->paperMtx[1];
    }
    if (mtx3 == NULL) {
        wp->paperMtxPtr[2] = NULL;
    } else {
        PSMTXCopy(mtx3, wp->paperMtx[2]);
        wp->paperMtxPtr[2] = &wp->paperMtx[2];
    }
}

BOOL animGroupBaseAsync(const char* name, s32 group, void* callback) {
    s32 fileId;
    s32 other;
    void** data;
    u8* entry;
    u8* result;
    s32 offset;
    s32 size;

    switch (group) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }
    fileId = fileAsyncf(5, callback, "%s/%s", "a", name);
    data = wp->mp_ag2tg->data;
    size = (s32)data[1];
    entry = (u8*)data[0];
    for (offset = 0; offset < size; offset += 0x20) {
        if (!strcmp((char*)entry, name)) {
            result = entry + 0x20;
            goto found;
        }
        entry += 0x20;
    }
    result = (u8*)&zero_80419a50;
found:
    other = fileAsyncf(4, NULL, "%s/%s-", "a", result);

    if (fileId == -1) {
        return TRUE;
    }
    if ((u32)fileId == 0) {
        return FALSE;
    }
    if (other == -1) {
        return TRUE;
    }
    return other != 0;
}

AnimationPose* animPoseGetAnimPosePtr(s32 poseId) {
    return &wp->poses[poseId];
}

AnimData* animPoseGetAnimDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return ANIM_POSE_DATA_ANIMS(animPoseGetAnimBaseDataPtr(poseId))[pose->animId].data;
}

AnimationPoseData* animPoseGetAnimBaseDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return *wp->files[pose->fileId].handle->data;
}

AnimTableEntry* animPoseGetCurrentAnim(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return &ANIM_POSE_DATA_ANIMS(data)[animPoseGetAnimPosePtr(poseId)->animId];
}

BOOL animPoseTestXLU(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return (*(u32*)data->field_0xC4 >> 3) & 1;
}

// evalProc
// NONMATCHING (94.21%): 4 instructions differ, 3 missing, 1 extra, plus register differences.
void evalProc(s32 parentGroupId, s32 groupId) {
    AnimPoseGroup* group;
    AnimPoseGroup* parent;
    AnimationPoseData* data;
    Vec pos;
    f32* nodes;
    f32 x, y, z;

    if (groupId == -1) {
        return;
    }
    data = wp->mpCurPoseData;
    group = &ANIM_POSE_DATA_GROUPS(data)[groupId];

    if (((s8*)wp->mpCurPose->mpGroupVisibility)[group->visibilityId] != 0) {
        f32* node;

        nodes = (f32*)wp->mpCurPose->mpNodes;
        node = &nodes[group->nodeId];
        if (group->useSSC) {
            if (parentGroupId == -1) {
                x = y = z = 1.0f;
            } else {
                parent = &ANIM_POSE_DATA_GROUPS(data)[parentGroupId];
                if (!parent->useSSC) {
                    x = y = z = 1.0f;
                } else {
                    x = nodes[parent->nodeId + 3];
                    y = nodes[parent->nodeId + 4];
                    z = nodes[parent->nodeId + 5];
                }
            }
            pushGXModelMtx_JointNode__(node, x, y, z);
        } else {
            pushGXModelMtx_TransformNode__(node);
        }

        if (wp->mpCurPose->flags & 0x40) {
            pos = vec3_802c2dd4[0];
            pos.x = (*g_modeling_mtx)[0][3];
            pos.y = (*g_modeling_mtx)[1][3];
            pos.z = (*g_modeling_mtx)[2][3];
            ((Vec*)*(void**)((u8*)wp->mpCurPose + 0x15C))[groupId] = pos;
        }
        if (wp->mpCurPose->flags & 0x80) {
            PSMTXCopy(*g_modeling_mtx, (*(Mtx**)((u8*)wp->mpCurPose + 0x164))[groupId]);
        }
        if (!(wp->mpCurPose->flags & 0x20) || groupId != *wp->mpCurPose->vivianGroupNo) {
            evalProc(groupId, group->childGroupId);
        }
        g_modeling_mtx_lv--;
        g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    }
    evalProc(groupId, group->brotherGroupId);
}

void animPoseWorldPositionEvalOn(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = *wp->files[pose->fileId].handle->data;
    u8 type;

    if (pose->flags & 0x40) {
        return;
    }
    pose->flags |= 0x40;
    switch (pose->type) {
        case 0:
            type = 1;
            break;
        case 1:
            type = 2;
            break;
        case 2:
            type = 0;
            break;
    }
    *(s32*)((u8*)pose + 0x160) = data->groupCount;
    *(void**)((u8*)pose + 0x15C) = smartAlloc(data->groupCount * sizeof(Vec), type);
}

void L_animPoseWorldPositionEvalOff(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    if (pose->flags & 0x40) {
        pose->flags &= ~0x40;
        smartFree(*(smartEntry**)((u8*)pose + 0x15C));
        *(void**)((u8*)pose + 0x15C) = NULL;
    }
}

void animPoseWorldMatrixEvalOn(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = *wp->files[pose->fileId].handle->data;
    u8 type;

    if (pose->flags & 0x80) {
        return;
    }
    pose->flags |= 0x80;
    switch (pose->type) {
        case 0:
            type = 1;
            break;
        case 1:
            type = 2;
            break;
        case 2:
            type = 0;
            break;
    }
    *(s32*)((u8*)pose + 0x168) = data->groupCount;
    *(void**)((u8*)pose + 0x164) = smartAlloc(data->groupCount * sizeof(Mtx), type);
}

void animPoseVivianMain(s32 poseId, Vec* pos) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->flags |= 0x20;
    pose->matrix[0][3] = pos->x;
    pose->matrix[1][3] = pos->y;
    pose->matrix[2][3] = pos->z;
}

s32 animPoseGetVivianType(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->vivianType;
}

void animPoseSetDispCallBack(s32 poseId, void* dispCallback, void* dispUserData) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->dispCallback = dispCallback;
    pose->dispUserDataCallback = dispUserData;
}

// animPoseDrawShape

s32 animPoseGetShapeIdx(s32 poseId, const char* name) {
    AnimPoseShape* shape;
    AnimationPoseData* data;
    s32 i;

    data = animPoseGetAnimBaseDataPtr(poseId);
    shape = data->shapes;
    for (i = 0; i < data->shapeCount; i++, shape++) {
        if (!strcmp(shape->name, name)) {
            return i;
        }
    }
    return -1;
}

s32 animPoseGetGroupIdx(s32 poseId, const char* name) {
    AnimPoseGroup* group;
    AnimationPoseData* data;
    s32 i;

    data = animPoseGetAnimBaseDataPtr(poseId);
    group = ANIM_POSE_DATA_GROUPS(data);
    for (i = 0; i < data->groupCount; i++, group++) {
        if (!strcmp(group->name, name)) {
            return i;
        }
    }
    return -1;
}

const char* animPoseGetGroupName(s32 poseId, s32 group) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return ANIM_POSE_DATA_GROUPS(data)[group].name;
}

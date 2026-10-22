#pragma once

#include "driver/camdrv.h"

#include "manager/filemgr.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>

extern u32 anim_ext_tex_size;

typedef struct AnimData AnimData;
typedef struct AnimPoseNode AnimPoseNode;
typedef struct AnimTexMtx AnimTexMtx;
typedef struct AnimTableEntry AnimTableEntry;

typedef struct AnimDrawCallEntry {
    s32 first; // 0x0
    s32 count; // 0x4, TODO: u32? for cast in GXBegin in renderProc
} AnimDrawCallEntry;

typedef struct AnimPoseShapeVertex {
    s32 first; // 0x0
    s32 count; // 0x4
} AnimPoseShapeVertex;

typedef struct AnimPoseShape {
    char name[64];                 // 0x0
    AnimPoseShapeVertex vtxPos;    // 0x40
    AnimPoseShapeVertex vtxNrm;    // 0x48
    AnimPoseShapeVertex vtxClr0;   // 0x50
    AnimPoseShapeVertex vtxTex[8]; // 0x48
    s32 shapeDrawStart;            // 0x98, TODO: rename?
    s32 shapeDrawCount;            // 0x9C, TODO: rename?
    s32 dispMode;                  // 0xA0, TODO: enum?
    s32 cullMode;                  // 0xA4, TODO: enum?
} AnimPoseShape;

typedef struct AnimPoseShapeDraw {
    s32 texCount;             // 0x0
    u8 field_0x4[0x8 - 0x4];  // 0x4
    s32 drawMode;             // 0x8
    u8 field_0xC[0x10 - 0xC]; // 0xC
    s32 texDataIds[8];        // 0x10
    s8 texArrayIds[8];        // 0x30
    s32 firstDrawCall;        // 0x38
    s32 drawCallCount;        // 0x3C
    s32 firstIdPos;           // 0x40
    s32 firstIdNrm;           // 0x44
    s32 firstIdClr0;          // 0x48
    s32 firstIdTex[8];        // 0x4C
} AnimPoseShapeDraw;

// all of these fall under "AnimPoseData"
typedef struct AnimDataClip {
    s32 field_0x0; // 0x0, tested by animPoseMain to choose between wrapping and clamping the clip time
    f32 field_0x4; // 0x4
    f32 field_0x8; // 0x8
} AnimDataClip;

// A keyframe of an animation: its time and the runs of delta records it applies.
typedef struct AnimFrame {
    f32 time;      // 0x0
    s32 posOff;    // 0x4
    s32 posCount;  // 0x8
    s32 nrmOff;    // 0xC
    s32 nrmCount;  // 0x10
    s32 texOff;    // 0x14
    s32 texCount;  // 0x18
    s32 visOff;    // 0x1C
    s32 visCount;  // 0x20
    s32 nodeOff;   // 0x24
    s32 nodeCount; // 0x28
} AnimFrame;

// An event record of an animation (frames2).
typedef struct AnimFrame2 {
    f32 time;       // 0x0
    s32 field_0x4;  // 0x4, the pose's evtValue once its time is reached (animPoseMain)
} AnimFrame2;

// Per-frame delta records of an animation (animPoseMain and its animPoseAdd*Delta helpers). field_0x0 steps
// the destination index from the previous record.
typedef struct AnimPoseVecDelta {
    u8 field_0x0; // 0x0
    s8 field_0x1; // 0x1, x, y and z deltas
    s8 field_0x2; // 0x2
    s8 field_0x3; // 0x3
} AnimPoseVecDelta;

typedef struct AnimPoseVisDelta {
    u8 field_0x0; // 0x0
    u8 field_0x1; // 0x1, added to the group visibility byte
} AnimPoseVisDelta;

typedef struct AnimPoseNodeDelta {
    u8 field_0x0; // 0x0
    s8 field_0x1; // 0x1, value delta
    u8 field_0x2; // 0x2, tanfTbl indices (signed) of the blend's end tangents
    u8 field_0x3; // 0x3
} AnimPoseNodeDelta;

typedef struct AnimPoseTexDelta {
    u8 field_0x0;    // 0x0
    u8 field_0x1;    // 0x1, added to AnimTexMtx.mTexBindIdx
    u8 pad_0x2[2];   // 0x2
    f32 field_0x4;   // 0x4, added to AnimTexMtx.mTransS
    f32 field_0x8;   // 0x8, added to AnimTexMtx.mTransT
} AnimPoseTexDelta;

struct AnimData {
    u8 field_0x0[0x8 - 0x0];   // 0x0
    s32 frameCount;            // 0x8, keyframes in frames
    u8 field_0xC[0x20 - 0xC];  // 0xC
    s32 frame2Count;           // 0x20, events in frames2
    AnimDataClip* clip;        // 0x24
    AnimFrame* frames;         // 0x28
    AnimPoseVecDelta* posData; // 0x2C
    AnimPoseVecDelta* nrmData; // 0x30
    AnimPoseTexDelta* texData; // 0x34
    AnimPoseVisDelta* visData; // 0x38
    AnimPoseNodeDelta* nodeData; // 0x3C
    AnimFrame2* frames2;       // 0x40
    f32 field_0x44;            // 0x44
    f32 field_0x48;            // 0x48
    f32 field_0x4C;            // 0x4C
    f32 field_0x50;            // 0x50
    f32 field_0x54;            // 0x54
    f32 field_0x58;            // 0x58
};

struct AnimTableEntry {
    char name[60];   // 0x0
    AnimData* data; // 0x3C
};

typedef struct AnimPoseTexEntry {
    s32 bindId;    // 0x0
    s32 wrapFlags; // 0x4
} AnimPoseTexEntry;

typedef struct AnimTexBind {
    u8 field_0x0[0x4 - 0x0];  // 0x0
    s32 palId;                // 0x4, texture palette id for TEXGet
    s32 texType;              // 0x8
    u8 field_0xC[0x40 - 0xC]; // 0xC
} AnimTexBind;

struct AnimTexMtx {
    u8 mTexBindIdx; // 0x0
    u8 align[3];    // 0x1, ???
    f32 mTransS;    // 0x4
    f32 mTransT;    // 0x8
    f32 mScaleS;    // 0xC
    f32 mScaleT;    // 0x10
    f32 mRotate;    // 0x14
};

// end "AnimPoseData" substructs
struct AnimPoseNode {
    Vec mTranslation;         // 0x0
    Vec mScale;               // 0xC
    Vec mRot1;                // 0x18
    Vec mRot2;                // 0x24
    Vec mRotationCenter;      // 0x30
    Vec mScaleCenter;         // 0x3C
    Vec mRotationTranslation; // 0x48
    Vec mScaleTranslation;    // 0x54
};

typedef struct AnimPoseGroup {
    char name[64];      // 0x0
    s32 brotherGroupId; // 0x40
    s32 childGroupId;   // 0x44
    s32 shapeId;        // 0x48
    s32 visibilityId;   // 0x4C
    s32 nodeId;         // 0x50
    BOOL useSSC;        // 0x54, Segment Scale Compensation, TODO: rename?
} AnimPoseGroup;

// this is the structure of the actual stored files
typedef struct AnimationPoseData {
    u32 mSize;                     // 0x0
    char fileName[64];             // 0x4
    char textureName[64];          // 0x44
    char mBuildTime[64];           // 0x84
    u32 xluFlags;                  // 0xC4, bit n: drawn at xluMode n (bit 0 also covers mode 2); bit 3: has XLU
    s32 radius;                    // 0xC8, animPoseGetRadius returns it times 2
    s32 height;                    // 0xCC, animPoseGetHeight returns it times 2
    Vec bboxMin;                   // 0xD0
    Vec bboxMax;                   // 0xDC
    s32 shapeCount;                // 0xE8
    u8 field_0xEC[0xF0 - 0xEC];    // 0xEC
    u32 mBufferPosNum;             // 0xF0
    u32 mBufferPosIdxNum;          // 0xF4
    u32 mBufferNrmNum;             // 0xF8
    u32 mBufferNrmIdxNum;          // 0xFC
    u8 field_0x100[0x12C - 0x100]; // 0x100
    u32 mBufferTexAnimNum;         // 0x12C
    s32 texDataCount;              // 0x130
    s32 texBindCount;              // 0x134
    u8 field_0x138[0x13C - 0x138]; // 0x138
    u32 mBufferVisibilityNum;      // 0x13C
    u32 mBufferNodeNum;            // 0x140
    s32 groupCount;                // 0x144
    s32 mAnimCount;                // 0x148
    AnimPoseShape* shapes;         // 0x14C
    AnimDrawCallEntry* drawCalls;  // 0x150, finished name
    Vec* bufferVtxPos;             // 0x154
    u16* posIndex;                 // 0x158
    Vec* nrmVtxBuffer;             // 0x15C
    u16* nrmIndex;                 // 0x160
    GXColor* clr0VtxBuffer;        // 0x164, finished name
    u16* clr0Index;                // 0x168, finished name
    u16* texIndex[8];              // 0x16C, finished name
    f32* texVtxBuffer;             // 0x18C, finished name, pointer to list of [S, T]
    AnimTexMtx* mBufferTexAnim;    // 0x190, texNum * 0x18 bytes
    AnimPoseTexEntry* texData;     // 0x194, TODO: rename
    AnimTexBind* texBinds;         // 0x198, TODO: rename
    AnimPoseShapeDraw* draws;      // 0x19C
    u8* mBufferVisibility;         // 0x1A0, visNum bytes
    u8* mBufferNode;               // 0x1A4, nodeNum * 4 bytes
    AnimPoseGroup* groups;         // 0x1A8
    AnimTableEntry* mpAnims;       // 0x1AC
} AnimationPoseData;

typedef struct AnimationPose {
    u32 flags;                          // 0x0, unsigned
    s32 typeFlags;                      // 0x4
    s32 references;                     // 0x8
    s32 type;                           // 0xC
    s32 fileId;                         // 0x10
    s32 animId;                         // 0x14, current animation from AnimPoseData
    OSTime localTime;                   // 0x18
    f32 animTime;                       // 0x20, elapsed frames: (now - localTime) * 60 / 1000 * localTimeRate
    f32 clipTime;                       // 0x24, animTime after loop wrap
    s32 keyFrame1;                      // 0x28, index of the FRAMES key before clipTime
    s32 keyFrame2;                      // 0x2C, keyFrame1 + 1, wrapped for non-looping clips
    f32 keyAlpha;                       // 0x30, blend factor between keyFrame1 and keyFrame2
    s32 evtFrame1;                      // 0x34, index of the FRAMES2 event before clipTime
    s32 evtFrame2;                      // 0x38, evtFrame1 + 1, wrapped for non-looping clips
    s32 lastFrameZero;                  // 0x3C
    f32 lastFrameTime;                  // 0x40
    f32 localTimeRate;                  // 0x44
    Vec* mpBufferVtxPos;                // 0x48
    Vec* mpVtxArrayPos;                 // 0x4C
    Vec* mpBufferVtxNrm;                // 0x50
    Vec* mpVtxArrayNrm;                 // 0x54
    u8* mpBufferGroupVisibility;        // 0x58
    u8* mpGroupVisibility;              // 0x5C
    AnimPoseNode* mpBufferNode;         // 0x60
    AnimPoseNode* mpNodes;              // 0x64
    AnimTexMtx* mpBufferTexAnimEntries; // 0x68
    AnimTexMtx* mpTexAnimEntries;       // 0x6C
    f32 drawRotY;                       // 0x70, draw rotY less 90 degrees; set when flags & 8 (faces the camera)
    f32 rotationY;                      // 0x74, current facing, eased toward targetRotY
    f32 facingSign;                     // 0x78, -1 or 1: mirrors the model when it faces away from the camera
    f32 targetRotY;                     // 0x7C, camera-facing angle that rotationY eases toward
    s32 evtValue;                       // 0x80, value of the FRAMES2 event at evtFrame1
    f32 loopTime;                       // 0x84
    OSTime mLocalTimeInit;              // 0x88
    s32 paperId;                        // 0x90
    const char* paperAnim;              // 0x94, -1 when unset; a name (animPoseDrawMtx) or a search index (animPoseSetPaperAnim)
    struct smartEntry* paperTexData[2]; // 0x98
    GXTexObj paperTexObj[2];            // 0xA0
    void (*gxCallback)(s32 wXluStage);  // 0xE0
    BOOL disableDraw;                   // 0xE4
    s32 materialFlag;                   // 0xE8
    s32 materialLightFlag;              // 0xEC
    GXColor materialColor;              // 0xF0
    GXColor materialColor2;             // 0xF4
    f32 paperScaleX;                    // 0xF8, paper width / 150, times the pose scale
    f32 paperScaleY;                    // 0xFC, paper height / 215, times the pose scale
    f32 paperOffsetY;                   // 0x100, bottom offset from ANIMDATA_F48
    f32 paperTexTrans[2];               // 0x104, paper texture S/T translation
    f32 paperTexScale[2];               // 0x10C, paper texture S/T scale
    s16 paperLeft;                      // 0x114, screen-space bounds of the paper quad
    s16 paperRight;                     // 0x116
    s16 paperTop;                       // 0x118
    s16 paperBottom;                    // 0x11A
    s32 vivianType;                     // 0x11C
    s32* vivianGroupNo;                 // 0x120
    Mtx matrix;                         // 0x124
    void (*dispCallback)(void* userData, s32 groupId, Mtx mtx); // 0x154, dispProc calls it before renderProc draws a group
    void* dispUserDataCallback;         // 0x158, dispCallback's first argument
    struct smartEntry* groupWorldPos;   // 0x15C, Vec per group, filled when flags & 0x40
    s32 groupWorldPosCount;             // 0x160
    struct smartEntry* groupWorldMtx;   // 0x164, Mtx per group, filled when flags & 0x80
    s32 groupWorldMtxCount;             // 0x168
    u8 field_0x16C[0x170 - 0x16C];      // 0x16C
} AnimationPose;

typedef struct AnimationPoseFile {
    u32 active;        // 0x0, in use or not, unsigned
    s32 references;    // 0x4, reference count
    FileEntry* handle; // 0x8
    s32 textureId;     // 0xC
} AnimationPoseFile;

typedef struct AnimationTextureFile {
    BOOL active;       // 0x0
    s32 references;    // 0x4
    FileEntry* handle; // 0x8
} AnimationTextureFile;

typedef struct AnimationWork {
    AnimationPoseFile* files;         // 0x0
    s32 totalFiles;                   // 0x4
    AnimationTextureFile* textures;   // 0x8
    s32 totalTextures;                // 0xC
    AnimationPose* poses;             // 0x10
    s32 totalPoses;                   // 0x14
    AnimationPose* mpCurPose;         // 0x18
    AnimData* mpCurAnimData;          // 0x1C
    AnimationPoseData* mpCurPoseData; // 0x20
    void* mpTexDatas[2];              // 0x24
    void* mpCurTexData;               // 0x2C
    GXTexObj* mpTexObjs[2];           // 0x30
    GXTexObj* mpCurTexObj;            // 0x38
    Mtx paperMtx[3];                  // 0x3C
    Mtx* paperMtxPtr[3];              // 0xCC
    u32 field_0xD8;                   // 0xD8
    u32 field_0xDC;                   // 0xDC
    s32 currDispMode;                 // 0xE0
    u8 field_0xE4[0xE8 - 0xE4];       // 0xE4
    u32 mbUseFloatScratch;            // 0xE8
    Vec* mpFloatScratch;              // 0xEC
    u32 mFloatScratchRP;              // 0xF0
    u32 mFloatScratchWP;              // 0xF4
    Vec* vtxArrayPos;                 // 0xF8
    Vec* vtxArrayNrm;                 // 0xFC
    FileEntry* mp_ag2tg;              // 0x100
    void* testHeap;                   // 0x104
    u32 testAlloc;                    // 0x108
    BOOL inBattle;                    // 0x10C
} AnimationWork;

AnimationWork* animGetPtr(void);
OSTime animTimeGetTime(BOOL globalTime);
void animInit(void);
void animMain(void);
void animPoseBattleInit(void);

s32 animPoseEntry(const char* animName, s32 type); // TODO: add enum?
s32 animPaperPoseEntry(const char* animName, s32 type);

BOOL animEffectAsync(const char* animName, u32 group);

void animPosePeraOn(s32 poseId);
void animPosePeraOff(s32 poseId);

void animPosePaperPeraOn(s32 poseId);
void animPosePaperPeraOff(s32 poseId);

void animPoseSetLocalTimeRate(s32 poseId, f32 localTimeRate);
void animPoseSetLocalTime(s32 poseId, f32 localTimeFrames);
void animPoseSetStartTime(s32 poseId, OSTime startTime);

void animPoseSetAnim(s32 poseId, const char* animName, BOOL reset);

s32 animPaperPoseGetId(const char* animName, s32 group);
void animPoseSetPaperAnimGroup(s32 poseId, const char* animName, BOOL someflag);
void animPoseSetPaperAnim(s32 poseId, const char* animName, BOOL unused);
void animPoseSetEffect(s32 poseId, const char* animName, BOOL someflag);
void animPoseSetEffectAnim(s32 poseId, const char* animName, BOOL reset);

AnimTableEntry* animPoseGetCurrentAnim(s32 poseId);
AnimationPoseData* animPoseGetAnimBaseDataPtr(s32 poseId);
AnimData* animPoseGetAnimDataPtr(s32 poseId);
AnimationPose* animPoseGetAnimPosePtr(s32 poseId);

void animPaperPoseRelease(s32 poseId);
void animPoseRelease(s32 poseId);

u32 animPoseGetMaterialEvtColor(s32 poseId);
u32 animPoseGetMaterialLightFlag(s32 poseId);
u32 animPoseGetMaterialFlag(s32 poseId);
void animPoseSetMaterialEvtColor(s32 poseId, GXColor color);
void animPoseSetMaterialLightFlagOff(s32 poseId, u32 mask);
void animPoseSetMaterialLightFlagOn(s32 poseId, u32 mask);
void animPoseSetMaterialFlagOff(s32 poseId, u32 mask);
void animPoseSetMaterialFlagOn(s32 poseId, u32 mask);
BOOL animPoseGetPeraEnd(s32 poseId);

f32 animPoseGetRadius(s32 poseId);
f32 animPoseGetHeight(s32 poseId);

s32 animPoseGetVivianType(s32 poseId);

f32 animPoseGetLoopTimes(s32 id);

void animPoseWorldPositionEvalOn(s32 poseId);
void L_animPoseWorldPositionEvalOff(s32 poseId);
void animPoseWorldMatrixEvalOn(s32 poseId);
void animPoseMain(s32 poseId);
BOOL animPoseTestXLU(s32 poseId);

BOOL animGroupBaseAsync(const char* name, s32 group, void* callback);

void animPoseSetGXFunc(s32 poseId, void (*callback)(s32 wXluStage), BOOL disableDraw);

s32 animPoseGetShapeIdx(s32 poseId, const char* name);
void animPoseDrawShape(s32 poseId, s32 shapeId);
void animPoseSetDispCallBack(s32 poseId, void (*dispCallback)(void* userData, s32 groupId, Mtx mtx), void* dispUserData);
s32 animPoseGetGroupIdx(s32 poseId, const char* name);
const char* animPoseGetGroupName(s32 poseId, s32 group);

void animSetPaperTexMtx(Mtx mtx1, Mtx mtx2, Mtx mtx3);
void animSetPaperTexObj(GXTexObj* a1, GXTexObj* a2, GXTexObj* a3, void* a4, void* a5, void* a6, s32 a7, s32 a8);
void animPoseDraw(s32 poseId, f32 x, f32 y, f32 z, f32 rotY, f32 scale, s32 xluMode);

void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);
void _animPoseDrawMtx(AnimationPose* pose, Mtx matrix, s32 xluMode, f32 f1, f32 f2);
void animPoseRefresh(void);
void pushGXModelMtx_JointNode__(void* node, f32 x, f32 y, f32 z);
void pushGXModelMtx_TransformNode__(void* node);
void animPaperPoseDisp(CameraId camId, void* param);
void animPaperPoseDispSub(s32 unused, void* param);
void animPoseDisp_MakeExtTexture(const char* name, const char* animName, f32 f1, void* texObj, void* base,
                                        f32 f2, s32* out0, s32* out1, s32* out2, s32* out3);
void animPoseAutoRelease(s32 type);
void animPoseVivianMain(s32 poseId, Vec* pos);
void materialProc(s32 shapeId, s32 keyFrame1);
typedef struct MoamoaChainEntry {
    char* name; // 0x0
    char* anim; // 0x4
} MoamoaChainEntry;
extern MoamoaChainEntry g_moamoa_chain_tbl[47];
extern const s32 moamoa_chain_count;
extern Vec vec3_802c2db0[];
// .sdata/.sdata2 objects referenced by name from driver/animdrv.c
void evalProc(s32 parentGroupId, s32 groupId);

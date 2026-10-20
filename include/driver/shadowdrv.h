#pragma once

#include "driver/camdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "memory.h"

// The char, proj and depth work blocks share one layout up to 0x100: shadowDisp runs the projection pass
// on cswp and dswp through a ProjShadowWork*, so the fields at 0x0-0x100 must line up.
// flags (0x0): 0x1 texture is valid (shadowGet*TexObj), 0x2 texture buffer is allocated at draw time
// (projShadowEnd, depthShadowEnd), 0x4 projShadowEnd copies at half resolution.
// drawCount (0x4): bumped per drawn caster; the *ShadowEnd passes run only when it is non-zero.
// configIdx (0x8): shadowConfig block (0x48 bytes) for this pass.
typedef struct DepthShadowWork {
    s32 flags; //0x0
    s32 drawCount; //0x4
    s32 configIdx; //0x8
    Mtx lookAt; //0xC, light view (C_MTXLookAt), copied into the camera view
    Mtx44 projection; //0x3C, light projection written by SHADOW_DISP_PROJ_SETUP
    Mtx projTexMtx; //0x7C, light projection * lookAt (depthShadowEnd)
    Mtx depthMtx; //0xAC, depth-compare matrix (depthShadowEnd)
    GXTexObj texObj; //0xDC
    smartEntry* texBuffer; //0xFC, smartAlloc'd copy target
    GXColor color; //0x100
} DepthShadowWork;

typedef struct ProjShadowWork {
    s32 flags; //0x0
    s32 drawCount; //0x4
    s32 configIdx; //0x8
    Mtx lookAt; //0xC
    Mtx44 projection; //0x3C
    Mtx projTexMtx; //0x7C
    u8 padAC[0xDC - 0xAC];
    GXTexObj texObj; //0xDC
    smartEntry* texBuffer; //0xFC
    GXColor color; //0x100
} ProjShadowWork;

// One caster. kind (0x2): 0 polygon blob, 1 texture blob (effGetTexObjN64), 2 cylinder, 3 projected onto
// the collision surface. flags (0x0): 0x1 collision (follow the hit normal), 0x2 matrix computed, 0x4 no decay.
typedef struct ShadowEntry {
    u16 flags;               // 0x0
    u8 kind;                 // 0x2
    u8 type;                 // 0x3, 0 cylinder, 1 cube (SHADOW_ENTRY_DRAW)
    Vec position;            // 0x4
    f32 size;                // 0x10
    f32 yScale;              // 0x14, Y scale of the collision matrix
    s16 offscreenId;         // 0x18, offscreenAddBoundingBox id, -1 until set
    s16 camId;               // 0x1A
    f32 decayX;              // 0x1C
    f32 decayY;              // 0x20
    f32 decayZ;              // 0x24
    u8 mode;                 // 0x28, TEV alpha of the cylinder colour
    u8 pad29[3];             // 0x29
    Mtx matrix;              // 0x2C
} ShadowEntry;

typedef struct CharShadowWork {
    s32 flags; //0x0
    s32 drawCount; //0x4
    s32 configIdx; //0x8
    Mtx lookAt; //0xC
    Mtx44 projection; //0x3C
    Mtx projTexMtx; //0x7C
    u8 padAC[0xDC - 0xAC];
    GXTexObj texObj; //0xDC
    smartEntry* texBuffer; //0xFC
    GXColor color; //0x100
    s32 entryCount; //0x104
    ShadowEntry* entries; //0x108
    s32 projectionEnabled; //0x10C, shadowMain runs the CAMERA_SHADOW projection pass when set
    s32 defaultKind; //0x110, shadowEntry's kind
    f32 defaultYScale; //0x114, shadowEntry's yScale
    s32 defaultCollision; //0x118, shadowEntry sets flags 0x1 when set
} CharShadowWork;

// One shadowConfig block (0x48 bytes), reached as shadowConfig + configIdx * 0x48.
// followCamera (0x0): 1 rebuilds the light position from CAMERA_3D each frame (shadowDisp, the char pass).
// halfWidth/halfHeight (0x28/0x2C): the C_MTXFrustum/C_MTXOrtho extents, also the Projection quad's size.
// nearZ/farZ (0x30/0x34): clip planes; depthShadowEnd builds the depth matrix from them.
// isOrtho (0x38): 0 perspective (C_MTXFrustum), otherwise C_MTXOrtho.
// depthMode (0x3C): mapdrv switches its depth TEV setup on it (0-5); depthShadowEnd uses GX_TF_Z8 for <= 2, GX_TF_Z16 above.
// sizeIdx (0x40): index into ShadowSizeTbl. depthBias (0x44): z offset of the depth matrix.
typedef struct ShadowConfigFields {
    s32 followCamera; //0x0
    Vec camPos;  //0x4
    Vec up;      //0x10
    Vec target;  //0x1C
    f32 halfWidth; //0x28
    f32 halfHeight; //0x2C
    f32 nearZ; //0x30
    f32 farZ; //0x34
    s32 isOrtho; //0x38
    u32 depthMode; //0x3C
    s32 sizeIdx; //0x40
    f32 depthBias; //0x44
} ShadowConfigFields;

void shadowInit(void);
void shadowMain(void);
s32 shadowEntry(f32 x, f32 y, f32 z, f32 size);
void shadowDisp(CameraId cameraId, void* param);
void U_shadowSetMode(s32 id, s32 mode);
s32 shadowEntryMode(s32 mode, f32 x, f32 y, f32 z, f32 size);
void shadowSetVolumeDecay(s32 id, f32 x, f32 y, f32 z);
void shadowSetCamId(s32 id, s32 camId);
void shadowSetType(s32 id, s32 type);
void shadowSetSize(s32 id, f32 size);
void shadowFlagOn(s32 id, u16 flag);
void cylinderShadowDraw(CameraId camId, void* param);
void shadowCharShadowDisp_Projection(CameraId camId, void* param);
void shadowSetCollision(s32 id, BOOL value);
s32 shadowGetCharShadowProjection(void);
void shadowDepthShadowDrawCountUp(void);
void shadowProjShadowDrawCountUp(void);
void depthShadowEnd(s32 arg0, DepthShadowWork* work);
GXTexObj* shadowGetCharShadowTexObj(void);
GXColor shadowGetCharShadowColor(void);
void* shadowGetCharShadowProjTexMtx(void);
GXTexObj* shadowGetDepthShadowTexObj(void);
void* shadowGetDepthShadowDepthMtx(void);
void* shadowGetDepthShadowProjTexMtx(void);
GXColor shadowGetDepthShadowColor(void);
ShadowConfigFields* shadowGetDepthShadowConfig(void);
GXTexObj* shadowGetProjShadowTexObj(void);
void* shadowGetProjShadowProjTexMtx(void);
GXColor shadowGetProjShadowColor(void);
ShadowConfigFields* shadowGetProjShadowConfig(void);
GXTexObj* shadowGetRamp8TexObj(void);
GXTexObj* shadowGetRamp16TexObj(void);
void shadowCharShadowDisp_Collision(CameraId cameraId, void* param);
void shadowCharShadowDisp_Polygon(CameraId camId, void* param);
void shadowCharShadowDisp_Texture(CameraId camId, void* param);
void cylinder(ShadowEntry* entry);
void projShadowEnd(s32 arg0, ProjShadowWork* work);

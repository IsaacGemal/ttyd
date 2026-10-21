#pragma once

#include "evt/evt_cmd.h"

#include "driver/camdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// PuniPose.flags
#define PUNI_POSE_VISIBLE   0x1  // drawn by puniDraw; texId is the sort key
#define PUNI_POSE_EASE_TURN 0x2  // billboardYaw eases toward its target instead of snapping (evt_puni_pera_onoff clears it)
#define PUNI_POSE_NO_SHADOW 0x4  // puniDrawShadow skips it
#define PUNI_POSE_SHADOW_Y  0x10 // the shadow sits at shadowY instead of under the matrix translation

typedef struct PuniPose {
    u32 flags;                // 0x00, PUNI_POSE_*
    f32 facing;               // 0x04, -PuniData.direction; puniMain flips the billboard when the camera sees the back
    f32 rotZ;                 // 0x08, degrees, applied after the rotOffsetY translation (puniDraw)
    f32 rotOffsetY;           // 0x0C, pivot height for rotZ
    f32 shadowY;              // 0x10, shadow height under PUNI_POSE_SHADOW_Y
    f32 scale;                // 0x14, x/y scale (puniDraw)
    Mtx* matrix;              // 0x18, model matrix; the translation is the puni position
    s32 texId;                // 0x1C, texture and display list index, the current animation frame
    f32 billboardYaw;         // 0x20, yaw puniMain writes into the matrix rotation
    f32 field_0x24;           // 0x24
    f32 billboardYawTarget;   // 0x28, camera yaw, or camera yaw + 180 when facing away
} PuniPose;

typedef struct PuniTexture {
    void* image;        // 0x00
    GXTexObj texObj;    // 0x04
    GXTexRegion region; // 0x24
} PuniTexture;

// puniMain sorts the poses by texId so puniDraw loads each texture once.
typedef struct PuniSortEntry {
    s32 poseId; // 0x00
    f32 key;    // 0x04, the pose's texId
} PuniSortEntry;

typedef struct PuniDrvWork {
    s32 poseNum;             // 0x00
    s32 texCount;            // 0x04
    PuniPose* poses;         // 0x08
    PuniTexture* textures;   // 0x0C
    Mtx* mtx;                // 0x10
    f32* vertices;           // 0x14
    void* normals;           // 0x18
    void* colors;            // 0x1C
    void* uvs;               // 0x20
    PuniSortEntry* sortList; // 0x24
    u8 (*dlBuffer)[0x80];    // 0x28, texCount display lists of 0x80 bytes
    u32* dlSizes;            // 0x2C
    void (*cb0)(void);       // 0x30
    void (*cb1)(void);       // 0x34
    s32 lastTex;             // 0x38
    Mtx shadowMtx;           // 0x3C, puniLoadShadowMtx; puniDrawShadow applies it after the pose matrix
    u32 texSize;             // 0x6C
    u32 fadeOff;             // 0x70, puniFadeOff; 0 makes puniLoadTev blend with the map colour
} PuniDrvWork;

void puniSinCosRet(u32 angle, f32* sinOut, f32* cosOut, f32 scaleSin, f32 scaleCos);
void puniInit(void);
void puniEntry(s32 count, void* param, void (*callback)(void), void (*cb0)(void), void (*cb1)(void));
void puniMakeTexture(CameraId camId, void* param);
void puniReset(void);
void puniFadeOff(s32 fadeOff);
void puniMain(void);
PuniPose* puniGetPosePtr(void);
s32 puniGetPoseNum(void);
void puniPoseDraw(s32 poseId);
void puniDraw(void);
void puniLoadRenderMode(void);
void puniLoadVertex(void);
void puniLoadTexture(void);
u32 puniGetUsedTmemSize(void);
void puniLoadTextureExit(void);
void puniLoadTev(void);
void puniLoadShadowVertex(void);
void puniLoadShadowTev(void);
void puniLoadShadowRenderMode(void);
void puniLoadShadowMtx(Mtx mtx);
void puniDrawShadow(void);

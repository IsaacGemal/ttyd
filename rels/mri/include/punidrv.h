#pragma once

#include "evt/evt_cmd.h"

#include "driver/camdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct PuniPose {
    u32 flags;      // 0x00
    f32 field_0x4;  // 0x04
    f32 field_0x8;  // 0x08
    f32 field_0xC;  // 0x0C
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    Mtx* matrix;    // 0x18
    s32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    f32 field_0x24; // 0x24
    f32 field_0x28; // 0x28
} PuniPose;

typedef struct PuniTexture {
    void* image;        // 0x00
    GXTexObj texObj;    // 0x04
    GXTexRegion region; // 0x24
} PuniTexture;

typedef struct PuniSortEntry {
    s32 field_0x0; // 0x00
    f32 field_0x4; // 0x04
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
    void* dlBuffer;          // 0x28
    u32* dlSizes;            // 0x2C
    void (*cb0)(void);       // 0x30
    void (*cb1)(void);       // 0x34
    s32 lastTex;             // 0x38
    Mtx field_0x3C;          // 0x3C
    u32 texSize;             // 0x6C
    u32 field_0x70;          // 0x70
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

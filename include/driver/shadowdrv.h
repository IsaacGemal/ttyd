#pragma once

#include "driver/camdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "memory.h"

typedef struct DepthShadowWork {
    s32 unk0; //0x0
    s32 unk4; //0x4
    s32 unk8; //0x8
    u8 unkC[0x100 - 0xC]; //0xC
    GXColor unk100; //0x100
} DepthShadowWork;

typedef struct ProjShadowWork {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    Mtx unkC;
    u8 pad3C[0x7C - 0x3C];
    Mtx unk7C;
    u8 padAC[0xDC - 0xAC];
    GXTexObj texObj;
    smartEntry* unkFC;
    GXColor unk100;
} ProjShadowWork;

typedef struct ShadowEntry {
    u16 flags;               // 0x0
    u8 unk2;                 // 0x2
    u8 type;                 // 0x3
    Vec position;            // 0x4
    f32 size;                // 0x10
    f32 unk14;               // 0x14
    s16 unk18;               // 0x18
    u16 camId;               // 0x1A
    f32 decayX;              // 0x1C
    f32 decayY;              // 0x20
    f32 decayZ;              // 0x24
    u8 mode;                 // 0x28
    u8 unk29[3];             // 0x29
    Mtx matrix;              // 0x2C
} ShadowEntry;

typedef struct CharShadowWork {
    s32 unk0; //0x0
    s32 unk4; //0x4
    s32 unk8; //0x8
    u8 field_0xC[0xFC - 0xC]; //0xC
    smartEntry* unkFC; //0xFC
    GXColor unk100; //0x100
    s32 unk104; //0x104
    ShadowEntry* entries; //0x108
    s32 unk10C; //0x10C
    s32 unk110; //0x110
    f32 unk114; //0x114
    s32 unk118; //0x118
} CharShadowWork;

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
void* shadowGetDepthShadowConfig(void);
GXTexObj* shadowGetProjShadowTexObj(void);
void* shadowGetProjShadowProjTexMtx(void);
GXColor shadowGetProjShadowColor(void);
void* shadowGetProjShadowConfig(void);
GXTexObj* shadowGetRamp8TexObj(void);
GXTexObj* shadowGetRamp16TexObj(void);
void shadowCharShadowDisp_Collision(CameraId cameraId, void* param);
void shadowCharShadowDisp_Polygon(CameraId camId, void* param);
void shadowCharShadowDisp_Texture(CameraId camId, void* param);
void cylinder(ShadowEntry* entry);
void projShadowEnd(s32 arg0, ProjShadowWork* work);

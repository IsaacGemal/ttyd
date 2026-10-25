#pragma once

#include "driver/camdrv.h"

#include <dolphin/gx.h>
#include "memory.h"

typedef struct EnvironmentGlareTexCoord {
    f32 unk0; //0x0
    f32 unk4; //0x4
} EnvironmentGlareTexCoord;

typedef struct EnvironmentGlare {
    GXBlendMode mode; //0x0
    GXBlendFactor srcFactor; //0x4
    GXBlendFactor dstFactor; //0x8
    GXTevKColorSel unkC; //0xC
    GXTevKColorSel unk10; //0x10
    f32 unk14; //0x14
    s32 count; //0x18, number of coords that follors
    EnvironmentGlareTexCoord coords[]; //0x1C
} EnvironmentGlare;

typedef struct EnvironmentGlareFilter {
    u16 flags; //0x0
    u16 type; //0x2
    u16 y0; //0x4
    u16 y1; //0x6
    u16 x0; //0x8
    u16 x1; //0xA
    f32 zscale; //0xC
} EnvironmentGlareFilter;

typedef struct EnvironmentUnk88 {
    u16 unk0; //0x0
    u8 unk2; //0x2
    u8 unk3; //0x3
    f32 unk4; //0x4
    f32 unk8; //0x8
} EnvironmentUnk88;

typedef struct EnvironmentBlur {
    u16 flags; //0x0
    u8 unk2[6]; //0x2
    u64 renderTime; //0x8
    s32 unk10; //0x10
    s32 unk14; //0x14
} EnvironmentBlur;

typedef struct EnvironmentWork {
    s32 flags; //0x0
    smartEntry* capture; //0x4
    smartEntry* capture2; //0x8
    GXTexObj captureObj; //0xC
    GXTexObj captureObj2; //0x2C
    void* envBlur; //0x4C, see envBlurOn
    GXTexObj envBlurObj; //0x50
    MtxPtr viewMtx; //0x70, envAddTev stores the current camera view; envTevLoadTexMtxImm reads it
    MtxPtr modelMtx; //0x74, envAddTev stores EnvironmentYami.mtx
    MtxPtr modelViewMtx; //0x78, envAddTev stores viewMtx * modelMtx; envTevLoadTexMtxImm reads it
    f32 field_0x7C; //0x7C, set per type by envAddTev, read by envTevLoadTexMtxImm
    f32 field_0x80; //0x80
    f32 field_0x84; //0x84
    EnvironmentUnk88 unk88; //0x88
    u8 unk94[0x98 - 0x94]; //0x94
    EnvironmentBlur unk98; //0x98, envBlurOn / envDisp_Blur
    EnvironmentGlareFilter filter; //0xB0
    Mtx yamiView; //0xC0, envSetYamiView
} EnvironmentWork;

typedef struct EnvironmentYami {
    struct MapObject* unk0; //0x0, the object being drawn (MapTevCallback.obj)
    MtxPtr mtx; //0x4, the object's model matrix (MapTevCallback.mtx; envAddTev and envSetWater multiply by it)
    s32 unk8; //0x8
    s32 tevStage; //0xC
    s32 texMap; //0x10
    s32 texCoord; //0x14
    s32 numIndStage; //0x18, as MapTevCallback's: the GXSetNumIndStages count (bom_01) and next ind stage
} EnvironmentYami;

void envInit(void);
void envReInit(void);
void envTexSetup(void);
void envMain(void);
void envBlurOn(s32 flag, s32 value);
void envBlurOff(void);
void envGlare(EnvironmentGlare* glare);
void envGlareFilterZ(f32 z);
void envSetYamiView(Mtx mtx);
void envGlareFilter(s32 type, s16 y0, s16 x0, s16 y1, s16 x1);
void envDisp_DepthOfField(void);
void envDisp_FF(void);
void envCapture(CameraId camId, void* param);
void envCapture2(CameraId camId, void* param);
void envDisp(CameraId camId, void* param);
void envDisp2(CameraId camId, void* param);
void envDisp_Blur(void);
void envSetWater(s32 arg0, void* arg1);
void envAddTev(s32 type, void* param);
void envTevLoadTexMtxImm(s32 type, u32 texMtx, u32 texMtx2, GXTexMtxType mtxType);
void envSetYami(EnvironmentYami* yami);
// The texture-matrix data the original keeps in .rodata at dat_802f9338: ten texture-matrix ids
// followed by five vectors.
typedef struct EnvTevMtxData {
    s32 tbl[10]; // 0x00
    Vec vec28;   // 0x28
    Vec vec34;   // 0x34
    Vec vec40;   // 0x40
    Vec vec4C;   // 0x4C
    Vec vec58;   // 0x58
} EnvTevMtxData;

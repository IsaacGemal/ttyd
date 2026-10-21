#pragma once

#include <dolphin/types.h>
#include "sdk/texPalette.h"

typedef struct eff_n64_work {
    TEXPalette* effTexture; // 0x0
    BOOL effTextureLoaded;  // 0x4
} eff_n64_work;

typedef struct eff_set {
    s16 id;     // 0x0
    char* name; // 0x4
} eff_set;

typedef struct effVtxArray {
    s16 pos[3];      // 0x0, XYZ
    s16 tex0[2];     // 0x6, ST
    u8 clr0[3][0xE]; // 0xA, RGBA8
    u32 unk;         // 0x34
} effVtxArray;

// Shared .data vertex array (defined by eff_kemuri4_n64.c).
extern u8 flowers_v[];

void effInit64(void);
struct EffectEntry* effGlassN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale);
void effTexSetupN64(void);
void effGetTexObjN64(u32 id, GXTexObj* obj);
eff_set* effGetSetN64(const char* name);
s32 effTblRandN64(u32 rand, s32 index);
extern u32 _rand_table[];
void effSetVtxDescN64(effVtxArray* array);
void tri1(s32 x, s32 y, s32 z, s32 unused);
void tri2(s32 x1, s32 y1, s32 z1, s32 unused1, s32 x2, s32 y2, s32 z2, s32 unused2);

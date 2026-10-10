#pragma once
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct SstarVtx {
    s16 pos[3];  // 0x0
    s16 unk[4];  // 0x6
} SstarVtx;
typedef struct SstarWork {
    s32 field_0x0;  // 0x0
    Vec position;   // 0x4
    f32 velX;       // 0x10
    f32 velY;       // 0x14
    f32 velZ;       // 0x18
    f32 field_0x1C; // 0x1C
    f32 angle;      // 0x20
    f32 field_0x24; // 0x24
    f32 field_0x28; // 0x28
    f32 field_0x2C; // 0x2C
    s32 lifetime;   // 0x30
    f32 field_0x34; // 0x34
    s32 type;       // 0x38
    s32 field_0x3C; // 0x3C
    Mtx mtx[8];     // 0x40
    f32 scale;      // 0x1C0
    s32 colR;       // 0x1C4
    s32 colG;       // 0x1C8
    s32 colB;       // 0x1CC
    SstarVtx vtx[10]; // 0x1D0
} SstarWork;
EffectEntry* effSstarN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6, f32 a7, f32 a8);
void effSstarMain(EffectEntry* effect);
void effSstarDisp(CameraId camId, void* param);
// .sdata2 objects of this unit, named as in the original object; defined in eff_sstar_n64.c.

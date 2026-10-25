#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/effdrv.h"
typedef struct EffStarPowerData {
    s16 field_0x0;  // 0x0
    s16 type;       // 0x2
    Vec position;   // 0x4
    s32 lifetime;   // 0x10
    s32 counter;    // 0x14, counts up every frame: fade-in and the type 1 spin/colour cycle
    s32 color1R;    // 0x18
    s32 color1G;    // 0x1C
    s32 color1B;    // 0x20
    s32 color1A;    // 0x24
    s32 color2R;    // 0x28
    s32 color2G;    // 0x2C
    s32 color2B;    // 0x30
    s32 color2A;    // 0x34, also gates the trailing star (drawn at half this alpha when nonzero)
    f32 scale;      // 0x38, Disp draws the star at 0.4 * scale
    f32 rotX;       // 0x3C, degrees
    f32 rotY;       // 0x40
    f32 rotZ;       // 0x44
    f32 prevRotX;   // 0x48, last frame's rotX/Y/Z and scale: the trailing star's pose
    f32 prevRotY;   // 0x4C
    f32 prevRotZ;   // 0x50
    f32 prevScale;  // 0x54
} EffStarPowerData;
EffectEntry* effStarPowerN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale);
void effStarPowerMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effStarPowerDisp(CameraId camId, void* param);

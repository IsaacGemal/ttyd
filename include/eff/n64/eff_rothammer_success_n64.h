#pragma once
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

/* One swirl particle. The first element is also the effect header (type,
   position, lifetime and counters share the element's leading fields). */
typedef struct effRotHammerSuccessData {
    s16 type;       // 0x00
    u8 pad_02[0x2]; // 0x02
    Vec position;   // 0x04
    Vec velocity;   // 0x10
    f32 angle;      // 0x1C, degrees (converted to radians at draw time)
    f32 rotSpeed;   // 0x20, degrees/frame added to angle
    s32 life;       // 0x24, counts down
    s32 frame;      // 0x28, counts up
    s32 lifeMax;    // 0x2C, initial life
    f32 scale;      // 0x30
} effRotHammerSuccessData;
EffectEntry* effRotHammerSuccessN64Entry(s32 type, f32 x, f32 y, f32 z, s32 count, s32 life);
void effRotHammerSuccessMain(EffectEntry* effect);
void effRotHammerSuccessDisp(CameraId camId, void* param);

#pragma once
#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct {
	u16 scale;
	u16 alpha;
} AnimeData;
typedef struct effReleaseN64Data {
    u32 typeFlags;    // 0x00 type & 0xFF00; nonzero makes state 0 drift on sin/cos
    u32 type;         // 0x04 type & 0xFF
    f32 x;            // 0x08
    f32 y;            // 0x0C
    f32 z;            // 0x10
    s32 life;         // 0x14 frames left (1000 = held until flagged; 16 once the effect is told to close)
    s32 counter;      // 0x18 frames elapsed
    s32 tevReg0R;     // 0x1C TEVREG0 red
    s32 tevReg0G;     // 0x20 TEVREG0 green
    s32 tevReg0B;     // 0x24 TEVREG0 blue
    s32 field_0x28;   // 0x28 0xFF at init, life << 4 once life < 16; never read
    s32 tevReg1R;     // 0x2C TEVREG1 red
    s32 tevReg1G;     // 0x30 TEVREG1 green
    s32 tevReg1B;     // 0x34 TEVREG1 blue
    s32 tevReg1A;     // 0x38 TEVREG1 alpha
    f32 scale;        // 0x3C
    f32 scales[4];    // 0x40
    s32 alphas[4];    // 0x50
    s32 field_0x60;   // 0x60
    s32 field_0x64;   // 0x64
    s32 field_0x68;   // 0x68
    s32 field_0x6C;   // 0x6C
    s32 state;        // 0x70 0 drift and pulse, 1 or 100 scale in along anime_data, 4 out along anime_data_2, other values idle
    s32 stateFrame;   // 0x74 frames into the current state
} effReleaseN64Data;
EffectEntry* effReleaseN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effReleaseMain(EffectEntry* effect);
void effReleaseDisp(CameraId camId, void* param);
extern f32 lbl_803A4C60[];

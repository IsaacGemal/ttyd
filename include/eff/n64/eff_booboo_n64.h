#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

typedef struct BoobooData {
	s32 field_0x0;   // 0x0 written 1000 at init, never read
	s32 timer;       // 0x4
	s32 flags[11];   // 0x8
	f32 x[11];       // 0x34
	f32 y[11];       // 0x60
	f32 z[11];       // 0x8c
	f32 rot[11];     // 0xb8
	u8 tex[11];      // 0xe4
	u8 restore[11];  // 0xef
	u8 state[11];    // 0xfa
	u8 poly[11];     // 0x105
	f32 bob[11];     // 0x110
	s8 counter[11];  // 0x13c
} BoobooData;

EffectEntry* effBoobooN64Entry(s32 type, s32 unused, f32 x, f32 y, f32 z, f32 scale);
void effBoobooMain(EffectEntry* effect);
void effBoobooDisp(CameraId camId, void* param);
BOOL texture_dl(EffectEntry* effect, s32 index, u32 tex);
void boo_polygon_0(void);
void boo_polygon_1(void);
void boo_polygon_2(void);

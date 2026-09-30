#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
#include "driver/camdrv.h"
void effOpukuJetwrainDisp(CameraId camId, void* param);
EffectEntry* effOpukuJetwrainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
typedef struct effOpukuJetwrainData {
	s32 type;                // 0x00
	f32 x;                   // 0x04
	f32 y;                   // 0x08
	f32 z;                   // 0x0C
	s32 lifetime;            // 0x10
	s32 field_0x14;          // 0x14
	f32 scale;               // 0x18
	u8 field_0x1C;           // 0x1C
	u8 field_0x1D;           // 0x1D
	u8 field_0x1E;           // 0x1E
	u8 field_0x1F;           // 0x1F
	u8 field_0x20;           // 0x20
	u8 field_0x21;           // 0x21
	u8 field_0x22;           // 0x22
	u8 field_0x23[19][13];   // 0x23
	s16 field_0x11A[19][13]; // 0x11A
} effOpukuJetwrainData;
void effOpukuJetwrainMain(EffectEntry* effect);
typedef struct effOpukuJetwrainVtx {
	s16 x;   // 0x0
	s16 y;   // 0x2
	s16 z;   // 0x4
	u16 s;   // 0x6
	u16 t;   // 0x8
	u8 r;    // 0xA
	u8 g;    // 0xB
	u8 b;    // 0xC
	u8 a;    // 0xD
} effOpukuJetwrainVtx;

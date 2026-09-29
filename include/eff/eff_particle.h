#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
// A single live particle. The array is a set of circular singly-linked lists;
// `next` is an index into the array, not a pointer.
typedef struct ParticleEntry {
	s32 next;         // 0x00
	Vec position;     // 0x04
	Vec velocity;     // 0x10
	s32 life;         // 0x1C
} ParticleEntry;      // 0x20
// Emission state at work+0x34. The trailing 8 bytes are not referenced by this
// file; they keep `particles` at work+0xA0.
typedef struct ParticleEmitter {
	Mtx matrix;         // 0x00
	Vec position;       // 0x30
	Vec field_0x3C;     // 0x3C
	Vec field_0x48;     // 0x48
	f32 field_0x54;     // 0x54
	f32 field_0x58;     // 0x58
	f32 field_0x5C;     // 0x5C
	f32 field_0x60;     // 0x60
	u8 field_0x64[0x8]; // 0x64
} ParticleEmitter;      // 0x6C
typedef struct ParticleWork {
	s32 type;                 // 0x00
	Vec position;             // 0x04
	Vec velocity;             // 0x10
	f32 angleX;               // 0x1C
	f32 angleZ;               // 0x20
	f32 scale;                // 0x24
	s32 life;                 // 0x28
	s32 frame;                // 0x2C
	s32 alpha;                // 0x30
	ParticleEmitter emitter;  // 0x34
	ParticleEntry* particles; // 0xA0
	u32 count;                // 0xA4
	u32 head;                 // 0xA8
	f32 field_0xAC;           // 0xAC
} ParticleWork;               // 0xB0
EffectEntry* effParticleEntry(s32 type, s32 count, s32 life, f32 x, f32 y, f32 z, f32 velX, f32 velY, f32 velZ);
void effParticleMain(EffectEntry* effect);
void effParticleDisp(CameraId camId, void* param);
void EmitterEmit(EffectEntry* effect, ParticleEmitter* emitter);
void ParticleEmit(EffectEntry* effect, ParticleEntry* particle, ParticleEmitter* emitter);

#include "eff/n64/eff_packn_bfire_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdlib.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0_8041f8ec;
extern const f32 float_1_8041f900;
extern const f32 float_0p97_8041f90c;
extern const f32 float_0p02_8041f910;
extern const f32 float_0p05_8041f914;
extern const f32 float_0p1_8041f918;
extern const f32 float_0p9_8041f91c;
extern const f32 float_0p2_8041f920;
extern const f32 float_neg0p6_8041f924;
extern const f32 float_0p07_8041f928;
extern const f32 float_6p2832_8041f8f0;
extern const f32 float_360_8041f8f4;
extern const f32 float_0p3_8041f92c;
extern const f32 float_neg10_8041f930;
extern const f32 float_5_8041f934;
extern const f32 float_3_8041f938;
extern const f32 float_2_8041f93c;
extern const f32 float_neg45_8041f940;

extern const char str_802FB324[];

// Compiler-pooled int->double bias, named after its original address.
const f64 double_to_int_802fb338 = 4503601774854144.0;

typedef struct effPacknBfireData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 vx;         // 0x10
	f32 vy;         // 0x14
	f32 vz;         // 0x18
	s32 field_0x1C; // 0x1C
	s32 lifetime;   // 0x20
	s32 timer;      // 0x24
	s32 field_0x28; // 0x28
	s32 field_0x2C; // 0x2C
	s32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
	s32 field_0x3C; // 0x3C
	s32 field_0x40; // 0x40
	s32 field_0x44; // 0x44
	f32 scale;      // 0x48
	f32 field_0x4C; // 0x4C
	f32 field_0x50; // 0x50
	f32 field_0x54; // 0x54
	f32 field_0x58; // 0x58
	f32 field_0x5C; // 0x5C
	f32 field_0x60; // 0x60
	f32 field_0x64; // 0x64
	f32 field_0x68; // 0x68
	f32 field_0x6C; // 0x6C
	f32 field_0x70; // 0x70
	f32 field_0x74; // 0x74
	f32 field_0x78; // 0x78
} effPacknBfireData;

EffectEntry* effPacknBfireN64Entry(s32 type, s32 a1, s32 a2, f32 x, f32 y, f32 z, f32 vx, f32 vy, f32 vz, f32 scale) {
	EffectEntry* effect;
	effPacknBfireData* data;
	s32 count;
	s32 i;
	f32 base;

	effect = effEntry();
	count = (type == 2) ? 8 : 1;
	effect->type = str_802FB324;
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(effPacknBfireData));
	effect->userdata = data;
	effect->callback = effPacknBfireMain;
	effect->flags |= 2;

	if (a2 <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = a2;
	}
	data->type = type;
	data->timer = 0;
	data->field_0x1C = a1;
	data->field_0x34 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->vx = vx;
	data->vy = vy;
	data->vz = vz;
	data->field_0x60 = (data->vx - data->x) / (f32)a1;
	data->field_0x64 = (data->vy - data->y) / (f32)a1;
	data->field_0x68 = (data->vz - data->z) / (f32)a1;
	data->scale = scale;

	switch (type) {
	case 0:
		break;
	case 1:
		data->field_0x28 = 0xFF;
		data->field_0x2C = 0x58;
		data->field_0x30 = 0x46;
		data->field_0x34 = 0x37;
		data->field_0x38 = 0x0B;
		data->field_0x3C = 0x33;
		data->field_0x40 = 0x7B;
		data->field_0x44 = 0xFF;
		data->field_0x4C = (f32)(rand() % 65);
		data->field_0x50 = (f32)(rand() % 65);
		data->field_0x70 = float_0p2_8041f920;
		data->field_0x6C = float_0p3_8041f92c;
		data->field_0x54 = float_0_8041f8ec;
		data->field_0x58 = float_0_8041f8ec;
		data->field_0x74 = float_0_8041f8ec;
		data->field_0x78 = float_neg10_8041f930;
		break;
	default:
		data->field_0x28 = 0xFF;
		data->field_0x2C = 0x58;
		data->field_0x30 = 0x46;
		data->field_0x34 = 0x37;
		data->field_0x38 = 0x0B;
		data->field_0x3C = 0x33;
		data->field_0x40 = 0x7B;
		data->field_0x44 = 0xFF;
		base = (f32)(rand() % 361);
		for (i = 0; i < count; i++, data++) {
			f32 rad;

			data->x = x;
			data->y = y;
			data->z = z;
			rad = float_6p2832_8041f8f0 * (base + (f32)(i * 360 / 8)) / float_360_8041f8f4;
			data->field_0x60 = float_5_8041f934 * (f32)sin(rad);
			data->field_0x64 = float_2_8041f93c * (f32)cos(rad) + float_3_8041f938;
			data->field_0x4C = (f32)(rand() % 65);
			data->field_0x50 = (f32)(rand() % 65);
			data->field_0x70 = float_1_8041f900;
			data->field_0x6C = float_1_8041f900;
			data->field_0x54 = float_0_8041f8ec;
			data->field_0x58 = float_0_8041f8ec;
			data->field_0x74 = float_neg45_8041f940;
			data->field_0x78 = float_neg10_8041f930;
		}
		break;
	}

	return effect;
}

void effPacknBfireMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	effPacknBfireData* data = effect->userdata;
	Vec disp = { 0.0f, 0.0f, 0.0f };
	s32 type;
	s32 timer;
	s32 i;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	if (data->lifetime == 1 && type == 1) {
		effPacknBfireN64Entry(2, 0x20, 0x20, data->x, data->y, data->z, data->vx, data->vy, data->vz, float_1_8041f900);
	}
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}

	timer = data->timer;
	switch (type) {
	case 0:
		if ((timer & 3) == 0) {
			effPacknBfireN64Entry(1, data->field_0x1C, data->field_0x1C, data->x, data->y, data->z, data->vx, data->vy,
			                      data->vz, float_1_8041f900);
		}
		break;
	case 1:
		data->field_0x60 *= float_0p97_8041f90c;
		data->field_0x68 *= float_0p97_8041f90c;
		data->field_0x64 += float_0p02_8041f910 * (float_1_8041f900 - data->field_0x64);
		data->x += data->field_0x60;
		data->y += data->field_0x64;
		data->z += data->field_0x68;
		data->field_0x70 += float_0p05_8041f914 * (float_1_8041f900 - data->field_0x70);
		data->field_0x6C += float_0p1_8041f918 * (float_1_8041f900 - data->field_0x6C);
		data->field_0x74 += data->field_0x78;
		break;
	case 2: {
		/* Declared in this order, assigned in this order: the register allocator must hand 0.9 to f6 and 0.2
		   to f5 (f4 for 0.0), which only happens when the declarations and the loads disagree. */
		f32 vz, vb, va;
		va = float_0p9_8041f91c;
		vb = float_0p2_8041f920;
		vz = float_0_8041f8ec;
		for (i = 0; i < effect->count; i++, data++) {
			data->field_0x60 *= va;
			data->field_0x64 -= vb;
			data->x += data->field_0x60;
			data->y += data->field_0x64;
			if (data->y < vz) {
				data->y = vz;
				if (data->field_0x64 < vz) {
					data->field_0x64 *= float_neg0p6_8041f924;
				}
			}
			data->field_0x70 = float_0p07_8041f928 * -data->field_0x70 + data->field_0x70;
			data->field_0x6C = float_0p05_8041f914 * -data->field_0x6C + data->field_0x6C;
			data->field_0x74 += data->field_0x78;
		}
		break;
	}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPacknBfireDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effPacknBfireDisp, effect);
#endif
}

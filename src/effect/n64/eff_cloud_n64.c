#include "eff/n64/eff_cloud_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

int __float_nan[];
#define NAN (*(f32*)__float_nan)

extern const f32 float_0_8041eba0;

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 __fpclassifyf(f32 x) {
	switch (*(s32*)&x & 0x7F800000) {
		case 0x7F800000:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_NAN;
			} else {
				return FP_INFINITE;
			}
			break;
		case 0:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_SUBNORMAL;
			} else {
				return FP_ZERO;
			}
			break;
		default:
			return FP_NORMAL;
	}
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
	if (x > float_0_8041eba0) {
		f64 guess = __frsqrte(x);
		guess = 0.5 * guess * (3.0 - guess * guess * x);
		guess = 0.5 * guess * (3.0 - guess * guess * x);
		guess = 0.5 * guess * (3.0 - guess * guess * x);
		return x * guess;
	} else if (x < 0.0) {
		return NAN;
	} else if (isnan(x)) {
		return NAN;
	}
	return x;
}

EffectEntry* effCloudN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effCloudData* data;
	s32 i;

	effect = effEntry();
	effect->type = "CloudN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effCloudData));
	effect->userdata = data;
	effect->callback = effCloudMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = lifetime;
	}
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	switch (type) {
		case 0:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0xff;
			data->field_0x20 = 0xff;
			data->field_0x28 = 0xe1;
			data->field_0x2C = 0xe1;
			data->field_0x30 = 0xe1;
			data->field_0x24 = 0xff;
			break;
		case 1:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0xf5;
			data->field_0x20 = 0xe6;
			data->field_0x28 = 0x2d;
			data->field_0x2C = 0x23;
			data->field_0x30 = 0x0f;
			data->field_0x24 = 0xff;
			break;
		case 2:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0;
			data->field_0x20 = 0;
			data->field_0x28 = 0x9b;
			data->field_0x2C = 0x7d;
			data->field_0x30 = 0x7d;
			data->field_0x24 = 0xff;
			break;
		case 3:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0xff;
			data->field_0x20 = 0xff;
			data->field_0x28 = 0xff;
			data->field_0x2C = 0x41;
			data->field_0x30 = 0x14;
			data->field_0x24 = 0xff;
			break;
		default:
			break;
	}
	data->field_0x38 = x;
	data->field_0x3C = y;
	data->field_0x40 = z;
#ifdef MUST_MATCH
	// The original unrolls this zeroing loop at the source level.
	data->field_0x230[0] = 0;
	data->field_0x230[1] = 0;
	data->field_0x230[2] = 0;
	data->field_0x230[3] = 0;
	data->field_0x230[4] = 0;
	data->field_0x230[5] = 0;
	data->field_0x230[6] = 0;
	data->field_0x230[7] = 0;
	data->field_0x230[8] = 0;
	data->field_0x230[9] = 0;
	data->field_0x230[10] = 0;
	data->field_0x230[11] = 0;
	data->field_0x230[12] = 0;
	data->field_0x230[13] = 0;
	data->field_0x230[14] = 0;
	data->field_0x230[15] = 0;
	data->field_0x230[16] = 0;
	data->field_0x230[17] = 0;
	data->field_0x230[18] = 0;
	data->field_0x230[19] = 0;
	data->field_0x230[20] = 0;
	data->field_0x230[21] = 0;
	data->field_0x230[22] = 0;
	data->field_0x230[23] = 0;
	data->field_0x230[24] = 0;
	data->field_0x230[25] = 0;
	data->field_0x230[26] = 0;
	data->field_0x230[27] = 0;
	data->field_0x230[28] = 0;
	data->field_0x230[29] = 0;
	data->field_0x2A8 = 0;
#else
	for (i = 0; i < 30; i++) {
		data->field_0x230[i] = 0;
	}
	data->field_0x2A8 = 0;
#endif
	data->field_0x50[0] = x;
	data->field_0xC8[0] = y;
	data->field_0x140[0] = z;
	data->field_0x230[0] = 1;
	data->field_0x1B8[0] = 0;
	data->field_0x2AC[0] = 0.0f;
	data->field_0x44 = (f32)(rand() % 30 + 10);
	data->field_0x48 = 0.0f;
	data->field_0x4C = 0.0f;
	return effect;
}

// MWCC's float register colouring in the ring-buffer block and the anonymous rodata pool
// order still differ: retail keeps the current position in f3/f4/f5 and the previous sample
// in f0/f1/f2 (this build picks f5/f6/f7 and f1/f3/f4), and retail places double_to_int at
// vec3+0x10 then the sqrtf doubles at +0x18/+0x20/+0x28, while this build places the sqrtf
// doubles first and double_to_int last, so the pooled lfd offsets are shifted.
// NONMATCHING (92.37%): see above; the double_to_int conversion constant is compiler-generated
// and always allocated after the literal sqrtf doubles.
void effCloudMain(EffectEntry* effect) {
	effCloudData* data = effect->userdata;
	s32 type;
	s32 counter;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effGlassMain). */
	Vec position;
#endif
	Vec disp = { 0 };

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	type = data->type;
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->field_0x14++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	counter = data->field_0x14;
	if (data->lifetime < 16) {
		data->field_0x24 = data->lifetime << 4;
	}

	if (type == 3) {
		data->field_0x44 += (f32)(rand() % 10 - 5);
		data->field_0x48 += (f32)(rand() % 10 - 5);
		data->field_0x38 += data->field_0x44;
		data->field_0x3C += data->field_0x48;
		data->field_0x40 += data->field_0x4C;
	}

	{
		f32 px = data->field_0x38;
		f32 py = data->field_0x3C;
		f32 pz = data->field_0x40;
		s32 index = data->field_0x2A8 % 30;
		f32 ox = data->field_0x50[index];
		f32 oy = data->field_0xC8[index];
		f32 oz = data->field_0x140[index];

		if (px != ox || py != oy || pz != oz) {
			f32 len = data->field_0x2AC[index];
			f32 dist;

			data->field_0x2A8++;
			if (data->field_0x2A8 >= 30) {
				data->field_0x2A8 = 0;
			}
			index = data->field_0x2A8;
			data->field_0x230[index] = 1;
			data->field_0x50[index] = px;
			data->field_0xC8[index] = py;
			data->field_0x140[index] = pz;
			data->field_0x1B8[index] = counter;
			dist = sqrtf((px - ox) * (px - ox) + (py - oy) * (py - oy) + (pz - oz) * (pz - oz));
			data->field_0x2AC[index] = len + dist;
		}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effCloudDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effCloudDisp, effect);
#endif
}

#include "eff/n64/eff_pokopi_thunder_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>
#include <stdlib.h>

// Inlined MetroWerks sqrtf and its NaN classification (same helper several other effect
// files carry). The two double literals and the comparison zero pool into the original
// .rodata (double_0p5_802fb438, double_3_802fb440, double_0_802fb448).
int __float_nan[];
#define NAN (*(f32*)__float_nan)

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

// The sqrtf constants are named .rodata objects in the original; declaring them extern forces the
// inlined Newton iterations to load the symbols instead of materialising fresh pool entries.
extern const f64 double_0p5_802fb438;
extern const f64 double_3_802fb440;
extern const f64 double_0_802fb448;

static inline f32 pt_sqrtf(f32 x) {
	if (x > 0.0f) {
		f64 guess = __frsqrte(x);
		guess = double_0p5_802fb438 * guess * (double_3_802fb440 - guess * guess * x);
		guess = double_0p5_802fb438 * guess * (double_3_802fb440 - guess * guess * x);
		guess = double_0p5_802fb438 * guess * (double_3_802fb440 - guess * guess * x);
		return x * guess;
	} else if (x < double_0_802fb448) {
		return NAN;
	} else if (isnan(x)) {
		return NAN;
	}
	return x;
}

// .rodata
const Vec vec3_802fb400 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_mask_802fb410 = 4503599627370496.0;
const f64 double_to_int_802fb418 = 4503601774854144.0;
const char str_802FB420[] = "PokopiThunderN64";
// Named in the original .rodata (see the .obj blocks).
const f64 double_0p5_802fb438 = 0.5;
const f64 double_3_802fb440 = 3.0;
const f64 double_0_802fb448 = 0.0;
const f32 float_0p5_8041fa54 = 0.5f;

// The named sqrtf doubles (0x38/0x40/0x48) sit where the original .rodata has them, and the
// else-return-0 block is placed next to the sqrt result; together with a real zero variable (so
// the outer fcmpu compares dist2 first, as retail does) that reproduces the original exactly.
EffectEntry* effPokopiThunderN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale) {
	EffectEntry* effect;
	s32 cameraId;
	EffPokopiThunderData* data;
	f32 dx;
	f32 dy;
	f32 dz;
	f32 dist2;

#ifdef MUST_MATCH
	/* MUST_MATCH: a named zero variable makes MWCC emit `fcmpu cr0, f4, f0` (dist2 first);
	   a literal `0.0f` puts the constant first. */
	f32 zero = 0.0f;
#endif

	effect = effEntry();
	cameraId = 4;
	if (type >= 10) {
		cameraId = 8;
	}
	type %= 10;
	effect->type = str_802FB420;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffPokopiThunderData));
	effect->userdata = data;
	effect->callback = effPokopiThunderMain;
	effect->flags |= 2;

	data->type = type;
	data->timer = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = lifetime;
	}
	data->field_0x3C = 0xFF;

	// The unnamed scratch fields the original keeps in the struct's reserved runs.
	*(f32*)((u8*)data + 0x10) = x;
	*(f32*)((u8*)data + 0x14) = y;
	*(f32*)((u8*)data + 0x18) = z;
	*(f32*)((u8*)data + 0x1C) = endX;
	*(f32*)((u8*)data + 0x20) = endY;
	*(f32*)((u8*)data + 0x24) = endZ;
	data->x = x;
	data->y = y;
	data->z = z;
	*(f32*)((u8*)data + 0x4C) = scale;

	data->cameraId = cameraId;

	dx = x - endX;
	dy = y - endY;
	dz = z - endZ;
	dist2 = dx * dx + dy * dy + dz * dz;
#ifdef MUST_MATCH
	if (dist2 != zero) {
#else
	if (dist2 != 0.0f) {
#endif
		*(f32*)((u8*)data + 0x114) = pt_sqrtf(dist2);
	} else {
		return NULL;
	}
	*(f32*)((u8*)data + 0x114) = 0.005f * *(f32*)((u8*)data + 0x114);
	data->field_0x30 = 0xFF;
	data->field_0x34 = 0xDC;
	data->field_0x38 = 0x14;
	data->field_0x40 = 0xFF;
	data->field_0x44 = 0xFF;
	data->field_0x48 = 0xFF;
	*(s32*)((u8*)data + 0x110) = rand() % 360;
	return effect;
}

void effPokopiThunderMain(EffectEntry* effect) {
	EffPokopiThunderData* data;
	Vec pos2;
	Vec pos;
	s32 type;
	s32 timer;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}

	timer = data->lifetime;
	if (timer < 1000) {
		data->lifetime = timer - 1;
	}
	data->timer++;

	timer = data->lifetime;
	if (timer < 0) {
		effDelete(effect);
		return;
	}

	timer = data->timer;
	// MWCC strength-reduces / 2.0f into fmuls by the 0.5 pool constant, and that division form is
	// what reproduces the original's f2/f0 constant registers (a plain * 0.5f allocates f1/f2).
	if (type == 2) {
		if (timer >= 40) {
			data->field_0x3C = (s32)((f32)data->field_0x3C / 2.0f);
		}
	} else {
		if (timer >= 11) {
			data->field_0x3C = (s32)((f32)data->field_0x3C / 2.0f);
		}
	}

	dispEntry(data->cameraId, dispCalcZ(&pos2), 2, effPokopiThunderDisp, effect);
}

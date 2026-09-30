#include "eff/n64/eff_sunlight_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "math_ext.h"

EffectEntry* effSunlightN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effSunlightData* data;

	entry = effEntry();
	entry->type = "SunlightN64";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effSunlightData));
	entry->userdata = data;
	entry->callback = effSunlightMain;
	entry->flags |= 2;
	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	}
	else {
		data->life = life;
	}
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->r0 = 0xff;
	data->g0 = 0xff;
	data->b0 = 0xff;
	data->r1 = 0xff;
	data->g1 = 0xff;
	data->b1 = 0xff;
	data->a1 = 0xff;
	data->field_0x20 = 0.0f;
	data->field_0x24 = 0.0f;
	data->field_0x28 = 0.0f;
	data->field_0x2C = 0.0f;
	data->field_0x30 = 0.0f;
	data->field_0x3C = 0xff;
	data->field_0x38 = 0;

	return entry;
}

void effSunlightMain(EffectEntry* effect) {
	effSunlightData* data = effect->userdata;
	s32 i;
	s32 counter;
	f32 base;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effFlameMain / effGlassMain). */
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
		data->life = 16;
	}

	if (data->life < 1000) {
		data->life--;
	}
	data->counter++;
	if (data->counter > 0x4F1A0) {
		data->counter = 0x100;
	}

	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	counter = data->counter;

	if (data->life < 16) {
		data->field_0x38 -= 16;
		if (data->field_0x38 < 0) {
			data->field_0x38 = 0;
		}
	}

	if (data->field_0x3C < 0) {
		data->field_0x3C = 0;
	}
	else if (data->field_0x3C > 255) {
		data->field_0x3C = 255;
	}

	if (data->field_0x38 > data->field_0x3C) {
		data->field_0x38 -= 8;
		if (data->field_0x38 < data->field_0x3C) {
			data->field_0x38 = data->field_0x3C;
		}
	}
	else if (data->field_0x38 < data->field_0x3C) {
		data->field_0x38 += 8;
		if (data->field_0x38 > data->field_0x3C) {
			data->field_0x38 = data->field_0x3C;
		}
	}

	base = float_0p25_8041ff8c * (f32)counter;

	for (i = 0; i < 5; i++) {
		f32 s1;
		f32 s2;

		s1 = (f32)sin((base + (f32)(i * i * 20)) * float_6p2832_8041ff90 / float_360_8041ff94);
		s2 = (f32)sin(float_6p2832_8041ff90 * (f32)(counter * 2 + i * 20) / float_360_8041ff94);
#ifdef MUST_MATCH
		/* MUST_MATCH: the factor has to be materialized first and multiplied as `inc * s1`,
		   otherwise MWCC emits `fnmsubs f0, f22, f1, f0` (s1 first) instead of the retail
		   `fnmsubs f0, f1, f22, f0`. */
		{
			f32 inc = float_4_8041ff98 * (float_0p01_8041ffa0 * s2 + float_0p05_8041ff9c);
			(&data->field_0x20)[i] -= inc * s1;
		}
#else
		(&data->field_0x20)[i] -= float_4_8041ff98 * (float_0p01_8041ffa0 * s2 + float_0p05_8041ff9c) * s1;
#endif
		if ((&data->field_0x20)[i] < float_0_8041ff80) {
			(&data->field_0x20)[i] += 256.0f;
		}
		if ((&data->field_0x20)[i] > 256.0f) {
			(&data->field_0x20)[i] -= 256.0f;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effSunlightDisp, effect);
}

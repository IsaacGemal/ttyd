#include "eff/n64/eff_stone_smoke_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

const char str_802FB7B8[] = "StoneSmokeN64";

EffectEntry* effStoneSmokeN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effStoneSmokeN64Data* data;

	effect = effEntry();
	effect->type = str_802FB7B8;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x3C);
	effect->userdata = data;
	effect->callback = effStoneSmokeMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (duration <= 0) {
		data->life = 1000;
	} else {
		data->life = duration;
	}
	data->field_0x24 = 0xFF;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x34 = scale;
	data->field_0x18 = 0xFF;
	data->field_0x1C = 0xFF;
	data->field_0x20 = 0xFF;
	data->field_0x28 = 10;
	data->field_0x2C = 10;
	data->field_0x30 = 0x78;
	return effect;
}

// .sdata2 (pool constants referenced by name so the symbol matches the original)
extern const f32 float_0p6_8041ff58;
extern const f32 float_0p01_8041ff5c;

// .sdata
static u8 scale_data[8] = { 0x00, 0x1E, 0x50, 0x78, 0x70, 0x68, 0x64, 0x00 };
static u8 scale_data2[8] = { 0x5E, 0x5C, 0x64, 0x69, 0x6B, 0x69, 0x64, 0x00 };

void effStoneSmokeMain(EffectEntry* effect) {
	effStoneSmokeN64Data* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effGlassMain). */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };
	s32 frame;

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
	data->field_0x14++;

	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	frame = data->field_0x14;

	if (data->life < 16) {
		data->field_0x24 = data->life * 16;
	}

	data->y += float_0p6_8041ff58;

	if (frame < 7) {
		data->field_0x38 = float_0p01_8041ff5c * (f32)scale_data[frame];
	} else {
		data->field_0x38 = float_0p01_8041ff5c * (f32)scale_data2[(frame - 7) % 7];
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effStoneSmokeDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effStoneSmokeDisp, effect);
#endif
}

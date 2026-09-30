#include "eff/n64/eff_akari_sflush_n64.h"
#include <dolphin/mtx.h>
#include "driver/dispdrv.h"
#include "matching.h"
#include "memory.h"

EffectEntry* effAkariSflushN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffAkariSflushData* data;

	effect = effEntry();
	effect->type = "AkariSflushN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffAkariSflushData));
	effect->userdata = data;
	effect->callback = effAkariSflushMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (count <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = count;
	}
	data->field_0x24 = 0xFF;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0xFF;
	data->field_0x1C = 0xE6;
	data->field_0x20 = 0x32;
	data->field_0x30 = 0;
	data->field_0x2C = 1.0f;
	return effect;
}

void effAkariSflushMain(EffectEntry* effect) {
	EffAkariSflushData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effGlassMain/effFlameMain). */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 life;
	s32 counter;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 16;
	}

	if (data->field_0x10 < 1000) {
		data->field_0x10--;
	}
	data->field_0x14++;

	life = data->field_0x10;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	counter = data->field_0x14;

	if (life < 8) {
		data->field_0x2C += float_0p3_8041e920 * (8 - life);
	}

	if (counter < 16) {
		data->field_0x24 = counter * 16 + 15;
	}
	if (life < 8) {
		data->field_0x24 = life * 32 + 31;
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effAkariSflushDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effAkariSflushDisp, effect);
#endif
}

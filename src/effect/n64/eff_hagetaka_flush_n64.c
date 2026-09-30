#include "eff/n64/eff_hagetaka_flush_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"

EffectEntry* effHagetakaFlushN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	effHagetakaFlushData* data;

	effect = effEntry();
	effect->type = "HagetakaFlushN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effHagetakaFlushData));
	effect->userdata = data;
	effect->callback = effHagetakaFlushMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	data->field_0x10 = 10;
	data->field_0x24 = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x28 = a5;
	data->field_0x18 = 70;
	data->field_0x1C = 180;
	data->field_0x20 = 120;
	data->field_0x2C = 0.0f;
	return effect;
}

void effHagetakaFlushMain(EffectEntry* effect) {
	effHagetakaFlushData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 counter;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 10;
	}

	if (data->field_0x10 < 1000) {
		data->field_0x10--;
	}
	data->field_0x14++;

	if (data->field_0x10 < 0) {
		effDelete(effect);
		return;
	}

	counter = data->field_0x14;
	if (data->field_0x10 < 4) {
		data->field_0x2C *= 0.5f;
	}
	else {
		data->field_0x2C = counter * 0.2f + 0.3f;
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effHagetakaFlushDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effHagetakaFlushDisp, effect);
#endif
}

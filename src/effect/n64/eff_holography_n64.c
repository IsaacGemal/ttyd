#include "eff/n64/eff_holography_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>

// .rodata
const Vec vec3_802fa7d0 = { 0.0f, 0.0f, 0.0f };

// .sdata2
extern const f32 float_0p1_8041f144;

EffectEntry* effHolographyN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffHolographyData* data;

	effect = effEntry();
	effect->type = "HolographyN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffHolographyData));
	effect->userdata = data;
	effect->callback = effHolographyMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (count <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = count;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x28 = float_0p1_8041f144 * scale;
	data->field_0x18 = 70;
	data->field_0x1C = 180;
	data->field_0x20 = 120;
	return effect;
}

void effHolographyMain(EffectEntry* effect) {
	EffHolographyData* data;
	s32 timer;
	Vec pos2;
	Vec pos;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 16;
	}

	timer = data->field_0x10;
	if (timer < 1000) {
		data->field_0x10 = timer - 1;
	}
	data->field_0x14++;
	timer = data->field_0x10;

	if (timer < 0) {
		effDelete(effect);
		return;
	}

	if (data->field_0x14 < 16) {
		data->field_0x24 = data->field_0x14 * 16 + 15;
	} else {
		data->field_0x24 = 0xFF;
	}
	if (timer < 16) {
		data->field_0x24 = timer * 16;
	}

	dispEntry(4, dispCalcZ(&pos2), 2, effHolographyDisp, effect);
}

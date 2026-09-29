#include "eff/n64/eff_stone_smoke_n64.h"
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

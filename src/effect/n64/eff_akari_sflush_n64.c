#include "eff/n64/eff_akari_sflush_n64.h"
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

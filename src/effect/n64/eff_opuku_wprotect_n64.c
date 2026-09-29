#include "eff/n64/eff_opuku_wprotect_n64.h"
#include "memory.h"

extern const f32 float_0_8041f890;

const char str_802FB300[] = "OpukuWprotectN64";

EffectEntry* effOpukuWprotectN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effOpukuWprotectData* data;

	effect = effEntry();
	effect->type = str_802FB300;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x98);
	effect->userdata = data;
	effect->callback = effOpukuWprotectMain;
	effect->flags |= 2;

	data->type = type;
	data->field_0x14 = 0;
	if (duration <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = duration;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0x2F;
	data->field_0x1C = 0x7F;
	data->field_0x20 = 0xFF;
	data->field_0x30 = float_0_8041f890;
	data->field_0x34 = float_0_8041f890;
	data->field_0x88 = -1;
	data->field_0x8C = -5;
	data->field_0x90 = -9;
	data->field_0x94 = -0xD;

	return effect;
}

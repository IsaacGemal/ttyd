#include "eff/n64/eff_fireworks_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"

// .rodata
const char str_802FA650[16] = "FireworksN64";

EffectEntry* effFireworksN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 a5, f32 a6, f32 a7, f32 a8) {
	EffectEntry* entry;
	effFireworksData* data;
	s32 i;

	entry = effEntry();
	entry->type = str_802FA650;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFireworksData));
	entry->userdata = data;
	entry->callback = effFireworksMain;
	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	}
	else {
		data->life = life + 0x20;
	}
	data->field_0x4C = 0;
	data->color0[3] = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x44 = 0.0f;
	data->field_0x10 = a5;
	data->field_0x14 = a6;
	data->field_0x18 = a7;
	data->field_0x48 = a8;
	data->color0[0] = 0xff;
	data->color0[1] = 0xff;
	data->color0[2] = 0x64;
	data->color1[0] = 0xff;
	data->color1[1] = 0x64;
	data->color1[2] = 0x96;
	data->color1[3] = 0xff;
	for (i = 0; i < 4; i++) {
		data->field_0x50[i] = data->x;
		data->field_0x60[i] = data->y - 1000.0f;
		data->field_0x70[i] = data->z;
		data->field_0x80[i] = 0.0f;
		data->field_0x90[i] = 0.0f;
		data->field_0xA0[i] = 0.0f;
	}
	return entry;
}

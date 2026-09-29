#include "eff/n64/eff_opuku_wcurtain_n64.h"
#include "memory.h"

#include <stdlib.h>

// .rodata
const char str_802FB2A4[] = "OpukuWcurtainN64";

// .sdata2
extern const f32 float_0_8041f820;
extern const f32 float_neg1_8041f834;
extern const f32 float_0p01_8041f838;
extern const f32 float_0p5_8041f83c;

EffectEntry* effOpukuWcurtainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effOpukuWcurtainData* data;
	s32 i;

	entry = effEntry();
	entry->type = str_802FB2A4;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effOpukuWcurtainData));
	entry->userdata = data;
	entry->callback = effOpukuWcurtainMain;
	entry->flags |= 2;

	data->type = type;
	data->counter = 0;
	if (duration <= 0) {
		data->duration = 1000;
	} else {
		data->duration = duration;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 20;
	data->field_0x1C = 120;
	data->field_0x20 = 255;
	for (i = 0; i < 12; i++) {
		data->field_0x2C[i] = float_0_8041f820;
		data->field_0x5C[i] = (rand() % 11) * float_0p01_8041f838 + float_neg1_8041f834;
		data->field_0x5C[i] *= float_0p5_8041f83c;
	}

	return entry;
}

#include "eff/n64/eff_opuku_jetwrain_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

const char str_802FB280[20] = "OpukuJetwrainN64";

EffectEntry* effOpukuJetwrainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effOpukuJetwrainData* data;
	s32 i;
	s32 j;

	effect = effEntry();
	effect->type = str_802FB280;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x308);
	effect->userdata = data;
	effect->callback = effOpukuJetwrainMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (duration <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = duration;
	}
	data->field_0x1F = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x1C = 10;
	data->field_0x1D = 0x6e;
	data->field_0x1E = 0xff;
	data->field_0x20 = 0xbe;
	data->field_0x21 = 0xdc;
	data->field_0x22 = 0xff;
	for (i = 0; i < 19; i++) {
		for (j = 0; 13 > j; j++) {
			data->field_0x23[i][j] = 0;
			data->field_0x11A[i][j] = 0;
		}
	}
	return effect;
}

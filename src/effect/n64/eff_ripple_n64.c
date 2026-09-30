#include "eff/n64/eff_ripple_n64.h"
#include "eff/eff_ripple.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <stdlib.h>

EffectEntry* effRippleN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffRippleData* data;

	effect = effEntry();
	effect->type = "RippleN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffRippleData));
	effect->userdata = data;
	effect->callback = effRippleMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x18 = 0;
	data->field_0x20 = 0;
	data->rx = 0.0f;
	data->rz = 0.0f;
	data->field_0x14 = 1.0f;
	data->camId = 4;
	switch (type) {
		case 1:
		case 10:
			data->field_0x10 = 0.6f;
			break;
		case 2:
		case 20:
			data->field_0x10 = 10.0f;
			break;
		case 3:
		case 30:
			data->field_0x10 = 10.0f;
			break;
		default:
			data->field_0x10 = 1.0f;
			break;
	}
	data->field_0x1C = 0x20;
	data->field_0x24 = rand() % 32;
	data->field_0x28 = y + scale;
	return effect;
}

void effRippleN64SetRxRz(EffectEntry* effect, f32 rx, f32 rz) {
	effRippleN64Data* data = effect->userdata;

	data->field_0x2C = rx;
	data->field_0x30 = rz;
}

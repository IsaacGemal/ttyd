#include "eff/n64/eff_shock_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

EffectEntry* effShockN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6) {
	EffectEntry* effect;
	EffShockData* data;

	effect = effEntry();
	effect->type = "ShockN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffShockData));
	effect->userdata = data;
	effect->callback = effShockMain;

	data->type = type;
	data->field_0x20 = 0;
	data->field_0x1C = 30;
	data->field_0x10 = 0xFF;
	data->field_0x14 = 0xFF;
	data->field_0x18 = 0xFF;
	data->x = x;
	data->y = y;
	data->z = z;

	if (type == 2) {
		data->field_0x24 = 1.0f;
	} else {
		data->field_0x24 = 0.0f;
	}

	data->field_0x28 = 0.5f * a5;
	data->field_0x2C = 0.5f * a6;

	switch (type) {
	case 0:
	case 1:
	case 3:
		data->field_0x30 = 0xFF;
		data->field_0x34 = 0xFF;
		data->field_0x38 = 0;
		data->field_0x3C = 0xFF;
		data->field_0x40 = 0xFF;
		data->field_0x44 = 0xFF;
		break;
	default:
		data->field_0x30 = 0x46;
		data->field_0x34 = 0xB4;
		data->field_0x38 = 0x78;
		data->field_0x3C = 0xFF;
		data->field_0x40 = 0xFF;
		data->field_0x44 = 0xFF;
		break;
	}

	return effect;
}

#include "eff/n64/eff_crystal_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

typedef struct EffCrystalData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	s32 lifetime;   // 0x10
	s32 timer;      // 0x14
	s32 field_0x18; // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	s32 field_0x24; // 0x24
	s32 field_0x28; // 0x28
	s32 field_0x2C; // 0x2C
	f32 scale;      // 0x30
	f32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
} EffCrystalData;

EffectEntry* effCrystalN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffCrystalData* data;

	effect = effEntry();
	effect->type = "CrystalN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffCrystalData));
	effect->userdata = data;
	effect->callback = effCrystalMain;
	effect->flags |= 2;

	data->type = type;
	data->timer = 0;
	data->field_0x38 = 4;
	if (time <= 0) {
		data->lifetime = 100;
	} else {
		data->lifetime = time;
	}
	data->field_0x2C = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0;

	switch (type) {
	case 0:
		data->field_0x1C = 0;
		break;
	case 1:
		data->field_0x1C = 2;
		break;
	case 2:
		data->field_0x1C = 4;
		break;
	case 3:
		data->field_0x1C = 6;
		break;
	case 4:
		data->field_0x1C = 7;
		break;
	case 5:
		data->field_0x1C = 8;
		break;
	case 6:
		data->field_0x1C = 9;
		break;
	default:
		data->field_0x1C = 10;
		data->field_0x34 = -(scale - 0.1f) / (f32)time;
		break;
	}

	return effect;
}

#include "eff/n64/eff_sunlight_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

EffectEntry* effSunlightN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effSunlightData* data;

	entry = effEntry();
	entry->type = "SunlightN64";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effSunlightData));
	entry->userdata = data;
	entry->callback = effSunlightMain;
	entry->flags |= 2;
	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	}
	else {
		data->life = life;
	}
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->r0 = 0xff;
	data->g0 = 0xff;
	data->b0 = 0xff;
	data->r1 = 0xff;
	data->g1 = 0xff;
	data->b1 = 0xff;
	data->a1 = 0xff;
	data->field_0x20 = 0.0f;
	data->field_0x24 = 0.0f;
	data->field_0x28 = 0.0f;
	data->field_0x2C = 0.0f;
	data->field_0x30 = 0.0f;
	data->field_0x3C = 0xff;
	data->field_0x38 = 0;

	return entry;
}

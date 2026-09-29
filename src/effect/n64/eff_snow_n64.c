#include "eff/n64/eff_snow_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

EffectEntry* effSnowN64Entry(s32 type, f32 x, f32 y, f32 z) {
	static s32 kaiten;
	EffectEntry* effect;
	effSnowData* data;

	effect = effEntry();
	effect->type = "SnowN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effSnowData));
	effect->userdata = data;
	effect->callback = effSnowMain;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x0C = 0.0f;
	data->field_0x10 = -1.5f;
	data->field_0x14 = 0.0f;
	data->field_0x18 = 0.0f;
	data->field_0x1C = 0.0f;
	data->field_0x20 = 1.5f;
	data->field_0x24 = kaiten != 0 ? -5.0f : 5.0f;
	data->type = type;
	kaiten++;
	if (kaiten > 1) {
		kaiten = 0;
	}
	return effect;
}

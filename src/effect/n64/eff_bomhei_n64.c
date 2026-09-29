#include "eff/n64/eff_bomhei_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

EffectEntry* effBomheiN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	effBomheiData* data;

	effect = effEntry();
	effect->type = "BomheiN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effBomheiData));
	effect->userdata = data;
	effect->callback = effBomheiMain;
	data->field_0x14 = count;
	data->field_0x18 = 0;
	data->field_0x1C = count;
	data->field_0x20 = 0.0f;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x10 = a5;
	data->field_0x24 = 0.0f;
	if (type == 0) {
		data->field_0x28 = 0.0f;
	}
	else {
		data->field_0x28 = 40.0f;
	}
	return effect;
}

#include "eff/n64/eff_vapor_n64.h"
#include "memory.h"

EffectEntry* effVaporN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effVaporN64Data* data;

	effect = effEntry();
	effect->type = "VaporN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effVaporN64Data));
	effect->userdata = data;
	effect->callback = effVaporMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = lifetime;
	}
	data->field_0x24 = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0xff;
	data->field_0x1c = 0xff;
	data->field_0x20 = 0xff;
	data->field_0x28 = 0xff;
	data->field_0x2c = 0xff;
	data->field_0x30 = 0xff;
	data->field_0x34 = 0xff;
	data->field_0x38 = 0.0f;
	data->field_0x3c = 0.0f;
	return effect;
}

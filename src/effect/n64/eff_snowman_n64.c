#include "eff/n64/eff_snowman_n64.h"
#include "memory.h"

EffectEntry* effSnowmanN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effSnowmanN64Data* data;

	effect = effEntry();
	effect->type = "SnowmanN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effSnowmanN64Data));
	effect->userdata = data;
	effect->callback = effSnowmanMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = lifetime;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x30 = 0.0f;
	data->field_0x34 = 200.0f;
	data->field_0x38 = 0.0f;
	data->field_0x3C = 0.0f;
	data->field_0x40 = 0.0f;
	data->field_0x44 = 0.0f;
	data->field_0x54 = 0;
	data->state = 2;
	data->scale = scale;
	data->field_0x5C = 0;
	data->field_0x18 = 0xc8;
	data->field_0x1C = 0xdc;
	data->field_0x20 = 0xf0;
	data->field_0x40 = -8.0f;
	data->field_0x2C = 0.0f;
	data->field_0x50 = -1.0f;
	return effect;
}

void effSnowmanDisp(CameraId cameraId, void* param) {
}

#include "eff/n64/eff_fireflower_n64.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <stdlib.h>

EffectEntry* effFireFlowerN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effFireFlowerData* data;
	effFireFlowerData* p;
	s32 i;

	effect = effEntry();
	effect->type = "FireFlowerN64";
	effect->count = 0x1f;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFireFlowerData) * 0x1f);
	effect->userdata = data;
	effect->callback = effFireFlowerMain;

	if (type == 1) {
		data->mode = 1;
		type = 0;
	}
	data->state = 0;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	if (duration <= 0) {
		duration = 1000;
	}
	data->lifetime = duration;
	data->timer = 0;
	data->field_0x48 = 0;
	data->field_0x3C = 0xff;
	data->scale = 0.5f;
	data->field_0x34 = 0;
	data->field_0x38 = 0;
	data->field_0x28 = 0.0f;
	data->field_0x2C = -10.0f;

	p = &data[1];
	for (i = 1; i < 0x1f; i++, p++) {
		p->type = 1;
		p->x = 0.0f;
		p->y = 0.0f;
		p->z = 0.0f;
		p->velX = 8.0f;
		p->velY = 0.0f;
		p->velZ = (f32)(rand() % 20 - 8);
		p->field_0x20 = 0.05f * (f32)(rand() % 10 - 5);
		p->field_0x24 = (f32)(rand() % 80 + 5);
		p->field_0x3C = 0xff;
		p->field_0x4C = i - 1;
		p->timer = 10;
		p->velX /= 1.5f;
		p->velY /= 1.5f;
		p->velZ /= 1.5f;
		p->field_0x4C *= 2;
	}
	return effect;
}

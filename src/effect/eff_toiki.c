#include "eff/eff_toiki.h"
#include "memory.h"
#include <math.h>

const char str_80421C90[] = "Toiki";

EffectEntry* effToikiEntry(s32 type, s32 life, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	ToikiData* data;
	f32 angle;

	entry = effEntry();
	entry->type = str_80421C90;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x34);
	entry->userdata = data;
	entry->callback = effToikiMain;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = 0.5f;
	data->rotation = 0.0f;
	data->life = life;
	data->field_0x2C = 0;
	data->field_0x28 = 255.0f;
	angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
	data->velX = 2.5f * (f32)cos(angle);
	angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
	data->velY = 2.5f * (f32)sin(angle);
	data->velZ = 0.0f;
	data->field_0x24 = 0.0f;
	return entry;
}

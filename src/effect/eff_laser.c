#include "eff/eff_laser.h"
#include "memory.h"

EffectEntry* effLaserEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effLaserData* data;

	entry = effEntry();
	entry->type = "Laser";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effLaserData) * entry->count);
	entry->userdata = data;
	entry->callback = effLaserMain;
	entry->flags |= 2;
	data->field_0x0 = type;
	data->field_0x10 = x;
	data->field_0x14 = y;
	data->field_0x18 = z;
	data->position.x = data->field_0x10;
	data->position.y = 1000.0f + data->field_0x14;
	data->position.z = data->field_0x18;
	data->scale = 1.0f;
	data->field_0x24 = 0;
	data->timer = 0;
	data->animId = -1;
	data->alpha = 0xff;
	data->state = 0;
	return entry;
}

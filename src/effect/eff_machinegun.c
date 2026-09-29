#include "eff/eff_machinegun.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern const char str_8030142C[];

EffectEntry* effMachinegunEntry(s32 type, f32 x, f32 y, f32 z, f32 angle, f32 scale, f32 field_0x10) {
	EffectEntry* entry;
	effMachinegunData* data;

	entry = effEntry();
	entry->type = str_8030142C;
	entry->count = 1;
	data = __memAlloc(3, 0x28);
	entry->userdata = data;
	entry->callback = effMachinegunMain;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->angle = angle;
	data->field_0x10 = field_0x10;
	data->lifetime = 0xFF;
	data->scale = scale;
	return entry;
}

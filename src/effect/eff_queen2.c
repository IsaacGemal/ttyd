#include "eff/eff_queen2.h"
#include "driver/camdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>

EffectEntry* effQueen2Entry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effQueen2Data* data;
	Vec pos;

	entry = effEntry();
	entry->type = "Queen2";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effQueen2Data) * entry->count);
	entry->userdata = data;
	entry->callback = effQueen2Main;
	entry->flags |= 2;

	data->field_0x0 = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;

	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = x;
	pos.y = y;
	pos.z = z;

	data->field_0x14 = PSVECDistance(&camGetPtr(CAMERA_3D)->cameraPos, &pos) / 10.0f;
	data->field_0x10 = data->field_0x14;
	data->field_0x18 = 0;
	data->field_0x24 = 0;
	data->field_0x20 = 0;
	data->field_0x28 = NULL;

	return entry;
}

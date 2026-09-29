#include "eff/eff_treasure_map.h"
#include "driver/effdrv.h"
#include "memory.h"

extern const char str_802FE770[];
extern const f32 float_1_804213e8;
extern const f32 float_0_804213dc;

EffectEntry* effTreasureMapEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	EffTreasureMapData* data;

	entry = effEntry();
	entry->type = str_802FE770;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffTreasureMapData));
	entry->userdata = data;
	entry->callback = effTreasureMapMain;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->timer = 1000;
	data->alpha = 0xff;
	data->scale = float_1_804213e8;
	data->angle = float_0_804213dc;
	data->animId = -1;
	data->cameraId = 4;
	return entry;
}

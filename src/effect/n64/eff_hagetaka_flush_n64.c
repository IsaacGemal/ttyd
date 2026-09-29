#include "eff/n64/eff_hagetaka_flush_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

EffectEntry* effHagetakaFlushN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	effHagetakaFlushData* data;

	effect = effEntry();
	effect->type = "HagetakaFlushN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effHagetakaFlushData));
	effect->userdata = data;
	effect->callback = effHagetakaFlushMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	data->field_0x10 = 10;
	data->field_0x24 = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x28 = a5;
	data->field_0x18 = 70;
	data->field_0x1C = 180;
	data->field_0x20 = 120;
	data->field_0x2C = 0.0f;
	return effect;
}

#include "eff/n64/eff_freeze_n64.h"
#include "eff/n64/eff_snow_dust_n64.h"
#include "eff/eff_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

// .sdata2 constants shared with the rest of the unit.
extern const f32 float_10_8041f010;
extern const f32 float_32_8041f030;
extern const f32 float_30_8041f034;

// .rodata
const char str_802FA6C0[] = "FreezeN64";

EffectEntry* effFreezeN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effFreezeData* data;

	entry = effEntry();
	entry->type = str_802FA6C0;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFreezeData));
	entry->userdata = data;
	entry->callback = effFreezeMain;
	entry->flags |= 2;

	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	}
	else {
		data->life = life;
	}
	data->color0[3] = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->color0[0] = 0xff;
	data->color0[1] = 0xff;
	data->color0[2] = 0xff;
	data->color1[0] = 0xf0;
	data->color1[1] = 0xff;
	data->color1[2] = 0xff;
	data->color1[3] = 0xff;
	data->field_0x25 = 0;
	data->field_0x24 = 0;
	data->snowDust = effSnowDustN64Entry(1, x, float_10_8041f010 + y, z, 4, 0, float_32_8041f030, float_30_8041f034, scale);
	return entry;
}

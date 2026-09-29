#include "eff/n64/eff_bush_n64.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <stdlib.h>

extern const char str_802FA390[];
extern const f32 float_0_8041eb28;

const f64 double_to_int_802fa388 = 4503601774854144.0;

EffectEntry* effBushN64Entry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	BushN64Data* data;
	BushN64Data* p;
	s32 i;

	effect = effEntry();
	effect->type = str_802FA390;
	effect->count = 5;
	data = __memAlloc(HEAP_EFFECT, 0x104);
	effect->userdata = data;
	effect->callback = effBushMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->lifetime = 100;
	data->field_0x2c = 0;
	data->field_0x24 = 0;
	p = data + 1;
	for (i = 1; i < 5; i++, p++) {
		p->x = (f32)(rand() % 30 - 15);
		p->y = float_0_8041eb28;
		p->z = (f32)((rand() & 0x1E) - 15);
		p->field_0x18 = (f32)(rand() & 0x168);
		p->field_0x20 = (f32)(rand() & 0x168);
		p->field_0x1c = (f32)((rand() % 100) / 10);
		p->field_0x10 = float_0_8041eb28;
		p->field_0x14 = float_0_8041eb28;
	}
	return effect;
}

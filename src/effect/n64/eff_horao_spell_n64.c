#include "eff/n64/eff_horao_spell_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

// .rodata
const char str_802FA810[] = "HoraoSpellN64";

EffectEntry* effHoraoSpellN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effHoraoSpellData* data;

	effect = effEntry();
	effect->type = str_802FA810;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x3C);
	effect->userdata = data;
	effect->callback = effHoraoSpellMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (life <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = life;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x38 = scale;
	data->field_0x18 = 70;
	data->field_0x1C = 180;
	data->field_0x20 = 120;
	data->field_0x28 = 20;
	data->field_0x2C = 230;
	data->field_0x30 = 50;
	data->field_0x34 = 255;
	return effect;
}

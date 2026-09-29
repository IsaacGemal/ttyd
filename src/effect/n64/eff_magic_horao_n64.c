#include "eff/n64/eff_magic_horao_n64.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <math.h>
#include <stdlib.h>

// .rodata
const char str_802FABA8[16] = "MagicHoraoN64";

EffectEntry* effMagicHoraoN64Entry(s32 type, s32 a1, s32 a2, s32 life, f32 x, f32 y, f32 z, f32 a4, f32 a5, f32 a6) {
	EffectEntry* effect;
	effMagicHoraoData* data;

	effect = effEntry();
	effect->type = str_802FABA8;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x48);
	effect->userdata = data;
	effect->callback = effMagicHoraoMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x28 = a6;
	data->field_0x14 = a4;
	data->field_0x1C = a1;
	data->field_0x18 = a5;
	data->field_0x20 = a2;
	data->alpha = float_1_8041f780;
	data->field_0x38 = rand() % 360;
	data->life = life;
	data->field_0x44 = 0;
	data->field_0x3C = a1;
	data->field_0x24 = a4;
	data->field_0x10 = float_0_8041f770;
	data->state = 0;
	return effect;
}

#include "eff/n64/eff_horao_spell_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>

// .rodata
const char str_802FA810[] = "HoraoSpellN64";
const Vec vec3_802fa7f8 = { 0.0f, 0.0f, 0.0f };

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

void effHoraoSpellMain(EffectEntry* effect) {
	effHoraoSpellData* data;
	s32 timer;
	s32 counter;
	Vec pos2;
	Vec pos;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 16;
	}

	timer = data->field_0x10;
	if (timer < 1000) {
		data->field_0x10 = timer - 1;
	}
	data->field_0x14++;
	timer = data->field_0x10;

	if (timer < 0) {
		effDelete(effect);
		return;
	}

	counter = data->field_0x14;
	if (timer < 16) {
		data->field_0x24 = timer * 16;
	}
	if (counter < 16) {
		data->field_0x24 = counter * 16 + 15;
	}
	dispEntry(4, dispCalcZ(&pos2), 2, effHoraoSpellDisp, effect);
}

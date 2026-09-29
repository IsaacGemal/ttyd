#include "eff/n64/eff_holography_n64.h"
#include "driver/dispdrv.h"

#include <dolphin/mtx.h>

// .rodata
const Vec vec3_802fa7d0 = { 0.0f, 0.0f, 0.0f };

void effHolographyMain(EffectEntry* effect) {
	EffHolographyData* data;
	s32 timer;
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

	if (data->field_0x14 < 16) {
		data->field_0x24 = data->field_0x14 * 16 + 15;
	} else {
		data->field_0x24 = 0xFF;
	}
	if (timer < 16) {
		data->field_0x24 = timer * 16;
	}

	dispEntry(4, dispCalcZ(&pos2), 2, effHolographyDisp, effect);
}

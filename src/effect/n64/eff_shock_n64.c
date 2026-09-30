#include "eff/n64/eff_shock_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>

// .sdata2 constants referenced by the original.
extern const f32 float_1p1_8041fc68;
extern const f32 float_0p05_8041fc6c;
extern const f32 float_8_8041fc70;
extern const f32 float_0p9_8041fc74;
extern const f32 float_0p8_8041fc78;

// .rodata constants owned by this unit in the original.
const Vec vec3_802fb5c8 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb5d8 = 4503601774854144.0;

EffectEntry* effShockN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6) {
	EffectEntry* effect;
	EffShockData* data;

	effect = effEntry();
	effect->type = "ShockN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffShockData));
	effect->userdata = data;
	effect->callback = effShockMain;

	data->type = type;
	data->field_0x20 = 0;
	data->field_0x1C = 30;
	data->field_0x10 = 0xFF;
	data->field_0x14 = 0xFF;
	data->field_0x18 = 0xFF;
	data->x = x;
	data->y = y;
	data->z = z;

	if (type == 2) {
		data->field_0x24 = 1.0f;
	} else {
		data->field_0x24 = 0.0f;
	}

	data->field_0x28 = 0.5f * a5;
	data->field_0x2C = 0.5f * a6;

	switch (type) {
	case 0:
	case 1:
	case 3:
		data->field_0x30 = 0xFF;
		data->field_0x34 = 0xFF;
		data->field_0x38 = 0;
		data->field_0x3C = 0xFF;
		data->field_0x40 = 0xFF;
		data->field_0x44 = 0xFF;
		break;
	default:
		data->field_0x30 = 0x46;
		data->field_0x34 = 0xB4;
		data->field_0x38 = 0x78;
		data->field_0x3C = 0xFF;
		data->field_0x40 = 0xFF;
		data->field_0x44 = 0xFF;
		break;
	}

	return effect;
}

void effShockMain(EffectEntry* effect) {
	EffShockData* data;
	Vec position2;
	Vec position;
	s32 type;
	s32 life;
	s32 frame;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	type = data->type;
	if (data->field_0x1C < 100) {
		data->field_0x1C--;
	}
	data->field_0x20++;
	life = data->field_0x1C;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	frame = data->field_0x20;
	if (type == 2) {
		if (frame <= 20) {
			data->field_0x24 *= float_1p1_8041fc68;
		}
	} else {
		data->field_0x24 += float_0p05_8041fc6c * (float_8_8041fc70 - data->field_0x24);
	}

	data->field_0x14 = (s32)((f32)data->field_0x14 * float_0p9_8041fc74);
	if (life < 5) {
		data->field_0x18 = (s32)((f32)data->field_0x18 * float_0p8_8041fc78);
	}

	if (frame > 6) {
		data->field_0x10 -= 13;
		if (data->field_0x10 < 0) {
			data->field_0x10 = 0;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effShockDisp, effect);
}

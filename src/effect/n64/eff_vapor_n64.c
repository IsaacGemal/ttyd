#include "eff/n64/eff_vapor_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

EffectEntry* effVaporN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effVaporN64Data* data;

	effect = effEntry();
	effect->type = "VaporN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effVaporN64Data));
	effect->userdata = data;
	effect->callback = effVaporMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = lifetime;
	}
	data->field_0x24 = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0xff;
	data->field_0x1c = 0xff;
	data->field_0x20 = 0xff;
	data->field_0x28 = 0xff;
	data->field_0x2c = 0xff;
	data->field_0x30 = 0xff;
	data->field_0x34 = 0xff;
	data->field_0x38 = 0.0f;
	data->field_0x3c = 0.0f;
	return effect;
}

// opt_common_subs off required: with CSE on MWCC reuses the 0.02f result register for
// data->field_0x3c in the y integration below (f0/f1 swapped vs the target's f2/f1).
#pragma opt_common_subs off

inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effVaporMain(EffectEntry* effect) {
	effVaporN64Data* data;
	Vec position2;
	Vec position;
	s32 type;

	data = effect->userdata;
	position = makeVec(data->x, data->y, data->z);
	position2 = position;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x10;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->field_0x14++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	if (data->lifetime < 0x10) {
		data->field_0x24 = data->lifetime << 4;
	}
	switch (type) {
	case 0:
	case 1:
		data->field_0x3c += float_0p02_80420100;
		break;
	}
	data->y += data->field_0x3c;
	if (data->field_0x3c > float_0p5_804200dc) {
		data->field_0x3c = float_0p5_804200dc;
	}
	if (data->field_0x3c < float_neg1p5_80420104) {
		data->field_0x3c = float_neg1p5_80420104;
	}
	data->field_0x38 += float_1_80420108;
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effVaporDisp, effect);
}
#pragma opt_common_subs reset

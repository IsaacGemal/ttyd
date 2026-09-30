#include "eff/n64/eff_hokori_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <stdlib.h>

// .sdata2 / .rodata constants used only by effHokoriN64Entry, named after their
// original addresses. The string lives in this unit's own .rodata.
extern const f32 float_0_8041f110;
extern const f32 float_neg1_8041f120;
extern const f32 float_1_8041f124;
extern const f32 float_200_8041f128;
extern const f32 float_0p1_8041f12c;
extern const f32 float_0p2_8041f130;
extern const f32 float_0p4_8041f134;
extern const f64 double_to_int_802fa7b8;
extern const char str_802FA7C0[];

EffectEntry* effHokoriN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effHokoriData* data;
	effHokoriData* p;
	s32 num;
	s32 i;

	effect = effEntry();
	num = (type <= 1) ? 5 : 4;
	effect->type = str_802FA7C0;
	effect->count = num;
	data = __memAlloc(HEAP_EFFECT, num * 0x48);
	effect->userdata = data;
	effect->callback = effHokoriMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->life = count;
	data->field_0x18 = 0xff;
	data->field_0x30 = 0xa0;
	data->field_0x34 = 0xa0;
	data->field_0x38 = 0x8c;
	data->field_0x3C = 0xa0;
	data->field_0x40 = 0xa0;
	data->field_0x44 = 0x8c;

	for (i = 1, p = data + 1; i < num; i++, p++) {
		p->x = (f32)(rand() % 200 - 100);
		p->y = (f32)(rand() % 50);
		p->z = float_0_8041f110;
		p->field_0x10 = float_neg1_8041f120;
		p->field_0x14 = float_0_8041f110;
		p->field_0x1C = float_1_8041f124;
		p->field_0x20 = float_1_8041f124;
		p->field_0x24 = (f32)(rand() % 10) / float_200_8041f128;
		switch (type) {
		case 0:
			p->field_0x28 = float_0p1_8041f12c * p->field_0x24;
			break;
		case 1:
			p->field_0x28 = float_0p2_8041f130 * p->field_0x24;
			break;
		default:
			p->field_0x24 = float_0p4_8041f134;
			p->field_0x28 = float_0p4_8041f134;
			p->field_0x10 = float_0_8041f110;
			p->x = float_0p1_8041f12c * (f32)(rand() % 40 - 20);
			p->y = float_0p1_8041f12c * (f32)(rand() % 40 - 20);
			break;
		}
	}
	return effect;
}

inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effHokoriMain(EffectEntry* effect) {
	effHokoriData* data;
	Vec position2;
	Vec position;
	s32 type;
	s32 i;

	data = effect->userdata;
	position = makeVec(data->x, data->y, data->z);
	position2 = position;
	data->life--;
	if (data->life < 0) {
		effDelete(effect);
	}
	else {
		type = data->type;
		if (data->life < 0x18) {
			data->field_0x18 = (s32)((f32)data->field_0x18 * float_0p9_8041f118);
		}
		i = 1;
		data++;
		if (type <= 1) {
			for (; i < effect->count; i++, data++) {
				data->field_0x10 += data->field_0x14;
				data->y += data->field_0x10;
				data->field_0x24 += float_0p005_8041f11c;
				data->field_0x1C += data->field_0x24;
				data->field_0x20 += data->field_0x28;
			}
		}
		else {
			for (; i < effect->count; i++, data++) {
				data->field_0x10 += data->field_0x14;
				data->y += data->field_0x10;
				data->field_0x1C += data->field_0x24;
				data->field_0x20 += data->field_0x28;
			}
		}
		dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effHokoriDisp, effect);
	}
}

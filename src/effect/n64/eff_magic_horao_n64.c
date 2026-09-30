#include "eff/n64/eff_magic_horao_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "eff/n64/eff_hokori_n64.h"
#include "memory.h"
#include "system.h"
#include <math.h>
#include <stdlib.h>

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// .rodata
const char str_802FABA8[16] = "MagicHoraoN64";

// .sdata2 owned by this unit; the zero Vec for initializeVec and the int->float
// magic are left to the compiler's constant pool (@NNN), which objdiff folds back
// to the original vec3_802fab90 / double_to_int_802faba0 symbols.
extern const f32 float_0p02_8041f77c;
extern const f32 float_5_8041f784;
extern const f32 float_0p05_8041f788;

// The unit's int->float magic constant (2^52 + 2^31), kept so the unit owns the
// symbol in .rodata as the original does.
const f64 double_to_int_802faba0 = 4503601774854144.0;

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

void effMagicHoraoMain(EffectEntry* effect) {
	effMagicHoraoData* data;
	Vec position;

	data = effect->userdata;
	position = initializeVec((Vec*)&data->x);
	data->life--;
	data->field_0x44++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	switch (data->state) {
	case 0:
		data->field_0x24 += data->field_0x28 * (data->field_0x18 - data->field_0x24);
		data->field_0x3C = (s32)(data->field_0x28 * (f32)(data->field_0x20 - data->field_0x3C) + (f32)data->field_0x3C);
		data->alpha -= float_0p02_8041f77c;
		if (data->alpha < 0.0f) {
			data->alpha = 0.0f;
			data->field_0x10 = 0.0f;
			data->field_0x30 = 10;
			data->state = 1;
		}
		break;
	case 1:
		data->field_0x30--;
		if (data->field_0x30 <= 0) {
			data->state = 2;
			data->field_0x30 = 10;
		}
		break;
	case 2:
		data->field_0x30--;
		if (data->field_0x30 <= 0) {
			data->state = 3;
		}
		break;
	case 3:
		data->field_0x10 -= float_1_8041f780;
		data->y += data->field_0x10;
		if (data->y < 0.0f) {
			data->y = 0.0f;
			effHokoriN64Entry(2, 30, float_5_8041f784 + data->x, data->y, data->z);
			effHokoriN64Entry(2, 30, data->x - float_5_8041f784, data->y, data->z);
			data->state = 4;
			data->field_0x30 = 20;
		}
		break;
	case 4:
		data->field_0x30--;
		if (data->field_0x30 <= 0) {
			data->state = 5;
		}
		break;
	case 5:
		if (data->life < 10) {
			data->field_0x3C = data->life * 25;
		}
		data->alpha -= float_0p05_8041f788;
		break;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMagicHoraoDisp, effect);
}

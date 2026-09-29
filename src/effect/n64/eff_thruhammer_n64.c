#include "eff/n64/eff_thruhammer_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
#include <math.h>

//local helper
static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// NONMATCHING (99.80%): only the sin block's register allocation differs (sin in f3
// instead of f1); the rest are anonymous .sdata2/.rodata pool labels.
void effThruHammerMain(EffectEntry* effect) {
	EffThruHammerData* data = effect->userdata;
	Vec disp;
	s32 i;

	disp = initializeVec((Vec*)&data->x);

	data->field_0x2C++;
	data->field_0x28--;
	if (data->field_0x28 < 0) {
		effDelete(effect);
		return;
	}
	if (data->field_0x2C >= 8) {
		data->field_0x38 *= 0.8f;
	}
	for (i = 1, data++; i < effect->count; i++, data++) {
		data->field_0x4C++;
		if (data->field_0x4C < 0) {
			continue;
		}
		if (data->field_0x4C == 0) {
			data->x = 0.0f;
			data->y = 0.0f;
			data->z = 0.0f;
			data->field_0x10 = -(f32)sin(6.2831855f * data->field_0x34 / 360.0f) * 0.5f;
			data->field_0x14 = 0.5f * (f32)cos(6.2831855f * data->field_0x34 / 360.0f);
			data->field_0x18 = 0.0f;
			data->field_0x3C = 32.0f;
			data->field_0x40 = 32.0f;
			data->field_0x48 = 32.0f;
			data->field_0x20 = 10.0f;
			data->field_0x1C = 0.5f;
			data->field_0x24 = 0.5f;
			data->field_0x44 = 32.0f;
		}
		data->field_0x3C += data->field_0x40;
		data->field_0x44 += data->field_0x48;
		data->field_0x40 *= 0.9f;
		data->field_0x1C += 0.2f;
		data->field_0x24 += 0.2f;
		data->field_0x20 += (0.1f - data->field_0x20) * 0.2f;
		if (data->field_0x3C > 128.0f) {
			data->field_0x3C = 128.0f;
		}
	}
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effThruHammerDisp, effect);
}

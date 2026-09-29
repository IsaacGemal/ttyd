#include "eff/n64/eff_opuku_wgun_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <math.h>

// .rodata
const Vec vec3_802fb2c0 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb2d0 = 4503601774854144.0;

// .sdata2 float constants (named after their original addresses).
const f32 float_6p2832_8041f858 = 6.2831855f;
const f32 float_360_8041f85c = 360.0f;
const f32 float_180_8041f874 = 180.0f;
const f32 float_120_8041f878 = 120.0f;
const f32 float_255_8041f87c = 255.0f;

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = vec3_802fb2c0;
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effOpukuWgunMain(EffectEntry* effect) {
	EffOpukuWgunData* data = effect->userdata;
	Vec disp = initializeVec(&data->position);
	s32 i;
	s32 type = data->type;
	s32 duration;
	f32 t;
	s32 value;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->timer = 0x10;
	}
	if (data->timer < 1000) {
		data->timer--;
	}
	data->frame++;
	if (data->timer < 0) {
		effDelete(effect);
		return;
	}
	duration = data->duration;
	data->field_0x54++;
	if (data->field_0x54 > 12) {
		data->field_0x54 = 12;
	}

	for (i = 0; i < 12; i++) {
		if (data->counter[i] != 0) {
			data->counter[i]--;
			if (data->counter[i] != 0) {
				continue;
			}
		}
		data->counter2[i]++;
		value = data->counter2[i];
		if (value >= duration) {
			value = duration;
		}
		t = (f32)value / (f32)duration;
		if (type == 0) {
			data->x[i] = data->start.x + t * (data->end.x - data->start.x);
			data->y[i] = data->start.y + t * (data->end.y - data->start.y);
			data->z[i] = data->start.z + t * (data->end.z - data->start.z);
		}
		else {
			data->x[i] = data->start.x + t * (data->end.x - data->start.x);
			data->y[i] = data->start.y + t * (data->end.y - data->start.y)
			           + float_120_8041f878 * (f32)sin(float_6p2832_8041f858 * (float_180_8041f874 * t) / float_360_8041f85c);
			data->z[i] = data->start.z + t * (data->end.z - data->start.z);
			data->alpha[i] = (u8)(float_255_8041f87c - float_255_8041f87c * t);
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effOpukuWgunDisp, effect);
}

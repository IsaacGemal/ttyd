#include "eff/n64/eff_opuku_wgun_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <math.h>

// .rodata
const Vec vec3_802fb2c0 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb2d0 = 4503601774854144.0;
const char str_802FB2D8[] = "OpukuWgunN64";

// .sdata2 float constants (named after their original addresses).
const f32 float_2_8041f844 = 2.0f;
const f32 float_0_8041f864 = 0.0f;
const f32 float_10_8041f880 = 10.0f;
const f32 float_6p2832_8041f858 = 6.2831855f;
const f32 float_360_8041f85c = 360.0f;
const f32 float_180_8041f874 = 180.0f;
const f32 float_120_8041f878 = 120.0f;
const f32 float_255_8041f87c = 255.0f;

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effOpukuWgunN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale) {
	EffectEntry* effect;
	EffOpukuWgunData* data;
	s32 i;

	effect = effEntry();
	effect->type = str_802FB2D8;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffOpukuWgunData));
	effect->userdata = data;
	effect->callback = effOpukuWgunMain;
	effect->flags |= 2;

	data->type = type;
	data->frame = 0;
	if (duration <= 0) {
		data->timer = 1000;
	} else if (type == 0) {
		data->timer = duration * 2;
	} else {
		data->timer = duration * 4;
	}
	data->duration = duration;
	if (type == 0) {
		data->field_0x40 = 200;
	} else {
		data->field_0x40 = 255;
	}
	data->start.x = x;
	data->start.y = y;
	data->start.z = z;
	data->end.x = endX;
	data->end.y = endY;
	data->end.z = endZ;
	data->scale = scale;
	data->field_0x34 = 210;
	data->field_0x38 = 230;
	data->field_0x3C = 255;
	data->field_0x44 = 0;
	data->field_0x48 = 30;
	data->field_0x4C = 205;
	data->field_0x54 = 0;
	data->position.x = data->start.x;
	data->position.y = data->start.y;
	data->position.z = data->start.z;

	for (i = 0; i < 12; i++) {
		data->field_0x58[i] = 10.0f;
		data->field_0x88[i] = 2.0f;
		data->field_0xB8[i] = 0.0f;
		if (type == 0) {
			data->counter[i] = i + 1;
		} else {
			data->counter[i] = i * 5 + 1;
		}
		data->x[i] = data->position.x;
		data->y[i] = data->position.y;
		data->z[i] = data->position.z;
		data->counter2[i] = 0;
	}

	return effect;
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

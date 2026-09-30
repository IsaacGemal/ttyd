#include "eff/n64/eff_fireworks_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

// .sdata2 constants (named after their original addresses in the asm).
extern const f32 float_6p2832_8041ef28;
extern const f32 float_360_8041ef2c;
extern const f32 float_0p5_8041ef30;
extern const f32 float_480_8041ef34;
extern const f32 float_0_8041ef38;
extern const f32 float_608_8041ef3c;
extern const f32 float_1_8041ef40;
extern const f32 float_32768_8041ef44;
extern const f32 float_8_8041ef48;
extern const f32 float_0p95_8041ef4c;
extern const f32 float_0p15_8041ef50;
extern const f32 float_0p11_8041ef54;
extern const f32 float_80_8041ef58;
extern const f32 float_0p1_8041ef5c;
extern const f32 float_neg1000_8041ef60;
extern const f32 float_1000_8041ef64;

// .rodata
const Vec vec3_802fa600 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fa60c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fa618 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fa624 = { 0.0f, 0.0f, 1000.0f };
const Vec vec3_802fa630 = { 0.0f, 1.0f, 0.0f };
const f64 double_to_int_802fa640 = 4503601774854144.0;
const f64 double_to_int_mask_802fa648 = 4503599627370496.0;
const char str_802FA650[16] = "FireworksN64";

EffectEntry* effFireworksN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 a5, f32 a6, f32 a7, f32 a8) {
	EffectEntry* entry;
	effFireworksData* data;
	s32 i;

	entry = effEntry();
	entry->type = str_802FA650;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFireworksData));
	entry->userdata = data;
	entry->callback = effFireworksMain;
	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	}
	else {
		data->life = life + 0x20;
	}
	data->field_0x4C = 0;
	data->color0[3] = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x44 = 0.0f;
	data->field_0x10 = a5;
	data->field_0x14 = a6;
	data->field_0x18 = a7;
	data->field_0x48 = a8;
	data->color0[0] = 0xff;
	data->color0[1] = 0xff;
	data->color0[2] = 0x64;
	data->color1[0] = 0xff;
	data->color1[1] = 0x64;
	data->color1[2] = 0x96;
	data->color1[3] = 0xff;
	for (i = 0; i < 4; i++) {
		data->field_0x50[i] = data->x;
		data->field_0x60[i] = data->y - 1000.0f;
		data->field_0x70[i] = data->z;
		data->field_0x80[i] = 0.0f;
		data->field_0x90[i] = 0.0f;
		data->field_0xA0[i] = 0.0f;
	}
	return entry;
}

// NONMATCHING (97.38%): sin/angle FP registers are rotated one slot (frsp f5 vs f6,
// Bf in f1 vs f0, 80.0f in f0 vs f5) and the prologue loads the vec3_802fa600 words
// a few instructions earlier than the original. Everything else matches.
void effFireworksMain(EffectEntry* effect) {
	effFireworksData* data = effect->userdata;
	Vec position2;
	Vec position = vec3_802fa600;
	s32 counter;
	s32 i;
	f32 angle;
	f32 sinAngle;
	s32 b;

	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}
	if (data->life < 1000) {
		data->life--;
	}
	data->counter++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	if (data->life < 0x20) {
		data->color0[3] = data->life << 3;
	}

	if (data->field_0x4C == 1) {
		data->x += data->field_0x10;
		data->y += data->field_0x14;
		data->z += data->field_0x18;
		data->field_0x10 *= float_0p95_8041ef4c;
		data->field_0x14 *= float_0p95_8041ef4c;
		data->field_0x18 *= float_0p95_8041ef4c;
		data->field_0x14 -= float_0p15_8041ef50;
		data->field_0x44 += float_0p11_8041ef54 * (data->field_0x48 - data->field_0x44);
	}
	else {
		b = 32 - counter;
		data->field_0x50[counter & 3] = data->x - data->field_0x10 * (f32)b;
		angle = float_6p2832_8041ef28 * (f32)(counter * 90 >> 5) / float_360_8041ef2c;
		sinAngle = (f32)sin(angle);
		data->field_0x60[counter & 3] = data->y - data->field_0x14 * (f32)b - (float_80_8041ef58 - float_80_8041ef58 * sinAngle);
		data->field_0x70[counter & 3] = data->z - data->field_0x18 * (f32)b;
		data->field_0x80[counter & 3] = (f32)(rand() % 11 - 5) * float_0p1_8041ef5c;
		data->field_0x90[counter & 3] = (f32)(rand() % 11 - 5) * float_0p1_8041ef5c;
		data->field_0xA0[counter & 3] = (f32)(rand() % 11 - 5) * float_0p1_8041ef5c;
		for (i = 0; i < 4; i++) {
			data->field_0x50[i] += data->field_0x80[i];
			data->field_0x60[i] += data->field_0x90[i];
			data->field_0x70[i] += data->field_0xA0[i];
			data->field_0x90[i] -= float_0p15_8041ef50;
			if (counter >= 27) {
				data->field_0x60[i] = float_neg1000_8041ef60;
			}
		}
		if (counter >= 0x20) {
			data->field_0x4C = 1;
			data->counter = 1;
		}
	}

	dispEntry(4, dispCalcZ(&position2), 2, effFireworksDisp, effect);
}

// .sdata2 constant definitions (placed last so the functions can't fold
// their values and must reference the original symbols).
const f32 float_6p2832_8041ef28 = 6.2831855f;
const f32 float_360_8041ef2c = 360.0f;
const f32 float_0p5_8041ef30 = 0.5f;
const f32 float_480_8041ef34 = 480.0f;
const f32 float_0_8041ef38 = 0.0f;
const f32 float_608_8041ef3c = 608.0f;
const f32 float_1_8041ef40 = 1.0f;
const f32 float_32768_8041ef44 = 32768.0f;
const f32 float_8_8041ef48 = 8.0f;
const f32 float_0p95_8041ef4c = 0.95f;
const f32 float_0p15_8041ef50 = 0.15f;
const f32 float_0p11_8041ef54 = 0.11f;
const f32 float_80_8041ef58 = 80.0f;
const f32 float_0p1_8041ef5c = 0.1f;
const f32 float_neg1000_8041ef60 = -1000.0f;
const f32 float_1000_8041ef64 = 1000.0f;

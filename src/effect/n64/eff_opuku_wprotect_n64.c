#include "eff/n64/eff_opuku_wprotect_n64.h"
#include "driver/dispdrv.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "memory.h"

#include <dolphin/mtx.h>
#include <math.h>
#include <stdlib.h>

extern const f32 float_0_8041f890;
extern const f32 float_10_8041f898;
extern const f32 float_0p1_8041f8a4;
extern const f32 float_24_8041f8b0;
extern const f32 float_2_8041f8b4;
extern const f32 float_6p2832_8041f8b8;
extern const f32 float_360_8041f8bc;
extern const f32 float_127_8041f8c0;
extern const f32 float_32_8041f8c4;
extern const f32 float_0p01_8041f8c8;
extern const f32 float_6_8041f8cc;
extern const f32 float_256_8041f8d0;
extern const f32 float_0p2_8041f8d4;
extern const f32 float_128_8041f8d8;
extern const f32 float_240_8041f8dc;
extern const f32 float_0p97_8041f8e0;
extern const f32 float_480_8041f8e4;

// .rodata scratch vector the original zeroes before filling in the effect position.
const f64 double_to_int_802fb2f8 = 4503601774854144.0;

// Inline copy of system.c's helper: zero-fill a Vec from a literal, then
// overwrite it with the supplied position; the temporary is returned by value.
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// Local view of the userdata tail: five float arrays of 4 and a counter array of 4,
// all interleaved (stride 4 bytes), which the original walks with a single moving base.
typedef struct WprotectData {
	s32 type;           // 0x00
	f32 x;              // 0x04
	f32 y;              // 0x08
	f32 z;              // 0x0C
	s32 field_0x10;     // 0x10
	s32 field_0x14;     // 0x14
	s32 field_0x18;     // 0x18
	s32 field_0x1C;     // 0x1C
	s32 field_0x20;     // 0x20
	s32 field_0x24;     // 0x24
	s32 field_0x28;     // 0x28
	f32 scale;          // 0x2C
	f32 field_0x30;     // 0x30
	f32 field_0x34;     // 0x34
	f32 field_0x38[4];  // 0x38
	f32 field_0x48[4];  // 0x48
	f32 field_0x58[4];  // 0x58
	f32 field_0x68[4];  // 0x68
	f32 field_0x78[4];  // 0x78
	s32 field_0x88[4];  // 0x88
} WprotectData;         // 0x98

void effOpukuWprotectMain(EffectEntry* effect) {
	WprotectData* data = effect->userdata;
	Vec pos = initializeVec((Vec*)&data->x);
	s32 type = data->type;
	s32 f14;
	s32 i;
	f32 t;
	f32 s;
	f32 sin1;
	f32 sin2;
	f32 a1;
	f32 cos1;
	f32 cos2;

	if (effect->flags & 4) {
		if (type == 1) {
			effect->flags &= ~4;
			data->field_0x10 = 4;
		} else {
			effect->flags &= ~4;
			data->field_0x10 = 0x10;
		}
	}
	if (data->field_0x10 < 1000) {
		data->field_0x10--;
	}
	data->field_0x14++;
	if (data->field_0x14 > 0x4F1A0) {
		data->field_0x14 = 0x100;
	}

	if (data->field_0x10 < 0) {
		if (type == 1) {
			effWaterDamageN64Entry(0, 30, data->x, float_24_8041f8b0 + data->y, data->z, float_2_8041f8b4);
			effWaterDamageN64Entry(1, 30, data->x, float_24_8041f8b0 + data->y, data->z, float_2_8041f8b4);
		}
		effDelete(effect);
		return;
	}

	f14 = data->field_0x14;
	if (type == 0) {
		if (data->field_0x10 < 0x10) {
			s32 v = data->field_0x10 << 4;
			data->field_0x24 = v;
			data->field_0x28 = v;
		}
		if (f14 < 0x10) {
			data->field_0x24 = (f14 << 4) + 0xf;
			data->field_0x28 = (f14 << 4) + 0xf;
		}
	} else {
		if (f14 < 4) {
			data->field_0x24 = (f14 << 6) + 0x3f;
			data->field_0x28 = (f14 << 6) + 0x3f;
		}
	}

#ifdef MUST_MATCH
	// Original emits the multiply as `f14*6 + f14` (mulli 6; add) instead of a single *7.
	s = (f32)sin(float_6p2832_8041f8b8 * (f32)(f14 * 6 + f14) / float_360_8041f8bc);
#else
	s = (f32)sin(float_6p2832_8041f8b8 * (f32)(f14 * 7) / float_360_8041f8bc);
#endif
	data->field_0x1C = (u8)(s32)(float_32_8041f8c4 * s + float_127_8041f8c0);
	t = float_0p01_8041f8c8 * (f32)f14;
	a1 = float_6p2832_8041f8b8 * t / float_360_8041f8bc;
	sin1 = (f32)sin(a1);
	sin2 = (f32)sin(float_6p2832_8041f8b8 * (float_6_8041f8cc * t) / float_360_8041f8bc);
	cos1 = (f32)cos(float_6p2832_8041f8b8 * (float_2_8041f8b4 * t) / float_360_8041f8bc);
	cos2 = (f32)cos(a1);
	{
		f32 sin2s = float_32_8041f8c4 * sin2;
		data->field_0x30 = float_256_8041f8d0
		    + sin2s * sin1
		    + float_32_8041f8c4 * cos1
		    + float_32_8041f8c4 * cos2;
	}
	data->field_0x34 += float_0p2_8041f8d4;
	if (data->field_0x34 >= float_128_8041f8d8) {
		data->field_0x34 = float_0_8041f890;
	}

	for (i = 0; i < 4; i++) {
		data->field_0x88[i]++;
		if (data->field_0x88[i] < 0) {
			continue;
		}
		if (data->field_0x88[i] == 0) {
			data->field_0x38[i] = float_0_8041f890;
			data->field_0x48[i] = float_240_8041f8dc;
			data->field_0x58[i] = float_0p2_8041f8d4 * (f32)(rand() % 11 - 5);
			data->field_0x68[i] = (f32)(rand() % 11 + 2);
			data->field_0x78[i] = (f32)(rand() % 11 + 1);
		}
		data->field_0x38[i] += data->field_0x58[i];
		data->field_0x48[i] += data->field_0x68[i];
		data->field_0x58[i] *= float_0p97_8041f8e0;
		data->field_0x68[i] += float_0p1_8041f8a4 * (float_10_8041f898 - data->field_0x68[i]);
		if (data->field_0x48[i] > float_480_8041f8e4) {
			data->field_0x88[i] = -(rand() % 11) - 1;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effOpukuWprotectDisp, effect);
}

const char str_802FB300[] = "OpukuWprotectN64";

EffectEntry* effOpukuWprotectN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effOpukuWprotectData* data;

	effect = effEntry();
	effect->type = str_802FB300;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x98);
	effect->userdata = data;
	effect->callback = effOpukuWprotectMain;
	effect->flags |= 2;

	data->type = type;
	data->field_0x14 = 0;
	if (duration <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = duration;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0x2F;
	data->field_0x1C = 0x7F;
	data->field_0x20 = 0xFF;
	data->field_0x30 = float_0_8041f890;
	data->field_0x34 = float_0_8041f890;
	data->field_0x88 = -1;
	data->field_0x8C = -5;
	data->field_0x90 = -9;
	data->field_0x94 = -0xD;

	return effect;
}

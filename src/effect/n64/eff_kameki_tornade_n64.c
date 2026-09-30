#include "eff/n64/eff_kameki_tornade_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

typedef struct effKamekiTornadeData {
	s32 type;            // 0x00
	f32 x;               // 0x04
	f32 y;               // 0x08
	f32 z;               // 0x0C
	s32 field_0x10;      // 0x10
	s32 field_0x14;      // 0x14
	s32 field_0x18;      // 0x18
	s32 field_0x1C;      // 0x1C
	s32 field_0x20;      // 0x20
	s32 field_0x24;      // 0x24
	s32 field_0x28;      // 0x28
	s32 field_0x2C;      // 0x2C
	s32 field_0x30;      // 0x30
	f32 scale;           // 0x34
	f32 field_0x38[8];   // 0x38
	f32 field_0x58[8];   // 0x58
	f32 field_0x78[8];   // 0x78
	f32 field_0x98[8];   // 0x98
	f32 field_0xB8[8];   // 0xB8
	f32 field_0xD8[8];   // 0xD8
	f32 field_0xF8[8];   // 0xF8
	f32 field_0x118[8];  // 0x118
	f32 field_0x138;     // 0x138
	f32 field_0x13C;     // 0x13C
	f32 field_0x140;     // 0x140
	f32 field_0x144;     // 0x144
	f32 field_0x148;     // 0x148
} effKamekiTornadeData;

// .sdata2 float constants (named after their original addresses).
extern const f32 float_4_8041f208;
extern const f32 float_5_8041f204;
extern const f32 float_10_8041f1f4;
extern const f32 float_12p96_8041f214;
extern const f32 float_17p28_8041f218;
extern const f32 float_64_8041f21c;
extern const f32 float_90_8041f20c;
extern const f32 float_neg90_8041f210;
extern const f32 float_0p1_8041f1ec;
extern const f32 float_0_8041f1f8;
extern const f32 float_1_8041f220;
extern const f32 float_0p2_8041f224;
extern const f32 float_2_8041f228;
extern const char str_802FA870[];
const f64 double_to_int_802fa868 = 4503601774854144.0;

EffectEntry* effKamekiTornadeN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effKamekiTornadeData* data;
	s32 i;

	effect = effEntry();
	effect->type = str_802FA870;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKamekiTornadeData));
	effect->userdata = data;
	effect->callback = effKamekiTornadeMain;
	effect->flags |= 2;
	data->type = a;
	data->field_0x14 = 0;
	if (b <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = b;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0xFF;
	data->field_0x1C = 0xFF;
	data->field_0x20 = 0xFF;
	data->field_0x28 = 0xFF;
	data->field_0x2C = 0xFF;
	data->field_0x30 = 0xEB;
	for (i = 0; i < 8; i++) {
		data->field_0x38[i] = x;
		data->field_0x58[i] = y;
		data->field_0x78[i] = z;
		data->field_0x98[i] = float_0p2_8041f224 * i + float_1_8041f220;
		data->field_0xB8[i] = rand() % 360;
		data->field_0xD8[i] = float_0p1_8041f1ec * (rand() % 100) + float_2_8041f228;
		data->field_0xF8[i] = float_0p1_8041f1ec * (rand() % 30);
		data->field_0x118[i] = float_0_8041f1f8;
	}
	data->field_0x138 = float_0_8041f1f8;
	data->field_0x144 = float_0_8041f1f8;
	data->field_0x13C = float_0_8041f1f8;
	data->field_0x140 = float_0_8041f1f8;
	data->field_0x148 = float_1_8041f220;
	return effect;
}

// With common-subexpression elimination on, MWCC reuses the loop temporaries for the 4.0/90/-90
// constants and permutes every FPR in the loop and the 0x138/0x13C/0x140 rotation block; turning it
// off keeps retail's f4/f1/f0 constants and f2/f3 temporaries.
#pragma opt_common_subs off
void effKamekiTornadeMain(EffectEntry* effect) {
	effKamekiTornadeData* data;
	Vec position;
	Vec disp = { 0 };
	s32 i;
	s32 timer;
	f32 scale;
	f32 f5;

	data = effect->userdata;
	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x10 = 0x10;
	}
	if (data->field_0x10 < 1000) {
		data->field_0x10--;
	}
	data->field_0x14++;
	if (data->field_0x10 < 0) {
		effDelete(effect);
		return;
	}
	timer = data->field_0x14;
	scale = data->scale;
	if (data->field_0x10 < 0x10) {
		data->field_0x24 = data->field_0x10 << 4;
	}
	if (timer < 0x10) {
		data->field_0x24 = (timer << 4) + 0xF;
	}
	data->field_0x38[0] = data->x;
	data->field_0x58[0] = data->y;
	data->field_0x78[0] = data->z;
	f5 = float_5_8041f204 * scale;
	for (i = 7; i != 0; i--) {
		data->field_0x118[i] = float_4_8041f208 * (data->field_0x38[i - 1] - data->field_0x38[i]);
		data->field_0x38[i] = data->field_0x38[i - 1];
		data->field_0x58[i] = data->field_0x58[i - 1] + f5;
		data->field_0x78[i] = data->field_0x78[i - 1];
		if (data->field_0x118[i] > float_90_8041f20c) {
			data->field_0x118[i] = float_90_8041f20c;
		}
		else if (data->field_0x118[i] < float_neg90_8041f210) {
			data->field_0x118[i] = float_neg90_8041f210;
		}
	}
	data->field_0x138 += float_10_8041f1f4;
	data->field_0x13C += float_12p96_8041f214;
	data->field_0x140 += float_17p28_8041f218;
	if (data->field_0x13C > float_64_8041f21c) {
		data->field_0x13C -= float_64_8041f21c;
	}
	if (data->field_0x140 > float_64_8041f21c) {
		data->field_0x140 -= float_64_8041f21c;
	}
	for (i = 0; i < 8; i++) {
		data->field_0xB8[i] += data->field_0xD8[i];
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKamekiTornadeDisp, effect);
}

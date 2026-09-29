#include "eff/n64/eff_kemuri4_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <math.h>
#include <stdlib.h>


extern const f32 float_0_8041f330;
extern const f32 float_1_8041f338;
extern const f32 float_5p4_8041f33c;
extern const f32 float_0p5_8041f340;
extern const f32 float_neg0p07_8041f344;
extern const f32 float_0p003_8041f348;
extern const f32 float_6p2832_8041f34c;
extern const f32 float_360_8041f350;
// .rodata constants (named after their original addresses).
const char str_802FA904[] = "Kemuri4N64";
const f64 double_to_int_802fa910 = 4503601774854144.0;


EffectEntry* effKemuri4N64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 a5) {
	EffectEntry* effect;
	effKemuri4Data* data;
	f32 angle;
	s32 i;

	effect = effEntry();
	effect->type = str_802FA904;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri4Data));
	effect->userdata = data;
	effect->callback = effKemuri4Main;

	for (i = 0; i < 1; i++) {
		data->field_0x00 = 1;
		data->type = type;
		data->field_0x6C = float_0_8041f330;
		data->position.x = x;
		data->position.y = y;
		data->position.z = z;
		data->field_0x18 = float_1_8041f338;
		data->field_0x1C = float_1_8041f338;
		data->field_0x20 = float_1_8041f338;
		data->field_0x08 = 0xFF;
		data->field_0x68 = float_1_8041f338;
		data->field_0x64 = float_5p4_8041f33c;
		data->field_0x60 = (f32)(a5 != float_0_8041f330 ? -10 : 10);
		data->field_0x70 = float_0p5_8041f340;
		data->field_0x74 = float_neg0p07_8041f344;
		data->field_0x78 = float_0p003_8041f348;
		data->field_0x7C = float_0_8041f330;
		data->field_0x06 = 15;
		data->field_0x28 = a4;
		data->field_0x24 = float_0_8041f330;
		data->field_0x2C = float_0_8041f330;
		data->field_0x09 = ((rand() % 100) >> 4) & 1;
		angle = reviseAngle(a4 + (f32)(a5 != float_0_8041f330 ? -90 : 90));
		angle = float_6p2832_8041f34c * angle / float_360_8041f350;
		data->field_0x80 = (f32)sin(angle);
		data->field_0x84 = (f32)cos(angle);
	}
	return effect;
}
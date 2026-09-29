#include "eff/n64/eff_radiation_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"

// The original keeps the default loop's fourth table load as a separate element index in r8
// (`addi r8, r8, 2; slwi r0, r8, 2; lwzx`); MWCC folds our `r_data[row + 2]` into the third
// load's byte base (`add r3, r7, r0; lwz 0x8(r3)`), and the type/flag and row/i nonvolatile
// register picks are swapped. All other instructions, stores, constants and control flow match.
// NONMATCHING (98.28%): default loop's 4th table load folded into the 3rd load's byte base.
EffectEntry* effRadiationN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	EffRadiationData* data;
	s32 flag;
	s32 count;
	s32 i;
	s32 row;
	s32 n;

	effect = effEntry();
	flag = 0;
	if (type >= 10) {
		flag = 1;
		type %= 10;
	}

	count = 17;
	if (type <= 1) {
		count = 9;
	}

	effect->type = "RadiationN64";
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(EffRadiationData));
	effect->userdata = data;
	effect->callback = effRadiationMain;
	effect->flags |= 2;

	data->type = type;
	data->field_0x2C = 0;
	data->field_0x28 = 100;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x20 = a5;
	data->field_0x1C = a5;
	data->field_0x94 = flag;
	data->field_0x98 = 4;

	switch (type) {
	case 0:
	case 1:
		data++;
		for (i = 1; i < effect->count; i++, data++) {
			data->field_0x68 = (f32)((i - 1) * 30 / (count - 1) - 10);
			data->field_0x7C = 50 - (i - 1) * 20 / (count - 1);
			data->field_0x6C = 2.5f + 25.0f / (f32)data->field_0x7C;
			data->field_0x70 = (f32)(s32)effTblRandN64(180, i * 3);
			data->field_0x74 = (f32)(s32)effTblRandN64(180, i * 3 + 1);
			data->field_0x78 = (f32)(s32)effTblRandN64(180, i * 3 + 2);
			data->field_0x58 = data->field_0x68;
			data->field_0x28 = data->field_0x7C;
			data->field_0x8C = data->field_0x6C;
			data->field_0x5C = data->field_0x70;
			data->field_0x60 = data->field_0x74;
			data->field_0x64 = data->field_0x78;
			data->field_0x24 = 0;
			data->field_0x2C = 0;
		}
		break;
	default:
		data++;
		n = 32;
		for (i = 1; i < effect->count; i++, n += 2, data++) {
			data->field_0x58 = 0.0f;
			data->field_0x28 = n;
			row = (i - 1) * 5;
			data->field_0x5C = (f32)r_data[row];
			data->field_0x60 = (f32)r_data[++row];
			data->field_0x64 = (f32)r_data[++row];
			data->field_0x24 = 0;
			data->field_0x2C = 0;
			data->field_0x38 = 0.0f;
			data->field_0x34 = 0.0f;
			data->field_0x48 = 0.0f;
			data->field_0x4C = -0.6f;
			data->field_0x3C = 10.0f;
			data->field_0x40 = 0.0f;
			data->field_0x50 = 0.0f;
			data->field_0x54 = 0.6f;
			data->field_0x80 = 0.0f;
			data->field_0x84 = 0.0f;
			data->field_0x88 = (f32)r_data[row + 2] * 0.1f;
			data->field_0x90 = 0;
		}
		break;
	}

	return effect;
}

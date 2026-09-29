#include "eff/n64/eff_cloud_n64.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <stdlib.h>

EffectEntry* effCloudN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effCloudData* data;
	s32 i;

	effect = effEntry();
	effect->type = "CloudN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effCloudData));
	effect->userdata = data;
	effect->callback = effCloudMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = lifetime;
	}
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	switch (type) {
		case 0:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0xff;
			data->field_0x20 = 0xff;
			data->field_0x28 = 0xe1;
			data->field_0x2C = 0xe1;
			data->field_0x30 = 0xe1;
			data->field_0x24 = 0xff;
			break;
		case 1:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0xf5;
			data->field_0x20 = 0xe6;
			data->field_0x28 = 0x2d;
			data->field_0x2C = 0x23;
			data->field_0x30 = 0x0f;
			data->field_0x24 = 0xff;
			break;
		case 2:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0;
			data->field_0x20 = 0;
			data->field_0x28 = 0x9b;
			data->field_0x2C = 0x7d;
			data->field_0x30 = 0x7d;
			data->field_0x24 = 0xff;
			break;
		case 3:
			data->field_0x18 = 0xff;
			data->field_0x1C = 0xff;
			data->field_0x20 = 0xff;
			data->field_0x28 = 0xff;
			data->field_0x2C = 0x41;
			data->field_0x30 = 0x14;
			data->field_0x24 = 0xff;
			break;
		default:
			break;
	}
	data->field_0x38 = x;
	data->field_0x3C = y;
	data->field_0x40 = z;
#ifdef MUST_MATCH
	// The original unrolls this zeroing loop at the source level.
	data->field_0x230[0] = 0;
	data->field_0x230[1] = 0;
	data->field_0x230[2] = 0;
	data->field_0x230[3] = 0;
	data->field_0x230[4] = 0;
	data->field_0x230[5] = 0;
	data->field_0x230[6] = 0;
	data->field_0x230[7] = 0;
	data->field_0x230[8] = 0;
	data->field_0x230[9] = 0;
	data->field_0x230[10] = 0;
	data->field_0x230[11] = 0;
	data->field_0x230[12] = 0;
	data->field_0x230[13] = 0;
	data->field_0x230[14] = 0;
	data->field_0x230[15] = 0;
	data->field_0x230[16] = 0;
	data->field_0x230[17] = 0;
	data->field_0x230[18] = 0;
	data->field_0x230[19] = 0;
	data->field_0x230[20] = 0;
	data->field_0x230[21] = 0;
	data->field_0x230[22] = 0;
	data->field_0x230[23] = 0;
	data->field_0x230[24] = 0;
	data->field_0x230[25] = 0;
	data->field_0x230[26] = 0;
	data->field_0x230[27] = 0;
	data->field_0x230[28] = 0;
	data->field_0x230[29] = 0;
	data->field_0x2A8 = 0;
#else
	for (i = 0; i < 30; i++) {
		data->field_0x230[i] = 0;
	}
	data->field_0x2A8 = 0;
#endif
	data->field_0x50[0] = x;
	data->field_0xC8[0] = y;
	data->field_0x140[0] = z;
	data->field_0x230[0] = 1;
	data->field_0x1B8[0] = 0;
	data->field_0x2AC[0] = 0.0f;
	data->field_0x44 = (f32)(rand() % 30 + 10);
	data->field_0x48 = 0.0f;
	data->field_0x4C = 0.0f;
	return effect;
}

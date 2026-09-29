#include "eff/n64/eff_thunderflare_n64.h"
#include "memory.h"

typedef struct EffThunderflareData {
	s32 field_0x00; // 0x00
	s32 type;       // 0x04
	f32 x;          // 0x08
	f32 y;          // 0x0C
	f32 z;          // 0x10
	s32 alpha;      // 0x14
	s32 timer;      // 0x18
	s32 counter;    // 0x1C
	f32 angle1;     // 0x20
	f32 angle2;     // 0x24
	f32 scale;      // 0x28
	f32 radius;     // 0x2C
	s32 colorR;     // 0x30
	s32 colorG;     // 0x34
	s32 colorB;     // 0x38
	s32 field_0x3C; // 0x3C
	s32 field_0x40; // 0x40
	s32 field_0x44; // 0x44
} EffThunderflareData;

EffectEntry* effThunderflareN64Entry(s32 type, s32 a2, f32 x, f32 y, f32 z, f32 a6) {
	EffectEntry* entry;
	EffThunderflareData* data;
	EffThunderflareData* elem;
	s32 i;
	s32 j;

	entry = effEntry();
	entry->type = "ThunderflareN64";
	entry->count = 0x1f;
	data = __memAlloc(HEAP_EFFECT, 0x8b8);
	entry->userdata = data;
	entry->callback = effThunderflareMain;
	data->type = type;
	data->counter = 0;
	if (a2 <= 0) {
		data->timer = 100;
	} else {
		data->timer = a2;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;

	switch (type) {
	case 0:
		data->colorR = 0xff;
		data->colorG = 0;
		data->colorB = 0xff;
		data->field_0x3C = 0xff;
		data->field_0x40 = 0xff;
		data->field_0x44 = 0xff;
		break;
	case 1:
	default:
		data->colorR = 0xff;
		data->colorG = 0x14;
		data->colorB = 0x21;
		data->field_0x3C = 0x32;
		data->field_0x40 = 0x14;
		data->field_0x44 = 0x0a;
		break;
	}

	for (i = 1; i < 0x1f; i += 2) {
		elem = &data[i];
		for (j = 0; j < 2; j++, elem++) {
			s32 n = j == 0 ? i - 1 : i;
			elem->angle1 = (f32)(n * 0xca8 / 30);
			elem->angle2 = 0.0f;
			elem->radius = (f32)(n * 100 / 30);
			elem->field_0x00 = 0;
		}
	}
	return entry;
}

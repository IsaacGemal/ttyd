#include "eff/n64/eff_timestamp_n64.h"
#include "memory.h"

typedef struct effTimestampN64Data {
	s32 type;            // 0x0
	f32 x;               // 0x4
	f32 y;               // 0x8
	f32 z;               // 0xC
	s32 field_0x10;      // 0x10
	s32 field_0x14;      // 0x14
	s32 field_0x18;      // 0x18
	s32 field_0x1c;      // 0x1C
	s32 field_0x20;      // 0x20
	s32 field_0x24;      // 0x24
	f32 scale;           // 0x28
	s32 field_0x2c;      // 0x2C
	f32 field_0x30;      // 0x30
	f32 posX[10];        // 0x34
	f32 posY[10];        // 0x5C
	f32 posZ[10];        // 0x84
	f32 rotX[10];        // 0xAC
	f32 rotY[10];        // 0xD4
	f32 rotZ[10];        // 0xFC
	s32 field_0x124[10]; // 0x124
	s32 field_0x14c[10]; // 0x14C
} effTimestampN64Data;

// .rodata / .sdata2 constants (float constants referenced through the linker)
const f64 double_to_int_802fb8c8 = 4503601774854144.0;
extern const f32 float_0_804200b8;
extern const f32 float_1_804200d4;

// .rodata
const char str_802FB8D8[16] = "TimestampN64";

// .data: ten vertices, { x, y, timer } each
s32 pos_data[10][3] = {
	{ -50, -70, 5 },
	{ 80, -90, 20 },
	{ 20, -50, 45 },
	{ -80, 30, 10 },
	{ 30, 0, 35 },
	{ 120, -30, 50 },
	{ 10, 50, 30 },
	{ 100, 30, 15 },
	{ 40, 100, 40 },
	{ 140, 90, 25 },
};

EffectEntry* effTimestampN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effTimestampN64Data* data;
	s32 i;

	effect = effEntry();
	effect->type = str_802FB8D8;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effTimestampN64Data));
	effect->userdata = data;
	effect->callback = effTimestampMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (time <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = time;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 70;
	data->field_0x1c = 180;
	data->field_0x20 = 120;
	data->field_0x2c = 0;
	data->field_0x30 = float_1_804200d4;

	for (i = 0; i < 10; i++) {
		data->posX[i] = (f32)pos_data[i][0];
		data->posY[i] = (f32)pos_data[i][1];
		data->posZ[i] = float_0_804200b8;
		data->rotX[i] = float_0_804200b8;
		data->rotY[i] = float_0_804200b8;
		data->rotZ[i] = float_0_804200b8;
		data->field_0x124[i] = 0;
		data->field_0x14c[i] = pos_data[i][2];
	}
	return effect;
}

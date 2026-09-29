#include "eff/n64/eff_kemuri2_n64.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>

// .rodata
const char str_802FA8C8[12] = "Kemuri2N64";

// .sdata2 float constants, named after their original addresses.
extern const f32 float_0p33333_8041f2d8;
extern const f32 float_1p75_8041f2dc;
extern const f32 float_neg0p16_8041f2e0;

// Local to this file in the original (`.fn effKemuri2Main, local`).
static void effKemuri2Main(EffectEntry* effect);

typedef struct effKemuri2Data {
	u32 field_0x0;             // 0x0
	u16 type;                  // 0x4
	u8 pad_6[2];               // 0x6
	Vec position;              // 0x8
	u8 field_0x14[0x44 - 0x14]; // 0x14
	f32 field_0x44;            // 0x44
	f32 field_0x48;            // 0x48
	f32 field_0x4c;            // 0x4c
	f32 field_0x50;            // 0x50
	f32 field_0x54;            // 0x54
	s32 field_0x58;            // 0x58
	s32 field_0x5c;            // 0x5c
	s32 field_0x60;            // 0x60
} effKemuri2Data;

EffectEntry* effKemuri2N64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6) {
	EffectEntry* entry;
	effKemuri2Data* data;

	entry = effEntry();
	entry->type = str_802FA8C8;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri2Data));
	entry->userdata = data;
	entry->callback = effKemuri2Main;

	data->field_0x58 = (type == 2);
	data->field_0x5c = 0;
	data->type = type;
	data->field_0x0 = 1;
	data->field_0x50 = a5;
	data->field_0x54 = a6;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->field_0x44 = float_0p33333_8041f2d8;
	data->field_0x48 = float_1p75_8041f2dc;
	data->field_0x4c = float_neg0p16_8041f2e0;
	return entry;
}

#include "eff/n64/eff_kemuri8_n64.h"
#include "driver/dispdrv.h"

#include <dolphin/mtx.h>

// .sdata2 constant (defined by this unit in the original).
extern const f32 float_16_8041f44c;

// One 0x5C-byte element of this effect's userdata array.
typedef struct effKemuri8Particle {
	s16 type;       // 0x0
	u8 pad_02[2];   // 0x2
	Vec position;   // 0x4
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	f32 field_0x1C; // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
	f32 field_0x2C; // 0x2C
	f32 field_0x30; // 0x30
	f32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
	s32 field_0x3C; // 0x3C
	s32 field_0x40; // 0x40
	u8 pad_44[4];   // 0x44
	s32 field_0x48; // 0x48
	s32 field_0x4C; // 0x4C
	s32 field_0x50; // 0x50
	s32 field_0x54; // 0x54
	s32 camId;      // 0x58
} effKemuri8Particle;

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effKemuri8N64SetCamId(EffectEntry* entry, s32 camId) {
	((effKemuri8Data*)entry->userdata)->camId = camId;
}

void effKemuri8Main(EffectEntry* effect) {
	effKemuri8Particle* data = effect->userdata;
	Vec position;
	s32 camId;
	s32 i;
	// b before a: the original's register allocation (a in r4, b in r0) depends on this order.
	s32 b;
	s32 a;

	position = initializeVec(&data->position);
	camId = data->camId;
	b = (s32)(float_16_8041f44c * (f32)data->field_0x3C / (f32)data->field_0x4C);
	a = (s32)(float_16_8041f44c * (f32)(data->field_0x3C + 1) / (f32)data->field_0x4C);
	data->field_0x3C++;
	if (a > 15) {
		a = 15;
	}
	if (b > 15) {
		effDelete(effect);
		return;
	}
	data->field_0x40 = b;
	data->field_0x48 = a;

	for (i = 1, data++; i < effect->count; i++, data++) {
		data->field_0x18 += data->field_0x1C;
		data->field_0x14 += data->field_0x18;
		data->field_0x10 += data->field_0x14;
		data->position.x += data->field_0x10 * data->field_0x30;
		data->position.y += data->field_0x10 * data->field_0x34;
	}

	dispEntry(camId, dispCalcZ(&position), 2, effKemuri8Disp, effect);
}

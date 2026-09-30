#include "eff/n64/eff_opuku_wcurtain_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <stdlib.h>

// .rodata
const char str_802FB2A4[] = "OpukuWcurtainN64";

// .sdata2
extern const f32 float_0_8041f820;
extern const f32 float_neg1_8041f834;
extern const f32 float_0p01_8041f838;
extern const f32 float_0p5_8041f83c;

// Local copy of system.c's inline helper so it inlines here (as in eff_opuku_jetw_n64.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effOpukuWcurtainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effOpukuWcurtainData* data;
	s32 i;

	entry = effEntry();
	entry->type = str_802FB2A4;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effOpukuWcurtainData));
	entry->userdata = data;
	entry->callback = effOpukuWcurtainMain;
	entry->flags |= 2;

	data->type = type;
	data->counter = 0;
	if (duration <= 0) {
		data->duration = 1000;
	} else {
		data->duration = duration;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 20;
	data->field_0x1C = 120;
	data->field_0x20 = 255;
	for (i = 0; i < 12; i++) {
		data->field_0x2C[i] = float_0_8041f820;
		data->field_0x5C[i] = (rand() % 11) * float_0p01_8041f838 + float_neg1_8041f834;
		data->field_0x5C[i] *= float_0p5_8041f83c;
	}

	return entry;
}

void effOpukuWcurtainMain(EffectEntry* effect) {
	effOpukuWcurtainData* data = effect->userdata;
	Vec position = initializeVec((Vec*)&data->x);
	s32 i;
	s32 counter;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->duration = 0x10;
	}
	if (data->duration < 1000) {
		data->duration--;
	}
	data->counter++;
	if (data->duration < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	if (data->duration < 0x10) {
		data->field_0x24 = data->duration << 4;
	}
	if (counter < 0x10) {
		data->field_0x24 = counter * 0x10 + 0xf;
	}
	for (i = 0; i < 12; i++) {
		data->field_0x2C[i] += data->field_0x5C[i];
		data->field_0x5C[i] += -0.05f;
		if (data->field_0x5C[i] > 5.0f) {
			data->field_0x5C[i] = 0.1f * (5.0f - data->field_0x5C[i]) + data->field_0x5C[i];
		}
	}
	dispEntry(4, dispCalcZ(&position), 2, effOpukuWcurtainDisp, effect);
}

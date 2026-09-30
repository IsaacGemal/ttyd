#include "eff/n64/eff_bomhei_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>

const f32 float_9_8041ea4c = 9.0f;

const Vec vec3_802fa2b0 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fa2c0 = 4503601774854144.0;

static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effBomheiN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	effBomheiData* data;

	effect = effEntry();
	effect->type = "BomheiN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effBomheiData));
	effect->userdata = data;
	effect->callback = effBomheiMain;
	data->field_0x14 = count;
	data->field_0x18 = 0;
	data->field_0x1C = count;
	data->field_0x20 = 0.0f;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x10 = a5;
	data->field_0x24 = 0.0f;
	if (type == 0) {
		data->field_0x28 = 0.0f;
	}
	else {
		data->field_0x28 = 40.0f;
	}
	return effect;
}

void effBomheiMain(EffectEntry* effect) {
	EffectEntry* e;
	effBomheiData* data;
	Vec position;

	e = effect;
	data = e->userdata;
	position = initializeVec((Vec*)&data->x);
	data->field_0x20 = float_9_8041ea4c * (f32)data->field_0x18 / (f32)data->field_0x1C;
	data->field_0x18++;
	data->field_0x14--;
	if (data->field_0x14 < 0) {
		effDelete(e);
		return;
	}

	data->field_0x24 += data->field_0x28;
	dispEntry(4, dispCalcZ(&position), 2, effBomheiDisp, e);
}

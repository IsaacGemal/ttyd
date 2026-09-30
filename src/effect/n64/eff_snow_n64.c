#include "eff/n64/eff_snow_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mario.h"
#include "memory.h"
#include <dolphin/mtx.h>

static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effSnowN64Entry(s32 type, f32 x, f32 y, f32 z) {
	static s32 kaiten;
	EffectEntry* effect;
	effSnowData* data;

	effect = effEntry();
	effect->type = "SnowN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effSnowData));
	effect->userdata = data;
	effect->callback = effSnowMain;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x0C = 0.0f;
	data->field_0x10 = -1.5f;
	data->field_0x14 = 0.0f;
	data->field_0x18 = 0.0f;
	data->field_0x1C = 0.0f;
	data->field_0x20 = 1.5f;
	data->field_0x24 = kaiten != 0 ? -5.0f : 5.0f;
	data->type = type;
	kaiten++;
	if (kaiten > 1) {
		kaiten = 0;
	}
	return effect;
}

void effSnowMain(EffectEntry* effect) {
	effSnowData* data = effect->userdata;
	Vec disp;

	disp = initializeVec((Vec*)&data->x);

	data->type--;
	if (data->type < 0) {
		effDelete(effect);
	} else {
		MarioWork* mario;

		data->x += data->field_0x0C;
		data->y += data->field_0x10;
		data->z += data->field_0x14;
		data->field_0x18 += data->field_0x20;
		if (data->field_0x18 < 10.0f) {
			data->field_0x18 = 10.0f;
			data->field_0x20 = -data->field_0x20;
		}
		if (data->field_0x18 > 45.0f) {
			data->field_0x18 = 45.0f;
			data->field_0x20 = -data->field_0x20;
		}
		data->field_0x1C += data->field_0x24;
		mario = marioGetPtr();
		if (data->x - mario->position.x > 200.0f) {
			data->x -= 400.0f;
		} else if (mario->position.x - data->x > 200.0f) {
			data->x += 400.0f;
		}
		if (data->z - mario->position.z > 200.0f) {
			data->z -= 400.0f;
		} else if (mario->position.z - data->z > 200.0f) {
			data->z += 400.0f;
		}
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effSnowDisp, effect);
	}
}

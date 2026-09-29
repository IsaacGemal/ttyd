#include "eff/n64/eff_pikkyolo_n64.h"
#include "driver/dispdrv.h"

#include <dolphin/mtx.h>

const Vec vec3_802fb370 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fb380 = 4503601774854144.0;

void effPikkyoloMain(EffectEntry* effect) {
	Vec position;
	Vec disp = { 0.0f, 0.0f, 0.0f };
	effPikkyoloN64Data* data = effect->userdata;
	s32 i;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	if (data->lifetime < 8) {
		data->a = data->lifetime << 5;
	}

	for (i = 0; i < effect->count; i++, data++) {
		data->velocityY -= 0.5f;
		data->position.x += data->velocityX;
		data->position.y += data->velocityY;
		data->position.z += data->velocityZ;
		if (data->position.y < 0.0f && data->velocityY < 0.0f) {
			data->position.y = 0.0f;
			data->velocityY *= -0.7f;
		}
		data->angle += (f32)(((i & 1) * 2 - 1) * 14);
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPikkyoloDisp, effect);
}

#include "eff/n64/eff_sweat_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>

static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effSweatMain(EffectEntry* effect) {
	effSweatData* data = effect->userdata;
	Vec disp;
	s32 i;
	s32 dead = 0;

	disp = initializeVec(&data->position);

	for (i = 1, data++; i < effect->count; i++, data++) {
		if (data->timer != 0) {
			data->timer--;
		} else {
			data->position.x += data->velocity.x;
			data->position.y += data->velocity.y;
			data->position.z += data->velocity.z;
			data->velocity.x *= 0.94f;
			data->velocity.y -= 0.05f;
			if (data->velocity.y < -0.1f) {
				data->velocity.y = -0.1f;
			}
			data->velocity.z *= 0.94f;
			if (data->age < 8) {
				data->alpha = (data->age << 5) + 0x1f;
			}
			if (data->life < 0x10) {
				data->alpha = data->life << 4;
			}
			if (data->alpha > 0xff) {
				data->alpha = 0xff;
			}
			if (data->alpha < 0) {
				data->alpha = 0;
			}
			data->age++;
			data->life--;
			if (data->life < 0) {
				dead++;
			}
		}
	}

	if (dead >= effect->count - 1) {
		effDelete(effect);
	} else {
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effSweatDisp, effect);
	}
}

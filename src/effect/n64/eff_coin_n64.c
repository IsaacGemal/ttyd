#include "eff/n64/eff_coin_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"

#include <dolphin/mtx.h>

typedef struct effCoinData {
	s32 field_0x0;  // 0x0
	s32 anim;       // 0x4
	Vec position;   // 0x8
	s32 counter;    // 0x14
	s32 frame;      // 0x18
	f32 field_0x1C; // 0x1C
	s32 timer;      // 0x20
} effCoinData;

void effCoinMain(EffectEntry* effect) {
	effCoinData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };
	s32 i;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data->counter--;
	data->frame++;
	if (data->counter < 0) {
		effDelete(effect);
		return;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		if (data->timer > 0) {
			data->timer--;
			if (data->timer > 0) {
				continue;
			}
		}
		data->anim++;
		if (data->anim >= 10) {
			data->timer = -1;
		}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effCoinDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effCoinDisp, effect);
#endif
}

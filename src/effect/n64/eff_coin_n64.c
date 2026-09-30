#include "eff/n64/eff_coin_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

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

// Per-coin spawn template (5 words, stride 0x14).
typedef struct effCoinN64Spawn {
	s32 timer; // 0x0
	s32 x;     // 0x4
	s32 y;     // 0x8
	s32 z;     // 0xC
	s32 scale; // 0x10
} effCoinN64Spawn;

const f64 double_to_int_802fa428 = 4503601774854144.0;
const f32 float_0p01_8041ebec = 0.01f;
const char str_802FA430[] = "CoinN64";

static effCoinN64Spawn _data[4] = {
	{ 2, 0, 0, 0, 100 },
	{ 4, 4, -2, 0, 100 },
	{ 6, -2, 2, 0, 70 },
	{ 8, 2, 4, 0, 40 },
};

EffectEntry* effCoinN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effCoinData* data;
	effCoinData* p;
	effCoinN64Spawn* src;
	s32 i;

	effect = effEntry();
	effect->type = str_802FA430;
	effect->count = 5;
	data = __memAlloc(HEAP_EFFECT, 0xb4);
	effect->userdata = data;
	effect->callback = effCoinMain;
	data->field_0x0 = a;
	data->frame = 0;
	data->counter = 14;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	p = data + 1;
	for (i = 1; i < 5; i++, p++) {
		src = &_data[i - 1];
		p->position.x = (f32)src->x;
		p->position.y = (f32)src->y;
		p->position.z = (f32)src->z;
		p->anim = -1;
		p->timer = src->timer;
		p->field_0x1C = scale * (f32)src->scale * float_0p01_8041ebec;
	}
	return effect;
}

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

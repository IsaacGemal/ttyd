#include "eff/n64/eff_ripple_n64.h"
#include "eff/eff_ripple.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

const Vec vec3_802fb520 = { 0.0f, 0.0f, 0.0f };

// dont_inline keeps effRippleN64Entry a real `bl` from effRippleMain as in retail.
#pragma dont_inline on

EffectEntry* effRippleN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffRippleData* data;

	effect = effEntry();
	effect->type = "RippleN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffRippleData));
	effect->userdata = data;
	effect->callback = effRippleMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x18 = 0;
	data->field_0x20 = 0;
	data->rx = 0.0f;
	data->rz = 0.0f;
	data->field_0x14 = 1.0f;
	data->camId = 4;
	switch (type) {
		case 1:
		case 10:
			data->field_0x10 = 0.6f;
			break;
		case 2:
		case 20:
			data->field_0x10 = 10.0f;
			break;
		case 3:
		case 30:
			data->field_0x10 = 10.0f;
			break;
		default:
			data->field_0x10 = 1.0f;
			break;
	}
	data->field_0x1C = 0x20;
	data->field_0x24 = rand() % 32;
	data->field_0x28 = y + scale;
	return effect;
}
#pragma dont_inline reset

// MWCC's callee-saved register colours differ: retail keeps `effect` in r28 and `timer` in r30, ours
// swaps them (effect r30, timer r28); that also moves the prologue's `mr r28, r3` and lets the
// scheduler hoist the vec loads. Every declaration/assignment order, `register`/`volatile`, variable
// splitting, opt_common_subs/opt_propagation and function order were tried.
// NONMATCHING (93.49%): only the effect/timer callee-saved colours (r28 vs r30) and the prologue schedule differ.
void effRippleMain(EffectEntry* effect) {
	EffRippleData* data = effect->userdata;
	Vec position2;
	Vec position;
	s32 type;
	s32 timer;

	position = vec3_802fb520;
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	timer = data->field_0x1C;
	type = data->type;
	data->field_0x1C--;
	data->field_0x20++;
	if (data->field_0x1C < 0) {
		effDelete(effect);
		return;
	}

	if (timer > 5) {
		data->field_0x18 = (s32)(0.3f * (f32)(120 - data->field_0x18) + (f32)data->field_0x18);
	}

	if ((type == 1 || type == 2 || type == 3) && timer == 15) {
#ifdef MUST_MATCH
		/* MUST_MATCH: the original loads the new effect's userdata into r3 before reading
		   data->rz, so rz lands in f1; the natural one-liner reads userdata first and reuses f0. */
		EffectEntry* e = effRippleN64Entry(type * 10, data->x, data->y, data->z, 0.0f);
		f32 rz = data->rz;
		EffRippleData* newData = e->userdata;
		f32 rx = data->rx;
		newData->rx = rx;
		newData->rz = rz;
#else
		EffRippleData* newData =
			effRippleN64Entry(type * 10, data->x, data->y, data->z, 0.0f)->userdata;
		newData->rx = data->rx;
		newData->rz = data->rz;
#endif
	}

	if (type == 2 || type == 20) {
		data->field_0x10 *= 1.05f;
	}
	if (type == 3 || type == 30) {
		data->field_0x10 *= 1.05f;
	}

	if (timer < 0x20) {
		data->field_0x18 = timer * 8;
	}

	data->field_0x24++;
	if (data->field_0x24 >= 0x20) {
		data->field_0x24 -= 0x20;
	}

	data->field_0x10 += 0.1f;
	dispEntry(data->camId, dispCalcZ(&position2), 2, effRippleDisp, effect);
}

void effRippleN64SetRxRz(EffectEntry* effect, f32 rx, f32 rz) {
	effRippleN64Data* data = effect->userdata;

	data->field_0x2C = rx;
	data->field_0x30 = rz;
}

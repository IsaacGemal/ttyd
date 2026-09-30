#include "eff/n64/eff_fireflower_n64.h"
#include "eff/n64/eff_rothammer_success_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

// .sdata scale table indexed by data->timer in state 1 (signed bytes: loaded with extsb).
static s8 scale_data[] = "27<AFKPUZ_dgigd";

EffectEntry* effFireFlowerN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effFireFlowerData* data;
	effFireFlowerData* p;
	s32 i;

	effect = effEntry();
	effect->type = "FireFlowerN64";
	effect->count = 0x1f;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFireFlowerData) * 0x1f);
	effect->userdata = data;
	effect->callback = effFireFlowerMain;

	if (type == 1) {
		data->mode = 1;
		type = 0;
	}
	data->state = 0;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	if (duration <= 0) {
		duration = 1000;
	}
	data->lifetime = duration;
	data->timer = 0;
	data->field_0x48 = 0;
	data->field_0x3C = 0xff;
	data->scale = 0.5f;
	data->field_0x34 = 0;
	data->field_0x38 = 0;
	data->field_0x28 = 0.0f;
	data->field_0x2C = -10.0f;

	p = &data[1];
	for (i = 1; i < 0x1f; i++, p++) {
		p->type = 1;
		p->x = 0.0f;
		p->y = 0.0f;
		p->z = 0.0f;
		p->velX = 8.0f;
		p->velY = 0.0f;
		p->velZ = (f32)(rand() % 20 - 8);
		p->field_0x20 = 0.05f * (f32)(rand() % 10 - 5);
		p->field_0x24 = (f32)(rand() % 80 + 5);
		p->field_0x3C = 0xff;
		p->field_0x4C = i - 1;
		p->timer = 10;
		p->velX /= 1.5f;
		p->velY /= 1.5f;
		p->velZ /= 1.5f;
		p->field_0x4C *= 2;
	}
	return effect;
}

// (1) case 0 hoists `li r4, 7` above the mode test; the original emits it after `lfs f2, data->y`.
// (2) case 6 coalesces `field_0x28 + 18.0f` into f1; the original keeps it in f2 and puts
// double_to_int in f3 and 0.9f in f1. Both are scheduler/register-allocation choices that could not
// be steered from C (tried compound vs plain assignment, operand order, temps, and the opt pragmas).
// NONMATCHING (98.96%): see the two differences above.
void effFireFlowerMain(EffectEntry* effect) {
	effFireFlowerData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };
	s32 i;
	s32 timer;
	s32 state;
	f32 fx;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data->timer++;
	data->field_0x48++;
	state = data->state;
	if (state == 7) {
		effDelete(effect);
		return;
	}

	timer = data->timer;
	switch (state) {
	case 0:
		if (data->mode == 0) {
			fx = data->x - 10.0f;
		} else {
			fx = 10.0f + data->x;
		}
		effRotHammerSuccessN64Entry(0, 7, fx, data->y, data->z, 0x14);
		data->field_0x34 = 0;
		data->field_0x38 = 0;
		data->state++;
		data->timer = 0;
		break;
	case 1:
		if (timer >= 0x10) {
			data->state++;
			data->timer = 0;
		} else {
			data->scale = (f32)scale_data[timer - 1] * 0.01f;
			data->field_0x34 = 0;
			data->field_0x38 = 0;
		}
		break;
	case 2:
		if (timer >= 4) {
			data->state++;
			data->timer = 0;
		} else {
			data->field_0x34 = 1;
		}
		break;
	case 3:
		if (timer >= 0x1E) {
			data->state++;
			data->timer = 0;
		} else {
			data->field_0x34 = 2;
		}
		break;
	case 4:
		if (timer > data->lifetime) {
			data->state++;
			data->timer = 0;
		} else {
			data->field_0x2C = 0.0f;
			data->field_0x34 = 3;
			if ((timer >> 2) & 1) {
				data->scale = 1.0f;
			} else {
				data->scale = 1.05f;
			}
		}
		break;
	case 5:
		if (timer >= 0x14) {
			data->state++;
			data->timer = 0;
		} else {
			data->field_0x34 = 0;
			data->field_0x2C = -10.0f;
		}
		break;
	case 6:
		data->field_0x34 = 0;
		data->field_0x28 += 18.0f;
		data->field_0x3C = (s32)((f32)data->field_0x3C * 0.9f);
		if (data->field_0x28 >= 720.0f) {
			data->field_0x28 = 720.0f;
			data->state++;
		}
		break;
	}

	state = data->state;
	data++;
	if (state >= 4 && state <= 6) {
		for (i = 1; i < effect->count; i++, data++) {
			if (data->field_0x4C > 0) {
				data->field_0x4C--;
				if (data->field_0x4C > 0) {
					continue;
				}
			}
			data->velY += data->field_0x20;
			data->x += data->velX;
			data->y += data->velY;
			data->z += data->velZ;
			if (state == 4 && data->x > 200.0f) {
				data->x = 0.0f;
				data->y = 0.0f;
				data->velY = 0.0f;
				data->z = 0.0f;
			}
		}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effFireFlowerDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effFireFlowerDisp, effect);
#endif
}

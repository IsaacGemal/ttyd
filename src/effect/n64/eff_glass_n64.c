#include "eff/n64/eff_glass_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <stdlib.h>

const char str_802FA710[12] = "GlassN64";
const f64 double_to_int_802fa708 = 4503601774854144.0;

EffectEntry* effGlassN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effGlassData* data;

	entry = effEntry();
	entry->type = str_802FA710;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x5C);
	entry->userdata = data;
	entry->callback = effGlassMain;
	entry->flags |= 2;
	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	} else {
		data->life = life;
	}
	data->alpha = 0xff;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;

	switch (type) {
	case 0:
	case 1:
		data->field_0x18 = 0xff;
		data->field_0x1C = 0xff;
		data->field_0x20 = 0xff;
		data->field_0x28 = 0xff;
		data->field_0x2C = 0xff;
		data->field_0x30 = 0xff;
		data->field_0x34 = 0xff;
		break;
	case 2:
	case 3:
		data->field_0x18 = 0;
		data->field_0x1C = 0xff;
		data->field_0x20 = 0;
		data->field_0x28 = 0;
		data->field_0x2C = 0xff;
		data->field_0x30 = 0;
		data->field_0x34 = 0xff;
		break;
	}

	data->angle = float_0_8041f094;
	data->angleVel = (0.1f * (f32)(rand() % 5) + 0.1f) * (0.25f * (f32)(rand() % 2 * 2 - 1));
	data->rot = (f32)(rand() % 360);
	data->velX = (f32)(rand() % 11 - 5);
	data->velY = (f32)(rand() % 11 - 5);
	data->velZ = (f32)(rand() % 11 - 5);
	data->rotVel = (f32)(rand() % 41 - 20);
	data->gravity = float_neg0p1_8041f0a4;
	return entry;
}

void effGlassMain(EffectEntry* effect) {
	effGlassData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effFlameMain). */
	Vec position;
#endif
	Vec disp = { 0 };

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}

	if (data->life < 1000) {
		data->life--;
	}
	data->counter++;

	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	if (data->life < 16) {
		data->alpha = data->life * 16;
	}

	data->angle += data->angleVel;
	if (data->angle < float_0_8041f094) {
		data->angle += float_1_8041f098;
	}
	if (data->angle > float_1_8041f098) {
		data->angle = data->angle - float_1_8041f098;
	}

	data->velY += data->gravity;
	data->position.x += data->velX;
	data->position.y += data->velY;
	data->position.z += data->velZ;
	data->rot += data->rotVel;

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effGlassDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effGlassDisp, effect);
#endif
}

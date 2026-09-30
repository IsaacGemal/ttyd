#include "eff/n64/eff_explosion_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"

const Vec vec3_802fa510 = { 0.0f, 0.0f, 0.0f };

EffectEntry* effExplosionN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effExplosionN64Data* data;

	effect = effEntry();
	effect->type = "ExplosionN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effExplosionN64Data));
	effect->userdata = data;
	effect->callback = effExplosionMain;
	data->life = duration;
	data->counter = 0;
	data->duration = duration;
	data->ratio = 0.0f;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->velocity = 0.0f;
	if (type == 0) {
		data->accel = 0.0f;
	}
	else {
		data->accel = 40.0f;
	}
	return effect;
}

void effExplosionMain(EffectEntry* effect) {
	effExplosionN64Data* data;
	Vec position2;
	Vec position;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	data->ratio = 8.0f * data->counter / data->duration;
	data->counter++;
	data->life--;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	data->velocity += data->accel;
	if (data->type == 0) {
		data->scale += 0.1f;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effExplosionDisp, effect);
}

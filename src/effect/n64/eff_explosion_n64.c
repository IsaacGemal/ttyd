#include "eff/n64/eff_explosion_n64.h"
#include "memory.h"

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

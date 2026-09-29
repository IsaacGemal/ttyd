#include "eff/n64/eff_balloon_n64.h"
#include "memory.h"

EffectEntry* effBalloonN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 a5) {
	EffectEntry* effect;
	effBalloonN64Data* data;

	effect = effEntry();
	effect->type = "BalloonN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effBalloonN64Data));
	effect->userdata = data;
	effect->callback = effBalloonMain;
	effect->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x10 = a5;
	data->field_0x1C = 100;
	data->field_0x14 = a4;
	data->field_0x18 = 0.0f;
	data->field_0x20 = 0;
	return effect;
}

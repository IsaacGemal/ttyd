#include "eff/n64/eff_balloon_heiho_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/mtx.h>

EffectEntry* effBalloonHeihoN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 a4, f32 a5) {
	EffectEntry* effect;
	BalloonHeihoData* data;

	effect = effEntry();
	effect->type = "BalloonHeihoN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(BalloonHeihoData));
	effect->userdata = data;
	effect->callback = effBalloonHeihoMain;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->field_0x10 = 0.0f;
	data->life = count;
	data->counter = 0;
	data->field_0x14 = a4;
	data->field_0x18 = a5;
	return effect;
}

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effBalloonHeihoMain(EffectEntry* effect) {
	BalloonHeihoData* data = effect->userdata;
	Vec position = initializeVec(&data->position);

	data->life--;
	data->counter++;
	if (data->life < 0) {
		effDelete(effect);
	} else {
		data->field_0x10 += data->field_0x14;
		if (data->field_0x10 > 2.0f) {
			data->field_0x10 = 2.0f;
		}
		data->position.y += data->field_0x10;
		dispEntry(4, dispCalcZ(&position), 2, effBalloonHeihoDisp, effect);
	}
}

#include "eff/n64/eff_balloon_heiho_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"

#include <dolphin/mtx.h>

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

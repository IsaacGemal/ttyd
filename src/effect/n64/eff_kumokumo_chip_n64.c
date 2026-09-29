#include "eff/n64/eff_kumokumo_chip_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"

const Vec vec3_802faa50 = { 0.0f, 0.0f, 0.0f };

void effKumokumoChipMain(EffectEntry* effect) {
	effKumokumoChipData* data;
	s32 lifetime;
	s32 type;
	s32 timer;
	Vec pos2;
	Vec pos;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	lifetime = data->lifetime;
	if (lifetime < 0) {
		effDelete(effect);
		return;
	}
	timer = data->timer;
	if (type == 0 || type == 2) {
		if (timer < 16) {
			data->a = (timer << 4) + 0xf;
		}
		else {
			data->a = 0xff;
		}
		if (lifetime < 16) {
			data->a = lifetime << 4;
		}
	}
	else {
		if (timer < 4) {
			data->a = (timer << 6) + 0x3f;
		}
		else {
			data->a = 0xff;
		}
		if (lifetime < 4) {
			data->a = lifetime << 6;
		}
	}
	data->x += data->xVel;
	data->y += data->yVel;
	data->z += data->zVel;

	dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effKumokumoChipDisp, effect);
}

#include "eff/n64/eff_pokopi_thunder_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"

#include <dolphin/mtx.h>

// .rodata
const Vec vec3_802fb400 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_mask_802fb410 = 4503599627370496.0;
const f64 double_to_int_802fb418 = 4503601774854144.0;
const f32 float_0p5_8041fa54 = 0.5f;

void effPokopiThunderMain(EffectEntry* effect) {
	EffPokopiThunderData* data;
	Vec pos2;
	Vec pos;
	s32 type;
	s32 timer;

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

	timer = data->lifetime;
	if (timer < 1000) {
		data->lifetime = timer - 1;
	}
	data->timer++;

	timer = data->lifetime;
	if (timer < 0) {
		effDelete(effect);
		return;
	}

	timer = data->timer;
	// MWCC strength-reduces / 2.0f into fmuls by the 0.5 pool constant, and that division form is
	// what reproduces the original's f2/f0 constant registers (a plain * 0.5f allocates f1/f2).
	if (type == 2) {
		if (timer >= 40) {
			data->field_0x3C = (s32)((f32)data->field_0x3C / 2.0f);
		}
	} else {
		if (timer >= 11) {
			data->field_0x3C = (s32)((f32)data->field_0x3C / 2.0f);
		}
	}

	dispEntry(data->cameraId, dispCalcZ(&pos2), 2, effPokopiThunderDisp, effect);
}

#include "eff/n64/eff_rappa_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>

// .rodata: zero vector the per-effect position helper starts from.
const Vec vec3_802fb4a0 = { 0.0f, 0.0f, 0.0f };

// Local copy of system.c's inline helper so it inlines here (as in eff_sstar.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = vec3_802fb4a0;
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// NONMATCHING (98.29%): the original loads effect->userdata into r7 before the vec3_802fb4a0 words; MWCC GC/2.6 hoists the first two global loads into the prologue slots instead. Everything else matches.
void effRappaMain(EffectEntry* effect) {
	EffectEntry* e;
	effRappaData* data;
	Vec position;
	s32 i;
	s32 life;

	e = effect;
	data = e->userdata;
	position = initializeVec(&data->position);

	data->life--;
	data->counter++;
	life = data->life;
	if (life < 0) {
		effDelete(e);
		return;
	}
	if (life < 10) {
		data->alpha = (s32)((f32)data->alpha * float_0p8_8041faec);
	}

	data++;
	for (i = 1; i < e->count; i++, data++) {
		data->position.x += data->scale * data->field_0x10;
		data->position.y += data->scale * data->field_0x14;
		data->position.z += data->scale * data->field_0x18;
		data->scale *= float_0p9_8041faf0;
		if (life < 10) {
			data->field_0x24 += data->scale / float_10_8041faf4;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effRappaDisp, e);
}

#include "eff/eff_teresa.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_kemutest.h"
#include "memory.h"
#include "pmario_sound.h"
#include <dolphin/mtx.h>

const char str_802FE944[] = "Teresa";
const Vec vec3_802fe920 = {0.0f, 0.0f, 0.0f};
extern const f32 float_0p1_80421564;
extern const f32 float_0p8_80421568;
extern const f32 float_1_8042156c;
extern const f32 float_10_80421570;
extern const f32 float_178p5_80421574;
extern const f32 float_20_80421578;
extern const f32 float_30_8042157c;
extern const f32 float_3_80421580;
extern const f32 float_0p98_80421584;
extern const f32 float_0_8042155c;

EffectEntry* effTeresaEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	EffTeresaData* data;
	Vec pos;

	entry = effEntry();
	entry->type = str_802FE944;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffTeresaData));
	entry->userdata = data;
	entry->callback = effTeresaMain;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = (type == 0) ? float_30_8042157c : float_0_8042155c;
	data->field_0x30 = 0;
	data->field_0x2C = 0;
	data->alpha = (f32)((type == 0) ? 0 : 255);
	data->field_0x3C = 0;

	if (type == 0) {
		pos = vec3_802fe920;
		pos.x = data->x;
		pos.y = data->y;
		pos.z = data->z;
		psndSFXOn_3D((const char*)0x5D5, &pos);
	}
	return entry;
}

void effTeresaMain(EffectEntry* effect) {
	EffTeresaData* data;
	Vec position;
	Vec disp = { 0 };

	data = effect->userdata;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;

	if (data->type == 0) {
		if (data->field_0x3C == 0) {
			if (data->scale > float_0p1_80421564) {
				data->scale *= float_0p8_80421568;
			} else {
				Vec pos;

				data->field_0x3C = 1;
				effKemuTestEntry(8, data->x, data->y, data->z, float_1_8042156c);
				pos = (Vec){0.0f, 0.0f, 0.0f};
				pos.x = data->x;
				pos.y = data->y;
				pos.z = data->z;
				psndSFXOn_3D((const char*)0x5D6, &pos);
			}
			data->angle += float_10_80421570;
			data->alpha = float_178p5_80421574;
		} else {
			data->angle = float_0_8042155c;
			data->scale += (float_20_80421578 - data->scale) / float_30_8042157c;
			data->alpha *= float_0p8_80421568;
			if (data->alpha < float_3_80421580) {
				effDelete(effect);
				return;
			}
		}
	} else {
		data->scale += (float_20_80421578 - data->scale) / float_30_8042157c;
		data->alpha *= float_0p98_80421584;
		if (data->alpha < float_3_80421580) {
			effDelete(effect);
			return;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effTeresaDisp, effect);
}

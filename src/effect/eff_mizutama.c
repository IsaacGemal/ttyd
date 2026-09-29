#include "eff/eff_mizutama.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
#include <dolphin/types.h>

typedef struct MizutamaData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 vx;         // 0x10
	f32 vy;         // 0x14
	f32 scale;      // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	s32 timer;      // 0x24
} MizutamaData;

extern const f32 float_0p97_80421288;
extern const f32 float_0p3_8042128c;
extern const f32 float_0p99_80421290;
extern const f32 float_0p01_80421294;

void effMizutamaMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	MizutamaData* data = effect->userdata;
	Vec disp = { 0 };
	s32 done = 0;
	s32 i;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->timer != 0) {
			data->timer--;
		} else {
			data->x += data->vx;
			data->y += data->vy;
			data->vx *= float_0p97_80421288;
			data->vy *= float_0p97_80421288;
			data->y += float_0p3_8042128c;
			data->scale *= float_0p99_80421290;
			if (data->scale < float_0p01_80421294) {
				done++;
			}
		}
	}

	if (done >= effect->count - 1) {
		effDelete(effect);
	} else {
#ifdef MUST_MATCH
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMizutamaDisp, effect);
#else
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMizutamaDisp, effect);
#endif
	}
}

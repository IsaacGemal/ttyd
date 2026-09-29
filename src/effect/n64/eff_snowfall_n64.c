#include "eff/n64/eff_snowfall_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/mtx.h>

// .rodata
extern const Vec vec3_802fb668;

// .sdata2
extern const f32 float_166_8041fd24;

typedef struct effSnowfallData {
	s32 field_0x00;  // 0x00
	s32 type;        // 0x04
	Vec position;    // 0x08
	Vec velocity;    // 0x14
	s32 life;        // 0x20
	s32 field_0x24;  // 0x24
	s32 field_0x28;  // 0x28
	s32 count;       // 0x2C
	s32 timer;       // 0x30
} effSnowfallData;

// Only the callee-saved pointer pair differs. GC/2.6 assigns the `effect` parameter the higher
// NONMATCHING (86.22%): data is r30/effect r31 here, the original is data r31/effect r30.
void effSnowfallMain(EffectEntry* effect) {
	effSnowfallData* data = effect->userdata;
	Vec position2;
	Vec position = vec3_802fb668;
	s32 i;
	s32 life;
	s32 count;
	s32 limit;

	position.x = data->position.x;
	position.y = data->position.y;
	position.z = data->position.z;
	position2 = position;
	camGetPtr(CAMERA_3D);
	if (data->life < 100) {
		data->life--;
	}
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	data->field_0x24++;
	life = data->life;
	count = data->count;
	if (life < 10) {
		data->field_0x28 = life * 25;
	}
	limit = (s32)float_166_8041fd24;
	data = (effSnowfallData*)((u8*)data + 0x34);
	for (i = 0; i < count; i++, data++) {
		if (data->timer > 0) {
			data->timer--;
			if (data->timer > 0) {
				continue;
			}
		}
		data->position.x += data->velocity.x;
		data->position.y += data->velocity.y;
		data->position.z += data->velocity.z;
		cam_clip_chk(data, 0x226, limit, life);
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 10, effSnowfallDisp, effect);
}

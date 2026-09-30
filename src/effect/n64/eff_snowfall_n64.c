#include "eff/n64/eff_snowfall_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

// .rodata
extern const Vec vec3_802fb668;
extern const char str_802FB674[];
extern const f64 double_to_int_802fb680;

// .sdata2
extern const f32 float_166_8041fd24;
extern const f32 float_1000_8041fd28;
extern const f32 float_0p05_8041fd2c;
extern const f32 float_10_8041fd30;
extern const f32 float_neg1p2_8041fd34;
extern const f32 float_0p01_8041fd38;
extern const f32 float_0p5_8041fd3c;

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

void cam_clip_chk(void* p, s32 a, s32 b, s32 c) {
	effSnowfallData* data = p;
	CameraEntry* cam = camGetPtr(CAMERA_3D);
	f32 cx = cam->cameraAt.x;
	f32 cy = (cam->cameraAt.y + cam->cameraPos.y) / 2.0f;
	f32 cz = (cam->cameraAt.z + cam->viewDistance) / 2.0f;
	f32 px = data->position.x;
	f32 py = data->position.y;
	f32 pz = data->position.z;
	s32 i;

	i = (s32)(px - cx);
	if (i > a) {
		i %= a;
		data->position.x = cx - a + i;
	} else if (i < -a) {
		i = -i;
		i %= a;
		data->position.x = cx + a - i;
	}

	i = (s32)(py - cy);
	if (i > b) {
		i %= b;
		data->position.y = cy - b + i;
	} else if (i < -b) {
		if (c < 100) {
			data->velocity.y = 0.0f;
			data->field_0x28 = 0;
		} else {
			i = -i;
			i %= b;
			data->position.y = cy + b - i;
		}
	}

	i = (s32)(pz - cz);
	if (i > a) {
		i %= a;
		data->position.z = cz - a + i;
	} else if (i < -a) {
		i = -i;
		i %= a;
		data->position.z = cz + a - i;
	}
}

EffectEntry* effSnowfallN64Entry(s32 type, s32 count) {
	EffectEntry* effect;
	effSnowfallData* data;
	effSnowfallData* particle;
	s32 i;
	s32 num;

	effect = effEntry();
	num = count + 1;
	effect->type = str_802FB674;
	effect->count = num;
	data = __memAlloc(HEAP_EFFECT, num * sizeof(effSnowfallData));
	effect->userdata = data;
	effect->callback = effSnowfallMain;
	data->field_0x24 = 0;
	data->life = 100;
	data->type = type;
	data->count = count;
	data->field_0x28 = 0xFF;
	data->field_0x00 = 1;
	particle = data + 1;
	for (i = 0; i < count; i++, particle++) {
		CameraEntry* cam = camGetPtr(CAMERA_3D);

		particle->position.x = cam->cameraPos.x + (f32)(rand() % 2000) - float_1000_8041fd28;
		particle->position.y = cam->cameraPos.y + (f32)(rand() % 2000) - float_1000_8041fd28;
		particle->position.z = cam->cameraPos.z + (f32)(rand() % 2000) - float_1000_8041fd28;
		particle->velocity.x = ((f32)(rand() % 20) - float_10_8041fd30) * float_0p05_8041fd2c;
		particle->velocity.y = float_neg1p2_8041fd34 - (f32)(rand() % 80) * float_0p01_8041fd38;
		particle->velocity.z = ((f32)(rand() % 20) - float_10_8041fd30) * float_0p05_8041fd2c;
		particle->field_0x28 = 0xFF;
		particle->velocity.x *= float_0p5_8041fd3c;
		particle->velocity.y *= float_0p5_8041fd3c;
		particle->velocity.z *= float_0p5_8041fd3c;
		particle->timer = 0;
	}
	return effect;
}

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

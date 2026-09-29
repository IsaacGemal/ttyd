#include "eff/n64/eff_hit_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include <dolphin/mtx.h>

// Constants referenced by the original update code (defined in this unit's data).
extern const f32 float_0_8041f0f0;
extern const f32 float_0p0039062_8041f100;
extern const f32 float_1_8041f104;

extern u8 cnt_table[4];
extern u8 anim_data[16];
extern u8 radius_data[16];

typedef struct effHitData {
	s32 type;
	Vec position;
	u8 pad_0x10[0x30 - 0x10];
	s32 lifetime;
	s32 timer;
	f32 field_0x38;
	u8 color[6];
	u8 pad_0x42[0x44 - 0x42];
} effHitData;

static inline Vec effHitInitVec(Vec* input) {
	Vec temp = { 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effHitMain(EffectEntry* effect) {
	effHitData* data = effect->userdata;
	Vec disp;
	s32 idx;
	s32 i;
	s32 timer;
	s32 type;

	disp = effHitInitVec(&data->position);
	type = data->type;
	data->lifetime--;
	data->timer++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	timer = data->timer;
	if (timer <= cnt_table[type]) {
		data->field_0x38 = (f32)anim_data[timer - 1] * float_0p0039062_8041f100;
	} else {
		data->field_0x38 = float_1_8041f104;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		idx = (timer <= cnt_table[type]) ? timer - 1 : 12;
		data->position.x = (f32)radius_data[idx];
		data->position.y = float_0_8041f0f0;
		data->position.z = float_0_8041f0f0;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effHitDisp, effect);
}

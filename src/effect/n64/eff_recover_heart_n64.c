#include "eff/n64/eff_recover_heart_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>
#include <math_ext.h>

// .rodata
const f64 double_to_int_802fb4d8 = 4503601774854144.0;
const char str_802FB4E0[] = "RecoverHeartN64";

// .sdata
f32 scale_w_dat[7] = { 0.25f, 0.5f, 0.75f, 1.0f, 0.9f, 1.1f, 1.0f };
f32 scale_h_dat[7] = { 0.25f, 0.5f, 0.75f, 1.0f, 1.2f, 0.9f, 1.0f };

// .sdata2
extern const f32 float_0_8041fb10;
extern const f32 float_1_8041fb14;
extern const f32 float_0p05_8041fb1c;
extern const f32 float_6p2832_8041fb20;
extern const f32 float_360_8041fb24;
extern const f32 float_neg30_8041fb28;
extern const f32 float_8_8041fb2c;
extern const f32 float_0p9_8041fb30;
extern const f32 float_1p4_8041fb34;

typedef struct RecoverHeartData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 vx;         // 0x10
	f32 vy;         // 0x14
	f32 vz;         // 0x18
	f32 angle;      // 0x1C
	f32 field_0x20; // 0x20
	f32 scale;      // 0x24
	s32 field_0x28; // 0x28
	s32 field_0x2C; // 0x2C
	s32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
	f32 scaleW;     // 0x38
	f32 scaleH;     // 0x3C
	s32 field_0x40; // 0x40
	s32 field_0x44; // 0x44
} RecoverHeartData; // 0x48

// NONMATCHING (85.66%): instructions differ; see check.py
EffectEntry* effRecoverHeartN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	RecoverHeartData* data;

	effect = effEntry();
	effect->type = str_802FB4E0;
	effect->count = 2;
	data = __memAlloc(HEAP_EFFECT, 0x90);
	effect->userdata = data;
	effect->callback = effRecoverHeartMain;
	data[0].type = type;
	data[0].x = x;
	data[0].y = y;
	data[0].z = z;
	data[0].field_0x2C = 0x2E;
	data[0].field_0x30 = 0;
	if (time < 0) {
		data[0].field_0x44 = 1;
		time = -time;
	} else {
		data[0].field_0x44 = 0;
	}
	data[0].field_0x40 = time;
	{
		s32 i;

		for (i = 1; i < 2; i++) {
			data[1].x = float_0_8041fb10;
			data[1].y = float_0_8041fb10;
			data[1].z = float_0_8041fb10;
			data[1].scale = float_0_8041fb10;
			data[1].vx = float_0_8041fb10;
			data[1].vz = float_0_8041fb10;
			data[1].field_0x20 = float_0_8041fb10;
			data[1].angle = float_0_8041fb10;
			if (i == 1) {
				data[1].x = float_0_8041fb10;
				data[1].field_0x34 = 0;
			} else {
				data[1].x = (f32)(((i & 1) * 2 - 1) * (i / 2) * 4);
				data[1].field_0x34 = 11;
			}
			data[1].vy = float_1p4_8041fb34;
			data[1].scaleW = float_0_8041fb10;
			data[1].scaleH = float_0_8041fb10;
		}
	}
	data[1].field_0x28 = 0xFF;
	data[1].field_0x2C = 0;
	return effect;
}

// Local copy of system.c's inline helper so it inlines here (as in eff_guruguru.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effRecoverHeartMain(EffectEntry* effect) {
	s32 phi;
	s32 scaleBase;
	s32 life;
	RecoverHeartData* data;
	s32 i;
	s32 field2C;
	f32 fade;
	f32 angle;
	Vec position;

	data = effect->userdata;
	position = initializeVec((Vec*)&data->x);
	data->field_0x2C--;
	data->field_0x30++;
	life = data->field_0x2C;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	phi = life * 12 + 38;
	scaleBase = life * 25;
	fade = float_0p05_8041fb1c * (f32)life;
	data++;
	for (i = 1; i < effect->count; phi += 38, i++, data++) {
		field2C = data->field_0x2C;
		if (data->field_0x34 > 0) {
			data->field_0x34--;
			if (data->field_0x34 > 0) {
				continue;
			}
		}
		if (field2C >= 7) {
			data->scaleH = float_1_8041fb14;
			data->scaleW = float_1_8041fb14;
		} else {
			data->scaleW = scale_w_dat[field2C];
			data->scaleH = scale_h_dat[field2C];
		}
		data->x += data->vx;
		data->y += data->vy;
		data->z += data->vz;
		angle = float_6p2832_8041fb20 * (f32)phi / float_360_8041fb24;
		data->angle = float_neg30_8041fb28 * (f32)sin(angle);
		data->scale = float_8_8041fb2c * (f32)cos(angle);
		if (life < 20) {
			data->scale *= fade;
			data->angle *= fade;
		}
		if (life < 10) {
			data->field_0x28 = scaleBase;
			data->vy *= float_0p9_8041fb30;
		}
		data->field_0x2C++;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effRecoverHeartDisp, effect);
}

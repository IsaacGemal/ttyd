#include "eff/n64/eff_rubble_n64.h"
#include "eff/n64/eff_hokori_n64.h"
#include "driver/dispdrv.h"
#include <dolphin/mtx.h>

const Vec vec3_802fb5a0 = { 0.0f, 0.0f, 0.0f };

typedef struct effRubbleData {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 field_0x10;  // 0x10
	f32 field_0x14;  // 0x14
	f32 field_0x18;  // 0x18
	f32 field_0x1C;  // 0x1C
	f32 field_0x20;  // 0x20
	f32 field_0x24;  // 0x24
	f32 field_0x28;  // 0x28
	f32 field_0x2C;  // 0x2C
	f32 field_0x30;  // 0x30
	f32 field_0x34;  // 0x34
	f32 field_0x38;  // 0x38
	f32 field_0x3C;  // 0x3C
	s32 field_0x40;  // 0x40
	s32 field_0x44;  // 0x44
	s32 field_0x48;  // 0x48
	s32 field_0x4C;  // 0x4C
} effRubbleData;

extern const f32 float_0p8_8041fc38;
const f64 double_to_int_802fb5b0 = 4503601774854144.0;

void effRubbleMain(EffectEntry* effect) {
	effRubbleData* data;
	Vec position2;
	Vec position;
	s32 life;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;
	data->field_0x48--;
	data->field_0x4C++;
	life = data->field_0x48;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	if (data->y != data->field_0x10) {
		data->field_0x18 += data->field_0x20;
		data->y += data->field_0x18;
		data->field_0x24 += data->field_0x30;
		data->field_0x2C += data->field_0x38;
		data->field_0x28 += data->field_0x34;
		if (data->y <= data->field_0x10) {
			data->y = data->field_0x10;
			effHokoriN64Entry(2, 0x14, data->x, data->y, data->z);
			data->field_0x48 = 0x1e;
		}
	}
	if (life < 10) {
		data->field_0x44 = (s32)((f32)data->field_0x44 * float_0p8_8041fc38);
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effRubbleDisp, effect);
}

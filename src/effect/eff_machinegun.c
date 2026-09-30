#include "eff/eff_machinegun.h"
#include "driver/dispdrv.h"
#include "math_ext.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern const char str_8030142C[];
extern const f32 float_6p2832_804223f8;
extern const f32 float_360_804223fc;
extern const f32 float_0p9_80422400;

EffectEntry* effMachinegunEntry(s32 type, f32 x, f32 y, f32 z, f32 angle, f32 scale, f32 field_0x10) {
	EffectEntry* entry;
	effMachinegunData* data;

	entry = effEntry();
	entry->type = str_8030142C;
	entry->count = 1;
	data = __memAlloc(3, 0x28);
	entry->userdata = data;
	entry->callback = effMachinegunMain;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->angle = angle;
	data->field_0x10 = field_0x10;
	data->lifetime = 0xFF;
	data->scale = scale;
	return entry;
}

void effMachinegunMain(EffectEntry* effect) {
	effMachinegunData* data = effect->userdata;
#ifdef MUST_MATCH
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif
	data->position.x += data->scale * (f32)cos(float_6p2832_804223f8 * data->angle / float_360_804223fc);
	data->position.y += data->scale * (f32)sin(float_6p2832_804223f8 * data->angle / float_360_804223fc);
	data->scale *= float_0p9_80422400;
	data->lifetime -= 10;
	if (data->lifetime < 0) {
		effDelete(effect);
	}
	else {
#ifdef MUST_MATCH
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMachinegunDisp, effect);
#else
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMachinegunDisp, effect);
#endif
	}
}

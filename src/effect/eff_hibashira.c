#include "eff/eff_hibashira.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "math_ext.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata
const char str_803013E4[] = "hibashira";
const f64 double_to_int_803013f0 = 4503601774854144.0;

// .sdata2
extern const u32 dat_80422338;
extern const f32 float_deg2rad_8042233c;
extern const f32 float_0_80422340;
extern const f32 float_10_80422344;
extern const f32 float_3p1416_80422350;
extern const f32 float_0p5_80422354;

inline GXColor makeHibashiraColor(s32 alpha) {
	GXColor color = *(GXColor*)&dat_80422338;
	color.a = alpha;
	return color;
}

EffectEntry* effHibashiraEntry(s32 type, s32 countArg, s32 c, f32 x, f32 y, f32 z, f32 x2, f32 y2, f32 z2, f32 scale) {
	EffectEntry* entry;
	HibashiraData* data;
	HibashiraData* elem;
	s32 count;
	s32 i;
	f32 dx;
	f32 dy;
	f32 dz;

	entry = effEntry();

	count = 1;
	switch (type) {
	case 0:
	case 1:
	case 2:
		count = 2;
		break;
	case 3:
	case 4:
	case 5:
		count = countArg + 1;
		break;
	}

	entry->type = str_803013E4;
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(HibashiraData));
	entry->userdata = data;
	entry->callback = effHibashiraMain;
	entry->flags |= 2;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;
	data->animPose = -1;
	data->field_0x18 = 0xFF;
	if (c <= 0) {
		data->field_0x1C = 1000;
	} else {
		data->field_0x1C = c;
	}
	data->field_0x20 = 0;

	dx = x2 - x;
	dy = y2 - y;
	dz = z2 - z;

	i = 1;
	elem = &data[1];
	for (; i < entry->count; i++, elem++) {
		elem->position.x = (f32)(i - 1) * (dx / (f32)(count - 1));
		elem->position.y = (f32)(i - 1) * (dy / (f32)(count - 1));
		elem->position.z = (f32)(i - 1) * (dz / (f32)(count - 1));
		elem->scale = float_0p5_80422354 + (f32)sin(float_3p1416_80422350 * (f32)i / (f32)(entry->count - 1));
		elem->animPose = -1;
		elem->field_0x18 = 0xFF;
		elem->field_0x1C = 0;
		elem->field_0x20 = 0;
		elem->field_0x24 = (i - 1) * 4;
	}

	return entry;
}

void effHibashiraDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	HibashiraData* data = effect->userdata;
	Mtx spA0;
	Mtx sp70;
	Mtx sp40;
	Mtx sp10;
	s32 i;
	s32 alpha = data->field_0x18;

	PSMTXTrans(spA0, data->position.x, data->position.y, data->position.z);
	PSMTXScale(sp70, data->scale, data->scale, data->scale);
	PSMTXRotRad(sp40, 'y', float_deg2rad_8042233c * -camGetPtr(camId)->viewYaw);
	PSMTXConcat(spA0, sp40, spA0);
	PSMTXConcat(spA0, sp70, sp10);

	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
		if (data->animPose != -1) {
			PSMTXTrans(spA0, data->position.x, data->position.y, data->position.z);
			PSMTXScale(sp70, data->scale, data->scale, data->scale);
			PSMTXConcat(spA0, sp70, spA0);
			PSMTXConcat(sp10, spA0, spA0);

			animPoseSetMaterialFlagOn(data->animPose, 0x40);
			animPoseSetMaterialEvtColor(data->animPose, makeHibashiraColor((data->field_0x18 * alpha) / 255));
			animPoseMain(data->animPose);
			animPoseDrawMtx(data->animPose, spA0, 1, float_0_80422340, float_10_80422344);
			animPoseDrawMtx(data->animPose, spA0, 2, float_0_80422340, float_10_80422344);
			animPoseDrawMtx(data->animPose, spA0, 3, float_0_80422340, float_10_80422344);
		}
	}
}

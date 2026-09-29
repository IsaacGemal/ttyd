#include "eff/eff_hibashira.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2
extern const u32 dat_80422338;
extern const f32 float_deg2rad_8042233c;
extern const f32 float_0_80422340;
extern const f32 float_10_80422344;

inline GXColor makeHibashiraColor(s32 alpha) {
	GXColor color = *(GXColor*)&dat_80422338;
	color.a = alpha;
	return color;
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

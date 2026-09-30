#include "eff/eff_laser.h"
#include "driver/animdrv.h"
#include "memory.h"

const u32 dat_804223a4 = 0xFFFFFF00;
extern const f32 float_deg2rad_804223a8;
extern const f32 float_0_804223ac;
extern const f32 float_10_804223b0;

EffectEntry* effLaserEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effLaserData* data;

	entry = effEntry();
	entry->type = "Laser";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effLaserData) * entry->count);
	entry->userdata = data;
	entry->callback = effLaserMain;
	entry->flags |= 2;
	data->field_0x0 = type;
	data->field_0x10 = x;
	data->field_0x14 = y;
	data->field_0x18 = z;
	data->position.x = data->field_0x10;
	data->position.y = 1000.0f + data->field_0x14;
	data->position.z = data->field_0x18;
	data->scale = 1.0f;
	data->field_0x24 = 0;
	data->timer = 0;
	data->animId = -1;
	data->alpha = 0xff;
	data->state = 0;
	return entry;
}

static inline void laserSetColor(effLaserData* data) {
	GXColor color = *(GXColor*)&dat_804223a4;
	color.a = data->alpha;
	animPoseSetMaterialEvtColor(data->animId, color);
}

void effLaserDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effLaserData* data = effect->userdata;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;

	if (data->animId != -1) {
		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
		PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
		PSMTXRotRad(rotMtx, 'y', float_deg2rad_804223a8 * -camGetPtr(camId)->viewYaw);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		animPoseSetMaterialFlagOn(data->animId, 0x40);
		laserSetColor(data);
		animPoseMain(data->animId);
		animPoseDrawMtx(data->animId, mtx, 1, float_0_804223ac, float_10_804223b0);
		animPoseDrawMtx(data->animId, mtx, 2, float_0_804223ac, float_10_804223b0);
		animPoseDrawMtx(data->animId, mtx, 3, float_0_804223ac, float_10_804223b0);
	}
}

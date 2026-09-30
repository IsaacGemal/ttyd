#include "eff/eff_laser.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/fadedrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"

const GXColor dat_804223a0 = { 0xFF, 0xFF, 0xFF, 0xFF };
const u32 dat_804223a4 = 0xFFFFFF00;
extern const f32 float_deg2rad_804223a8;
extern const f32 float_0_804223ac;
extern const f32 float_10_804223b0;
const char str_804223B4[] = "S_1";
const f32 float_0p5_804223b8 = 0.5f;
const f32 float_500_804223bc = 500.0f;
const char str_804223C0[] = "A_1";

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

void effLaserMain(EffectEntry* effect) {
	s32 inBattle;
	Vec position;
	Vec disp = { 0 };
	effLaserData* data = effect->userdata;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;

	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (data->animId != -1) {
			animPoseRelease(data->animId);
		}
		effDelete(effect);
		return;
	}
	if (!animGroupBaseAsync("MOBJ_EFF_laser", inBattle, NULL)) {
		return;
	}
	switch (data->state) {
	case 0:
		if (data->animId == -1) {
			data->animId = animPoseEntry("MOBJ_EFF_laser", inBattle);
			animPoseSetAnim(data->animId, str_804223B4, TRUE);
			animPoseSetLocalTimeRate(data->animId, float_0p5_804223b8);
		}
		data->state++;
		break;
	case 1:
		data->timer++;
		data->position.y = intplGetValue(1, data->timer, 0x3c, float_500_804223bc + data->field_0x14, data->field_0x14);
		if (data->timer >= 0x3c) {
			animPoseSetAnim(data->animId, str_804223C0, TRUE);
			animPoseSetLocalTimeRate(data->animId, float_0p5_804223b8);
			data->state += 2;
		}
		break;
	case 2:
		if (fadeIsFinish()) {
			fadeEntry(0x10, 0x64, *(GXColor*)&dat_804223a0);
			animPoseSetAnim(data->animId, str_804223C0, TRUE);
			animPoseSetLocalTimeRate(data->animId, float_0p5_804223b8);
			data->state++;
		}
		break;
	case 3:
		data->alpha--;
		if (data->alpha < 0) {
			animPoseRelease(data->animId);
			effDelete(effect);
			return;
		}
		break;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effLaserDisp, effect);
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

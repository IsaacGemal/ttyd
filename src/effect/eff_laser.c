#include "eff/eff_laser.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/fadedrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"


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
			animPoseSetAnim(data->animId, "S_1", TRUE);
			animPoseSetLocalTimeRate(data->animId, 0.5f);
		}
		data->state++;
		break;
	case 1:
		data->timer++;
		data->position.y = intplGetValue(1, data->timer, 0x3c, 500.0f + data->field_0x14, data->field_0x14);
		if (data->timer >= 0x3c) {
			animPoseSetAnim(data->animId, "A_1", TRUE);
			animPoseSetLocalTimeRate(data->animId, 0.5f);
			data->state += 2;
		}
		break;
	case 2:
		if (fadeIsFinish()) {
			fadeEntry(0x10, 0x64, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
			animPoseSetAnim(data->animId, "A_1", TRUE);
			animPoseSetLocalTimeRate(data->animId, 0.5f);
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
	GXColor color = (GXColor){ 0xFF, 0xFF, 0xFF, 0x00 };
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
		PSMTXRotRad(rotMtx, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		animPoseSetMaterialFlagOn(data->animId, 0x40);
		laserSetColor(data);
		animPoseMain(data->animId);
		animPoseDrawMtx(data->animId, mtx, 1, 0.0f, 10.0f);
		animPoseDrawMtx(data->animId, mtx, 2, 0.0f, 10.0f);
		animPoseDrawMtx(data->animId, mtx, 3, 0.0f, 10.0f);
	}
}

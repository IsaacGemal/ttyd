#include "eff/eff_uranoko.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"

extern const f32 float_0p125_80422330;

EffectEntry* effUranokoEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effUranokoData* data;

	effect = effEntry();
	effect->type = "Uranoko";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effUranokoData) * effect->count);
	effect->userdata = data;
	effect->callback = effUranokoMain;
	effect->flags |= 2;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->baseScale = 1.0f;
	data->scale = 0.0f;
	data->field_0x18 = 0;
	if (count <= 0) {
		data->timer = 1000;
	}
	else {
		data->timer = count;
	}
	data->alpha = 0xFF;
	data->animId = -1;

	return effect;
}

void effUranokoMain(EffectEntry* effect) {
	effUranokoData* data = effect->userdata;
	Vec disp;
	BOOL inBattle;

	disp = initializeVec(&data->position);

	inBattle = gp->inBattle != 0;

	if (!animGroupBaseAsync("MOBJ_EFF_uranoko", inBattle, NULL)) {
		return;
	}

	if (data->animId == -1) {
		data->animId = animPoseEntry("MOBJ_EFF_uranoko", inBattle);
		animPoseSetAnim(data->animId, "A_1", 1);
	}

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->timer = 16;
	}

	if (data->timer < 1000) {
		data->timer--;
	}

	if (data->timer < 16) {
		data->alpha = data->timer * 16;
	}

	if (data->timer < 0) {
		if (data->animId != -1) {
			animPoseRelease(data->animId);
		}
		effDelete(effect);
	}
	else {
		f32 t;
		data->field_0x18++;
		t = 1.0f - data->scale;
		data->scale = t * float_0p125_80422330 + data->scale;
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effUranokoDisp, effect);
	}
}

// NONMATCHING (99.87%): same instructions; only register numbers or operands differ.
void effUranokoDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effUranokoData* data = effect->userdata;
	CameraEntry* cam;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	f32 deg;
	f32 s;

	if (data->animId == -1) {
		return;
	}

	PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
	s = data->scale * data->baseScale;
	PSMTXScale(scale, s, s, s);
	cam = camGetPtr(camId);
	deg = 0.017453292f;
	PSMTXRotRad(rot, 'y', -cam->viewYaw * deg);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, trans);
	animPoseSetMaterialFlagOn(data->animId, 0x40);
	animPoseSetMaterialEvtColor(data->animId, (GXColor){ 0xFF, 0xFF, 0xFF, data->alpha });
	animPoseMain(data->animId);
	animPoseDrawMtx(data->animId, trans, 1, 0.0f, 6.0f);
	animPoseDrawMtx(data->animId, trans, 2, 0.0f, 6.0f);
	animPoseDrawMtx(data->animId, trans, 3, 0.0f, 6.0f);
}

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

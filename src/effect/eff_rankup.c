#include "eff/eff_rankup.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>

static const GXColor sRankupColor = { 0xFF, 0xFF, 0xFF, 0x00 };

EffectEntry* effRankupEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effRankupData* data;

	entry = effEntry();
	entry->type = "Rankup";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effRankupData) * entry->count);
	entry->userdata = data;
	entry->callback = effRankupMain;
	entry->flags |= 2;

	data->field_0x0 = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = 1.0f;
	data->animId = -1;
	data->field_0x18 = 0;
	data->field_0x1C = 0;
	data->field_0x20 = 0xFF;

	return entry;
}

void effRankupMain(EffectEntry* effect) {
	s32 inBattle;
	Vec disp;
	effRankupData* data = effect->userdata;

	disp = initializeVec(&data->position);

	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (data->animId != -1) {
			animPoseRelease(data->animId);
		}
		effDelete(effect);
	}
	else {
		switch (data->field_0x18) {
		case 0:
			if (!animGroupBaseAsync("EFF_TekiKyouka", inBattle, NULL)) {
				return;
			}
			data->animId = animPoseEntry("EFF_TekiKyouka", inBattle);
			animPoseSetAnim(data->animId, "Z_1", 1);
			data->field_0x18++;
			break;
		case 1:
			if (animPoseGetLoopTimes(data->animId) > 1.0f) {
				animPoseRelease(data->animId);
				effDelete(effect);
				return;
			}
			break;
		}
		if (!(animPoseGetLoopTimes(data->animId) < 0.61538464f)) {
			data->field_0x20 -= 0x14;
			if (data->field_0x20 < 0) {
				data->field_0x20 = 0;
			}
		}
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effRankupDisp, effect);
	}
}

// unknown function, is used for many functions to "initialize"/copy some value into the local stack, with an extra vec
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

inline GXColor makeRankupColor(u8 alpha) {
	GXColor color = sRankupColor;
	color.a = alpha;
	return color;
}

void effRankupDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effRankupData* data = effect->userdata;
	Mtx sp70;
	Mtx sp40;
	Mtx sp10;

	if (data->animId != -1) {
		PSMTXTrans(sp70, data->position.x, data->position.y, data->position.z);
		PSMTXScale(sp40, data->scale, data->scale, data->scale);
		PSMTXRotRad(sp10, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
		PSMTXConcat(sp70, sp10, sp70);
		PSMTXConcat(sp70, sp40, sp70);
		animPoseSetMaterialFlagOn(data->animId, 0x40);
		animPoseSetMaterialEvtColor(data->animId, makeRankupColor(data->field_0x20));
		animPoseMain(data->animId);
		animPoseDrawMtx(data->animId, sp70, 1, 0.0f, 6.0f);
		animPoseDrawMtx(data->animId, sp70, 2, 0.0f, 6.0f);
		animPoseDrawMtx(data->animId, sp70, 3, 0.0f, 6.0f);
	}
}

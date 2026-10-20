#include "eff/eff_queen.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"

#include <dolphin/mtx.h>

// Local copy of system.c's inline helper so it inlines here (as in eff_ultra_hammer.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effQueenEntry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effQueenData* data;

	entry = effEntry();
	entry->type = "Queen";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effQueenData) * entry->count);
	entry->userdata = data;
	entry->callback = effQueenMain;
	entry->flags |= 2;

	data->field_0x0 = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->field_0x14 = scale;
	data->field_0x10 = 0.0f;

	// 1000 means no time limit; the main loop only counts down below it.
	if (lifetime <= 0) {
		data->field_0x20 = 1000;
	} else {
		data->field_0x20 = lifetime;
	}

	data->field_0x1C = 0;
	data->animId = -1;
	data->field_0x24 = 0;
	data->field_0x28 = 0xFF;
	data->field_0x10 = 1.0f;

	return entry;
}

void effQueenMain(EffectEntry* effect) {
	effQueenData* data = effect->userdata;
	Vec disp;
	BOOL inBattle;
	s32 maxAlpha;

	disp = initializeVec(&data->position);

	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x20 = 100; // forced removal: fade out over the last 100 ticks
	}

	if (animGroupBaseAsync("MOBJ_EFF_queen_tornade", inBattle, NULL) == 0) {
		return;
	}

	if (data->animId == -1) {
		data->animId = animPoseEntry("MOBJ_EFF_queen_tornade", inBattle);
		animPoseSetAnim(data->animId, "S_1", 1);
	}

	if (data->field_0x20 < 1000) {
		data->field_0x20--;
	}

	if (data->field_0x20 < 0) {
		if (data->animId != -1) {
			animPoseRelease(data->animId);
		}
		effDelete(effect);
		return;
	}

	if (data->field_0x20 < 100) {
		data->field_0x24 -= 3;
	} else {
		data->field_0x24 += 3;
	}

	maxAlpha = (s32)(0.8f * data->field_0x28);
	if (data->field_0x24 > maxAlpha) {
		data->field_0x24 = maxAlpha;
	}

	if (data->field_0x24 < 0) {
		data->field_0x24 = 0;
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effQueenDisp, effect);
}

void effQueenDisp(CameraId camId, void* param) {
	EffectEntry* entry = param;
	effQueenData* data = entry->userdata;
	Mtx mtx0, mtx1, rot;

	if (data->animId == -1) {
		return;
	}

	PSMTXTrans(mtx0, data->position.x, data->position.y, data->position.z);
	PSMTXScale(mtx1, data->field_0x10 * data->field_0x14, data->field_0x14, data->field_0x10 * data->field_0x14);

	PSMTXRotRad(rot, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));

	PSMTXConcat(mtx0, rot, mtx0);
	PSMTXConcat(mtx0, mtx1, mtx0);

	animPoseSetMaterialFlagOn(data->animId, 0x40);

	animPoseSetMaterialEvtColor(data->animId, (GXColor){ 0xFF, 0xFF, 0xFF, data->field_0x24 });

	animPoseMain(data->animId);
	animPoseDrawMtx(data->animId, mtx0, 1, 0.0f, 10.0f);
	animPoseDrawMtx(data->animId, mtx0, 2, 0.0f, 10.0f);
	animPoseDrawMtx(data->animId, mtx0, 3, 0.0f, 10.0f);
}

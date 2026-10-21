#include "eff/eff_scanning.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern GlobalWork* gp;

EffectEntry* effScanningEntry(s32 int2, f32 float3, f32 float4, f32 float5) {
	EffectEntry* entry;
	effScanningData* data;
	
	
	entry = effEntry();
	entry->type = "Scanning";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effScanningData) * entry->count);
	entry->userdata = data;
	entry->callback = effScanningMain;
	entry->flags |= 2;

	data->field_0x0 = int2;
	data->position.x = float3;
	data->position.y = float4;
	data->position.z = float5;
	data->scale = 1.0f;
	data->animId = -1;
	data->state = 0;
	data->field_0x1C = 0;
	data->field_0x20 = 255;

	return entry;
}

// Local copy of system.c's inline helper so it inlines here (as in eff_queen.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effScanningMain(EffectEntry* effect) {
	BOOL inBattle;
	effScanningData* data = effect->userdata;
	Vec disp = initializeVec(&data->position);

	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) { //soft delete
		effect->flags &= ~4;
		if (data->animId != -1) {
			animPoseRelease(data->animId);
		}
		effDelete(effect);
	}
	else {
		switch (data->state) {
		case 0:
			if (!animGroupBaseAsync("EFF_Scan", inBattle, NULL)) {
				return;
			}
			data->animId = animPoseEntry("EFF_Scan", (u32)inBattle);
			animPoseSetAnim(data->animId, "Z_1", 1);
			data->state++;
			break;
		case 1:
			if (animPoseGetLoopTimes(data->animId) > 1.0f) {
				animPoseRelease(data->animId);
				effDelete(effect);
				return;
			}
			break;
		}
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effScanningDisp, effect);
	}
}

void effScanningDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effScanningData* data = effect->userdata;
	Mtx trans;
	Mtx scale;
	Mtx rot;

	if (data->animId == -1) {
		return;
	}

	PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, trans);
	animPoseMain(data->animId);
	animPoseDrawMtx(data->animId, trans, 1, 0.0f, 10.0f);
	animPoseDrawMtx(data->animId, trans, 2, 0.0f, 10.0f);
	animPoseDrawMtx(data->animId, trans, 3, 0.0f, 10.0f);
}

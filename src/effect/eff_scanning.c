#include "eff/eff_scanning.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern GlobalWork* gp;

typedef struct effScanningData {
	s32 field_0x0; //0x0
	Vec position; //0x4
	f32 scale; //0x10
	s32 animId; //0x14
	s32 state; //0x18
	s32 field_0x1C; //0x1C
	s32 field_0x20; //0x20
} effScanningData;


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

void effScanningMain(EffectEntry* effect) {
	BOOL inBattle;
#ifdef MUST_MATCH
	// the position is built in one Vec and copied into another
	Vec disp;
	Vec pos = { 0 };
	effScanningData* data = effect->userdata;
	pos.x = data->position.x;
	pos.y = data->position.y;
	pos.z = data->position.z;
	disp = pos;
#else
	effScanningData* data = effect->userdata;
	Vec disp = data->position;
#endif

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
	CameraEntry* cam;
	f32 deg;
#ifdef MUST_MATCH
	f32 rad;
#endif

	if (data->animId == -1) {
		return;
	}

	PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	cam = camGetPtr(camId);
#ifdef MUST_MATCH
	rad = -cam->viewYaw;
	deg = 0.017453292f;
	rad = deg * rad;
	PSMTXRotRad(rot, 'y', rad);
#else
	deg = 0.017453292f;
	PSMTXRotRad(rot, 'y', -cam->viewYaw * deg);
#endif
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, trans);
	animPoseMain(data->animId);
	animPoseDrawMtx(data->animId, trans, 1, 0.0f, 10.0f);
	animPoseDrawMtx(data->animId, trans, 2, 0.0f, 10.0f);
	animPoseDrawMtx(data->animId, trans, 3, 0.0f, 10.0f);
}

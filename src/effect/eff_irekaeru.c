#include "eff/eff_irekaeru.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

typedef struct EffIrekaeruData {
	s32 type;      // 0x0
	f32 x;         // 0x4
	f32 y;         // 0x8
	f32 z;         // 0xC
	f32 scale;     // 0x10
	s32 poseId;    // 0x14
	s32 state;     // 0x18
	s32 field_0x1C; // 0x1C
	s32 alpha;     // 0x20
} EffIrekaeruData;


EffectEntry* effIrekaeruEntry(s32 type, f32 x, f32 y, f32 z) {
	EffIrekaeruData* userdata;
	EffectEntry* effect;

	effect = effEntry();
	effect->type = "Irekaeru";
	effect->count = 1;
	userdata = __memAlloc(HEAP_EFFECT, sizeof(EffIrekaeruData) * effect->count);
	effect->userdata = userdata;
	effect->callback = effIrekaeruMain;
	effect->flags |= 2;

	userdata->type = type;
	userdata->x = x;
	userdata->y = y;
	userdata->z = z;
	userdata->scale = 1.0f;
	userdata->poseId = -1;
	userdata->state = 0;
	userdata->field_0x1C = 0;
	userdata->alpha = 0;

	return effect;
}

void effIrekaeruMain(EffectEntry* effect) {
	EffIrekaeruData* userdata = effect->userdata;
	Vec pos2;
	Vec pos;
	s32 inBattle;

	pos = (Vec){0.0f, 0.0f, 0.0f};
	pos.x = userdata->x;
	pos.y = userdata->y;
	pos.z = userdata->z;
	pos2 = pos;
	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (userdata->poseId != -1) {
			animPoseRelease(userdata->poseId);
		}
		effDelete(effect);
		return;
	}

	switch (userdata->state) {
		case 0:
			if (!animGroupBaseAsync("EFF_Irekae", inBattle, NULL)) {
				return;
			}
			userdata->poseId = animPoseEntry("EFF_Irekae", inBattle);
			animPoseSetAnim(userdata->poseId, "Z_1", TRUE);
			userdata->state++;
			break;
		case 1:
			if (animPoseGetLoopTimes(userdata->poseId) > 1.0f) {
				animPoseRelease(userdata->poseId);
				effDelete(effect);
				return;
			}
			break;
	}

	if (animPoseGetLoopTimes(userdata->poseId) < 0.75f) {
		userdata->alpha += 20;
		if (userdata->alpha > 0xFF) {
			userdata->alpha = 0xFF;
		}
	} else {
		userdata->alpha -= 20;
		if (userdata->alpha < 0) {
			userdata->alpha = 0;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effIrekaeruDisp, effect);
}

void effIrekaeruDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffIrekaeruData* userdata = effect->userdata;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;

	if (userdata->poseId != -1) {
		PSMTXTrans(mtx, userdata->x, userdata->y, userdata->z);
		PSMTXScale(scaleMtx, userdata->scale, userdata->scale, userdata->scale);
		PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		animPoseSetMaterialFlagOn(userdata->poseId, 0x40);
		animPoseSetMaterialEvtColor(userdata->poseId, (GXColor){ 0xFF, 0xFF, 0xFF, userdata->alpha });
		animPoseMain(userdata->poseId);
		animPoseDrawMtx(userdata->poseId, mtx, 1, 0.0f, 2.0f);
		animPoseDrawMtx(userdata->poseId, mtx, 2, 0.0f, 2.0f);
		animPoseDrawMtx(userdata->poseId, mtx, 3, 0.0f, 2.0f);
	}
}

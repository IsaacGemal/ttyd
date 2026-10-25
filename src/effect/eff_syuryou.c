#include "eff/eff_syuryou.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>


/* Full work data for the syuryou effect. Its first fields (type/position/scale)
   line up with EffSyuryouData in the header, which las_28.c reads. */
typedef struct SyuryouData {
	s32 type;       // 0x00
	Vec position;   // 0x04
	f32 scale;      // 0x10
	s32 poseId;     // 0x14
	s32 alpha;      // 0x18, the pose material colour's alpha
	s32 timer;      // 0x1C
	u8 pad_0x20[4]; // 0x20
} SyuryouData;

EffectEntry* effSyuryouEntry(s32 type, s32 duration, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	SyuryouData* data;

	effect = effEntry();
	effect->type = "Syuryou";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, effect->count * sizeof(SyuryouData));
	effect->userdata = data;
	effect->callback = effSyuryouMain;
	effect->flags |= 2;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = 1.0f;
	data->alpha = 0xFF;
	if (duration <= 0) {
		data->timer = 1000;
	} else {
		data->timer = duration;
	}
	/* Retail stores 0xff here a second time; it is required to match. */
	data->alpha = 0xFF;
	data->poseId = -1;
	return effect;
}

void effSyuryouMain(EffectEntry* effect) {
	SyuryouData* data = effect->userdata;
	Vec pos2;
	Vec pos;
	s32 type;
	s32 inBattle;

	pos = (Vec){ data->position.x, data->position.y, data->position.z };
	pos2 = pos;
	type = data->type;
	inBattle = gp->inBattle ? 1 : 0;

	switch (type) {
		case 0:
			if (!animGroupBaseAsync("EFF_batug_tue", inBattle, NULL)) {
				return;
			}
			if (data->poseId == -1) {
				data->poseId = animPoseEntry("EFF_batug_tue", inBattle);
				animPoseSetAnim(data->poseId, "A_1", TRUE);
			}
			if (animPoseGetLoopTimes(data->poseId) >= 1.0f) {
				animPoseRelease(data->poseId);
				effDelete(effect);
				return;
			}
			break;
		case 1:
			if (!animGroupBaseAsync("MOBJ_EFF_baria", inBattle, NULL)) {
				return;
			}
			if (data->poseId == -1) {
				data->poseId = animPoseEntry("MOBJ_EFF_baria", inBattle);
				animPoseSetAnim(data->poseId, "A_1", TRUE);
			}
			if (animPoseGetLoopTimes(data->poseId) >= 1.0f) {
				animPoseSetAnim(data->poseId, "S_1", FALSE);
			}
			break;
	}

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->timer = 0x20;
	}
	if (data->timer < 1000) {
		data->timer--;
	}
	if (data->timer < 0x20) {
		data->alpha = data->timer * 8;
	}
	if (data->timer < 0) {
		if (data->poseId != -1) {
			animPoseRelease(data->poseId);
		}
		effDelete(effect);
		return;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effSyuryouDisp, effect);
}

void effSyuryouDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	SyuryouData* data = effect->userdata;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;

	if (data->poseId != -1) {
		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
		PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
		PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		animPoseSetMaterialFlagOn(data->poseId, 0x40);
		animPoseSetMaterialEvtColor(data->poseId, (GXColor){ 0xFF, 0xFF, 0xFF, data->alpha });
		animPoseMain(data->poseId);
		animPoseDrawMtx(data->poseId, mtx, 1, 0.0f, 10.0f);
		animPoseDrawMtx(data->poseId, mtx, 2, 0.0f, 10.0f);
		animPoseDrawMtx(data->poseId, mtx, 3, 0.0f, 10.0f);
	}
}

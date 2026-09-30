#include "eff/eff_syuryou.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Full white with a zero alpha, replaced by the effect's colour below.
const u32 dat_804222f8 = 0xFFFFFF00;

/* Full work data for the syuryou effect. Its first fields (type/position/scale)
   line up with EffSyuryouData in the header, which las_28.c reads. */
typedef struct SyuryouData {
	s32 type;       // 0x00
	Vec position;   // 0x04
	f32 scale;      // 0x10
	s32 poseId;     // 0x14
	s32 color;      // 0x18
	s32 timer;      // 0x1C
	s32 field_0x20; // 0x20
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
	data->color = 0xFF;
	if (duration <= 0) {
		data->timer = 1000;
	} else {
		data->timer = duration;
	}
	/* Retail stores 0xff here a second time; it is required to match. */
	data->color = 0xFF;
	data->poseId = -1;
	return effect;
}

static inline void syuryouSetColor(SyuryouData* data) {
	GXColor color = *(GXColor*)&dat_804222f8;
	color.a = data->color;
	animPoseSetMaterialEvtColor(data->poseId, color);
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
		syuryouSetColor(data);
		animPoseMain(data->poseId);
		animPoseDrawMtx(data->poseId, mtx, 1, 0.0f, 10.0f);
		animPoseDrawMtx(data->poseId, mtx, 2, 0.0f, 10.0f);
		animPoseDrawMtx(data->poseId, mtx, 3, 0.0f, 10.0f);
	}
}

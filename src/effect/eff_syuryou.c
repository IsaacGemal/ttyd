#include "eff/eff_syuryou.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Full white with a zero alpha, replaced by the effect's colour below.
const u32 dat_804222f8 = 0xFFFFFF00;

// The strings are reached only as offsets from the anchor, so keep them linked.
#pragma force_active on
// .rodata anchor used to build the effect's world position.
const Vec vec3_80301358 = { 0.0f, 0.0f, 0.0f };
const char str_80301364[16] = "EFF_batug_tue";
const char str_80301374[16] = "MOBJ_EFF_baria";
#pragma force_active reset

/* The original resolves the two .rodata group names as offsets from
   vec3_80301358, so mwcc pools them under that base instead of a
   compiler-generated pool symbol. */
#ifdef MUST_MATCH
#define SYU_STR(base, off, sym) ((base) + (off))
#else
#define SYU_STR(base, off, sym) ((const char*)&(sym))
#endif

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

void effSyuryouMain(EffectEntry* effect) {
	SyuryouData* data = effect->userdata;
	Vec pos2;
	Vec pos;
	s32 type;
	s32 inBattle;
	const char* base = (const char*)&vec3_80301358;

	pos = vec3_80301358;
	pos.x = data->position.x;
	pos.y = data->position.y;
	pos.z = data->position.z;
	pos2 = pos;
	type = data->type;
	inBattle = gp->inBattle ? 1 : 0;

	switch (type) {
		case 0:
			if (!animGroupBaseAsync(SYU_STR(base, 0xC, str_80301364), inBattle, NULL)) {
				return;
			}
			if (data->poseId == -1) {
				data->poseId = animPoseEntry(SYU_STR(base, 0xC, str_80301364), inBattle);
				animPoseSetAnim(data->poseId, "A_1", TRUE);
			}
			if (animPoseGetLoopTimes(data->poseId) >= 1.0f) {
				animPoseRelease(data->poseId);
				effDelete(effect);
				return;
			}
			break;
		case 1:
			if (!animGroupBaseAsync(SYU_STR(base, 0x1C, str_80301374), inBattle, NULL)) {
				return;
			}
			if (data->poseId == -1) {
				data->poseId = animPoseEntry(SYU_STR(base, 0x1C, str_80301374), inBattle);
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
		data->color = data->timer * 8;
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

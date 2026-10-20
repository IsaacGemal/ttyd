#include "eff/eff_las_mon.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

static const GXColor dat_80422408 = { 0xFF, 0xFF, 0xFF, 0x00 };

EffectEntry* effLasMonEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effLasMonData* data;

	entry = effEntry();
#ifdef MUST_MATCH
	/* MUST_MATCH: the extra explicit null reproduces the retail 8-byte string object. */
	entry->type = "LasMon\0";
#else
	entry->type = "LasMon";
#endif
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effLasMonData) * entry->count);
	entry->userdata = data;
	entry->callback = effLasMonMain;
	entry->flags |= 2;
	data->field_0x0 = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = 1.0f;
	data->field_0x18 = 0;
	data->animId = -1;
	data->field_0x1c = 0xff;
	data->field_0x20 = 1000;
	return entry;
}

void effLasMonMain(EffectEntry* effect) {
	BOOL inBattle;
	/* Retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
	Vec disp = { 0 };
	effLasMonData* data = effect->userdata;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;

	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x20 = 16;
	}
	if (data->field_0x20 < 1000) {
		data->field_0x20--;
	}
	if (data->field_0x20 < 16) {
		data->field_0x1c = data->field_0x20 << 4;
	}
	if (data->field_0x20 < 0) {
		if (data->animId != -1) {
			animPoseRelease(data->animId);
		}
		effDelete(effect);
	}
	else {
		if (!animGroupBaseAsync("MOBJ_EFF_las_mon", inBattle, NULL)) {
			return;
		}
		if (data->animId == -1) {
			data->animId = animPoseEntry("MOBJ_EFF_las_mon", inBattle);
			animPoseSetAnim(data->animId, "A_1", TRUE);
			data->field_0x18 = 0;
		}
		switch (data->field_0x18) {
		case 0:
			if (animPoseGetLoopTimes(data->animId) >= 1.0f) {
				animPoseSetAnim(data->animId, "S_1", TRUE);
				data->field_0x18++;
			}
			break;
#ifdef MUST_MATCH
		/* MUST_MATCH: the empty case 1 forces MWCC's switch decision tree; without it the
		   one-case switch folds to a single compare. */
		case 1:
			break;
#endif
		}
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effLasMonDisp, effect);
	}
}

/* Returning the colour through an inline aggregate forces MWCC to build it in the
   return temporary (r1+8) and copy it to the by-value argument slot (r1+0xC), as retail does. */
static inline GXColor effLasMonColor(u8 alpha) {
	GXColor color = dat_80422408;
	color.a = alpha;
	return color;
}

void effLasMonDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effLasMonData* data = effect->userdata;
	Mtx mtx1;
	Mtx mtx2;

	if (data->animId == -1) {
		return;
	}

	PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
	PSMTXScale(mtx2, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx1, mtx2, mtx1);
	animPoseSetMaterialFlagOn(data->animId, 0x40);
	animPoseSetMaterialEvtColor(data->animId, effLasMonColor(data->field_0x1c));
	animPoseMain(data->animId);
	animPoseDrawMtx(data->animId, mtx1, 1, 0.0f, 10.0f);
	animPoseDrawMtx(data->animId, mtx1, 2, 0.0f, 10.0f);
	animPoseDrawMtx(data->animId, mtx1, 3, 0.0f, 10.0f);
}

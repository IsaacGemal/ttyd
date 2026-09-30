#include "eff/eff_treasure_map.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

EffectEntry* effTreasureMapEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	EffTreasureMapData* data;

	entry = effEntry();
	entry->type = "Treasure_Map";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffTreasureMapData));
	entry->userdata = data;
	entry->callback = effTreasureMapMain;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->timer = 1000;
	data->alpha = 0xff;
	data->scale = 1.0f;
	data->angle = 0.0f;
	data->animId = -1;
	data->cameraId = 4;
	return entry;
}

void effTreasureMapMain(EffectEntry* effect) {
	EffTreasureMapData* data = effect->userdata;
	Vec pos2;
	Vec pos;
	s32 inBattle;

	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;
	inBattle = gp->inBattle != 0;

	if (data->animId == -1) {
		if (!animGroupBaseAsync("EFF_treasure_map", inBattle, NULL)) {
			return;
		}
		data->animId = animPoseEntry("EFF_treasure_map", inBattle);
		animPoseSetAnim(data->animId, "Z_1", TRUE);
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
		return;
	}

	data->counter++;
	dispEntry(data->cameraId, dispCalcZ(&pos2), 2, effTreasureMapDisp, effect);
}

void effTreasureMapDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffTreasureMapData* data = effect->userdata;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx mtx;
	f32 baseScale;
	s32 i;

	camGetPtr(camId);
	if (data->animId == -1) {
		return;
	}
	for (i = 0; i < 2; i++) {
		baseScale = 1.1f * data->scale;
		if (i == 0) {
			PSMTXTrans(trans, data->x, data->y, data->z);
			PSMTXScale(scale, 1.06f * baseScale, 1.06f * baseScale, 1.06f * baseScale);
			PSMTXRotRad(rot, 'y', 0.017453292f * data->angle);
			PSMTXConcat(trans, scale, mtx);
			PSMTXConcat(mtx, rot, mtx);
			animPoseSetMaterialFlagOn(data->animId, 0x40);
			animPoseSetMaterialEvtColor(data->animId, (GXColor){ 0x2E, 0x1B, 0x08, data->alpha });
		}
		else {
			PSMTXTrans(trans, data->x, data->y, data->z);
			PSMTXScale(scale, baseScale, baseScale, baseScale);
			PSMTXRotRad(rot, 'y', 0.017453292f * data->angle);
			PSMTXConcat(trans, scale, mtx);
			PSMTXConcat(mtx, rot, mtx);
			animPoseSetMaterialFlagOn(data->animId, 0x40);
			animPoseSetMaterialEvtColor(data->animId, (GXColor){ 0xFF, 0xFF, 0xFF, data->alpha });
		}
		animPoseMain(data->animId);
		animPoseDrawMtx(data->animId, mtx, 1, 0.0f, 10.0f);
		animPoseDrawMtx(data->animId, mtx, 2, 0.0f, 10.0f);
		animPoseDrawMtx(data->animId, mtx, 3, 0.0f, 10.0f);
	}
}

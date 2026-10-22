#include "eff/eff_queen2.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_indirect.h"
#include "eff/eff_teresa.h"
#include "eff/n64/eff_akari_charge_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

EffectEntry* effQueen2Entry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effQueen2Data* data;
	Vec pos;

	entry = effEntry();
	entry->type = "Queen2";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effQueen2Data) * entry->count);
	entry->userdata = data;
	entry->callback = effQueen2Main;
	entry->flags |= 2;

	data->field_0x0 = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;

	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = x;
	pos.y = y;
	pos.z = z;

	data->baseScale = PSVECDistance(&camGetPtr(CAMERA_3D)->cameraPos, &pos) / 10.0f;
	data->scale = data->baseScale;
	data->alpha = 0;
	data->state = 0;
	data->phaseTimer = 0;
	data->chargeEntry = NULL;

	return entry;
}

void effQueen2Main(EffectEntry* effect) {
	effQueen2Data* data;
	Vec pos2;
	Vec pos;

	data = effect->userdata;

	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->position.x;
	pos.y = data->position.y;
	pos.z = data->position.z;
	pos2 = pos;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (data->chargeEntry != NULL) {
			effDelete(data->chargeEntry);
		}
		effDelete(effect);
		return;
	}

	switch (data->state) {
	case 0:
		data->alpha = (s32)intplGetValue(0xB, data->phaseTimer++, 0x3C, 0.0f, 128.0f);
		if (data->phaseTimer > 0x3C) {
			data->phaseTimer = 0;
			data->state++;
		}
		break;
	case 1: {
		f32 start = data->baseScale;
		s32 timer = data->phaseTimer++;

		data->scale = intplGetValue(0xB, timer, 0x78, start, 3.0f);
		if (data->phaseTimer > 0x78) {
			data->phaseTimer = 0;
			data->state++;
			data->chargeEntry = effAkariChargeN64Entry(0, 0, data->position.x, data->position.y, data->position.z, 1.0f);
			((EffAkariChargeWork*)data->chargeEntry->userdata)->reg0R = 0;
			((EffAkariChargeWork*)data->chargeEntry->userdata)->reg0G = 0x9A;
			((EffAkariChargeWork*)data->chargeEntry->userdata)->reg0B = 0xFF;
			((EffAkariChargeWork*)data->chargeEntry->userdata)->reg1R = 0xFF;
			((EffAkariChargeWork*)data->chargeEntry->userdata)->reg1G = 0xFF;
			((EffAkariChargeWork*)data->chargeEntry->userdata)->reg1B = 0xFF;
		}
		break;
	}
	case 2:
		if (++data->phaseTimer > 0x78) {
			data->phaseTimer = 0;
			data->state++;
			effTeresaEntry(1, data->position.x, data->position.y, data->position.z);
			effIndirectEntry(1, 0x3C, data->position.x, data->position.y, data->position.z);
		}
		break;
	case 3:
		if (++data->phaseTimer > 0x1E) {
			data->phaseTimer = 0;
			data->state++;
			effTeresaEntry(1, data->position.x, data->position.y, data->position.z);
			effIndirectEntry(1, 0x3C, data->position.x, data->position.y, data->position.z);
			effSoftDelete(data->chargeEntry);
		}
		break;
	case 4:
		effDelete(effect);
		return;
	}

	if (data->state >= 3) {
		data->alpha -= 2;
		if (data->alpha < 0) {
			data->alpha = 0;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effQueen2Disp, effect);
}

void effQueen2Disp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effQueen2Data* data = effect->userdata;
	CameraEntry* camera = camGetPtr(camId);
	Mtx mtx;
	Mtx scale;
	f32 s;

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);

	GXSetChanMatColor(4, (GXColor){ 0, 0, 0, data->alpha });

	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0xff, 0xff, 4);
	GXSetTevOp(0, GX_PASSCLR);
	GXSetCullMode(2);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);

	PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
	s = 10.0f * data->scale;
	PSMTXScale(scale, s, s, s);
	PSMTXConcat(mtx, scale, scale);
	PSMTXConcat(camera->view, scale, scale);
	GXLoadPosMtxImm(scale, 0);
	GXSetCurrentMtx(0);
	GXSetCullMode(2);
	GXDrawSphere(0x10, 0x10);
}

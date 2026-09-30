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

// .sbss2: the default .sbss2 is only used for compiler-generated pool
// templates, so pair it with a small-data section and place it by hand.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423360;

// Builds the channel material colour from the .sbss2 template plus the effect's alpha.
static inline GXColor makeQueen2Color(s32 alpha) {
	GXColor color = *(GXColor*)&unk_80423360;
	color.a = alpha;
	return color;
}

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

	data->field_0x14 = PSVECDistance(&camGetPtr(CAMERA_3D)->cameraPos, &pos) / 10.0f;
	data->field_0x10 = data->field_0x14;
	data->field_0x18 = 0;
	data->field_0x24 = 0;
	data->field_0x20 = 0;
	data->field_0x28 = NULL;

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
		if (data->field_0x28 != NULL) {
			effDelete(data->field_0x28);
		}
		effDelete(effect);
		return;
	}

	switch (data->field_0x24) {
	case 0:
		data->field_0x18 = (s32)intplGetValue(0xB, data->field_0x20++, 0x3C, 0.0f, 128.0f);
		if (data->field_0x20 > 0x3C) {
			data->field_0x20 = 0;
			data->field_0x24++;
		}
		break;
	case 1: {
		f32 start = data->field_0x14;
		s32 timer = data->field_0x20++;

		data->field_0x10 = intplGetValue(0xB, timer, 0x78, start, 3.0f);
		if (data->field_0x20 > 0x78) {
			data->field_0x20 = 0;
			data->field_0x24++;
			data->field_0x28 = effAkariChargeN64Entry(0, 0, data->position.x, data->position.y, data->position.z, 1.0f);
			((EffAkariChargeData*)data->field_0x28->userdata)->field_0x18 = 0;
			((EffAkariChargeData*)data->field_0x28->userdata)->field_0x1C = 0x9A;
			((EffAkariChargeData*)data->field_0x28->userdata)->field_0x20 = 0xFF;
			((EffAkariChargeData*)data->field_0x28->userdata)->field_0x28 = 0xFF;
			((EffAkariChargeData*)data->field_0x28->userdata)->field_0x2C = 0xFF;
			((EffAkariChargeData*)data->field_0x28->userdata)->field_0x30 = 0xFF;
		}
		break;
	}
	case 2:
		if (++data->field_0x20 > 0x78) {
			data->field_0x20 = 0;
			data->field_0x24++;
			effTeresaEntry(1, data->position.x, data->position.y, data->position.z);
			effIndirectEntry(1, 0x3C, data->position.x, data->position.y, data->position.z);
		}
		break;
	case 3:
		if (++data->field_0x20 > 0x1E) {
			data->field_0x20 = 0;
			data->field_0x24++;
			effTeresaEntry(1, data->position.x, data->position.y, data->position.z);
			effIndirectEntry(1, 0x3C, data->position.x, data->position.y, data->position.z);
			effSoftDelete(data->field_0x28);
		}
		break;
	case 4:
		effDelete(effect);
		return;
	}

	if (data->field_0x24 >= 3) {
		data->field_0x18 -= 2;
		if (data->field_0x18 < 0) {
			data->field_0x18 = 0;
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
	GXColor color;
	f32 s;

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);

	color = makeQueen2Color(data->field_0x18);
	GXSetChanMatColor(4, color);

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
	s = 10.0f * data->field_0x10;
	PSMTXScale(scale, s, s, s);
	PSMTXConcat(mtx, scale, scale);
	PSMTXConcat(camera->view, scale, scale);
	GXLoadPosMtxImm(scale, 0);
	GXSetCurrentMtx(0);
	GXSetCullMode(2);
	GXDrawSphere(0x10, 0x10);
}

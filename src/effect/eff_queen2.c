#include "eff/eff_queen2.h"
#include "driver/camdrv.h"
#include "memory.h"

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

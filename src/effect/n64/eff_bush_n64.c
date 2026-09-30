#include "eff/n64/eff_bush_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <stdlib.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

extern const char str_802FA390[];
extern const u32 dat_8041eb18;
extern const f32 float_deg2rad_8041eb1c;
extern const f32 float_4_8041eb20;
extern const f32 float_1_8041eb24;
extern const f32 float_0_8041eb28;

const f64 double_to_int_802fa388 = 4503601774854144.0;

static inline void bushSetColor(u8 a) {
	GXColor color = *(GXColor*)&dat_8041eb18;
	color.a = a;
	GXSetTevColor(1, color);
}

// .data: retail vertex array (two 0x38-byte effVtxArray entries).
static u32 size12x12_tex64x64_vtx[28] = {
	0xFFFAFFFA, 0x00000000, 0x00001955, 0x11FF0006,
	0xFFFA0000, 0x08000000, 0x195511FF, 0x00060006,
	0x00000800, 0x080068AC, 0x10FFFFFA, 0x00060000,
	0x00000800, 0x68AC10FF, 0xFFFAFFFA, 0x00000000,
	0x08001837, 0x21FF0006, 0xFFFA0000, 0x08000800,
	0x183721FF, 0x00060006, 0x00000800, 0x1000306C,
	0x25FFFFFA, 0x00060000, 0x00001000, 0x306C25FF,
};

EffectEntry* effBushN64Entry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	BushN64Data* data;
	BushN64Data* p;
	s32 i;

	effect = effEntry();
	effect->type = str_802FA390;
	effect->count = 5;
	data = __memAlloc(HEAP_EFFECT, 0x104);
	effect->userdata = data;
	effect->callback = effBushMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->lifetime = 100;
	data->field_0x2c = 0;
	data->field_0x24 = 0;
	p = data + 1;
	for (i = 1; i < 5; i++, p++) {
		p->x = (f32)(rand() % 30 - 15);
		p->y = float_0_8041eb28;
		p->z = (f32)((rand() & 0x1E) - 15);
		p->field_0x18 = (f32)(rand() & 0x168);
		p->field_0x20 = (f32)(rand() & 0x168);
		p->field_0x1c = (f32)((rand() % 100) / 10);
		p->field_0x10 = float_0_8041eb28;
		p->field_0x14 = float_0_8041eb28;
	}
	return effect;
}

void effBushDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	BushN64Data* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	s32 i;
	f32 deg2rad;

	cam = camGetPtr(camId);
	data = effect->userdata;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041eb1c * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, work);
	PSMTXConcat(cam->view, work, work);

	bushSetColor(data->field_0x24);
	data++;
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scale, float_4_8041eb20, float_1_8041eb24, float_0_8041eb28);
	GXLoadTexMtxImm(scale, 0x1e, 1);
	effGetTexObjN64(0x2a, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xa, 8, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 3);
	GXSetVtxDesc(0xb, 3);
	GXSetVtxDesc(0xd, 3);
	GXSetVtxAttrFmt(0, 9, 1, 3, 0);
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xc);
	GXSetArray(9, size12x12_tex64x64_vtx, 0xe);
	GXSetArray(0xb, (u8*)size12x12_tex64x64_vtx + 0xa, 0xe);
	GXSetArray(0xd, (u8*)size12x12_tex64x64_vtx + 6, 0xe);

	deg2rad = float_deg2rad_8041eb1c;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXRotRad(rot, 'z', deg2rad * data->field_0x18);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'y', deg2rad * data->field_0x20);
		PSMTXConcat(trans, rot, trans);
		PSMTXConcat(work, trans, trans);
		GXLoadPosMtxImm(trans, 0);
		GXSetCurrentMtx(0);
		GXBegin(0x90, 0, 0xc);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		tri2(6, 5, 4, 4, 7, 6, 4, 4);
	}
}

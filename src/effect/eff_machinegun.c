#include "eff/eff_machinegun.h"
#include "driver/dispdrv.h"
#include "math_ext.h"
#include "memory.h"
#include <dolphin/mtx.h>

EffectEntry* effMachinegunEntry(s32 type, f32 x, f32 y, f32 z, f32 angle, f32 speed, f32 scale) {
	EffectEntry* entry;
	effMachinegunData* data;

	entry = effEntry();
	entry->type = "Machinegun";
	entry->count = 1;
	data = __memAlloc(3, 0x28);
	entry->userdata = data;
	entry->callback = effMachinegunMain;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->angle = angle;
	data->scale = scale;
	data->lifetime = 0xFF;
	data->speed = speed;
	return entry;
}

void effMachinegunMain(EffectEntry* effect) {
	effMachinegunData* data = effect->userdata;
	Vec position;
	Vec disp = { 0.0f, 0.0f, 0.0f };

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;
	data->position.x += data->speed * (f32)cos(6.2831855f * data->angle / 360.f);
	data->position.y += data->speed * (f32)sin(6.2831855f * data->angle / 360.f);
	data->speed *= 0.9f;
	data->lifetime -= 10;
	if (data->lifetime < 0) {
		effDelete(effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMachinegunDisp, effect);
	}
}

static inline void effMachinegunSetColor(effMachinegunData* data) {
	GXColor color = *(GXColor*)&"|tl";
	color.a = data->lifetime;
	GXSetChanMatColor(4, color);
}

void effMachinegunDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effMachinegunData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj tex;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	f32 x1;
	f32 x0;

	cam = camGetPtr(camId);

	effGetTexObj(0x89, &tex);
	GXLoadTexObj(&tex, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	effMachinegunSetColor(data);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
	GXSetTevAlphaIn(0, 7, 5, 4, 7);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

	PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXRotRad(rot, 'z', (180.f + data->angle) * 0.017453292f);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(cam->view, mtx, mtx);

	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);

	GXBegin(0x80, 0, 4);
	// Splitting the negation and multiply keeps x0 in f2 and x1 in f1.
	{
		f32 f = 32.0f;
		f32 h = 0.5f;
		x0 = -f * h;
	}
	x1 = 16.0f;
	GXPosition3f32(x0, x1, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(x1, x1, 0.0f);
	GXTexCoord2f32(2.0f, 0.0f);
	GXPosition3f32(x1, x0, 0.0f);
	GXTexCoord2f32(2.0f, 2.0f);
	GXPosition3f32(x0, x0, 0.0f);
	GXTexCoord2f32(0.0f, 2.0f);
}

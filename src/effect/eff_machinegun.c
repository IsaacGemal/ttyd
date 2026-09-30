#include "eff/eff_machinegun.h"
#include "driver/dispdrv.h"
#include "math_ext.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern const char str_8030142C[];
extern u32 str_804223D8;
extern const f32 float_deg2rad_804223dc;
extern const f32 float_180_804223e0;
extern const f32 float_32_804223e4;
extern const f32 float_0p5_804223e8;
extern const f32 float_16_804223ec;
// MUST_MATCH: the original reloads these two .sdata2 constants before every FIFO store;
// as plain `const` MWCC keeps them live and the vertex block differs.
#ifdef MUST_MATCH
extern volatile const f32 float_0_804223f0;
extern volatile const f32 float_2_804223f4;
#else
extern const f32 float_0_804223f0;
extern const f32 float_2_804223f4;
#endif
extern const f32 float_6p2832_804223f8;
extern const f32 float_360_804223fc;
extern const f32 float_0p9_80422400;

EffectEntry* effMachinegunEntry(s32 type, f32 x, f32 y, f32 z, f32 angle, f32 scale, f32 field_0x10) {
	EffectEntry* entry;
	effMachinegunData* data;

	entry = effEntry();
	entry->type = str_8030142C;
	entry->count = 1;
	data = __memAlloc(3, 0x28);
	entry->userdata = data;
	entry->callback = effMachinegunMain;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->angle = angle;
	data->field_0x10 = field_0x10;
	data->lifetime = 0xFF;
	data->scale = scale;
	return entry;
}

void effMachinegunMain(EffectEntry* effect) {
	effMachinegunData* data = effect->userdata;
#ifdef MUST_MATCH
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif
	data->position.x += data->scale * (f32)cos(float_6p2832_804223f8 * data->angle / float_360_804223fc);
	data->position.y += data->scale * (f32)sin(float_6p2832_804223f8 * data->angle / float_360_804223fc);
	data->scale *= float_0p9_80422400;
	data->lifetime -= 10;
	if (data->lifetime < 0) {
		effDelete(effect);
	}
	else {
#ifdef MUST_MATCH
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMachinegunDisp, effect);
#else
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMachinegunDisp, effect);
#endif
	}
}

static inline void effMachinegunSetColor(effMachinegunData* data) {
	GXColor color = *(GXColor*)&str_804223D8;
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
	PSMTXScale(scale, data->field_0x10, data->field_0x10, data->field_0x10);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_804223dc);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXRotRad(rot, 'z', (float_180_804223e0 + data->angle) * float_deg2rad_804223dc);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(cam->view, mtx, mtx);

	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);

	GXBegin(0x80, 0, 4);
#ifdef MUST_MATCH
	// MUST_MATCH: splitting the negation and multiply keeps x0 in f2 and x1 in f1.
	x0 = -float_32_804223e4;
	x1 = float_16_804223ec;
	x0 = x0 * float_0p5_804223e8;
#else
	x0 = -float_32_804223e4 * float_0p5_804223e8;
	x1 = float_16_804223ec;
#endif
	GXPosition3f32(x0, x1, float_0_804223f0);
	GXTexCoord2f32(float_0_804223f0, float_0_804223f0);
	GXPosition3f32(x1, x1, float_0_804223f0);
	GXTexCoord2f32(float_2_804223f4, float_0_804223f0);
	GXPosition3f32(x1, x0, float_0_804223f0);
	GXTexCoord2f32(float_2_804223f4, float_2_804223f4);
	GXPosition3f32(x0, x0, float_0_804223f0);
	GXTexCoord2f32(float_0_804223f0, float_2_804223f4);
}

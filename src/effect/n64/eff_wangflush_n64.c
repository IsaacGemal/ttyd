#include "eff/n64/eff_wangflush_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "math_ext.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804232c4;
__declspec(section ".sdata") GXColor unk_804232c0;


extern const f32 float_deg2rad_80420114;
extern const f32 float_0p03125_80420118;
extern const f32 float_0_8042011c;
extern const f32 float_0p5_80420120;
extern const f32 float_0p78_80420124;
extern const f32 float_0p29_80420128;
extern const f32 float_0p93_8042012c;
extern const f32 float_6p2832_80420130;
extern const f32 float_360_80420134;

typedef struct effWangFlushData {
	s32 type;         // 0x0
	Vec position;     // 0x4
	f32 rot;          // 0x10
	f32 rotSpeed;     // 0x14
	f32 scale;        // 0x18
	f32 scaleSpeed;   // 0x1C
	f32 alphaScale;   // 0x20
	s32 alpha;        // 0x24
	s32 field_0x28;   // 0x28
	s32 duration;     // 0x2C
	s32 frame;        // 0x30
} effWangFlushData;

EffectEntry* effWangFlushN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effWangFlushData* data;

	effect = effEntry();
	effect->type = "WangflushN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effWangFlushData));
	effect->userdata = data;
	effect->callback = effWangFlushMain;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->duration = duration;
	data->frame = 0;
	data->alpha = 0;
	data->rot = float_0_8042011c;
	if (type == 0) {
		data->rotSpeed = 30.0f;
	} else {
		data->rotSpeed = float_0_8042011c;
	}
	data->scale = float_0_8042011c;
	data->scaleSpeed = 0.8f;
	data->field_0x28 = 0;
	data->alphaScale = float_0_8042011c;
	return effect;
}

static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effWangFlushMain(EffectEntry* effect) {
	s32 duration;
	s32 frame;
	effWangFlushData* data = effect->userdata;
	Vec disp = initializeVec(&data->position);

	data->duration--;
	data->frame++;
	duration = data->duration;
	if (duration < 0) {
		effDelete(effect);
		return;
	}

	frame = data->frame;
	if (frame <= 6) {
		data->alpha = float_0p5_80420120 * (218 - data->alpha) + data->alpha;
	}
	if (frame > 6) {
		data->alpha = data->alpha * float_0p78_80420124;
	}

	data->scale += data->scaleSpeed;
	data->scaleSpeed += float_0p29_80420128;
	data->rot += data->rotSpeed;
	data->rotSpeed *= float_0p93_8042012c;
	data->alphaScale = float_0p5_80420120 * (f32)sin(float_6p2832_80420130 * (f32)(duration * 50) / float_360_80420134) + 0.9f;
	data->field_0x28 = 64.0f * (f32)sin(float_6p2832_80420130 * (f32)(duration * 40) / float_360_80420134) + 144.0f;

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effWangFlushDisp, effect);
}


static u8 color_rotation_data[24] = {
	0xFF, 0xFF, 0x52, 0xFF, 0xFF, 0x52, 0xB5, 0x94, 0x00, 0xB5, 0x94, 0x00,
	0xFF, 0xFF, 0xD6, 0xFF, 0xFF, 0xD6, 0xB5, 0x94, 0x00, 0xB5, 0x94, 0x00,
};

static u32 size16x16_tex64x64_vtx[16] = {
	0xFFF8FFF8, 0x00000000, 0x04000000, 0x00FF0008, 0xFFF80000, 0x08000400, 0x000000FF,
	0x00080008, 0x00000800, 0x0C000000, 0x00FFFFF8, 0x00080000, 0x00000C00, 0x000000FF,
	0x00000000, 0x00000000,
};

// Inlined in the original; the helper scope reproduces the stack slots the compiler
// picks for the by-value GXColor copy (type color: local at 0x10, outgoing copy at
// 0x14; alpha color: local at 0x8, outgoing copy at 0xc).
static inline void effWangFlushSetTypeColor(s32 idx, s32 alpha) {
	GXColor color = unk_804232c0;

	color.r = color_rotation_data[idx];
	color.g = color_rotation_data[idx + 1];
	color.b = color_rotation_data[idx + 2];
	color.a = alpha;
	GXSetTevColor(1, color);
}

static inline void effWangFlushSetAlphaColor(s32 alpha) {
	GXColor color = (GXColor){ 0xFF, 0xFF, 0xFF, 0x00 };

	color.a = alpha;
	GXSetTevColor(1, color);
}

void effWangFlushDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	s32 idx;
	s32 alpha;
	s32 field28;
	s32 type;
	effWangFlushData* data;
	CameraEntry* cam;
	Mtx mtxA;
	Mtx mtxD;
	Mtx mtxB;
	Mtx mtxC;
	GXTexObj texObj;

	cam = camGetPtr(camId);
	data = effect->userdata;
	type = data->type;
	PSMTXTrans(mtxA, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxB, 'y', float_deg2rad_80420114 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(mtxA, mtxB, mtxC);
	PSMTXConcat(cam->view, mtxC, mtxC);
	idx = data->frame * 3;
	field28 = data->field_0x28;

	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	effGetTexObjN64(0x1C, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	PSMTXScale(mtxA, float_0p03125_80420118, float_0p03125_80420118, float_0_8042011c);
	GXLoadTexMtxImm(mtxA, 0x1E, 1);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size16x16_tex64x64_vtx);

	if (type == 0) {
		PSMTXScale(mtxD, data->alphaScale, data->alphaScale, data->alphaScale);
		PSMTXConcat(mtxC, mtxD, mtxD);
		GXLoadPosMtxImm(mtxD, 0);
		GXSetCurrentMtx(0);
		idx %= 24;
		effWangFlushSetTypeColor(idx, field28);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}

	alpha = data->alpha;
	if (alpha > 0xFF) {
		alpha = 0xFF;
	}
	if (alpha > 0) {
		PSMTXScale(mtxD, data->scale, data->scale, data->scale);
		PSMTXRotRad(mtxB, 'z', float_deg2rad_80420114 * data->rot);
		PSMTXConcat(mtxB, mtxD, mtxD);
		PSMTXConcat(mtxC, mtxD, mtxD);
		GXLoadPosMtxImm(mtxD, 0);
		GXSetCurrentMtx(0);
		effWangFlushSetAlphaColor(alpha);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_80420114 = 0.017453292f;
const f32 float_0p03125_80420118 = 0.03125f;
const f32 float_0_8042011c = 0.0f;
const f32 float_0p5_80420120 = 0.5f;
const f32 float_0p78_80420124 = 0.78f;
const f32 float_0p29_80420128 = 0.29f;
const f32 float_0p93_8042012c = 0.93f;
const f32 float_6p2832_80420130 = 6.2831855f;
const f32 float_360_80420134 = 360.0f;

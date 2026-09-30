#include "eff/n64/eff_wangflush_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include <math.h>

extern const f32 float_deg2rad_80420114;
extern const f32 float_0p03125_80420118;
extern const f32 float_0_8042011c;
extern GXColor dat_80420110;
extern const f32 float_0p5_80420120;
extern const f32 float_0p78_80420124;
extern const f32 float_0p29_80420128;
extern const f32 float_0p93_8042012c;
extern const f32 float_6p2832_80420130;
extern const f32 float_360_80420134;
extern const f32 float_0p9_80420138;
extern const f32 float_144_8042013c;
extern const f32 float_64_80420140;

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
	data->rot = 0.0f;
	if (type == 0) {
		data->rotSpeed = 30.0f;
	} else {
		data->rotSpeed = 0.0f;
	}
	data->scale = 0.0f;
	data->scaleSpeed = 0.8f;
	data->field_0x28 = 0;
	data->alphaScale = 0.0f;
	return effect;
}

void effWangFlushMain(EffectEntry* effect) {
	s32 duration;
	s32 frame;
	effWangFlushData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effGlassMain). */
	Vec position;
#endif
	Vec disp = { 0 };

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

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
	data->alphaScale = float_0p5_80420120 * (f32)sin(float_6p2832_80420130 * (f32)(duration * 50) / float_360_80420134) + float_0p9_80420138;
	data->field_0x28 = float_64_80420140 * (f32)sin(float_6p2832_80420130 * (f32)(duration * 40) / float_360_80420134) + float_144_8042013c;

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effWangFlushDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effWangFlushDisp, effect);
#endif
}

GXColor unk_804232c0;

static u8 color_rotation_data[24] = {
	0xFF, 0xFF, 0x52, 0xFF, 0xFF, 0x52, 0xB5, 0x94, 0x00, 0xB5, 0x94, 0x00,
	0xFF, 0xFF, 0xD6, 0xFF, 0xFF, 0xD6, 0xB5, 0x94, 0x00, 0xB5, 0x94, 0x00,
};

static u32 size16x16_tex64x64_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x04000000, 0x00FF0008, 0xFFF80000, 0x08000400, 0x000000FF,
	0x00080008, 0x00000800, 0x0C000000, 0x00FFFFF8, 0x00080000, 0x00000C00, 0x000000FF,
};

// NONMATCHING (99.97%): the by-value GXColor argument slots (original: type color at
// 0x10 copied to 0x14, alpha color at 0x8 copied to 0xc) come out at 0xc and 0x14/0x8
// here; every other instruction matches.
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
	GXColor color;
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
		GXColor color1;

		PSMTXScale(mtxD, data->alphaScale, data->alphaScale, data->alphaScale);
		PSMTXConcat(mtxC, mtxD, mtxD);
		GXLoadPosMtxImm(mtxD, 0);
		GXSetCurrentMtx(0);
		idx %= 24;
		color1 = unk_804232c0;
		color1.r = color_rotation_data[idx];
		color1.g = color_rotation_data[idx + 1];
		color1.b = color_rotation_data[idx + 2];
		color1.a = field28;
		GXSetTevColor(1, color1);
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
		color = dat_80420110;
		color.a = alpha;
		GXSetTevColor(1, color);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}

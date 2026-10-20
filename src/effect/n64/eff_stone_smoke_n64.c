#include "eff/n64/eff_stone_smoke_n64.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_8042328c;
__declspec(section ".sdata") GXColor unk_80423288;


#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2 scratch GXColors the original builds its TEV colours from.

// .data
static u32 size16x16_tex32x32_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x04000000, 0x000000FF,
	0x00080008, 0x00000400, 0x04000000, 0x00FFFFF8, 0x00080000, 0x00000400, 0x000000FF,
};


EffectEntry* effStoneSmokeN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effStoneSmokeN64Data* data;

	effect = effEntry();
	effect->type = "StoneSmokeN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x3C);
	effect->userdata = data;
	effect->callback = effStoneSmokeMain;
	effect->flags |= 2;
	data->type = type;
	data->age = 0;
	if (duration <= 0) {
		data->life = 1000;
	} else {
		data->life = duration;
	}
	data->alpha = 0xFF;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->color1R = 0xFF;
	data->color1G = 0xFF;
	data->color1B = 0xFF;
	data->color2R = 10;
	data->color2G = 10;
	data->color2B = 0x78;
	return effect;
}

// .sdata
// Scale keyframes in hundredths: scale_data for the first seven frames, then scale_data2 as a repeating seven-frame cycle.
static u8 scale_data[8] = { 0x00, 0x1E, 0x50, 0x78, 0x70, 0x68, 0x64, 0x00 };
static u8 scale_data2[8] = { 0x5E, 0x5C, 0x64, 0x69, 0x6B, 0x69, 0x64, 0x00 };

void effStoneSmokeMain(EffectEntry* effect) {
	effStoneSmokeN64Data* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effGlassMain). */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };
	s32 frame;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}

	if (data->life < 1000) {
		data->life--;
	}
	data->age++; // elapsed-frame counter, indexes the scale keyframes below

	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	frame = data->age;

	if (data->life < 16) {
		data->alpha = data->life * 16;
	}

	data->y += 0.6f;

	if (frame < 7) {
		data->scaleKey = 0.01f * (f32)scale_data[frame];
	} else {
		data->scaleKey = 0.01f * (f32)scale_data2[(frame - 7) % 7];
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effStoneSmokeDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effStoneSmokeDisp, effect);
#endif
}

#ifdef MUST_MATCH
static inline void effStoneSmokeSetColor0(effStoneSmokeN64Data* data, s32 alpha) {
	GXColor c = unk_80423288;
	c.r = data->color1R;
	c.g = data->color1G;
	c.b = data->color1B;
	c.a = alpha;
	GXSetTevColor(GX_TEVREG0, c);
}
static inline void effStoneSmokeSetColor1(effStoneSmokeN64Data* data) {
	GXColor c = unk_8042328c;
	c.r = data->color2R;
	c.g = data->color2G;
	c.b = data->color2B;
	GXSetTevColor(GX_TEVREG1, c);
}
#endif

// Building each TEV colour and calling GXSetTevColor inside the same inline makes mwcc
// allocate that colour's build slot and its by-value argument copy together
// (0x8/0xc for colour1, 0x10/0x14 for colour0), as retail does. Building both colours
// first and calling GXSetTevColor afterwards groups the build slots and the copies
// (the old 99.90% layout). Two separate inlines, one per colour, are needed.
void effStoneSmokeDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effStoneSmokeN64Data* data;
	CameraEntry* cam;
	GXTexObj texObj;
#ifndef MUST_MATCH
	GXColor color0;
	GXColor color1;
#endif
	Mtx mtx;
	Mtx scaleMtx;
	s32 alpha;
	f32 scale;

	cam = camGetPtr(camId);
	data = effect->userdata;
	alpha = data->alpha;
	scale = data->scale * data->scaleKey;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXScale(scaleMtx, scale, scale, scale);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
	effStoneSmokeSetColor0(data, alpha);
	effStoneSmokeSetColor1(data);
#else
	color0 = unk_80423288;
	color0.r = data->color1R;
	color0.g = data->color1G;
	color0.b = data->color1B;
	color0.a = alpha;
	GXSetTevColor(GX_TEVREG0, color0);
	color1 = unk_8042328c;
	color1.r = data->color2R;
	color1.g = data->color2G;
	color1.b = data->color2B;
	GXSetTevColor(GX_TEVREG1, color1);
#endif
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
	PSMTXScale(scaleMtx, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	effGetTexObjN64(0x5C, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}


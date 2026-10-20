#include "eff/eff_updown.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdlib.h>

static void polygon(s32 type, s32 alpha, f32 x, f32 y, f32 w, f32 h, f32 u, f32 v);

// .sdata2 (defined before the code: the compiler then knows float_0, as the original does)

// .rodata
extern const f64 double_to_int_mask_802f8e48;
extern const f64 double_to_int_802f8e50;

// .sdata
u16 icon_id[10] = { 0x01DA, 0x01DC, 0x01DE, 0x01E0, 0x01E2, 0x01E4, 0x01E6, 0x01E8, 0x01EA, 0x01EC };
static u8 scale_data[32] = {
	0x78, 0x00, 0x73, 0x0D, 0x6E, 0x1C, 0x68, 0x2C, 0x63, 0x3B, 0x5E, 0x4A, 0x59, 0x58, 0x55, 0x64,
	0x52, 0x6F, 0x50, 0x78, 0x50, 0x7B, 0x54, 0x78, 0x59, 0x71, 0x5F, 0x69, 0x64, 0x64, 0x00, 0x00,
};

// the 4 bytes after scale_data in the target's .sdata (dtk's gap_NN_..._sdata)
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") static u32 gap_after_scale_data = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

// .sdata2

// .text:0x81C | 0x8018E348 | size: 0x100
EffectEntry* effUpdownEntry(s32 type, f32 x, f32 y, f32 z, s32 field18, s32 lifetime) {
	EffectEntry* entry;
	EffUpdownWork* work;

	entry = effEntry();
	entry->type = "Updown";
	entry->count = 1;
	work = __memAlloc(HEAP_EFFECT, 0x28);
	entry->userdata = work;
	entry->callback = effUpdownMain;
	entry->flags |= 2;
	work->type = type;
	work->x = x;
	work->y = y;
	work->z = z;
	work->scaleY = 1.0f;
	work->scaleX = 1.0f;
	work->field18 = field18;
	work->lifetime = lifetime;
	work->frame = 0;
	work->alpha = 0xFF;
	return entry;
}

// .text:0x45C | 0x8018DF88 | size: 0x1E8
// .text:0x644 | 0x8018E170 | size: 0x1D8
// NONMATCHING (96.97%): scaleX/scaleY and the final Vec values exactly match; the remaining diff
// is the frame++/renderMode/Vec-copy schedule (the original materialises &pos into r3 before the
// frame store and delays the first lwz past li r29) and the resulting r5-vs-r6 colouring.
void effUpdownMain(EffectEntry* effect) {
	EffUpdownWork* work = effect->userdata;
	s32 type;
	s32 renderMode;
	Vec pos;
	Vec* p;

	type = work->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		work->lifetime = 16;
	}
	if (work->lifetime < 1000) {
		work->lifetime--;
	}
	if (work->lifetime < 0) {
		effDelete(effect);
		return;
	}
	if (work->lifetime < 16) {
		work->alpha = work->lifetime * 16;
	}
	if (type == 0 || type == 1) {
		if (work->frame < 15) {
			work->scaleX = (f32)scale_data[work->frame * 2] / 100.0f;
			work->scaleY = (f32)scale_data[work->frame * 2 + 1] / 100.0f;
		} else {
			/* Splitting out the signed frame delta and the constant (and the sine as a temp: it pools
			   0.25 before 0.1) fixes the FP register allocation of the int->float conversion and the fnmsubs. */
			f32 k = 0.25f;
			s32 t = work->frame - 15;
			f32 wave;
			work->scaleX = 1.0f;
			wave = (f32)sin((f32)t * k);
			work->scaleY = 1.0f - wave * 0.1f;
		}
	}
	p = &pos;
	work->frame++;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = work->x;
	pos.y = work->y;
	pos.z = work->z;
	renderMode = 1;
	if (work->alpha != 0xFF) {
		renderMode = 2;
	}
	dispEntry(CAMERA_3D, dispCalcZ(p), renderMode, effUpdownDisp, effect);
}

// The alpha is copied into `a` at each case entry: MWCC coalesces a single `a = alpha` with the parameter,
// shifting every color register, and the per-case copy reproduces the original r28.
static void polygon(s32 type, s32 alpha, f32 x, f32 y, f32 w, f32 h, f32 u, f32 v) {
	u8 r1, g1, b1;
	u8 a;
	u8 r2, g2, b2;

	switch (type) {
	case 0:
		a = alpha;
		r1 = g1 = b1 = 0xFF;
		r2 = g2 = b2 = 0xFF;
		break;
	case 1:
		a = alpha;
		r1 = 0xE9;
		g1 = 0x4A;
		b1 = 0x3F;
		r2 = 0xFE;
		g2 = 0xE6;
		b2 = 0x3E;
		break;
	case 2:
		a = alpha;
		r1 = 0x3F;
		g1 = 0x3F;
		b1 = 0xE9;
		r2 = 0x74;
		g2 = 0xC9;
		b2 = 0xFF;
		break;
	}

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(x, y, 0.0f);
	GXColor4u8(r1, g1, b1, a);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(x + w, y, 0.0f);
	GXColor4u8(r1, g1, b1, a);
	GXTexCoord2f32(u, 0.0f);
	GXPosition3f32(x + w, y - h, 0.0f);
	GXColor4u8(r2, g2, b2, a);
	GXTexCoord2f32(u, v);
	GXPosition3f32(x, y - h, 0.0f);
	GXColor4u8(r2, g2, b2, a);
	GXTexCoord2f32(0.0f, v);
}

// .text:0x0 | 0x8018DB2C | size: 0x45C
void effUpdownDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	EffUpdownWork* work = effect->userdata;
	s32 field18;
	s32 alpha;
	CameraEntry* cam;
	s32 type;
	GXTexObj texObj;
	Mtx mtxPos;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxA;
	Mtx mtxB;
	Mtx mtxC;

	cam = camGetPtr(cameraId);
	field18 = work->field18;
	type = work->type;
	alpha = work->alpha;
	effGetTexObj(0x1D, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, GX_FALSE, 0x7D);
	if (field18 < 0) {
		PSMTXRotRad(mtxRot, 'z', 3.1415927f);
	} else {
		PSMTXIdentity(mtxRot);
	}
	GXLoadTexMtxImm(mtxRot, 0x1E, GX_MTX2x4);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_TEV_ADD);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	PSMTXTrans(mtxPos, work->x, work->y, work->z);
	PSMTXScale(mtxScale, work->scaleX, work->scaleY, 1.0f);
	PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(cameraId)->viewYaw);
	PSMTXConcat(mtxPos, mtxScale, mtxA);
	PSMTXConcat(mtxA, mtxRot, mtxB);
	PSMTXConcat(mtxPos, mtxRot, mtxC);
	PSMTXConcat(cam->view, mtxB, mtxA);
	GXLoadPosMtxImm(mtxA, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	if (type == 0 || type == 2) {
		polygon(1, alpha, -12.0f, 24.0f, 24.0f, 24.0f,
				1.0f, 1.0f);
	} else {
		polygon(2, alpha, -12.0f, 24.0f, 24.0f, 24.0f,
				1.0f, 1.0f);
	}
	iconGetTexObj(&texObj, icon_id[abs(field18) % 10]);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
	if (field18 >= 0) {
		PSMTXTrans(mtxPos, 5.0f, 0.0f, 1.0f);
	} else {
		PSMTXTrans(mtxPos, 5.0f, 7.0f, 1.0f);
	}
	if (work->frame < 0xF) {
		PSMTXConcat(mtxB, mtxPos, mtxA);
	} else {
		PSMTXConcat(mtxC, mtxPos, mtxA);
	}
	PSMTXConcat(cam->view, mtxA, mtxA);
	GXLoadPosMtxImm(mtxA, GX_PNMTX0);
	polygon(0, alpha, -8.0f, 16.0f, 16.0f, 16.0f,
			1.0f, 1.0f);
	if (field18 >= 0) {
		iconGetTexObj(&texObj, 0x1ED);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		PSMTXTrans(mtxPos, -5.0f, 0.0f, 1.0f);
	} else {
		iconGetTexObj(&texObj, 0x1EE);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		PSMTXTrans(mtxPos, -5.0f, 7.0f, 1.0f);
	}
	if (work->frame < 0xF) {
		PSMTXConcat(mtxB, mtxPos, mtxA);
	} else {
		PSMTXConcat(mtxC, mtxPos, mtxA);
	}
	PSMTXConcat(cam->view, mtxA, mtxA);
	GXLoadPosMtxImm(mtxA, GX_PNMTX0);
	polygon(0, alpha, -8.0f, 16.0f, 16.0f, 16.0f,
			1.0f, 1.0f);
}

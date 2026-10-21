#include "eff/n64/eff_sunlight_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "math_ext.h"
#include "eff/eff_n64.h"

// .sbss2 zero GXColor scratch objects (declared in reverse: placed in reverse of declaration order)
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423294;
__declspec(section ".sdata") GXColor unk_80423290;

u32 testsrc__s_v[98] = {
	0x021301BA, 0x00190000, 0x0000FFFF, 0xFFFF02C3, 0x011C0019, 0x02000000, 0xFFFFFFFF,
	0x064A0507, 0x00190200, 0x0800FFFF, 0xFFFF059A, 0x05A50019, 0x00000800, 0xFFFFFFFF,
	0x0547FF44, 0x00190000, 0x0000FFFF, 0xFFFF05E4, 0xFF080019, 0x02000000, 0xFFFFFFFF,
	0x06D6017E, 0x00190200, 0x0800FFFF, 0xFFFF0639, 0x01BA0019, 0x00000800, 0xFFFFFFFF,
	0xFD5C02BB, 0x00020000, 0x0000FFFF, 0xFF7EFE16, 0x01790002, 0x02000000, 0xFFFFFF7E,
	0x04F0056E, 0x00020200, 0x0800FFFF, 0xFF7E0436, 0x06AF0002, 0x00000800, 0xFFFFFF7E,
	0xFC210350, 0x001B0000, 0x0000FFFF, 0xFFE5FC4B, 0x02F3001B, 0x02000000, 0xFFFFFFE5,
	0x0247059D, 0x001B0200, 0x0800FFFF, 0xFFE5021E, 0x05FA001B, 0x00000800, 0xFFFFFFE5,
	0x023BFF32, 0x00190000, 0x0000FFFF, 0xFFFF02C7, 0xFED30019, 0x02000000, 0xFFFFFFFF,
	0x06E804F2, 0x00190200, 0x02E9FFFF, 0xFFFF065C, 0x05510019, 0x000002E9, 0xFFFFFFFF,
	0xFE0400CA, 0x00190000, 0x0000FFFF, 0xFFFFFE69, 0x00440019, 0x02000000, 0xFFFFFFFF,
	0x05B605C4, 0x00190200, 0x0800FFFF, 0xFFFF0551, 0x064B0019, 0x00000800, 0xFFFFFFFF,
	0x01E3FC33, 0x0002FDD1, 0x0044FFFF, 0xFF7E032B, 0xFB840002, 0xFFD10044, 0xFFFFFF7E,
	0x081E04D4, 0x0002FFD1, 0x0844FFFF, 0xFF7E06D6, 0x05830002, 0xFDD10844, 0xFFFFFF7E,
};

EffectEntry* effSunlightN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effSunlightData* data;

	entry = effEntry();
	entry->type = "SunlightN64";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effSunlightData));
	entry->userdata = data;
	entry->callback = effSunlightMain;
	entry->flags |= 2;
	data->type = type;
	data->counter = 0;
	if (life <= 0) {
		data->life = 1000;
	}
	else {
		data->life = life;
	}
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->r0 = 0xff;
	data->g0 = 0xff;
	data->b0 = 0xff;
	data->r1 = 0xff;
	data->g1 = 0xff;
	data->b1 = 0xff;
	data->a1 = 0xff;
	data->offset[0] = float_0_8041ff80;
	data->offset[1] = float_0_8041ff80;
	data->offset[2] = float_0_8041ff80;
	data->offset[3] = float_0_8041ff80;
	data->offset[4] = float_0_8041ff80;
	data->alphaTarget = 0xff;
	data->alpha = 0;

	return entry;
}

void effSunlightMain(EffectEntry* effect) {
	effSunlightData* data = effect->userdata;
	s32 i;
	s32 counter;
	f32 base;
	/* Retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effFlameMain / effGlassMain). */
	Vec position;
	Vec disp = { 0 };

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}

	if (data->life < 1000) {
		data->life--;
	}
	data->counter++;
	if (data->counter > 0x4F1A0) {
		data->counter = 0x100;
	}

	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	counter = data->counter;

	if (data->life < 16) {
		data->alpha -= 16;
		if (data->alpha < 0) {
			data->alpha = 0;
		}
	}

	if (data->alphaTarget < 0) {
		data->alphaTarget = 0;
	}
	else if (data->alphaTarget > 255) {
		data->alphaTarget = 255;
	}

	if (data->alpha > data->alphaTarget) {
		data->alpha -= 8;
		if (data->alpha < data->alphaTarget) {
			data->alpha = data->alphaTarget;
		}
	}
	else if (data->alpha < data->alphaTarget) {
		data->alpha += 8;
		if (data->alpha > data->alphaTarget) {
			data->alpha = data->alphaTarget;
		}
	}

	base = float_0p25_8041ff8c * (f32)counter;

	for (i = 0; i < 5; i++) {
		f32 s1;
		f32 s2;

		s1 = (f32)sin((base + (f32)(i * i * 20)) * float_6p2832_8041ff90 / float_360_8041ff94);
		s2 = (f32)sin(float_6p2832_8041ff90 * (f32)(counter * 2 + i * 20) / float_360_8041ff94);
		/* The factor has to be materialized first and multiplied as `inc * s1`,
		   otherwise MWCC emits `fnmsubs f0, f22, f1, f0` (s1 first) instead of the retail
		   `fnmsubs f0, f1, f22, f0`. */
		{
			f32 inc = float_4_8041ff98 * (float_0p01_8041ffa0 * s2 + float_0p05_8041ff9c);
			data->offset[i] -= inc * s1;
		}
		if (data->offset[i] < float_0_8041ff80) {
			data->offset[i] += 256.0f;
		}
		if (data->offset[i] > 256.0f) {
			data->offset[i] -= 256.0f;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effSunlightDisp, effect);
}

// Matched by pooling the three view Vecs as file-local consts right after effSunlightMain's zero Vec,
// so mwcc keeps one r31 base and reaches target/eye/up and the int->float double as offsets from it.
// The colours are built and passed by inlined helpers: a helper that returns the GXColor leaves the
// inlined local and the by-value copy in the wrong two stack slots when two colours are passed, while
// inlining the whole GXSetTevColor call (helper builds the colour, then calls) reproduces the target
// slots. `alpha` is declared before `type` to put them in r29/r28.
const Vec vec3_802fb7d4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fb7e0 = { 0.0f, 0.0f, 100.0f };
const Vec vec3_802fb7ec = { 0.0f, 1.0f, 0.0f };

/* Inlining the whole call keeps the colour and its by-value copy in the target's two
   stack slots; a helper that returns the GXColor puts them the other way round and mis-matches
   when the function passes two colours. */
static inline void effSunlightSetColor1(GXTevRegID id, effSunlightData* data, s32 alpha) {
	GXColor c = unk_80423290;
	c.r = data->r0;
	c.g = data->g0;
	c.b = data->b0;
	c.a = alpha >> 1;
	GXSetTevColor(id, c);
}
static inline void effSunlightSetColor2(GXTevRegID id, effSunlightData* data) {
	GXColor c = unk_80423294;
	c.r = data->r1;
	c.g = data->g1;
	c.b = data->b1;
	c.a = data->a1;
	GXSetTevColor(id, c);
}

void effSunlightDisp(CameraId cameraId, void* param) {
	EffectEntry* effect;
	effSunlightData* data;
	Mtx mtx2;
	Mtx mtx;
	Mtx44 ortho;
	Mtx lookAt;
	GXTexObj texObj;
	f32 proj[7];
	Vec target;
	Vec eye;
	Vec up;
	s32 alpha;
	s32 type;
	s32 i;

	effect = param;
	camGetPtr(cameraId);
	data = effect->userdata;
	type = data->type;
	alpha = data->alpha;

	target = vec3_802fb7d4;
	eye = vec3_802fb7e0;
	up = vec3_802fb7ec;

	if (alpha == 0) {
		return;
	}

	GXGetProjectionv(proj);
	if (type != 0) {
		C_MTXOrtho(ortho, float_1200_8041ff60, float_neg1200_8041ff64, float_neg1600_8041ff68, float_1600_8041ff6c, float_1_8041ff70, float_32768_8041ff74);
	}
	else {
		C_MTXOrtho(ortho, float_1200_8041ff60, float_neg1200_8041ff64, float_1600_8041ff6c, float_neg1600_8041ff68, float_1_8041ff70, float_32768_8041ff74);
	}
	GXSetProjection(ortho, GX_ORTHOGRAPHIC);
	C_MTXLookAt(lookAt, &eye, &up, &target);
	GXLoadPosMtxImm(lookAt, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	effSunlightSetColor1(GX_TEVREG0, data, alpha);
	effSunlightSetColor2(GX_TEVREG1, data);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_APREV);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(mtx, float_0p0625_8041ff78, float_0p015625_8041ff7c, float_0_8041ff80);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	effGetTexObjN64(0x42, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	effGetTexObjN64(0x43, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP1);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)testsrc__s_v);

	for (i = 0; i < 5; i++) {
		PSMTXScale(mtx, float_0p0039062_8041ff84, float_0p125_8041ff88, float_0_8041ff80);
#ifdef MUST_MATCH
		/* MUST_MATCH: naming float_0_8041ff80 directly here makes mwcc hoist it into a
		   callee-saved fpr (f30), growing the frame by 0x10 and shifting every save; the
		   volatile read keeps the natural one-load-plus-fmr code. */
		{
			f32 zero = *(volatile f32*)&float_0_8041ff80;
			PSMTXTrans(mtx2, data->offset[i] + (f32)(i * 11), zero, zero);
		}
#else
		PSMTXTrans(mtx2, data->offset[i] + (f32)(i * 11), float_0_8041ff80, float_0_8041ff80);
#endif
		PSMTXConcat(mtx, mtx2, mtx);
		GXLoadTexMtxImm(mtx, GX_TEXMTX1, GX_MTX2x4);
		switch (i) {
		case 0:
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 12);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			tri2(4, 5, 6, 0, 4, 6, 7, 0);
			break;
		case 1:
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(8, 9, 10, 8, 8, 10, 11, 8);
			break;
		case 2:
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(12, 13, 14, 12, 12, 14, 15, 12);
			break;
		case 3:
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 12);
			tri2(16, 17, 18, 16, 16, 18, 19, 16);
			tri2(20, 21, 22, 16, 20, 22, 23, 16);
			break;
		case 4:
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(24, 25, 26, 24, 24, 26, 27, 24);
			break;
		}
	}
	GXSetProjectionv(proj);
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_1200_8041ff60 = 1200.0f;
const f32 float_neg1200_8041ff64 = -1200.0f;
const f32 float_neg1600_8041ff68 = -1600.0f;
const f32 float_1600_8041ff6c = 1600.0f;
const f32 float_1_8041ff70 = 1.0f;
const f32 float_32768_8041ff74 = 32768.0f;
const f32 float_0p0625_8041ff78 = 0.0625f;
const f32 float_0p015625_8041ff7c = 0.015625f;
const f32 float_0_8041ff80 = 0.0f;
const f32 float_0p0039062_8041ff84 = 0.00390625f;
const f32 float_0p125_8041ff88 = 0.125f;
const f32 float_0p25_8041ff8c = 0.25f;
const f32 float_6p2832_8041ff90 = 6.2831855f;
const f32 float_360_8041ff94 = 360.0f;
const f32 float_4_8041ff98 = 4.0f;
const f32 float_0p05_8041ff9c = 0.05f;
const f32 float_0p01_8041ffa0 = 0.01f;

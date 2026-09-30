#include "eff/n64/eff_sunlight_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "math_ext.h"
#include "eff/eff_n64.h"

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
	data->field_0x20 = 0.0f;
	data->field_0x24 = 0.0f;
	data->field_0x28 = 0.0f;
	data->field_0x2C = 0.0f;
	data->field_0x30 = 0.0f;
	data->field_0x3C = 0xff;
	data->field_0x38 = 0;

	return entry;
}

void effSunlightMain(EffectEntry* effect) {
	effSunlightData* data = effect->userdata;
	s32 i;
	s32 counter;
	f32 base;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effFlameMain / effGlassMain). */
	Vec position;
#endif
	Vec disp = { 0 };

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
		data->field_0x38 -= 16;
		if (data->field_0x38 < 0) {
			data->field_0x38 = 0;
		}
	}

	if (data->field_0x3C < 0) {
		data->field_0x3C = 0;
	}
	else if (data->field_0x3C > 255) {
		data->field_0x3C = 255;
	}

	if (data->field_0x38 > data->field_0x3C) {
		data->field_0x38 -= 8;
		if (data->field_0x38 < data->field_0x3C) {
			data->field_0x38 = data->field_0x3C;
		}
	}
	else if (data->field_0x38 < data->field_0x3C) {
		data->field_0x38 += 8;
		if (data->field_0x38 > data->field_0x3C) {
			data->field_0x38 = data->field_0x3C;
		}
	}

	base = float_0p25_8041ff8c * (f32)counter;

	for (i = 0; i < 5; i++) {
		f32 s1;
		f32 s2;

		s1 = (f32)sin((base + (f32)(i * i * 20)) * float_6p2832_8041ff90 / float_360_8041ff94);
		s2 = (f32)sin(float_6p2832_8041ff90 * (f32)(counter * 2 + i * 20) / float_360_8041ff94);
#ifdef MUST_MATCH
		/* MUST_MATCH: the factor has to be materialized first and multiplied as `inc * s1`,
		   otherwise MWCC emits `fnmsubs f0, f22, f1, f0` (s1 first) instead of the retail
		   `fnmsubs f0, f1, f22, f0`. */
		{
			f32 inc = float_4_8041ff98 * (float_0p01_8041ffa0 * s2 + float_0p05_8041ff9c);
			(&data->field_0x20)[i] -= inc * s1;
		}
#else
		(&data->field_0x20)[i] -= float_4_8041ff98 * (float_0p01_8041ffa0 * s2 + float_0p05_8041ff9c) * s1;
#endif
		if ((&data->field_0x20)[i] < float_0_8041ff80) {
			(&data->field_0x20)[i] += 256.0f;
		}
		if ((&data->field_0x20)[i] > 256.0f) {
			(&data->field_0x20)[i] -= 256.0f;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effSunlightDisp, effect);
}

// Register allocation / stack-slot artifacts only, no wrong logic:
// the target keeps the vec3_802fb7c8 base in r31 across the whole function so it can load
// double_to_int as `lfd f31, 0x30(r31)`; mwcc here puts the userdata pointer in r31 and the vec
// base in r11 (dead after the three Vec copies), which also swaps the r30/r31 pair used for the
// 0x4330 conversion constant and the i*11 loop counter. The GXColor locals land at 0x14/0x10 and
// are copied to the outgoing slots 0xc/0x8; the target builds at 0x10/0x8 and copies to 0x14/0xc
// (the same local/outgoing swap eff_akari_pose_n64.c / eff_booboo_n64.c document).
// NONMATCHING (97.88%): register allocation (r11 vs r31 vec base, r30/r31 swap) and the GXColor
// local/outgoing slots differ.
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
	GXColor color2;
	GXColor color1;
	s32 type;
	s32 alpha;
	s32 i;

	effect = param;
	camGetPtr(cameraId);
	data = effect->userdata;
	type = data->type;
	alpha = data->field_0x38;

	target = vec3_802fb7c8[1];
	eye = vec3_802fb7c8[2];
	up = vec3_802fb7c8[3];

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

	color1 = unk_80423290;
	color1.r = data->r0;
	color1.g = data->g0;
	color1.b = data->b0;
	color1.a = alpha >> 1;
	GXSetTevColor(GX_TEVREG0, color1);
	color2 = unk_80423294;
	color2.r = data->r1;
	color2.g = data->g1;
	color2.b = data->b1;
	color2.a = data->a1;
	GXSetTevColor(GX_TEVREG1, color2);

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
			PSMTXTrans(mtx2, (&data->field_0x20)[i] + (f32)(i * 11), zero, zero);
		}
#else
		PSMTXTrans(mtx2, (&data->field_0x20)[i] + (f32)(i * 11), float_0_8041ff80, float_0_8041ff80);
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

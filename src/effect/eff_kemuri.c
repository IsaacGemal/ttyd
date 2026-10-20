#include "eff/eff_kemuri.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"
#include "matching.h"

#include <dolphin/gx.h>

typedef struct effKemuriData {
	s32 kind; //0x0 the int2 from effKemuriEntry: selects the scale table and texture
	f32 x; //0x4
	f32 y; //0x8
	f32 z; //0xC
	u8 field_0x10[0x40 - 0x10]; //0x10
	s32 frame; //0x40 advanced once per main tick; indexes the scale tables
	f32 scaleX; //0x44 puff size from touchdown_scale/heavy_scale
	f32 scaleY; //0x48
	f32 velX; //0x4C sideways speed, damped by 0.85 per tick (type 0)
	u8 field_0x50[0x58 - 0x50]; //0x50
	f32 offX; //0x58 x offset, accumulates velX
	f32 field_0x5C; //0x5C
	u8 field_0x60[0x74 - 0x60]; //0x60
	f32 scale; //0x74 uniform scale of the whole puff, starts at 1
	u8 field_0x78[0x80 - 0x78]; //0x78
	s32 sheetU0; //0x80 texture column of this frame (16 columns over lifetime)
	s32 sheetU1; //0x84 texture column of the next frame, clamped to 15
	s32 lifetime; //0x88 frames before effDelete (32 or 64)
} effKemuriData;

// Small-data constants owned by this effect unit; definitions follow the functions.
extern const f32 float_1_8041c6b0;
extern const f32 float_0_8041c6bc;
extern const f32 float_360_8041c70c;
extern const f32 float_1000_8041c710;
extern const f32 float_6p2832_8041c714;
extern const f32 float_5_8041c718;
extern const f32 float_0p7_8041c71c;
extern const f32 float_neg20_8041c720;
extern const f32 float_12_8041c688;
extern const f32 float_8_8041c68c;
extern const f32 float_12p8_8041c690;
extern const f32 float_20_8041c694;
extern const f32 float_160_8041c698;
extern const f32 float_180_8041c69c;
extern const f32 float_340_8041c6a0;
extern const f32 float_deg2rad_8041c6a4;
extern const f32 float_0p17453_8041c6a8;
extern const f32 float_neg1_8041c6ac;
extern const f32 float_10_8041c6b4;
extern const f32 float_neg6_8041c6b8;
extern const f32 float_neg10_8041c6c0;
extern const f32 float_6_8041c6c4;
extern const f32 float_neg4_8041c6c8;
extern const f32 float_0p5_8041c6cc;
extern const f32 float_4_8041c6d0;
extern const f32 float_6p4_8041c6d4;
extern const GXColor unk_80423010;
extern const GXColor unk_80423014;
extern const GXColor dat_8041c680;
extern const GXColor dat_8041c684;

extern const f32 float_0p0625_8041c6d8;
extern const f32 float_neg0p2_8041c6dc;
extern const f32 float_3p1416_8041c6e0;
extern const f32 float_30_8041c6e4;
extern const f32 float_0p9_8041c6e8;
extern const f32 float_0p95_8041c6ec;
extern const f32 float_0p8_8041c6f0;
extern const f32 float_16_8041c6f4;
extern const f32 float_0p0078125_8041c6f8;
extern const f32 float_1p5_8041c6fc;
extern const f32 float_0p85_8041c700;
extern const f32 float_2_8041c704;
extern const f32 float_1p2_8041c708;
extern const f32 float_3_8041c724;
extern const f32 float_neg3_8041c728;
// local per the original: .fn effKemuri2Disp, local
static void effKemuri2Disp(CameraId camId, void* param);
// local per the original: .fn effKemuri2Main, local
static void effKemuri2Main(EffectEntry* effect);

EffectEntry* effKemuriEntry(s32 int2, f32 float3, f32 float4, f32 float5, f32 float6) {
	effKemuriData* data;
	s32 numEffects, i;
	
	EffectEntry* entry = effEntry();
	numEffects = 1;
	switch (int2) {
		case 0:
			numEffects = 3;
			break;
		case 1:
			numEffects = 2;
			break;
		case 2:
			numEffects = 2;
			break;
	}
#ifdef MUST_MATCH
	// Retail reserves eight bytes for this string.
	entry->type = "Kemuri\0";
#else
	entry->type = "Kemuri";
#endif
	entry->count = numEffects;
	data = (effKemuriData*)__memAlloc(HEAP_EFFECT, sizeof(effKemuriData) * numEffects);
	entry->userdata = data;
	entry->callback = effKemuriMain;

	data->kind = int2;
	data->x = float3;
	data->y = float4;
	data->z = float5;

	data->frame = 0;

	data->scaleX = float_1_8041c6b0;
	data->scaleY = float_1_8041c6b0;
	data->scale = float_1_8041c6b0;
	data++;
	for (i = 1; i < entry->count; i++, data++) {
		data->x = float_0_8041c6bc;
		data->y = float_0_8041c6bc;
		data->z = float_0_8041c6bc;

		data->scaleX = float_1_8041c6b0;
		data->scaleY = float_1_8041c6b0;
		data->scale = float_1_8041c6b0;

		data->offX = float_0_8041c6bc;
		data->field_0x5C = float_0_8041c6bc;

		data->frame = 0;
		data->sheetU0 = 0;
		data->sheetU1 = 0;

		switch (int2) {
			case 0:
				data->velX = i & 1 ? float_3_8041c724 : float_neg3_8041c728;
				data->lifetime = 32;
				break;
			case 1:
				data->velX = float_0_8041c6bc;
				data->lifetime = 32;
				break;
			case 2:
				data->velX = float_0_8041c6bc;
				data->lifetime = 64;
				break;
		}
	}
	return entry;
}

// The viewYaw and a2 (the second angleABf result) temps are ordinary locals; declaring them in this
// order and computing them before the sum is what makes MWCC reuse f26 for the value live across the
// call (sinVal, viewYaw and one share f26) and keeps the other registers as retail.
EffectEntry* effKemuri2Entry(s32 int2, f32 float3, f32 float4, f32 float5, f32 float6, f32 float7) {
	EffectEntry* entry = effEntry();
	effKemuri2Data* data;
	f32 zero;
	f32 neg20;
	f32 p7;
	f32 one;
	f32 viewYaw;
	f32 a2;
	f32 sinVal;
	f32 z;
	f32 angle2;
	f32 angle;
	f32 cosVal;

	entry->type = "Kemuri2";
	entry->count = 1;
	data = (effKemuri2Data*)__memAlloc(HEAP_EFFECT, sizeof(effKemuri2Data));
	entry->userdata = data;
	memset(data, 0, sizeof(effKemuri2Data));
	entry->callback = effKemuri2Main;

	sinVal = float_1000_8041c710 * float6;
	cosVal = float_1000_8041c710 * float7;
	angle = float_360_8041c70c * angleABf(float_0_8041c6bc, float_0_8041c6bc, sinVal, cosVal) / float_6p2832_8041c714;
	viewYaw = camGetPtr(CAMERA_3D)->viewYaw;
	a2 = float_360_8041c70c * angleABf(float_0_8041c6bc, float_0_8041c6bc, sinVal, cosVal) / float_6p2832_8041c714;
	data->heading = reviseAngle(a2 + viewYaw);
	angle2 = float_6p2832_8041c714 * (f32)(s32)angle / float_360_8041c70c;
	z = float_5_8041c718 + float5;
	data->counter = 0;
	one = float_1_8041c6b0;
	p7 = float_0p7_8041c71c;
	neg20 = float_neg20_8041c720;
	zero = float_0_8041c6bc;
	data->dirX = sin(angle2);
	data->dirZ = cos(angle2);
	data->x = float3;
	data->y = float4;
	data->z = z;
	data->speed = one;
	data->velY = p7;
	data->spinVel = neg20;
	data->spin = zero;
	data->alpha = 0xFF;
	data->pulseScale = one;
	data->scale = one;
	return entry;
}

// The count tables are u8 and converted with the unsigned (2^52) mask. Each table value goes through
// the `scale` temp before the multiply by 1/128: that keeps the converted value as the first fmuls
// operand (retail's `fmuls f2, f2, f3`); multiplying the conversion directly puts 1/128 first.
void effKemuriMain(EffectEntry* effect) {
	effKemuriData* data = effect->userdata;
	s32 i = 1;
	s32 type;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	type = data->kind;
	data++;
	for (; i < effect->count; i++, data++) {
		data->sheetU0 = (s32)(float_16_8041c6f4 * (f32)data->frame / (f32)data->lifetime);
		data->sheetU1 = (s32)(float_16_8041c6f4 * (f32)(data->frame + 1) / (f32)data->lifetime);
		if (data->sheetU1 > 15) {
			data->sheetU1 = 15;
		}
		switch (type) {
		case 0:
			if ((u32)data->frame < 30U) {
				f32 scale;

				scale = touchdown_scale[data->frame * 2];
				data->scaleX = scale * float_0p0078125_8041c6f8;
				scale = touchdown_scale[data->frame * 2 + 1];
				data->scaleY = scale * float_0p0078125_8041c6f8;
			} else {
				data->scaleX = float_1p5_8041c6fc;
				data->scaleY = float_0p8_8041c6f0;
			}
			data->velX *= float_0p85_8041c700;
			data->offX += data->velX;
			data->frame++;
			break;
		case 1:
			data->frame++;
			break;
		case 2:
			if ((u32)data->frame < 30U) {
				f32 scale;

				scale = heavy_scale[data->frame * 2];
				data->scaleX = scale * float_0p0078125_8041c6f8;
				scale = heavy_scale[data->frame * 2 + 1];
				data->scaleY = scale * float_0p0078125_8041c6f8;
			} else {
				data->scaleX = float_2_8041c704;
				data->scaleY = float_1p2_8041c708;
			}
			data->frame++;
			break;
		}
		if (data->frame >= data->lifetime) {
			effDelete(effect);
			return;
		}
	}
#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuriDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effKemuriDisp, effect);
#endif
}

// The MUST_MATCH re-read of effect->userdata below is needed for the register allocation: it makes MWCC
// load it into a volatile register and copy it to the callee-saved r31 before the sin call (retail's
// `lwz r7; mr r31, r7`).
static void effKemuri2Main(EffectEntry* effect) {
	effKemuri2Data* data = effect->userdata;
	s32 i;
	f32 angle;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
	/* MUST_MATCH: redundant re-read makes MWCC split data into r7/r31 as retail does. */
	data = effect->userdata;
#endif
	angle = (f32)sin(float_3p1416_8041c6e0 * (f32)data->counter / float_30_8041c6e4);
	data->counter++;
	if (data->counter > 30) {
		effDelete(effect);
		return;
	}

	for (i = 0; i < effect->count; i++, data++) {
		data->x += data->speed * data->dirX;
		data->z += data->speed * data->dirZ;
		data->velY *= float_0p9_8041c6e8;
		data->y += data->velY;
		data->spinVel *= float_0p95_8041c6ec;
		data->spin += data->spinVel;
		data->pulseScale = angle;
		if (data->counter > 15) {
			data->alpha = (s32)((f32)data->alpha * float_0p8_8041c6f0);
		}
	}
#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri2Disp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effKemuri2Disp, effect);
#endif
}

// Draws a Kemuri puff as two camera-facing quads.
#ifdef MUST_MATCH
// MUST_MATCH: with the front end's common-subexpression pass off, the 0.5 and 20 loads stay separate
// until the back end merges and hoists them, ranking them by first use as it does pool literals (half
// f31, twenty f30, the int->float magic f29); `one` and `left` are the preheader locals below f29.
#pragma opt_common_subs off
#endif
void effKemuriDisp(CameraId camId, void* param) {
#ifdef MUST_MATCH
	f32 one;
	f32 left;
#endif
	EffectEntry* effect = param;
	effKemuriData* data = effect->userdata;
	s32 i;
	s32 j;
	s32 type = data->kind;
	CameraEntry* camera;
	GXTexObj texobj;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx outMtx;
	Mtx tmpMtx;

	camera = camGetPtr(camId);

	switch (type) {
	case 0:
		effGetTexObj(0x23, &texobj);
		GXLoadTexObj(&texobj, GX_TEXMAP0);
		break;
	case 1:
		effGetTexObj(0x24, &texobj);
		GXLoadTexObj(&texobj, GX_TEXMAP0);
		break;
	case 2:
		effGetTexObj(0x23, &texobj);
		GXLoadTexObj(&texobj, GX_TEXMAP0);
		break;
	}

	PSMTXTrans(transMtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041c6a4 * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scaleMtx, data->scale, data->scale, float_1_8041c6b0);
	PSMTXConcat(transMtx, rotMtx, outMtx);
	PSMTXConcat(outMtx, scaleMtx, outMtx);
	PSMTXConcat(camera->view, outMtx, outMtx);

	GXSetCurrentMtx(GX_PNMTX0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetChanMatColor(GX_COLOR0A0, dat_8041c670);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
	GXSetTevColor(GX_TEVREG0, dat_8041c674);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

#ifdef MUST_MATCH
	one = float_1_8041c6b0;
#endif
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		for (j = 0; j < 2; j++) {
			if (j == 0) {
				GXColor color = dat_8041c678;

				GXSetChanMatColor(GX_COLOR0A0, color);
			} else {
				GXColor color = dat_8041c67c;

				GXSetChanMatColor(GX_COLOR0A0, color);
			}

			{
				s32 t = 1 - j;

				PSMTXTrans(transMtx, data->offX + (f32)t, (f32)-t, float_0_8041c6bc);
			}
			PSMTXScale(scaleMtx, float_20_8041c694 * data->scaleX, float_20_8041c694 * data->scaleY, float_20_8041c694);
			PSMTXConcat(outMtx, transMtx, tmpMtx);
			PSMTXConcat(tmpMtx, scaleMtx, tmpMtx);
			GXLoadPosMtxImm(tmpMtx, GX_PNMTX0);

#ifdef MUST_MATCH
			{ f32 unity = MATCH_F32(float_1_8041c6b0); PSMTXScale(scaleMtx, float_0p0625_8041c6d8, unity, unity); }
#else
			PSMTXScale(scaleMtx, float_0p0625_8041c6d8, float_1_8041c6b0, float_1_8041c6b0);
#endif
			PSMTXTrans(transMtx, (f32)data->sheetU0, float_neg0p2_8041c6dc, float_0_8041c6bc);
			PSMTXConcat(scaleMtx, transMtx, transMtx);
			GXLoadTexMtxImm(transMtx, GX_TEXMTX0, GX_MTX2x4);

#ifdef MUST_MATCH
			{ f32 unity = MATCH_F32(float_1_8041c6b0); PSMTXScale(scaleMtx, float_0p0625_8041c6d8, unity, unity); }
#else
			PSMTXScale(scaleMtx, float_0p0625_8041c6d8, float_1_8041c6b0, float_1_8041c6b0);
#endif
			PSMTXTrans(transMtx, (f32)data->sheetU1, float_neg0p2_8041c6dc, float_0_8041c6bc);
			PSMTXConcat(scaleMtx, transMtx, transMtx);
			GXLoadTexMtxImm(transMtx, GX_TEXMTX1, GX_MTX2x4);

			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
#ifdef MUST_MATCH
			left = -one * float_0p5_8041c6cc;
			GXParam1f32(left);
			GXParam1f32(one);
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
			GXPosition3f32(-float_1_8041c6b0 * float_0p5_8041c6cc, float_1_8041c6b0, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
			GXTexCoord2f32(float_0_8041c6bc, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
			GXParam1f32(float_0p5_8041c6cc);
			GXParam1f32(one);
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
			GXPosition3f32(float_0p5_8041c6cc, float_1_8041c6b0, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
			GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
			GXTexCoord2f32(float_1_8041c6b0, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
			GXParam1f32(float_0p5_8041c6cc);
			GXParam1f32(MATCH_F32(float_0_8041c6bc));
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
			GXPosition3f32(float_0p5_8041c6cc, float_0_8041c6bc, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
			GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
			GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
#else
			GXTexCoord2f32(float_1_8041c6b0, float_1_8041c6b0);
#endif
#ifdef MUST_MATCH
			GXParam1f32(left);
			GXParam1f32(MATCH_F32(float_0_8041c6bc));
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
			GXPosition3f32(-float_1_8041c6b0 * float_0p5_8041c6cc, float_0_8041c6bc, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
#else
			GXTexCoord2f32(float_0_8041c6bc, float_1_8041c6b0);
#endif
		}
	}
}

#ifdef MUST_MATCH
#pragma opt_common_subs reset
#endif

// Draws two camera-facing textured quads for the secondary smoke effect.
#ifdef MUST_MATCH
static inline GXColor kemuriTevColor(u8 alpha, GXColor c) {
	c.a = alpha;
	return c;
}
#endif

// 100% matched: the p8/p12/n12p8 preheader locals (declared p8,p12,n12p8 and assigned
// n12p8,p12,p8) and the quad's x6/y4/px/py locals reproduce retail's volatile-FPR colouring.
#ifdef MUST_MATCH
// MUST_MATCH: reading 6.4 through a cast keeps the front end from merging its uses.
#define KEMURI_SIXP4 (*(f32*)&float_6p4_8041c6d4)
#endif
static void effKemuri2Disp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effKemuri2Data* data = effect->userdata;
	CameraEntry* camera;
	GXTexObj texobj;
	Mtx transMtx;
	Mtx rotMtx;
	Mtx rotMtx2;
	Mtx scaleMtx;
	Mtx outMtx;
	Mtx tmpMtx;
	s32 i;
#ifdef MUST_MATCH
	/* MUST_MATCH: names for the negSize source loads keep their volatile-FPR colouring. */
	f32 p8;
	f32 p12;
	f32 n12p8;
#endif

	camera = camGetPtr(cameraId);

	GXSetNumChans(0);
	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetCullMode(GX_CULL_NONE);
	mapSetMaterialFog();
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

#ifdef MUST_MATCH
	n12p8 = -float_12p8_8041c690;
	p12 = float_12_8041c688;
	p8 = float_8_8041c68c;
#endif

	for (i = 0; i < 2; i++) {
#ifdef MUST_MATCH
		const f32 negSizeX = -p12;
		const f32 negSizeY = -p8;
		const f32 negSize2 = n12p8 * float_0p5_8041c6cc;
#else
		const f32 negSizeX = -float_12_8041c688;
		const f32 negSizeY = -float_8_8041c68c;
		const f32 negSize2 = -float_12p8_8041c690 * float_0p5_8041c6cc;
#endif

		if ((data->heading > float_20_8041c694 && data->heading < float_160_8041c698) ||
		    (data->heading > float_180_8041c69c && data->heading < float_340_8041c6a0)) {
#ifdef MUST_MATCH
			/* MUST_MATCH: named quad temps reproduce retail's volatile-FPR colouring. */
			f32 x6;
			f32 y4;
			f32 px;
			f32 py;
#endif
			PSMTXTrans(transMtx, data->x + (f32)(1 - i), data->y - (f32)(1 - i), data->z);
			PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041c6a4 * -camGetPtr(cameraId)->viewYaw);
			PSMTXRotRad(rotMtx2, 'z', float_0p17453_8041c6a8);
			PSMTXScale(scaleMtx, data->pulseScale, data->pulseScale, data->pulseScale);
			PSMTXConcat(transMtx, rotMtx, outMtx);
			if (data->heading < float_180_8041c69c) {
				PSMTXScale(tmpMtx, float_neg1_8041c6ac, float_1_8041c6b0, float_neg1_8041c6ac);
				PSMTXConcat(outMtx, tmpMtx, outMtx);
			}
			PSMTXTrans(tmpMtx, float_10_8041c6b4, float_neg6_8041c6b8, float_0_8041c6bc);
			PSMTXConcat(outMtx, tmpMtx, outMtx);
			PSMTXScale(tmpMtx, data->scale, data->scale, data->scale);
			PSMTXConcat(outMtx, tmpMtx, outMtx);
			PSMTXTrans(tmpMtx, float_neg10_8041c6c0, float_6_8041c6c4, float_0_8041c6bc);
			PSMTXConcat(outMtx, tmpMtx, outMtx);
			PSMTXConcat(outMtx, rotMtx2, outMtx);
			PSMTXConcat(outMtx, scaleMtx, outMtx);
			PSMTXConcat(camera->view, outMtx, outMtx);
			PSMTXTrans(transMtx, float_6_8041c6c4, float_neg4_8041c6c8, float_0_8041c6bc);
			PSMTXConcat(outMtx, transMtx, outMtx);
			GXLoadPosMtxImm(outMtx, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			effGetTexObj(0xD, &texobj);
			GXLoadTexObj(&texobj, GX_TEXMAP0);
			if (i == 0) {
#ifdef MUST_MATCH
				GXSetTevColor(GX_TEVREG0, kemuriTevColor(data->alpha, unk_80423010));
#else
				GXColor color = unk_80423010;

				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
#endif
			} else {
#ifdef MUST_MATCH
				GXSetTevColor(GX_TEVREG0, kemuriTevColor(data->alpha, dat_8041c680));
#else
				GXColor color = dat_8041c680;

				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
#endif
			}
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
#ifdef MUST_MATCH
			px = negSizeX * float_0p5_8041c6cc;
			py = negSizeY * float_0p5_8041c6cc;
			y4 = float_4_8041c6d0;
			x6 = float_6_8041c6c4;
			GXWGFifo.f32 = px;
			GXWGFifo.f32 = y4;
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = x6;
			GXWGFifo.f32 = y4;
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = x6;
			GXWGFifo.f32 = py;
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
			GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
			GXWGFifo.f32 = px;
			GXWGFifo.f32 = py;
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
			GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
#else
			GXPosition3f32(negSizeX * float_0p5_8041c6cc, float_4_8041c6d0, float_0_8041c6bc);
			GXTexCoord2f32(float_0_8041c6bc, float_0_8041c6bc);
			GXPosition3f32(float_6_8041c6c4, float_4_8041c6d0, float_0_8041c6bc);
			GXTexCoord2f32(float_1_8041c6b0, float_0_8041c6bc);
			GXPosition3f32(float_6_8041c6c4, negSizeY * float_0p5_8041c6cc, float_0_8041c6bc);
			GXTexCoord2f32(float_1_8041c6b0, float_1_8041c6b0);
			GXPosition3f32(negSizeX * float_0p5_8041c6cc, negSizeY * float_0p5_8041c6cc, float_0_8041c6bc);
			GXTexCoord2f32(float_0_8041c6bc, float_1_8041c6b0);
#endif
		}

		PSMTXTrans(transMtx, data->x + (f32)(1 - i), data->y - (f32)(1 - i), data->z);
		PSMTXRotRad(rotMtx, 'y', float_deg2rad_8041c6a4 * -camGetPtr(cameraId)->viewYaw);
		PSMTXRotRad(rotMtx2, 'z', float_deg2rad_8041c6a4 * data->spin);
		PSMTXScale(scaleMtx, data->pulseScale, data->pulseScale, data->pulseScale);
		PSMTXConcat(transMtx, rotMtx, outMtx);
		if (data->heading < float_180_8041c69c) {
			PSMTXScale(tmpMtx, float_neg1_8041c6ac, float_1_8041c6b0, float_neg1_8041c6ac);
			PSMTXConcat(outMtx, tmpMtx, outMtx);
		}
		PSMTXTrans(tmpMtx, float_10_8041c6b4, float_neg6_8041c6b8, float_0_8041c6bc);
		PSMTXConcat(outMtx, tmpMtx, outMtx);
		PSMTXScale(tmpMtx, data->scale, data->scale, data->scale);
		PSMTXConcat(outMtx, tmpMtx, outMtx);
		PSMTXTrans(tmpMtx, float_neg10_8041c6c0, float_6_8041c6c4, float_0_8041c6bc);
		PSMTXConcat(outMtx, tmpMtx, outMtx);
		PSMTXConcat(outMtx, rotMtx2, outMtx);
		PSMTXConcat(outMtx, scaleMtx, outMtx);
		PSMTXConcat(camera->view, outMtx, outMtx);
		GXLoadPosMtxImm(outMtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		if (i == 0) {
			effGetTexObj(0xC, &texobj);
			GXLoadTexObj(&texobj, GX_TEXMAP0);
#ifdef MUST_MATCH
			GXSetTevColor(GX_TEVREG0, kemuriTevColor(data->alpha, unk_80423014));
#else
			{
				GXColor color = unk_80423014;

				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
			}
#endif
		} else {
			effGetTexObj(0xB, &texobj);
			GXLoadTexObj(&texobj, GX_TEXMAP0);
#ifdef MUST_MATCH
			GXSetTevColor(GX_TEVREG0, kemuriTevColor(data->alpha, dat_8041c684));
#else
			{
				GXColor color = dat_8041c684;

				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
			}
#endif
		}
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
#ifdef MUST_MATCH
		GXWGFifo.f32 = negSize2;
		GXWGFifo.f32 = KEMURI_SIXP4;
		GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
		GXPosition3f32(negSize2, float_6p4_8041c6d4, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
		GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
		GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
		GXTexCoord2f32(float_0_8041c6bc, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
		GXWGFifo.f32 = KEMURI_SIXP4;
		GXWGFifo.f32 = KEMURI_SIXP4;
		GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
		GXPosition3f32(float_6p4_8041c6d4, float_6p4_8041c6d4, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
		GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
		GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
		GXTexCoord2f32(float_1_8041c6b0, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
		GXWGFifo.f32 = KEMURI_SIXP4;
		GXWGFifo.f32 = negSize2;
		GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
		GXPosition3f32(float_6p4_8041c6d4, negSize2, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
		GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
		GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
#else
		GXTexCoord2f32(float_1_8041c6b0, float_1_8041c6b0);
#endif
#ifdef MUST_MATCH
		GXWGFifo.f32 = negSize2;
		GXWGFifo.f32 = negSize2;
		GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
#else
		GXPosition3f32(negSize2, negSize2, float_0_8041c6bc);
#endif
#ifdef MUST_MATCH
		GXWGFifo.f32 = MATCH_F32(float_0_8041c6bc);
		GXWGFifo.f32 = MATCH_F32(float_1_8041c6b0);
#else
		GXTexCoord2f32(float_0_8041c6bc, float_1_8041c6b0);
#endif
	}
}


const GXColor dat_8041c670 = { 0xff, 0xff, 0xff, 0xff };
const GXColor dat_8041c674 = { 0x80, 0x80, 0x80, 0x80 };
const GXColor dat_8041c678 = { 0x0, 0x0, 0x0, 0x60 };
const GXColor dat_8041c67c = { 0xff, 0xff, 0xff, 0xff };
const GXColor dat_8041c680 = { 0xff, 0xff, 0xff, 0x0 };
const GXColor dat_8041c684 = { 0xff, 0xff, 0xff, 0x0 };
const f32 float_12_8041c688 = 12.0f;
const f32 float_8_8041c68c = 8.0f;
const f32 float_12p8_8041c690 = 12.8000002f;
const f32 float_20_8041c694 = 20.0f;
const f32 float_160_8041c698 = 160.0f;
const f32 float_180_8041c69c = 180.0f;
const f32 float_340_8041c6a0 = 340.0f;
const f32 float_deg2rad_8041c6a4 = 0.0174532924f;
const f32 float_0p17453_8041c6a8 = 0.17453292f;
const f32 float_neg1_8041c6ac = -1.0f;
const f32 float_1_8041c6b0 = 1.0f;
const f32 float_10_8041c6b4 = 10.0f;
const f32 float_neg6_8041c6b8 = -6.0f;
const f32 float_0_8041c6bc = 0.0f;
const f32 float_neg10_8041c6c0 = -10.0f;
const f32 float_6_8041c6c4 = 6.0f;
const f32 float_neg4_8041c6c8 = -4.0f;
const f32 float_0p5_8041c6cc = 0.5f;
const f32 float_4_8041c6d0 = 4.0f;
const f32 float_6p4_8041c6d4 = 6.4000001f;
const f32 float_0p0625_8041c6d8 = 0.0625f;
const f32 float_neg0p2_8041c6dc = -0.200000003f;
const f32 float_3p1416_8041c6e0 = 3.14159274f;
const f32 float_30_8041c6e4 = 30.0f;
const f32 float_0p9_8041c6e8 = 0.899999976f;
const f32 float_0p95_8041c6ec = 0.949999988f;
const f32 float_0p8_8041c6f0 = 0.800000012f;
const f32 float_16_8041c6f4 = 16.0f;
const f32 float_0p0078125_8041c6f8 = 0.0078125f;
const f32 float_1p5_8041c6fc = 1.5f;
const f32 float_0p85_8041c700 = 0.850000024f;
const f32 float_2_8041c704 = 2.0f;
const f32 float_1p2_8041c708 = 1.20000005f;
const f32 float_360_8041c70c = 360.0f;
const f32 float_1000_8041c710 = 1000.0f;
const f32 float_6p2832_8041c714 = 6.28318548f;
const f32 float_5_8041c718 = 5.0f;
const f32 float_0p7_8041c71c = 0.699999988f;
const f32 float_neg20_8041c720 = -20.0f;
const f32 float_3_8041c724 = 3.0f;
const f32 float_neg3_8041c728 = -3.0f;
#ifdef MUST_MATCH
__declspec(section ".data")
#endif
const u8 touchdown_scale[60] = { 0x80, 0x80, 0x82, 0x7f, 0x84, 0x7e, 0x87, 0x7d, 0x89, 0x7c, 0x8c, 0x7a, 0x8f, 0x79, 0x92, 0x78, 0x95, 0x77, 0x98, 0x76, 0x9b, 0x75, 0x9d, 0x74, 0xa0, 0x72, 0xa3, 0x71, 0xa6, 0x70, 0xa9, 0x6f, 0xac, 0x6e, 0xae, 0x6d, 0xb1, 0x6c, 0xb3, 0x6b, 0xb5, 0x6a, 0xb7, 0x69, 0xb9, 0x69, 0xbb, 0x68, 0xbc, 0x67, 0xbd, 0x67, 0xbe, 0x66, 0xbf, 0x66, 0xbf, 0x66, 0xc0, 0x66 };
#ifdef MUST_MATCH
__declspec(section ".data")
#endif
const u8 heavy_scale[60] = { 0x80, 0x80, 0x84, 0x80, 0x89, 0x81, 0x8e, 0x82, 0x93, 0x83, 0x99, 0x85, 0x9e, 0x86, 0xa4, 0x87, 0xaa, 0x88, 0xb0, 0x89, 0xb6, 0x8a, 0xbb, 0x8b, 0xc1, 0x8d, 0xc7, 0x8e, 0xcd, 0x8f, 0xd2, 0x90, 0xd8, 0x91, 0xdd, 0x92, 0xe2, 0x93, 0xe6, 0x94, 0xeb, 0x95, 0xef, 0x96, 0xf2, 0x96, 0xf6, 0x97, 0xf9, 0x98, 0xfb, 0x98, 0xfd, 0x99, 0xfe, 0x99, 0xff, 0x99, 0xff, 0x99 };
#ifdef MUST_MATCH
// MWCC emits uninitialized objects in reverse declaration order.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") const GXColor unk_80423014;
__declspec(section ".sdata") const GXColor unk_80423010;
#else
const GXColor unk_80423014 = { 0, 0, 0, 0 };
const GXColor unk_80423010 = { 0, 0, 0, 0 };
#endif

#ifdef MUST_MATCH
#pragma force_active on
__declspec(section ".sdata2") const u32 gap_8041c72c = 0;
#pragma force_active reset
#endif

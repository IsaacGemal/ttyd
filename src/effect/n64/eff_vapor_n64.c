#include "eff/n64/eff_vapor_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

EffectEntry* effVaporN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effVaporN64Data* data;

	effect = effEntry();
	effect->type = "VaporN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effVaporN64Data));
	effect->userdata = data;
	effect->callback = effVaporMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = lifetime;
	}
	data->color1A = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->color1R = 0xff;
	data->color1G = 0xff;
	data->color1B = 0xff;
	data->color2R = 0xff;
	data->color2G = 0xff;
	data->color2B = 0xff;
	data->color2A = 0xff;
	data->field_0x38 = 0.0f;
	data->field_0x3c = 0.0f;
	return effect;
}

// opt_common_subs off required: with CSE on MWCC reuses the 0.02f result register for
// data->field_0x3c in the y integration below (f0/f1 swapped vs the target's f2/f1).
#pragma opt_common_subs off

inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effVaporMain(EffectEntry* effect) {
	effVaporN64Data* data;
	Vec position2;
	Vec position;
	s32 type;

	data = effect->userdata;
	position = makeVec(data->x, data->y, data->z);
	position2 = position;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x10;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->field_0x14++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
	if (data->lifetime < 0x10) {
		data->color1A = data->lifetime << 4;
	}
	switch (type) {
	case 0:
	case 1:
		data->field_0x3c += float_0p02_80420100;
		break;
	}
	data->y += data->field_0x3c;
	if (data->field_0x3c > float_0p5_804200dc) {
		data->field_0x3c = float_0p5_804200dc;
	}
	if (data->field_0x3c < float_neg1p5_80420104) {
		data->field_0x3c = float_neg1p5_80420104;
	}
	data->field_0x38 += float_1_80420108;
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effVaporDisp, effect);
}
#pragma opt_common_subs reset

static inline GXColor effVaporN64Color1(effVaporN64Data* data, s32 alpha) {
	GXColor color = unk_804232b8;
	color.r = data->color1R;
	color.g = data->color1G;
	color.b = data->color1B;
	color.a = alpha;
	return color;
}

static inline GXColor effVaporN64Color2(effVaporN64Data* data, s32 alpha) {
	GXColor color = unk_804232bc;
	color.r = data->color2R;
	color.g = data->color2G;
	color.b = data->color2B;
	color.a = alpha;
	return color;
}

// The body is complete and byte-correct except for (a) the compiler keeps `data` in
// r30 and `frame` in r29, the original the other way round; (b) the table reads use
// base+index then an offset load instead of the original's base+offset then an indexed
// load; (c) the int->float helpers are pooled as anonymous .rodata copies instead of
// double_to_int_802fb8f8/_mask_802fb900; (d) the `band < 4` branch polarity is inverted.
// NONMATCHING (95.94%): see list above.
void effVaporDisp(CameraId cameraId, void* param) {
	EffectEntry* effect;
	effVaporN64Data* data;
	CameraEntry* cam;
	EffVaporN64DispData* disp;
	GXTexObj texObj;
	Mtx mtxA;
	Mtx mtxB;
	Mtx mtxC;
	Mtx mtxD;
	GXColor color;
	f32 scale;
	f32 rz;
	f32 dx;
	f32 t;
	s32 frame;
	s32 alpha1;
	s32 type;
	s32 index;
	s32 band;
	s32 frac;

	effect = param;
	disp = &lbl_803A85F8;
	cam = camGetPtr(cameraId);
	data = effect->userdata;
	frame = data->field_0x14;
	alpha1 = data->color1A;
	type = data->type;
	PSMTXTrans(mtxA, data->x, data->y, data->z);
	scale = data->scale;
	PSMTXScale(mtxB, scale, scale, scale);
	PSMTXRotRad(mtxC, 'y', float_deg2rad_804200d8 * -camGetPtr(cameraId)->viewYaw);
	PSMTXConcat(mtxA, mtxC, mtxA);
	PSMTXConcat(mtxA, mtxB, mtxA);
	PSMTXConcat(cam->view, mtxA, mtxD);

	color = effVaporN64Color1(data, float_0p5_804200dc * (f32)alpha1);
	GXSetTevColor(GX_TEVREG0, color);

	switch (type) {
	case 0:
	case 2:
		GXSetNumChans(0);
		GXSetNumTevStages(3);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_A1, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A1, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_C1, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
		effGetTexObjN64(0x69, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXLoadTexObj(&texObj, GX_TEXMAP1);
		GXSetNumTexGens(2);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
		GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, 0x7D);
		break;
	default:
		GXSetNumChans(0);
		GXSetNumTevStages(3);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_A1, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A1, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_C1, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		effGetTexObjN64(0x6A, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXLoadTexObj(&texObj, GX_TEXMAP1);
		GXSetNumTexGens(2);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
		GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, 0x7D);
		break;
	}

	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)disp->vtx);
	if (frame >= 0x1E) {
		frame = 0x1D;
	}
	index = type * 0x1E + frame;
#ifdef MUST_MATCH
	// The original materialises each table base (base+offset) before the indexed load;
	// the struct field form below common-subexpresses to base+index and an offset load.
	scale = float_0p01_804200e0 * *(u8*)((u8*)disp + 0x9C + index);
	rz = -(f32)*(u8*)((u8*)disp + 0x154 + index);
	dx = *(s8*)((u8*)disp + 0x40 + index);
#else
	scale = float_0p01_804200e0 * disp->scale_data[index];
	rz = -(f32)disp->rz_data[index];
	dx = disp->dx_data[index];
#endif
	PSMTXTrans(mtxA, dx, float_0_804200e4, float_0_804200e4);
	PSMTXRotRad(mtxC, 'z', float_deg2rad_804200d8 * rz);
	PSMTXScale(mtxB, scale, scale, scale);
	PSMTXConcat(mtxA, mtxC, mtxA);
	PSMTXConcat(mtxA, mtxB, mtxA);
	PSMTXConcat(mtxD, mtxA, mtxA);
	GXLoadPosMtxImm(mtxA, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

#ifdef MUST_MATCH
	t = (f32)*(u8*)((u8*)disp + 0xF8 + index) * float_5_804200e8 * float_0p0039062_804200ec;
#else
	t = (f32)disp->seq_data[index] * float_5_804200e8 * float_0p0039062_804200ec;
#endif
	band = (s32)t;
	frac = (s32)(float_256_804200f0 * (t - (f32)band));
	PSMTXScale(mtxB, float_0p032258_804200f4, float_0p007874_804200f8, float_0_804200e4);
	PSMTXTrans(mtxA, float_0_804200e4, float_32_804200fc, float_0_804200e4);
	PSMTXConcat(mtxB, mtxA, mtxB);
	GXLoadTexMtxImm(mtxB, GX_TEXMTX0, GX_MTX2x4);
	if (band <= 4) {
		if (band < 4) {
			PSMTXScale(mtxB, float_0p032258_804200f4, float_0p007874_804200f8, float_0_804200e4);
			GXLoadTexMtxImm(mtxB, GX_TEXMTX1, GX_MTX2x4);
		}
		else {
			PSMTXScale(mtxB, float_0_804200e4, float_0_804200e4, float_0_804200e4);
			GXLoadTexMtxImm(mtxB, GX_TEXMTX1, GX_MTX2x4);
		}
		color = effVaporN64Color2(data, frac);
		GXSetTevColor(GX_TEVREG1, color);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(band * 4, band * 4 + 1, band * 4 + 2, 0, band * 4, band * 4 + 2, band * 4 + 3, 0);
	}
}


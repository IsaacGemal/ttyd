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

// .data float pairs and tables, in the original's order.
static f32 negone_one_374[2] = { -1.0f, 1.0f };
f32 lbl_803A8608[2] = { 1.0f, -1.0f };
static f32 one_negone_373[2] = { 1.0f, -1.0f };
f32 lbl_803A8618[2] = { -1.0f, 1.0f };
static f32 negone_one_360[2] = { -1.0f, 1.0f };
f32 lbl_803A8628[2] = { 1.0f, -1.0f };
static f32 one_negone_359[2] = { 1.0f, -1.0f };
s8 dx_data[0x5C] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -8, -15,
	-21, -26, -30, -34, -37, -39, -40, -41, -42, -43, -44, -45, -46, -47, -48, -49,
	-50, -51, -52, -53, -54, -55, -56, -57, -58, -59, -60, -61, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static u8 scale_data[0x5C] = {
	0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x32, 0x32,
	0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x34, 0x37, 0x39, 0x3C, 0x3E, 0x41, 0x43, 0x46,
	0x48, 0x4B, 0x4D, 0x50, 0x52, 0x55, 0x57, 0x5A, 0x5C, 0x5F, 0x61, 0x64, 0x28, 0x29, 0x2A, 0x2B,
	0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
	0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x00, 0x00,
};
static u8 seq_data[0x5C] = {
	0x00, 0x14, 0x28, 0x3C, 0x50, 0x64, 0x78, 0x8C, 0xA0, 0xB4, 0xBE, 0xC3, 0xD2, 0xD0, 0xD3, 0xD6,
	0xD9, 0xDC, 0xDF, 0xE2, 0xE5, 0xE8, 0xEB, 0xEE, 0xF1, 0xF4, 0xF7, 0xFA, 0xFD, 0xFF, 0x00, 0x14,
	0x28, 0x3C, 0x50, 0x64, 0x78, 0x8C, 0xA0, 0xB4, 0xBE, 0xC3, 0xD2, 0xD0, 0xD3, 0xD6, 0xD9, 0xDC,
	0xDF, 0xE2, 0xE5, 0xE8, 0xEB, 0xEE, 0xF1, 0xF4, 0xF7, 0xFA, 0xFD, 0xFF, 0x00, 0x14, 0x28, 0x3C,
	0x50, 0x64, 0x78, 0x8C, 0xA0, 0xB4, 0xBE, 0xC3, 0xD2, 0xD0, 0xD3, 0xD6, 0xD9, 0xDC, 0xDF, 0xE2,
	0xE5, 0xE8, 0xEB, 0xEE, 0xF1, 0xF4, 0xF7, 0xFA, 0xFD, 0xFF, 0x00, 0x00,
};
u8 rz_data[0x5C] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0xB4, 0xB4, 0xB4, 0xB4,
	0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4,
	0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0xB4, 0x00, 0x00,
};
static u32 size64x64_tex32x32_vtx[0x118 / 4] = {
	0xFFE00000, 0x00020000, 0x04007878, 0x00FF0020, 0x00000002, 0x04000400, 0x787800FF,
	0x00200040, 0x00020400, 0x00007878, 0x00FFFFE0, 0x00400002, 0x00000000, 0x787800FF,
	0xFFE00000, 0x00020000, 0x08007878, 0x00FF0020, 0x00000002, 0x04000800, 0x787800FF,
	0x00200040, 0x00020400, 0x04007878, 0x00FFFFE0, 0x00400002, 0x00000400, 0x787800FF,
	0xFFE00000, 0x00020000, 0x0C007878, 0x00FF0020, 0x00000002, 0x04000C00, 0x787800FF,
	0x00200040, 0x00020400, 0x08007878, 0x00FFFFE0, 0x00400002, 0x00000800, 0x787800FF,
	0xFFE00000, 0x00020000, 0x10007878, 0x00FF0020, 0x00000002, 0x04001000, 0x787800FF,
	0x00200040, 0x00020400, 0x0C007878, 0x00FFFFE0, 0x00400002, 0x00000C00, 0x787800FF,
	0xFFE00000, 0x00020000, 0x14007878, 0x00FF0020, 0x00000002, 0x04001400, 0x787800FF,
	0x00200040, 0x00020400, 0x10007878, 0x00FFFFE0, 0x00400002, 0x00001000, 0x787800FF,
};

#ifdef MUST_MATCH
// The original reaches the tables from the address 8 bytes before its .data.
extern u8 lbl_803A85F8[];
#endif

// The original materialises each table base (base+offset) before an indexed load;
// MWCC 2.6 instead folds the four bases into base+index plus an offset load. The two
// GXColor inline temporaries also land in swapped stack slots (color1 0x10 / color2
// 0xc in the original, 0xc / 0x10 here), and the seq*5*0.0039062 multiply keeps the
// 5.0 and 0.0039062 registers swapped. The int->float pool entries are anonymous
// @NNN@ locals, which objdiff resolves to the original's double_to_int_* symbols.
// opt_common_subs off: with CSE on MWCC folds the table bases through one register,
// costing more instructions than the original's per-base form.
#pragma opt_common_subs off
// NONMATCHING (97.28%): the table address form, the GXColor temporaries' stack slots
// and the seq multiply's f-registers still differ.
void effVaporDisp(CameraId cameraId, void* param) {
	EffectEntry* effect;
	s32 frame;
	effVaporN64Data* data;
	CameraEntry* cam;
#ifdef MUST_MATCH
	u8* disp;
#endif
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
	s32 alpha1;
	s32 type;
	s32 index;
	s32 frac;
	s32 band;

	effect = param;
#ifdef MUST_MATCH
	disp = lbl_803A85F8;
#endif
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
#ifdef MUST_MATCH
	effSetVtxDescN64((effVtxArray*)(disp + 0x1B0));
#else
	effSetVtxDescN64((effVtxArray*)size64x64_tex32x32_vtx);
#endif
	if (frame >= 0x1E) {
		frame = 0x1D;
	}
	index = type * 0x1E + frame;
#ifdef MUST_MATCH
	// The original materialises each table base (base+offset) before the indexed load;
	// the struct field form below common-subexpresses to base+index and an offset load.
	scale = float_0p01_804200e0 * *(u8*)(disp + 0x9C + index);
	rz = -(f32)*(u8*)(disp + 0x154 + index);
	dx = *(s8*)(disp + 0x40 + index);
#else
	scale = float_0p01_804200e0 * scale_data[index];
	rz = -(f32)rz_data[index];
	dx = dx_data[index];
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
	t = (f32)*(u8*)(disp + 0xF8 + index) * float_5_804200e8 * float_0p0039062_804200ec;
#else
	t = (f32)seq_data[index] * float_5_804200e8 * float_0p0039062_804200ec;
#endif
	band = (s32)t;
	frac = (s32)(float_256_804200f0 * (t - (f32)band));
	PSMTXScale(mtxB, float_0p032258_804200f4, float_0p007874_804200f8, float_0_804200e4);
	PSMTXTrans(mtxA, float_0_804200e4, float_32_804200fc, float_0_804200e4);
	PSMTXConcat(mtxB, mtxA, mtxB);
	GXLoadTexMtxImm(mtxB, GX_TEXMTX0, GX_MTX2x4);
	if (band <= 4) {
		if (band >= 4) {
			PSMTXScale(mtxB, float_0_804200e4, float_0_804200e4, float_0_804200e4);
			GXLoadTexMtxImm(mtxB, GX_TEXMTX1, GX_MTX2x4);
		}
		else {
			PSMTXScale(mtxB, float_0p032258_804200f4, float_0p007874_804200f8, float_0_804200e4);
			GXLoadTexMtxImm(mtxB, GX_TEXMTX1, GX_MTX2x4);
		}
		color = effVaporN64Color2(data, frac);
		GXSetTevColor(GX_TEVREG1, color);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(band * 4, band * 4 + 1, band * 4 + 2, 0, band * 4, band * 4 + 2, band * 4 + 3, 0);
	}
}


#pragma opt_common_subs reset

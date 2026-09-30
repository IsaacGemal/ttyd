#include "eff/n64/eff_packn_bfire_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"
#include "matching.h"

#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdlib.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0_8041f8ec;
extern const f32 float_1_8041f900;
extern const f32 float_0p03125_8041f8e8;
extern const f32 float_128_8041f8f8;
extern const f32 float_deg2rad_8041f8fc;
extern const f32 float_0p0039062_8041f904;
extern const f32 float_0p015625_8041f908;
extern GXColor unk_804231f8;
extern GXColor unk_804231fc;

// .data vertex block (local symbol in the original object).
static u32 size64x64_tex32x32_vtx[14] = {
	0xFFE0FFE0, 0x00000000, 0x00000000, 0x00FF0020,
	0xFFE00000, 0x04000000, 0x000000FF, 0x00200020,
	0x00000400, 0x04000000, 0x00FFFFE0, 0x00200000,
	0x00000400, 0x000000FF,
};
extern const f32 float_0p97_8041f90c;
extern const f32 float_0p02_8041f910;
extern const f32 float_0p05_8041f914;
extern const f32 float_0p1_8041f918;
extern const f32 float_0p9_8041f91c;
extern const f32 float_0p2_8041f920;
extern const f32 float_neg0p6_8041f924;
extern const f32 float_0p07_8041f928;
extern const f32 float_6p2832_8041f8f0;
extern const f32 float_360_8041f8f4;
extern const f32 float_0p3_8041f92c;
extern const f32 float_neg10_8041f930;
extern const f32 float_5_8041f934;
extern const f32 float_3_8041f938;
extern const f32 float_2_8041f93c;
extern const f32 float_neg45_8041f940;

extern const char str_802FB324[];

// Compiler-pooled int->double bias, named after its original address.
const f64 double_to_int_802fb338 = 4503601774854144.0;

typedef struct effPacknBfireData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 vx;         // 0x10
	f32 vy;         // 0x14
	f32 vz;         // 0x18
	s32 field_0x1C; // 0x1C
	s32 lifetime;   // 0x20
	s32 timer;      // 0x24
	s32 field_0x28; // 0x28
	s32 field_0x2C; // 0x2C
	s32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
	s32 field_0x3C; // 0x3C
	s32 field_0x40; // 0x40
	s32 field_0x44; // 0x44
	f32 scale;      // 0x48
	f32 field_0x4C; // 0x4C
	f32 field_0x50; // 0x50
	f32 field_0x54; // 0x54
	f32 field_0x58; // 0x58
	f32 field_0x5C; // 0x5C
	f32 field_0x60; // 0x60
	f32 field_0x64; // 0x64
	f32 field_0x68; // 0x68
	f32 field_0x6C; // 0x6C
	f32 field_0x70; // 0x70
	f32 field_0x74; // 0x74
	f32 field_0x78; // 0x78
} effPacknBfireData;

EffectEntry* effPacknBfireN64Entry(s32 type, s32 a1, s32 a2, f32 x, f32 y, f32 z, f32 vx, f32 vy, f32 vz, f32 scale) {
	EffectEntry* effect;
	effPacknBfireData* data;
	s32 count;
	s32 i;
	f32 base;

	effect = effEntry();
	count = (type == 2) ? 8 : 1;
	effect->type = str_802FB324;
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(effPacknBfireData));
	effect->userdata = data;
	effect->callback = effPacknBfireMain;
	effect->flags |= 2;

	if (a2 <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = a2;
	}
	data->type = type;
	data->timer = 0;
	data->field_0x1C = a1;
	data->field_0x34 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->vx = vx;
	data->vy = vy;
	data->vz = vz;
	data->field_0x60 = (data->vx - data->x) / (f32)a1;
	data->field_0x64 = (data->vy - data->y) / (f32)a1;
	data->field_0x68 = (data->vz - data->z) / (f32)a1;
	data->scale = scale;

	switch (type) {
	case 0:
		break;
	case 1:
		data->field_0x28 = 0xFF;
		data->field_0x2C = 0x58;
		data->field_0x30 = 0x46;
		data->field_0x34 = 0x37;
		data->field_0x38 = 0x0B;
		data->field_0x3C = 0x33;
		data->field_0x40 = 0x7B;
		data->field_0x44 = 0xFF;
		data->field_0x4C = (f32)(rand() % 65);
		data->field_0x50 = (f32)(rand() % 65);
		data->field_0x70 = float_0p2_8041f920;
		data->field_0x6C = float_0p3_8041f92c;
		data->field_0x54 = float_0_8041f8ec;
		data->field_0x58 = float_0_8041f8ec;
		data->field_0x74 = float_0_8041f8ec;
		data->field_0x78 = float_neg10_8041f930;
		break;
	default:
		data->field_0x28 = 0xFF;
		data->field_0x2C = 0x58;
		data->field_0x30 = 0x46;
		data->field_0x34 = 0x37;
		data->field_0x38 = 0x0B;
		data->field_0x3C = 0x33;
		data->field_0x40 = 0x7B;
		data->field_0x44 = 0xFF;
		base = (f32)(rand() % 361);
		for (i = 0; i < count; i++, data++) {
			f32 rad;

			data->x = x;
			data->y = y;
			data->z = z;
			rad = float_6p2832_8041f8f0 * (base + (f32)(i * 360 / 8)) / float_360_8041f8f4;
			data->field_0x60 = float_5_8041f934 * (f32)sin(rad);
			data->field_0x64 = float_2_8041f93c * (f32)cos(rad) + float_3_8041f938;
			data->field_0x4C = (f32)(rand() % 65);
			data->field_0x50 = (f32)(rand() % 65);
			data->field_0x70 = float_1_8041f900;
			data->field_0x6C = float_1_8041f900;
			data->field_0x54 = float_0_8041f8ec;
			data->field_0x58 = float_0_8041f8ec;
			data->field_0x74 = float_neg45_8041f940;
			data->field_0x78 = float_neg10_8041f930;
		}
		break;
	}

	return effect;
}

void effPacknBfireMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	effPacknBfireData* data = effect->userdata;
	Vec disp = { 0.0f, 0.0f, 0.0f };
	s32 type;
	s32 timer;
	s32 i;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	if (data->lifetime == 1 && type == 1) {
		effPacknBfireN64Entry(2, 0x20, 0x20, data->x, data->y, data->z, data->vx, data->vy, data->vz, float_1_8041f900);
	}
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}

	timer = data->timer;
	switch (type) {
	case 0:
		if ((timer & 3) == 0) {
			effPacknBfireN64Entry(1, data->field_0x1C, data->field_0x1C, data->x, data->y, data->z, data->vx, data->vy,
			                      data->vz, float_1_8041f900);
		}
		break;
	case 1:
		data->field_0x60 *= float_0p97_8041f90c;
		data->field_0x68 *= float_0p97_8041f90c;
		data->field_0x64 += float_0p02_8041f910 * (float_1_8041f900 - data->field_0x64);
		data->x += data->field_0x60;
		data->y += data->field_0x64;
		data->z += data->field_0x68;
		data->field_0x70 += float_0p05_8041f914 * (float_1_8041f900 - data->field_0x70);
		data->field_0x6C += float_0p1_8041f918 * (float_1_8041f900 - data->field_0x6C);
		data->field_0x74 += data->field_0x78;
		break;
	case 2: {
		/* Declared in this order, assigned in this order: the register allocator must hand 0.9 to f6 and 0.2
		   to f5 (f4 for 0.0), which only happens when the declarations and the loads disagree. */
		f32 vz, vb, va;
		va = float_0p9_8041f91c;
		vb = float_0p2_8041f920;
		vz = float_0_8041f8ec;
		for (i = 0; i < effect->count; i++, data++) {
			data->field_0x60 *= va;
			data->field_0x64 -= vb;
			data->x += data->field_0x60;
			data->y += data->field_0x64;
			if (data->y < vz) {
				data->y = vz;
				if (data->field_0x64 < vz) {
					data->field_0x64 *= float_neg0p6_8041f924;
				}
			}
			data->field_0x70 = float_0p07_8041f928 * -data->field_0x70 + data->field_0x70;
			data->field_0x6C = float_0p05_8041f914 * -data->field_0x6C + data->field_0x6C;
			data->field_0x74 += data->field_0x78;
		}
		break;
	}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPacknBfireDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effPacknBfireDisp, effect);
#endif
}

// Inlined in the original; keeping each TEV colour build in its own helper reproduces the stack
// slot the compiler picks for the by-value GXColor copy.
static inline void effPacknBfireSetColor0(effPacknBfireData* data, s32 alpha) {
	GXColor color = unk_804231f8;
	color.r = data->field_0x28;
	color.g = data->field_0x2C;
	color.b = data->field_0x30;
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
}

static inline void effPacknBfireSetColor1(effPacknBfireData* data) {
	GXColor color = unk_804231fc;
	color.r = data->field_0x38;
	color.g = data->field_0x2C;
	color.b = data->field_0x40;
	color.a = data->field_0x44;
	GXSetTevColor(GX_TEVREG1, color);
}

void effPacknBfireDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	effPacknBfireData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtxA;
	Mtx mtxB;
	Mtx mtxC;
	s32 i;
	s32 alpha;
	f32 scale;
	f32 deg2rad;
	f32 half;
	f32 zero;
	f32 fullTurn;
	f32 twoPi;
	f32 angle;
	f32 omega;
	f32 z;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	alpha = data->field_0x34;
	scale = data->scale;

	if (data->type == 0) {
		return;
	}

	effPacknBfireSetColor0(data, alpha);
	effPacknBfireSetColor1(data);

	GXSetNumChans(0);
	GXSetNumTevStages(5);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C2, GX_CC_TEXC, GX_CC_C1);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A1, GX_CA_TEXA, GX_CA_A2);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C2, GX_CC_C2, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_A0, GX_CA_A1, GX_CA_APREV);
	GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_APREV, GX_CA_A2, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE4, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_C2, GX_CC_TEXC, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);

	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, 0x7D);
	PSMTXScale(mtxB, float_0p03125_8041f8e8, float_0p03125_8041f8e8, float_0_8041f8ec);
	GXLoadTexMtxImm(mtxB, GX_TEXMTX0, GX_MTX2x4);
	effGetTexObjN64(0x71, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	effGetTexObjN64(0x72, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP1);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size64x64_tex32x32_vtx);

	twoPi = float_6p2832_8041f8f0;
	fullTurn = float_360_8041f8f4;
	zero = float_0_8041f8ec;
	half = float_128_8041f8f8;
	deg2rad = float_deg2rad_8041f8fc;

	for (i = 0; i < effect->count; i++, data++) {
		data->field_0x4C += data->field_0x54;
		data->field_0x50 += data->field_0x58;
		z = MATCH_F32(float_0_8041f8ec);
		angle = angleABf(z, z, data->field_0x60, data->field_0x64);
		omega = twoPi * (data->field_0x74 - angle) / fullTurn;
		data->field_0x54 = (f32)sin(omega);
		data->field_0x58 = (f32)cos(omega);
		if (data->field_0x4C < zero) {
			data->field_0x4C += float_128_8041f8f8;
		}
		if (data->field_0x50 > half) {
			data->field_0x50 -= half;
		}

		PSMTXTrans(mtxA, data->x, data->y, data->z);
		PSMTXRotRad(mtxC, 'y', deg2rad * -camGetPtr(CAMERA_3D)->viewYaw);
		PSMTXScale(mtxB, scale, scale, scale);
		PSMTXConcat(mtxA, mtxC, mtxA);
		PSMTXConcat(mtxA, mtxB, mtxA);
		PSMTXScale(mtxB, data->field_0x70, data->field_0x6C, float_1_8041f900);
		PSMTXRotRad(mtxC, 'z', deg2rad * data->field_0x74);
		PSMTXConcat(mtxB, mtxC, mtxB);
		PSMTXConcat(mtxA, mtxB, mtxA);
		PSMTXConcat(cam->view, mtxA, mtxA);
		GXLoadPosMtxImm(mtxA, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		PSMTXScale(mtxB, float_0p0039062_8041f904, float_0p015625_8041f908, float_0_8041f8ec);
		PSMTXTrans(mtxA, data->field_0x4C, data->field_0x50, float_0_8041f8ec);
		PSMTXConcat(mtxB, mtxA, mtxB);
		GXLoadTexMtxImm(mtxB, GX_TEXMTX1, GX_MTX2x4);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}

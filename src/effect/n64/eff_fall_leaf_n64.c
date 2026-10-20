#include "eff/n64/eff_fall_leaf_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdlib.h>
#include "memory.h"

typedef struct effFallLeafParticle {
	s32 type;             // 0x00 the header's type (effFallLeafData::type), read through a particle pointer in disp
	f32 x;                // 0x04
	f32 y;                // 0x08
	f32 z;                // 0x0C
	f32 velX;             // 0x10
	f32 velY;             // 0x14
	f32 rotZ;             // 0x18 degrees, the disp's 'z' rotation
	f32 swayAngle;        // 0x1C degrees; drives the sway in rotZ and rotY
	f32 rotY;             // 0x20 degrees, the disp's 'y' rotation
	u8 field_0x24[0x0C];  // 0x24
} effFallLeafParticle;

typedef struct effFallLeafData {
	s32 type;                        // 0x00
	f32 x;                           // 0x04
	f32 y;                           // 0x08
	f32 z;                           // 0x0C
	u8 field_0x10[0x14];             // 0x10
	s32 tevReg0A;                       // 0x24 TEVREG0 alpha, fades up over the first 10 frames and down over the last 10
	s32 life;                        // 0x28
	s32 counter;                     // 0x2C
	effFallLeafParticle particle[4]; // 0x30
} effFallLeafData;

#ifdef MUST_MATCH
// Match-only: a second description of the same 0x30-byte record as effFallLeafData/effFallLeafParticle.
// Walking the loop with `data++` over this type is what reproduces the original's allocation; the
// natural form (under #else in effFallLeafMain) walks effFallLeafData::particle instead.
typedef struct effFallLeafMainData {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 field_0x10;  // 0x10
	f32 field_0x14;  // 0x14
	f32 field_0x18;  // 0x18
	f32 field_0x1C;  // 0x1C
	f32 field_0x20;  // 0x20
	s32 field_0x24;  // 0x24
	s32 life;        // 0x28
	s32 counter;     // 0x2C
} effFallLeafMainData;
#endif

extern const u32 dat_8041edb0;
extern const u32 dat_8041edb4;
extern const u32 dat_8041edb8;
extern const u32 str_8041EDBC;
extern const f32 float_deg2rad_8041edc0;
extern const f32 float_0p03125_8041edc4;
extern const f32 float_0p0078125_8041edc8;
extern const f32 float_1_8041edcc;
extern const f32 float_0p015625_8041edd0;
extern const f32 float_0p3_8041edd4;
extern const f32 float_0p8_8041edd8;
extern const f32 float_6p2832_8041eddc;
extern const f32 float_2_8041ede0;
extern const f32 float_360_8041ede4;
extern const f32 float_neg0p05_8041ede8;
extern const f32 float_0p2_8041edec;
extern const f32 float_0p94_8041edf0;
extern const f32 float_0p95_8041edf4;
extern const f32 float_0p05_8041edf8;
extern const f32 float_0p92_8041edfc;
extern const f32 float_10_8041ee00;
extern const f32 float_0p5_8041ee04;
extern const f32 float_0_8041ee08;


const Vec vec3_802fa538 = { 0.0f, 0.0f, 0.0f };

static inline void leafSetColor0(u8 a) {
	GXColor color = *(GXColor*)&dat_8041edb0;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}

static inline void leafSetColor1(u8 a) {
	GXColor color = *(GXColor*)&dat_8041edb4;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}

static inline void leafSetColor2(u8 a) {
	GXColor color = *(GXColor*)&dat_8041edb8;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}

static inline void leafSetColor3(u8 a) {
	GXColor color = *(GXColor*)&str_8041EDBC;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}

// .data: retail vertex array (two 0x38-byte effVtxArray entries).
static u32 size12x12_tex64x64_vtx[28] = {
	0xFFFAFFFA, 0x00000000, 0x00001955, 0x11FF0006,
	0xFFFA0000, 0x08000000, 0x195511FF, 0x00060006,
	0x00000800, 0x080068AC, 0x10FFFFFA, 0x00060000,
	0x00000800, 0x68AC10FF, 0xFFFAFFFA, 0x00000000,
	0x08001837, 0x21FF0006, 0xFFFA0000, 0x08000800,
	0x183721FF, 0x00060006, 0x00000800, 0x1000306C,
	0x25FFFFFA, 0x00060000, 0x00001000, 0x306C25FF,
};

EffectEntry* effFallLeafN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effFallLeafData* data;
	effFallLeafParticle* particle;
	s32 i;

	effect = effEntry();
	effect->type = "FallLeafN64";
	effect->count = 5;
	data = __memAlloc(HEAP_EFFECT, 0xF0);
	effect->userdata = data;
	effect->callback = effFallLeafMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->life = life;
	data->counter = 0;
	data->tevReg0A = 0;
	particle = data->particle;
	for (i = 1; i < 5; i++, particle++) {
		switch (type) {
		case 0:
		case 2:
		case 3:
		case 4:
			particle->x = (f32)(rand() % 50 - 25);
			particle->y = (f32)(rand() % 50 - 25);
			particle->z = float_0_8041ee08;
			particle->rotZ = (f32)(rand() % 360);
			particle->rotY = (f32)(rand() % 360);
			particle->swayAngle = (f32)(rand() % 100) / float_10_8041ee00;
			particle->velX = float_0_8041ee08;
			particle->velY = float_0_8041ee08;
			break;
		case 1:
			particle->x = (f32)(rand() % 10 - 5);
			particle->y = (f32)(rand() % 10 + 5);
			particle->z = float_10_8041ee00;
			particle->rotZ = (f32)(rand() % 360);
			particle->rotY = (f32)(rand() % 360);
			particle->swayAngle = (f32)(rand() % 100) / float_10_8041ee00;
			particle->velX = float_0_8041ee08;
			particle->velY = float_2_8041ede0;
			break;
		}
	}
	return effect;
}

// The `cos` result is stored in `t` before the `+=` (rather than writing the cos directly in the
// += statement): that makes the compiler pool the cos-argument constants after the statement's
// 10.0, reproducing retail's FPR assignment (f24=0.5, f31=10.0) and the loop-setup load order.
#ifdef MUST_MATCH
void effFallLeafMain(EffectEntry* effect) {
	effFallLeafMainData* data = effect->userdata;
	Vec position2;
	Vec position;
	s32 type;
	s32 life;
	s32 counter;
	s32 idx;
	s32 i;
	f32 s;
	f32 t;
#ifdef MUST_MATCH
	// MUST_MATCH: `sn` pins the case-branch blend to the target's scratch registers.
	f32 sn;
#endif

	position = vec3_802fa538;
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	data->life--;
	data->counter++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	type = data->type;
	life = data->life;
	if (counter < 10) {
		data->field_0x24 = (s32)(float_0p3_8041edd4 * (f32)(0xFF - data->field_0x24) + (f32)data->field_0x24);
	}
	if (life < 10) {
		data->field_0x24 = (s32)((f32)data->field_0x24 * float_0p8_8041edd8);
	}
	idx = counter + 20;
	i = 1;
	data++;
	for (; i < effect->count; idx += 20, i++, data++) {
		s = (f32)sin(float_6p2832_8041eddc * (float_2_8041ede0 * data->field_0x18) / float_360_8041ede4);
		data->field_0x14 += float_neg0p05_8041ede8;
		t = float_0p2_8041edec * s;
		data->field_0x10 += t;
		switch (type) {
		case 0:
		case 2:
		case 3:
		case 4:
			data->field_0x10 *= float_0p94_8041edf0;
#ifdef MUST_MATCH
			sn = (f32)sin(float_6p2832_8041eddc * data->field_0x18 / float_360_8041ede4);
			data->field_0x14 *= float_0p05_8041edf8 * sn + float_0p95_8041edf4;
#else
			data->field_0x14 *= float_0p05_8041edf8 * (f32)sin(float_6p2832_8041eddc * data->field_0x18 / float_360_8041ede4) + float_0p95_8041edf4;
#endif
			break;
		case 1:
			data->field_0x14 += float_neg0p05_8041ede8;
			data->field_0x10 *= float_0p92_8041edfc;
			break;
		}
		data->field_0x1C += (f32)(s32)(effTblRandN64(50, idx) - 25);
		data->field_0x18 += float_10_8041ee00 * (f32)sin(float_6p2832_8041eddc * data->field_0x1C / float_360_8041ede4);
		t = (f32)cos(float_6p2832_8041eddc * (data->field_0x1C * float_0p5_8041ee04) / float_360_8041ede4);
		data->field_0x20 += float_10_8041ee00 * t;
		data->x += data->field_0x10;
		data->y += data->field_0x14;
	}
	dispEntry(4, dispCalcZ(&position2), 2, effFallLeafDisp, effect);
}
#else
void effFallLeafMain(EffectEntry* effect) {
	effFallLeafData* data = effect->userdata;
	effFallLeafParticle* particle;
	Vec position2;
	Vec position;
	s32 type;
	s32 life;
	s32 counter;
	s32 i;
	s32 idx;
	f32 s;
	f32 t;

	position = vec3_802fa538;
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	data->life--;
	data->counter++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	type = data->type;
	life = data->life;
	if (counter < 10) {
		data->tevReg0A = (s32)(float_0p3_8041edd4 * (f32)(0xFF - data->tevReg0A) + (f32)data->tevReg0A);
	}
	if (life < 10) {
		data->tevReg0A = (s32)((f32)data->tevReg0A * float_0p8_8041edd8);
	}
	idx = counter + 20;
	i = 1;
	particle = &data->particle[0];
	for (; i < effect->count; i++, particle++, idx += 20) {
		s = (f32)sin(float_6p2832_8041eddc * (float_2_8041ede0 * particle->rotZ) / float_360_8041ede4);
		particle->velY += float_neg0p05_8041ede8;
		t = float_0p2_8041edec * s;
		particle->velX += t;
		switch (type) {
		case 0:
		case 2:
		case 3:
		case 4:
			particle->velX *= float_0p94_8041edf0;
			particle->velY *= float_0p05_8041edf8 * (f32)sin(float_6p2832_8041eddc * particle->rotZ / float_360_8041ede4) + float_0p95_8041edf4;
			break;
		case 1:
			particle->velY += float_neg0p05_8041ede8;
			particle->velX *= float_0p92_8041edfc;
			break;
		}
		particle->swayAngle += (f32)(s32)(effTblRandN64(50, idx) - 25);
		particle->rotZ += float_10_8041ee00 * (f32)sin(float_6p2832_8041eddc * particle->swayAngle / float_360_8041ede4);
		particle->rotY += float_10_8041ee00 * (f32)cos(float_6p2832_8041eddc * (particle->swayAngle * float_0p5_8041ee04) / float_360_8041ede4);
		particle->x += particle->velX;
		particle->y += particle->velY;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effFallLeafDisp, effect);
}
#endif

void effFallLeafDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effFallLeafParticle* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	s32 type;
	s32 i;
	f32 deg2rad;

	cam = camGetPtr(cameraId);
	data = effect->userdata;
	type = data->type;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041edc0 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, work);
	PSMTXConcat(cam->view, work, work);

	switch (type) {
	case 0:
	case 1:
		leafSetColor0(((effFallLeafData*)data)->tevReg0A);
		break;
	case 2:
		leafSetColor1(((effFallLeafData*)data)->tevReg0A);
		break;
	case 3:
		leafSetColor2(((effFallLeafData*)data)->tevReg0A);
		break;
	case 4:
		leafSetColor3(((effFallLeafData*)data)->tevReg0A);
		break;
	}

	data++;
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	switch (type) {
	case 0:
	case 1:
		PSMTXScale(scale, float_0p03125_8041edc4, float_0p0078125_8041edc8, float_1_8041edcc);
		GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
		effGetTexObjN64(0x14, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		break;
	default:
		PSMTXScale(scale, float_0p03125_8041edc4, float_0p015625_8041edd0, float_1_8041edcc);
		GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
		effGetTexObj(0x87, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		break;
	}
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)size12x12_tex64x64_vtx);

	deg2rad = float_deg2rad_8041edc0;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXRotRad(rot, 'z', deg2rad * data->rotZ);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'y', deg2rad * data->rotY);
		PSMTXConcat(trans, rot, trans);
		PSMTXConcat(work, trans, trans);
		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xc);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		tri2(6, 5, 4, 4, 7, 6, 4, 4);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const u32 dat_8041edb0 = 0x14641400;
const u32 dat_8041edb4 = 0x32913C00;
const u32 dat_8041edb8 = 0xFFFFFF00;
const u32 str_8041EDBC = 0x7592A600;
const f32 float_deg2rad_8041edc0 = 0.017453292f;
const f32 float_0p03125_8041edc4 = 0.03125f;
const f32 float_0p0078125_8041edc8 = 0.0078125f;
const f32 float_1_8041edcc = 1.0f;
const f32 float_0p015625_8041edd0 = 0.015625f;
const f32 float_0p3_8041edd4 = 0.3f;
const f32 float_0p8_8041edd8 = 0.8f;
const f32 float_6p2832_8041eddc = 6.2831855f;
const f32 float_2_8041ede0 = 2.0f;
const f32 float_360_8041ede4 = 360.0f;
const f32 float_neg0p05_8041ede8 = -0.05f;
const f32 float_0p2_8041edec = 0.2f;
const f32 float_0p94_8041edf0 = 0.94f;
const f32 float_0p95_8041edf4 = 0.95f;
const f32 float_0p05_8041edf8 = 0.05f;
const f32 float_0p92_8041edfc = 0.92f;
const f32 float_10_8041ee00 = 10.0f;
const f32 float_0p5_8041ee04 = 0.5f;
const f32 float_0_8041ee08 = 0.0f;

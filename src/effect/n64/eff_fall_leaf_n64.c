#include "eff/n64/eff_fall_leaf_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include <stdlib.h>
#include "memory.h"

// userdata is effect->count of these records: [0] is the header (type, position, fade and life),
// [1..] are the leaves (position, velocity and rotation).
typedef struct effFallLeafData {
	s32 type;        // 0x00 header only
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 velX;        // 0x10 leaves only
	f32 velY;        // 0x14 leaves only
	f32 rotZ;        // 0x18 leaves only: degrees, the disp's 'z' rotation
	f32 swayAngle;   // 0x1C leaves only: degrees; drives the sway in rotZ and rotY
	f32 rotY;        // 0x20 leaves only: degrees, the disp's 'y' rotation
	s32 tevReg0A;    // 0x24 header only: TEVREG0 alpha, fades up over the first 10 frames and down over the last 10
	s32 life;        // 0x28 header only
	s32 counter;     // 0x2C header only
} effFallLeafData;

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
	effFallLeafData* particle;
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
	particle = data + 1;
	for (i = 1; i < 5; i++, particle++) {
		switch (type) {
		case 0:
		case 2:
		case 3:
		case 4:
			particle->x = (f32)(rand() % 50 - 25);
			particle->y = (f32)(rand() % 50 - 25);
			particle->z = 0.0f;
			particle->rotZ = (f32)(rand() % 360);
			particle->rotY = (f32)(rand() % 360);
			particle->swayAngle = (f32)(rand() % 100) / float_10_8041ee00;
			particle->velX = 0.0f;
			particle->velY = 0.0f;
			break;
		case 1:
			particle->x = (f32)(rand() % 10 - 5);
			particle->y = (f32)(rand() % 10 + 5);
			particle->z = float_10_8041ee00;
			particle->rotZ = (f32)(rand() % 360);
			particle->rotY = (f32)(rand() % 360);
			particle->swayAngle = (f32)(rand() % 100) / float_10_8041ee00;
			particle->velX = 0.0f;
			particle->velY = float_2_8041ede0;
			break;
		}
	}
	return effect;
}

// The `cos` result is stored in `t` before the `+=` (rather than writing the cos directly in the
// += statement): that makes the compiler pool the cos-argument constants after the statement's
// 10.0, reproducing retail's FPR assignment (f24=0.5, f31=10.0) and the loop-setup load order.
void effFallLeafMain(EffectEntry* effect) {
	effFallLeafData* data = effect->userdata;
	Vec position2;
	Vec position;
	s32 type;
	s32 life;
	s32 counter;
	s32 idx;
	s32 i;
	f32 s;
	f32 t;

	position = (Vec){ data->x, data->y, data->z };
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
	data++;
	for (; i < effect->count; idx += 20, i++, data++) {
		s = (f32)sin(float_6p2832_8041eddc * (float_2_8041ede0 * data->rotZ) / float_360_8041ede4);
		data->velY += float_neg0p05_8041ede8;
		t = float_0p2_8041edec * s;
		data->velX += t;
		switch (type) {
		case 0:
		case 2:
		case 3:
		case 4:
			data->velX *= float_0p94_8041edf0;
			t = (f32)sin(float_6p2832_8041eddc * data->rotZ / float_360_8041ede4);
			data->velY *= float_0p05_8041edf8 * t + float_0p95_8041edf4;
			break;
		case 1:
			data->velY += float_neg0p05_8041ede8;
			data->velX *= float_0p92_8041edfc;
			break;
		}
		data->swayAngle += (f32)(s32)(effTblRandN64(50, idx) - 25);
		data->rotZ += float_10_8041ee00 * (f32)sin(float_6p2832_8041eddc * data->swayAngle / float_360_8041ede4);
		t = (f32)cos(float_6p2832_8041eddc * (data->swayAngle * float_0p5_8041ee04) / float_360_8041ede4);
		data->rotY += float_10_8041ee00 * t;
		data->x += data->velX;
		data->y += data->velY;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effFallLeafDisp, effect);
}

void effFallLeafDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effFallLeafData* data;
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
		GXSetTevColor(GX_TEVREG0, (GXColor){ 0x14, 0x64, 0x14, data->tevReg0A });
		break;
	case 2:
		GXSetTevColor(GX_TEVREG0, (GXColor){ 0x32, 0x91, 0x3C, data->tevReg0A });
		break;
	case 3:
		GXSetTevColor(GX_TEVREG0, (GXColor){ 0xFF, 0xFF, 0xFF, data->tevReg0A });
		break;
	case 4:
		GXSetTevColor(GX_TEVREG0, (GXColor){ 0x75, 0x92, 0xA6, data->tevReg0A });
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

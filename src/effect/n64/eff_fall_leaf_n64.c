#include "eff/n64/eff_fall_leaf_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>
#include <stdlib.h>
#include "memory.h"

typedef struct effFallLeafParticle {
	s32 field_0x00;       // 0x00
	f32 x;                // 0x04
	f32 y;                // 0x08
	f32 z;                // 0x0C
	f32 field_0x10;       // 0x10
	f32 field_0x14;       // 0x14
	f32 field_0x18;       // 0x18
	f32 field_0x1C;       // 0x1C
	f32 field_0x20;       // 0x20
	u8 field_0x24[0x0C];  // 0x24
} effFallLeafParticle;

typedef struct effFallLeafData {
	s32 type;                        // 0x00
	f32 x;                           // 0x04
	f32 y;                           // 0x08
	f32 z;                           // 0x0C
	u8 field_0x10[0x14];             // 0x10
	s32 field_0x24;                  // 0x24
	s32 life;                        // 0x28
	s32 counter;                     // 0x2C
	effFallLeafParticle particle[4]; // 0x30
} effFallLeafData;

// Header-plus-particle view of the same 0x30-byte record effFallLeafMain walks with `data++`.
// effFallLeafData names the array form (effFallLeafN64Entry), this names the scalar fields the
// main loop steps through; both describe the same layout.
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

extern const char str_802FA550[];

const Vec vec3_802fa538 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fa548 = 4503601774854144.0;

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
	effect->type = str_802FA550;
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
	data->field_0x24 = 0;
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
			particle->field_0x18 = (f32)(rand() % 360);
			particle->field_0x20 = (f32)(rand() % 360);
			particle->field_0x1C = (f32)(rand() % 100) / float_10_8041ee00;
			particle->field_0x10 = float_0_8041ee08;
			particle->field_0x14 = float_0_8041ee08;
			break;
		case 1:
			particle->x = (f32)(rand() % 10 - 5);
			particle->y = (f32)(rand() % 10 + 5);
			particle->z = float_10_8041ee00;
			particle->field_0x18 = (f32)(rand() % 360);
			particle->field_0x20 = (f32)(rand() % 360);
			particle->field_0x1C = (f32)(rand() % 100) / float_10_8041ee00;
			particle->field_0x10 = float_0_8041ee08;
			particle->field_0x14 = float_2_8041ede0;
			break;
		}
	}
	return effect;
}

// Every instruction is byte-identical except the order of the last two loop-setup float loads:
// retail loads f31 (10.0) then f24 (0.5), this compiler emits the 0.5 load one slot earlier.
// Retail's allocator hands the 8 loop constants f25..f31 and wraps the last (0.5) into f24;
// MWCC 2.6 instead hands them f24..f31, so without help every constant operand is one register
// low. `half` (MUST_MATCH) pins 0.5 to f24; its definition has to sit before the `field_0x18` sin
// call so the value stays live and is kept in a callee-saved FPR, which is also what schedules it
// ahead of the 10.0 load.
// NONMATCHING (99.96%): the f24 (0.5) and f31 (10.0) loop-setup loads are swapped.
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
	// MUST_MATCH: `half` pins 0.5 to f24 and `sn` pins the case-branch blend to the target's
	// scratch registers; both only exist to reproduce the original allocation (see above).
	f32 half;
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
#ifdef MUST_MATCH
		half = float_0p5_8041ee04;
#endif
		data->field_0x18 += float_10_8041ee00 * (f32)sin(float_6p2832_8041eddc * data->field_0x1C / float_360_8041ede4);
#ifdef MUST_MATCH
		data->field_0x20 += float_10_8041ee00 * (f32)cos(float_6p2832_8041eddc * (data->field_0x1C * half) / float_360_8041ede4);
#else
		data->field_0x20 += float_10_8041ee00 * (f32)cos(float_6p2832_8041eddc * (data->field_0x1C * float_0p5_8041ee04) / float_360_8041ede4);
#endif
		data->x += data->field_0x10;
		data->y += data->field_0x14;
	}
	dispEntry(4, dispCalcZ(&position2), 2, effFallLeafDisp, effect);
}

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
	type = data->field_0x00;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041edc0 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, work);
	PSMTXConcat(cam->view, work, work);

	switch (type) {
	case 0:
	case 1:
		leafSetColor0(((effFallLeafData*)data)->field_0x24);
		break;
	case 2:
		leafSetColor1(((effFallLeafData*)data)->field_0x24);
		break;
	case 3:
		leafSetColor2(((effFallLeafData*)data)->field_0x24);
		break;
	case 4:
		leafSetColor3(((effFallLeafData*)data)->field_0x24);
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
		PSMTXRotRad(rot, 'z', deg2rad * data->field_0x18);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'y', deg2rad * data->field_0x20);
		PSMTXConcat(trans, rot, trans);
		PSMTXConcat(work, trans, trans);
		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xc);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		tri2(6, 5, 4, 4, 7, 6, 4, 4);
	}
}

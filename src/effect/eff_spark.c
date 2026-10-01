#include "eff/eff_spark.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

// The original has these constants in .sdata2. Declared const, MWCC folds their values into its own pool;
// the matching build keeps them writable so the loads reference the original symbols.
#ifdef MUST_MATCH
#define SDATA2_CONST
#else
#define SDATA2_CONST const
#endif
SDATA2_CONST u32 dat_8041e270 = 0xFFFFFF00;
SDATA2_CONST f32 float_deg2rad_8041e274 = 0.017453292f;
SDATA2_CONST f32 float_16_8041e278 = 16.0f;
SDATA2_CONST f32 float_0_8041e27c = 0.0f;
SDATA2_CONST f32 float_0p5_8041e280 = 0.5f;
SDATA2_CONST f32 float_8_8041e284 = 8.0f;
SDATA2_CONST f32 float_1_8041e288 = 1.0f;
SDATA2_CONST f32 float_0p1_8041e28c = 0.1f;
SDATA2_CONST f32 float_0p2_8041e290 = 0.2f;
SDATA2_CONST f32 float_360_8041e294 = 360.0f;
SDATA2_CONST f32 float_6p2832_8041e298 = 6.2831855f;
SDATA2_CONST char str_8041E29C[8] = "Spark";
SDATA2_CONST f32 float_0p9_8041e2a4 = 0.9f;
SDATA2_CONST f32 float_neg1_8041e2a8 = -1.0f;

const Vec vec3_802f8e20 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802f8e30 = 4503601774854144.0;

// .data
static f32 scale_data[24] = {
	0.9f,  1.0f,  1.0f,  0.9f,  0.85f, 0.8f,  0.75f, 0.7f,
	0.65f, 0.6f,  0.55f, 0.5f,  0.45f, 0.4f,  0.35f, 0.3f,
	0.25f, 0.2f,  0.15f, 0.1f,  0.05f, 0.03f, 0.02f, 0.01f,
};

// .sdata
static s32 max_seq_num[2] = { 0x18, 0xc };
static u8 a_data[24] = {
	0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
static f32 scale_data2[12] = {
	0.9f, 1.0f, 1.0f, 0.9f, 0.8f, 0.7f,
	0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f,
};
static u8 a_data2[12] = {
	0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

// Match-only: returning the colour from an inline lays the caller's local and the by-value
// argument copy out at 0xc/0x8, as the original does (a plain local lands the pair the other way).
#ifdef MUST_MATCH
static inline GXColor sparkColor(s32 a) {
	GXColor c = *(GXColor*)&dat_8041e270;
	c.a = a;
	return c;
}
#endif

// Reads of the named .sdata2 vertex floats go through volatile pointers so MWCC reloads float_0/float_1
// per vertex like the original; the color build goes through the sparkColor inline for the 0xc/0x8 pair.
// NONMATCHING (93.99%): the -size*0.5 product lands in f3 vs the original's f2, shifting the whole
// vertex register schedule, and the i=1/particle++ order at the loop head is swapped.
void effSparkDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effSparkData* data = effect->userdata;
	effSparkData* particle;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx mtxParticle;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxGlobal;
	s32 i;
	s32 alpha;
	f32 size;

	cam = camGetPtr(camId);
	alpha = data->alpha;

	effGetTexObj(0x1c, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, 0x7d);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
	PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
	PSMTXRotRad(mtxRot, 'y', float_deg2rad_8041e274 * -camGetPtr(camId)->viewYaw);
	PSMTXConcat(mtxTrans, mtxRot, mtxGlobal);
	PSMTXConcat(mtxGlobal, mtxScale, mtxGlobal);

	size = float_16_8041e278;
	particle = data + 1;
	for (i = 1; i < effect->count; i++, particle++) {
		if (particle->seq < 0) {
			continue;
		}
		PSMTXRotRad(mtxRot, 'z', float_deg2rad_8041e274 * particle->angle);
		PSMTXScale(mtxScale, particle->scale, particle->scale, particle->scale);
		PSMTXTrans(mtxTrans, particle->position.x, particle->position.y, float_0_8041e27c);
		PSMTXConcat(mtxTrans, mtxRot, mtxParticle);
		PSMTXConcat(mtxParticle, mtxScale, mtxParticle);
		PSMTXConcat(mtxGlobal, mtxParticle, mtxParticle);
		PSMTXConcat(cam->view, mtxParticle, mtxParticle);
		GXLoadPosMtxImm(mtxParticle, 0);
		GXSetCurrentMtx(0);

#ifdef MUST_MATCH
		color = sparkColor((alpha * particle->alpha) >> 8);
#else
		color = *(GXColor*)&dat_8041e270;
		color.a = (alpha * particle->alpha) >> 8;
#endif
		GXSetChanMatColor(GX_COLOR0A0, color);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
#ifdef MUST_MATCH
		// Match-only: volatile reads make MWCC reload these per vertex like the original, instead of
		// CSE-ing the writable globals into f0/f3 for the whole quad.
#define VF0 (*(volatile f32*)&float_0_8041e27c)
#define VF1 (*(volatile f32*)&float_1_8041e288)
#else
#define VF0 (float_0_8041e27c)
#define VF1 (float_1_8041e288)
#endif
		GXPosition3f32(-size * float_0p5_8041e280, float_8_8041e284, VF0);
		GXTexCoord2f32(VF0, VF0);
		GXPosition3f32(float_8_8041e284, float_8_8041e284, VF0);
		GXTexCoord2f32(VF1, VF0);
		GXPosition3f32(float_8_8041e284, -size * float_0p5_8041e280, VF0);
		GXTexCoord2f32(VF1, VF1);
		GXPosition3f32(-size * float_0p5_8041e280, -size * float_0p5_8041e280, VF0);
		GXTexCoord2f32(VF0, VF1);
#undef VF0
#undef VF1
	}
}

// The declaration order is tuned so MWCC gives data r28 (matching the original) and the float temps
// f29/f28 take the original's roles.
// NONMATCHING (95.77%): type lands in r29 / parity in r26 (original reversed), the parity mask is
// scheduled before the loop's type*4, and the int->float magic pools anonymously.
void effSparkMain(EffectEntry* effect) {
	Vec disp;
	Vec position;
	s32 type;
	effSparkData* data = effect->userdata;
	s32 i;
	s32 parity;
	s32 counter;
	s32 mode;
	u32 alpha;
	f32 f31;
	f32 f30;
	f32 f29;
	f32 f28;

	position = vec3_802f8e20;
	position.x = data->position.x;
	position.y = data->position.y;
	position.z = data->position.z;
	disp = position;

	type = data->type;
	alpha = data->alpha & 0xff;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 0x10;
	}
	if (data->life < 1000) {
		data->life--;
	}
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	if (data->life < 0x10) {
		data->alpha = data->life << 4;
	}
	data->counter++;
	counter = data->counter;
	data = data + 1;

	switch (type) {
	case 0:
		f31 = float_0p1_8041e28c;
		f30 = float_0p2_8041e290;
		break;
	default:
		f31 = float_0p2_8041e290;
		f30 = f31;
		break;
	}
	parity = counter & 1;

	for (i = 1; i < effect->count; i++, data++) {
		data->seq++;
		if (data->seq >= max_seq_num[type]) {
			data->seq = 0;
		}
		if (data->seq < 0) {
			continue;
		}
		if (data->seq == 0) {
			f28 = float_360_8041e294 * (f32)i / (f32)(effect->count - 1);
			f29 = float_6p2832_8041e298 * f28 / float_360_8041e294;
			data->position.x = float_16_8041e278 * (f32)sin(f29);
			data->position.y = float_16_8041e278 * (f32)cos(f29);
			data->vx = data->position.x * f31;
			data->vy = data->position.y * f31;
			data->targetVx = float_0_8041e27c;
			data->targetVy = float_0_8041e27c;
			data->angle = -f28;
			data->scale = float_1_8041e288;
		}
		switch (type) {
		case 0:
			data->alpha = a_data[data->seq];
			data->scale = scale_data[data->seq];
			break;
		default:
			data->alpha = a_data2[data->seq];
			data->scale = scale_data2[data->seq];
			break;
		}
		if (parity == (i & 1)) {
			data->position.x += data->vx;
			data->position.y += data->vy;
		} else {
			data->position.x += float_0p2_8041e290 * data->vx;
			data->position.y += float_0p2_8041e290 * data->vy;
		}
		data->vx += f30 * (data->targetVx - data->vx);
		data->vy += f30 * (data->targetVy - data->vy);
	}

	mode = 1;
	if (alpha != 0xff) {
		mode = 2;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&disp), mode, effSparkDisp, effect);
}

// Hoisting the loop-invariant -1.0 into the `neg1` local makes MWCC keep it in a register across the
// unrolled launch loop like the original; routing the 0.9 scale through a local assigned before the
// position stores and stored after them recovers a little more.
// NONMATCHING (91.94%): FP register allocation only -- 0.5 lands in f1 (original f0), neg1 in f5
// (original f2), the conversion magic in f4 (original f5); the 2^52 double pools as @218 not the symbol.
EffectEntry* effSparkEntry(s32 type, s32 a2, f32 x, f32 y, f32 z, f32 a6) {
	EffectEntry* entry;
	effSparkData* data;
	s32 i;
	f32 scale;
	f32 neg1;
	f32 half;
#ifdef MUST_MATCH
	// Match-only: split into two statements so MWCC loads the 0.5 constant into f0 and the converted
	// max_seq_num into f1; a single expression makes it swap the two (same shape as eff_akari_pose_n64).
	f32 maxF;
#endif
	effSparkData* p;

	entry = effEntry();
	entry->type = str_8041E29C;
	entry->count = 6;
	data = __memAlloc(HEAP_EFFECT, 0x150);
	entry->userdata = data;
	entry->callback = effSparkMain;
	entry->flags |= 2;

	data->type = type;
	data->counter = 0;
	data->life = 1000;
	data->alpha = 0xff;
	scale = float_0p9_8041e2a4;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;

	neg1 = float_neg1_8041e2a8;
#ifdef MUST_MATCH
	maxF = (f32)max_seq_num[type];
	half = maxF * float_0p5_8041e280;
#else
	half = (f32)max_seq_num[type] * float_0p5_8041e280;
#endif
	p = data + 1;
	for (i = 1; i < 6; i++, p++) {
		p->seq = (s32)(neg1 - half * (f32)(i & 1));
	}

	return entry;
}

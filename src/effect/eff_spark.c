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

// NONMATCHING (92.09%): logic checked against the asm in review. The original reloads the named
// .sdata2 float constants (float_0/float_1) for each vertex while MWCC reuses the loaded registers, and a
// few GX call fronts differ in scheduling; the rest are register-number operands.
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
	s32 alpha;
	s32 i;
	f32 size;

	alpha = data->alpha;
	cam = camGetPtr(camId);

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

		color = *(GXColor*)&dat_8041e270;
		color.a = (alpha * particle->alpha) >> 8;
		GXSetChanMatColor(GX_COLOR0A0, color);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-size * float_0p5_8041e280, float_8_8041e284, float_0_8041e27c);
		GXTexCoord2f32(float_0_8041e27c, float_0_8041e27c);
		GXPosition3f32(float_8_8041e284, float_8_8041e284, float_0_8041e27c);
		GXTexCoord2f32(float_1_8041e288, float_0_8041e27c);
		GXPosition3f32(float_8_8041e284, -size * float_0p5_8041e280, float_0_8041e27c);
		GXTexCoord2f32(float_1_8041e288, float_1_8041e288);
		GXPosition3f32(-size * float_0p5_8041e280, -size * float_0p5_8041e280, float_0_8041e27c);
		GXTexCoord2f32(float_0_8041e27c, float_1_8041e288);
	}
}

// NONMATCHING (94.12%): type selections written as `switch (type) { case 0: …; default: …; }` give the
// original's `beq`+`b` layout; remaining diffs are register operands (data/alpha/type r29/r28/r27 vs
// r28/r25/r26) and the parity mask being hoisted ahead of the loop's type*4.
void effSparkMain(EffectEntry* effect) {
	effSparkData* data = effect->userdata;
	Vec position;
	Vec disp;
	s32 type;
	s32 parity;
	s32 counter;
	s32 mode;
	s32 i;
	u32 alpha;
	f32 f31;
	f32 f30;
	f32 f28;
	f32 f29;

	position = vec3_802f8e20;
	position.x = data->position.x;
	position.y = data->position.y;
	position.z = data->position.z;
	disp = position;

	alpha = data->alpha & 0xff;
	type = data->type;
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

// NONMATCHING (69.50%): `data->scale` assigned before the position stores makes MWCC hoist the float_0p9 load,
// and a `data + 1` pointer makes it unroll the launch loop like the original. Diffs: half stays in f4 (target
// f6), float_neg1 is reloaded per iteration, and the 2^52 magic is pooled as @215, not double_to_int_802f8e30.
EffectEntry* effSparkEntry(s32 type, s32 a2, f32 x, f32 y, f32 z, f32 a6) {
	EffectEntry* entry;
	effSparkData* data;
	s32 i;
	f32 half;
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
	data->scale = float_0p9_8041e2a4;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;

	half = (f32)max_seq_num[type] * float_0p5_8041e280;
	p = data + 1;
	for (i = 1; i < 6; i++, p++) {
		p->seq = (s32)(float_neg1_8041e2a8 - half * (f32)(i & 1));
	}

	return entry;
}

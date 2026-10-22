#include "eff/n64/eff_akari_charge_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "math_ext.h"
#include "system.h"

#include <stdlib.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

u32 size2x8_tex8x16_vtx[14] = {
    0xFFFCFFFC, 0x00000000, 0x0000FFFF, 0xFFFF0004,
    0xFFFC0000, 0x02000000, 0x000000FF, 0x00040014,
    0x00000200, 0x04000000, 0x00FFFFFC, 0x00140000,
    0x00000400, 0xFF0000FF,
};

// .rodata double used for the integer-to-float conversion.

// .rodata zero vector and the double constants the inlined sqrtf pools after it.
extern const f64 double_0p5_802fa190;
extern const f64 double_3_802fa198;
extern const f64 double_0_802fa1a0;

int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 __fpclassifyf(f32 x) {
	switch (*(s32*)&x & 0x7F800000) {
		case 0x7F800000:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_NAN;
			} else {
				return FP_INFINITE;
			}
			break;
		case 0:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_SUBNORMAL;
			} else {
				return FP_ZERO;
			}
			break;
		default:
			return FP_NORMAL;
	}
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

// Inlined MSL sqrtf; the original pools _half/_three/0.0 after vec3_802fa178.
static inline f32 sqrtf(f32 x) {
	const f64 _half = .5;
	const f64 _three = 3.0;

	if (x > 0.0f) {
		f64 guess = __frsqrte(x);
		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		return x * guess;
	} else if (x < 0.0) {
		return NAN;
	} else if (isnan(x)) {
		return NAN;
	}
	return x;
}

// .data vertex array, local to this file.

EffectEntry* effAkariChargeN64Entry(s32 type, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6) {
	EffectEntry* entry;
	EffAkariChargeWork* work;
	EffAkariChargeParticle* p;
	s32 n;

	entry = effEntry();
	entry->type = "AkariChargeN64";
	entry->count = 0x16;
	work = __memAlloc(HEAP_EFFECT, 0x5D8);
	entry->userdata = work;
	entry->callback = effAkariChargeMain;
	entry->flags |= 2;

	work->type = type;
	work->counter = 0;
	if (a2 <= 0) {
		work->life = 1000;
	} else {
		work->life = a2;
	}
	work->alpha = 0;
	work->x = a3;
	work->y = a4;
	work->z = a5;
	work->scale = a6;
	work->reg0R = 0xFF;
	work->reg0G = 0xE6;
	work->reg0B = 0;
	work->reg1R = 0xFF;
	work->reg1G = 0xFF;
	work->reg1B = 0xFF;

	n = 1;
	p = work->particles;
	while (n < 22) {
		p->life = 20;
		p->counter = 0;
		p->angle = (f32)((n - 1) * 360 / 21);
		p->x = 0.0f;
		p->y = 0.0f;
		p->scale = 1.0f;
		p->index = n & 0xF;
		n++;
		p++;
	}

	return entry;
}

// The work header and the first particle are accessed through one EffAkariChargeParticle* and advanced by
// p++ into the particle array: a separate `p = work->particles` makes mwcc colour the header pointer r5 and
// the loop cursor r29. The two fcmpu instructions compare against a local copy of the zero constant so the
// register (dx/dy) operand comes first.
void effAkariChargeMain(EffectEntry* effect) {
	/* Retail builds the Vec in a scratch slot and copies it into a second local before dispCalcZ; a
	   single local folds the copy away (same idiom as effAkariSflushMain/effNokotarouMain). */
	Vec position;
	Vec disp = { 0 };
	EffAkariChargeParticle* p;
	s32 i;
	s32 type;
	s32 life;
	s32 counter;
	f32 x;
	f32 y;
	f32 dx;
	f32 dy;
	f32 s;
	f32 zero;

	p = effect->userdata;

	disp.x = p->x;
	disp.y = p->y;
	disp.z = p->z;
	position = disp;

	type = p->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		p->life = 16;
	}
	if (p->life < 1000) {
		p->life--;
	}
	p->counter++;

	if (p->life < 0) {
		effDelete(effect);
		return;
	}
	life = p->life;
	counter = p->counter;
	if (life < 16) {
		p->alpha = life << 4;
	}
	if (counter < 16) {
		p->alpha = (counter << 4) + 15;
	}

	p++;

	for (i = 1; i < effect->count; i += 3, p += 3) {
		if (p->counter >= p->life) {
			p->life = rand() % 50 + 20;
			p->counter = 0;
			p->angle = (f32)(rand() % 360);
			p->x = 0.0f;
			p->y = 0.0f;
		}
		if (rand() % 100 <= 50) {
			x = p->x;
			y = p->y;
			dx = (f32)(rand() % 17 - 8);
			dy = (f32)(rand() % 17 - 8);
			p[2] = p[1];
			p[1] = p[0];
			p->x = x + dx;
			p->y = y + dy;
			p->angle = angleABf(0.0f, 0.0f, dx, dy);
			// Comparing against a local copy of the constant makes mwcc put dx/dy first in the fcmpu, as the
			// original does; the literal is compared the other way round.
			zero = 0.0f;
			if (dx != zero || dy != zero) {
				p->scale = sqrtf(dx * dx + dy * dy);
			} else {
				p->scale = 0.0f;
			}
		}
		s = (f32)sin(6.2831855f * (f32)((p->counter * 180) / p->life) / 360.f);
		p->alpha = (s32)(1024.0f * s);
		if (p->alpha > 255) {
			p->alpha = 255;
		}
		p->counter++;
	}

	p = effect->userdata;
	p++;
	for (i = 1; i < effect->count; i++, p++) {
		p->index = rand() % 16;
	}

	if (type == 0) {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effAkariChargeDisp, effect);
	} else {
		dispEntry(CAMERA_2D, dispCalcZ(&position), 2, effAkariChargeDisp, effect);
	}
}

// Naming 0.0f in the two integer-to-float PSMTXTrans calls would make mwcc hoist it into a fourth
// fpr, which grows the frame and shifts every local, so they use 0.0f literals.
// The original reuses the work pointer as the particle cursor (`data++` here and in the for-increment, as
// effAkariPoseDisp does); because work ends in a flexible particle array, `data++` steps 0x44. A separate
// `part = data->particles` pointer makes mwcc schedule the preheader `addi r30,r30,0x44` before `li r29,1`.
void effAkariChargeDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffAkariChargeWork* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx viewMtx;
	s32 i;
	s32 alpha;
	s32 type;
	s32 counter;
	s32 colR;
	s32 colG;
	s32 colB;
	f32 proj[7];
	f32 viewport[6];
	f32 k = 0.0625f;

	cam = camGetPtr(CAMERA_3D);
	data = effect->userdata;
	type = data->type;
	counter = data->counter;
	alpha = data->alpha;
	colR = data->reg0R;
	colG = data->reg0G;
	colB = data->reg0B;
	if (type == 1) {
		GXGetProjectionv(proj);
		GXGetViewportv(viewport);
		GXSetProjection(cam->projection, cam->projType);
		GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	}

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scaleMtx, viewMtx);
	PSMTXConcat(cam->view, viewMtx, viewMtx);

	GXSetTevColor(GX_TEVREG1, (GXColor){ data->reg1R, data->reg1G, data->reg1B, 0 });
	GXSetNumChans(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
	effGetTexObjN64(0x70, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64(size2x8_tex8x16_vtx);

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		EffAkariChargeParticle* part = (EffAkariChargeParticle*)data;
		GXSetTevColor(GX_TEVREG0, (GXColor){ colR, colG, colB, (part->alpha * alpha) / 255 });

		PSMTXTrans(mtx, part->x, part->y, 0.0f);
		PSMTXRotRad(rotMtx, 'z', 0.017453292f * part->angle);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXScale(scaleMtx, part->scale * k, part->scale * k,
		           1.0f);
		PSMTXConcat(mtx, scaleMtx, mtx);
		PSMTXConcat(viewMtx, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);

		PSMTXScale(scaleMtx, 0.00390625f, 0.03125f, 0.0f);
		PSMTXTrans(mtx, (f32)(part->index << 4), 0.0f, 0.0f);
		PSMTXConcat(scaleMtx, mtx, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);

		PSMTXScale(scaleMtx, 0.00390625f, 0.03125f, 0.0f);
		PSMTXTrans(mtx, (f32)(counter * 3), 0.0f, 0.0f);
		PSMTXConcat(scaleMtx, mtx, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX1, GX_MTX2x4);

		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}

	if (type == 1) {
		GXSetProjectionv(proj);
		GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).

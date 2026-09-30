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

// .sdata2 float constants (named after their original addresses).
// float_0_8041e8ac is defined here (not just declared) so the file's 0.0f literals pool with the
// original symbol instead of an anonymous @N; objdiff compares the relocation target by name.
extern const f32 float_deg2rad_8041e8a8;
extern const f32 float_0_8041e8ac;
const f32 float_0_8041e8ac = 0.0f;
extern const f32 float_0p0625_8041e8b0;
extern const f32 float_1_8041e8b4;
extern const f32 float_0p0039062_8041e8b8;
extern const f32 float_0p03125_8041e8bc;
extern const f32 float_6p2832_8041e8c0;
extern const f32 float_360_8041e8c4;
extern const f32 float_1024_8041e8c8;

// .rodata double used for the integer-to-float conversion.
extern const f64 double_to_int_802fa188;

// .rodata zero vector and the double constants the inlined sqrtf pools after it.
extern const Vec vec3_802fa178;
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

	if (x > float_0_8041e8ac) {
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

// .sbss2 scratch GXColors the original builds its TEV colours from.
extern GXColor unk_80423090;
extern GXColor unk_80423094;

// .data vertex array, local to this file.
extern u32 size2x8_tex8x16_vtx[];

typedef struct EffAkariChargeParticle {
	s32 field_0x0;  //0x0
	f32 field_0x4;  //0x4
	f32 field_0x8;  //0x8
	f32 field_0xC;  //0xC
	s32 life;       //0x10
	s32 counter;    //0x14
	u8 pad_0x18[0xC]; //0x18
	s32 field_0x24;   //0x24
	u8 pad_0x28[0xC]; //0x28
	f32 scale;      //0x34
	f32 field_0x38; //0x38
	f32 angle;      //0x3C
	s32 index;      //0x40
} EffAkariChargeParticle; //0x44

typedef struct EffAkariChargeWork {
	s32 type;       //0x0
	f32 x;          //0x4
	f32 y;          //0x8
	f32 z;          //0xC
	s32 field_0x10; //0x10
	s32 field_0x14; //0x14
	s32 field_0x18; //0x18
	s32 field_0x1C; //0x1C
	s32 field_0x20; //0x20
	s32 field_0x24; //0x24
	s32 field_0x28; //0x28
	s32 field_0x2C; //0x2C
	s32 field_0x30; //0x30
	f32 field_0x34; //0x34
	f32 field_0x38; //0x38
	f32 field_0x3C; //0x3C
	s32 field_0x40; //0x40
	EffAkariChargeParticle particles[21]; //0x44
} EffAkariChargeWork;

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
	work->field_0x14 = 0;
	if (a2 <= 0) {
		work->field_0x10 = 1000;
	} else {
		work->field_0x10 = a2;
	}
	work->field_0x24 = 0;
	work->x = a3;
	work->y = a4;
	work->z = a5;
	work->field_0x38 = a6;
	work->field_0x18 = 0xFF;
	work->field_0x1C = 0xE6;
	work->field_0x20 = 0;
	work->field_0x28 = 0xFF;
	work->field_0x2C = 0xFF;
	work->field_0x30 = 0xFF;

	n = 1;
	p = work->particles;
	while (n < 22) {
		p->life = 20;
		p->counter = 0;
		p->angle = (f32)((n - 1) * 360 / 21);
		p->field_0x4 = 0.0f;
		p->field_0x8 = 0.0f;
		p->scale = 1.0f;
		p->index = n & 0xF;
		n++;
		p++;
	}

	return entry;
}

// The early work pointer lands in r5 instead of the original's r29, so p = work->particles is computed at
// the top of the early block rather than at loop setup; the same pointer lands in r3 instead of r25 at the
// bottom, and the two fcmpu operands are swapped. Every other instruction, GPR and FPR matches; this is the
// same mwcc allocator/coalescing difference documented for the other effect units.
// NONMATCHING (99.14%): early work pointer r5 not r29; bottom pointer r3 not r25; fcmpu operands swapped.
void effAkariChargeMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effAkariSflushMain/effNokotarouMain). */
	Vec position;
#endif
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

	{
		EffAkariChargeWork* work = effect->userdata;
		p = work->particles;

		disp.x = work->x;
		disp.y = work->y;
		disp.z = work->z;
#ifdef MUST_MATCH
		position = disp;
#endif

		type = work->type;

		if (effect->flags & 4) {
			effect->flags &= ~4;
			work->field_0x10 = 16;
		}
		if (work->field_0x10 < 1000) {
			work->field_0x10--;
		}
		work->field_0x14++;

		if (work->field_0x10 < 0) {
			effDelete(effect);
			return;
		}
		life = work->field_0x10;
		counter = work->field_0x14;
		if (life < 16) {
			work->field_0x24 = life << 4;
		}
		if (counter < 16) {
			work->field_0x24 = (counter << 4) + 15;
		}

	}

	for (i = 1; i < effect->count; i += 3, p += 3) {
		if (p->counter >= p->life) {
			p->life = rand() % 50 + 20;
			p->counter = 0;
			p->angle = (f32)(rand() % 360);
			p->field_0x4 = float_0_8041e8ac;
			p->field_0x8 = float_0_8041e8ac;
		}
		if (rand() % 100 <= 50) {
			x = p->field_0x4;
			y = p->field_0x8;
			dx = (f32)(rand() % 17 - 8);
			dy = (f32)(rand() % 17 - 8);
			p[2] = p[1];
			p[1] = p[0];
			p->field_0x4 = x + dx;
			p->field_0x8 = y + dy;
			p->angle = angleABf(float_0_8041e8ac, float_0_8041e8ac, dx, dy);
			if (dx != float_0_8041e8ac || dy != float_0_8041e8ac) {
				p->scale = sqrtf(dx * dx + dy * dy);
			} else {
				p->scale = float_0_8041e8ac;
			}
		}
		p->field_0x24 = (s32)(float_1024_8041e8c8 *
		                     (f32)sin(float_6p2832_8041e8c0 * (f32)((p->counter * 180) / p->life) /
		                              float_360_8041e8c4));
		if (p->field_0x24 > 255) {
			p->field_0x24 = 255;
		}
		p->counter++;
	}

	{
		EffAkariChargeWork* work = effect->userdata;

		p = work->particles;
	}
	for (i = 1; i < effect->count; i++, p++) {
		p->index = rand() % 16;
	}

	if (type == 0) {
#ifdef MUST_MATCH
		dispEntry(4, dispCalcZ(&position), 2, effAkariChargeDisp, effect);
#else
		dispEntry(4, dispCalcZ(&disp), 2, effAkariChargeDisp, effect);
#endif
	} else {
#ifdef MUST_MATCH
		dispEntry(8, dispCalcZ(&position), 2, effAkariChargeDisp, effect);
#else
		dispEntry(8, dispCalcZ(&disp), 2, effAkariChargeDisp, effect);
#endif
	}
}

// The original builds each TEV colour in a local GXColor and copies it into the outgoing argument
// slot.  Returning the colour from an inline makes mwcc lay those two slots out the same way it did
// in the original; building them inline in the caller puts the local/outgoing pair the wrong way
// round (the same quirk eff_akari_pose_n64.c documents).
#ifdef MUST_MATCH
static inline GXColor akariColorProj(GXColor c, s32 r, s32 g, s32 b) {
	c.r = r;
	c.g = g;
	c.b = b;
	return c;
}

static inline GXColor akariColorPart(s32 r, s32 g, s32 b, s32 a) {
	GXColor c = unk_80423094;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}
#endif

// The two integer-to-float PSMTXTrans calls use 0.0f literals; naming float_0_8041e8ac there makes mwcc hoist
// it into a fourth fpr, which grows the frame and shifts every local. Defining the constant in this unit (the
// original's .sdata2 does) instead lets the literals pool with it, so objdiff sees the original symbol.
// NONMATCHING (99.17%): only the loop-setup order of `li r29,1` (i) and `addi r30,r30,0x44`
// (part = data->particles) differs, the same mwcc scheduler artifact eff_akari_pose_n64.c reports.
void effAkariChargeDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffAkariChargeParticle* part;
	EffAkariChargeWork* data;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx viewMtx;
	s32 i;
	s32 field24;
	s32 type;
	s32 field14;
	s32 colR;
	s32 colG;
	s32 colB;
	f32 proj[7];
	f32 viewport[6];

	cam = camGetPtr(CAMERA_3D);
	data = effect->userdata;
	type = data->type;
	field14 = data->field_0x14;
	field24 = data->field_0x24;
	colR = data->field_0x18;
	colG = data->field_0x1C;
	colB = data->field_0x20;
	if (type == 1) {
		GXGetProjectionv(proj);
		GXGetViewportv(viewport);
		GXSetProjection(cam->projection, cam->projType);
		GXSetZMode(0, 7, 0);
	}

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041e8a8);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXScale(scaleMtx, data->field_0x38, data->field_0x38, data->field_0x38);
	PSMTXConcat(mtx, scaleMtx, viewMtx);
	PSMTXConcat(cam->view, viewMtx, viewMtx);

#ifdef MUST_MATCH
	GXSetTevColor(2, akariColorProj(unk_80423090, data->field_0x28, data->field_0x2C, data->field_0x30));
#else
	color = unk_80423090;
	color.r = data->field_0x28;
	color.g = data->field_0x2C;
	color.b = data->field_0x30;
	GXSetTevColor(2, color);
#endif
	GXSetNumChans(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 8, 8, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 2, 4, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 7, 7, 0);
	effGetTexObjN64(0x70, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size2x8_tex8x16_vtx);

	i = 1;
	for (part = data->particles; i < effect->count; i++, part++) {
		GXColor color2;
#ifdef MUST_MATCH
		color2 = akariColorPart(colR, colG, colB, (part->field_0x24 * field24) / 255);
#else
		color2 = unk_80423094;
		color2.r = colR;
		color2.g = colG;
		color2.b = colB;
		color2.a = (part->field_0x24 * field24) / 255;
#endif
		GXSetTevColor(1, color2);

		PSMTXTrans(mtx, part->field_0x4, part->field_0x8, float_0_8041e8ac);
		PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e8a8 * part->angle);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXScale(scaleMtx, part->scale * float_0p0625_8041e8b0, part->scale * float_0p0625_8041e8b0,
		           float_1_8041e8b4);
		PSMTXConcat(mtx, scaleMtx, mtx);
		PSMTXConcat(viewMtx, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);

		PSMTXScale(scaleMtx, float_0p0039062_8041e8b8, float_0p03125_8041e8bc, float_0_8041e8ac);
		PSMTXTrans(mtx, (f32)(part->index << 4), 0.0f, 0.0f);
		PSMTXConcat(scaleMtx, mtx, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, 0x1e, 1);

		PSMTXScale(scaleMtx, float_0p0039062_8041e8b8, float_0p03125_8041e8bc, float_0_8041e8ac);
		PSMTXTrans(mtx, (f32)(field14 * 3), 0.0f, 0.0f);
		PSMTXConcat(scaleMtx, mtx, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, 0x21, 1);

		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}

	if (type == 1) {
		GXSetProjectionv(proj);
		GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
	}
}

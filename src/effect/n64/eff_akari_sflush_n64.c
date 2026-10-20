#include "eff/n64/eff_akari_sflush_n64.h"
#include "eff/eff_n64.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include "driver/dispdrv.h"
#include "matching.h"
#include "memory.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804230a0;


// .sdata2 constants owned by this unit (named after their original addresses).
extern const GXColor dat_8041e900;
extern const f32 float_deg2rad_8041e904;
extern const f32 float_6p2832_8041e908;
extern const f32 float_360_8041e90c;

// .sbss2 scratch GXColor the original builds its TEV colour from.
// .rodata integer-to-float conversion constant.

// .data vertex array, local to this unit (0x54 bytes).
static u32 size16x16_tex32x32_vtx[0x54 / 4] = {
    0x00100000, 0x00000000, 0x00000000, 0x00FF0003, 0x00040000, 0x00000000, 0x000000FF,
    0x00000008, 0x00000000, 0x00000000, 0x00FF0000, 0xFFF80000, 0x00000000, 0x000000FF,
    0xFFFDFFFC, 0x00000000, 0x0000C8C8, 0xC8FFFFF0, 0x00000000, 0x00000000, 0xFFFFFFFF,
};

EffectEntry* effAkariSflushN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffAkariSflushData* data;

	effect = effEntry();
	effect->type = "AkariSflushN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffAkariSflushData));
	effect->userdata = data;
	effect->callback = effAkariSflushMain;
	effect->flags |= 2;
	data->type = type;
	data->age = 0;
	if (count <= 0) {
		data->life = 1000;
	} else {
		data->life = count;
	}
	data->alpha = 0xFF;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->color1R = 0xFF;
	data->color1G = 0xE6;
	data->color1B = 0x32;
	data->lastRing = 0;
	data->growScale = 1.0f;
	return effect;
}

void effAkariSflushMain(EffectEntry* effect) {
	EffAkariSflushData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effGlassMain/effFlameMain). */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 life;
	s32 counter;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}

	if (data->life < 1000) {
		data->life--;
	}
	data->age++;

	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	counter = data->age;

	if (life < 8) {
		data->growScale += 0.3f * (8 - life);
	}

	if (counter < 16) {
		data->alpha = counter * 16 + 15;
	}
	if (life < 8) {
		data->alpha = life * 32 + 31;
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effAkariSflushDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effAkariSflushDisp, effect);
#endif
}

// effAkariSflushDisp: the TEV colour build is routed through two inlines, one per GXSetTevColor.  A plain
// `color = <global>; color.r = ...; GXSetTevColor(1, color);` puts mwcc's colour local at the higher
// stack slot and the by-value copy at the lower one (target: local r1+0xc, first copy r1+0x10, second
// call's copy r1+0x8).  Two helpers that each build a local and make the call put all three slots where
// the original has them (the same shape as effPikkyoloDisp).  MUST_MATCH only: a codegen shape, with the
// plain build under #else.
//
// The 1.0f/30.0f/0.0f arguments stay as literals: the original rematerialises them from the pool each
// inner iteration (pool entries 1.0f / 30.f / 0.0f); referencing
// the named externs makes mwcc hoist them into callee-saved FPRs and shifts every other constant's
// register.  The literals get anonymous .sdata2 labels (@227-@229), which objdiff scores as equal to
// the named symbols, so the function is 100%.
#ifdef MUST_MATCH
static inline void effAkariSflushColor1(s32 r, s32 g, s32 b, s32 a) {
	GXColor c = unk_804230a0;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetTevColor(1, c);
}

static inline void effAkariSflushColor2(void) {
	GXColor c = dat_8041e900;
	GXSetTevColor(2, c);
}
#endif

void effAkariSflushDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffAkariSflushData* data;
	CameraEntry* cam;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx viewMtx;
	Mtx outerMtx;
	Mtx workMtx;
	s32 age;
	s32 i;
	s32 alpha;
	s32 j;
	f32 scaleVal;
#ifdef MUST_MATCH
	/* colours are built inside the MUST_MATCH inlines above */
#else
	GXColor color;
#endif

	cam = camGetPtr(camId);
	data = effect->userdata;
	age = data->age;
	alpha = data->alpha;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041e904);
	PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(cam->view, mtx, viewMtx);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0xff, 0xff, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 0xa);
	GXSetTevAlphaIn(0, 7, 1, 5, 7);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 2, 4, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 7, 7, 0);
	GXSetNumTexGens(0);

#ifdef MUST_MATCH
	effAkariSflushColor1(data->color1R, data->color1G, data->color1B, alpha);
	effAkariSflushColor2();
#else
	color = unk_804230a0;
	color.r = data->color1R;
	color.g = data->color1G;
	color.b = data->color1B;
	color.a = alpha;
	GXSetTevColor(1, color);
	GXSetTevColor(2, dat_8041e900);
#endif

	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);

	for (i = 0; i < data->lastRing + 1; i++) {
		scaleVal = 0.1f * (f32)cos(float_6p2832_8041e908 *
		                                        (f32)(age * 8 + i * 180 / (data->lastRing + 1)) /
		                                        float_360_8041e90c) +
		           (f32)(i + 1);
		scaleVal *= data->growScale;
		PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e904 * (f32)(i * 36));
		PSMTXScale(scaleMtx, scaleVal, scaleVal, scaleVal);
		PSMTXConcat(rotMtx, scaleMtx, outerMtx);

		for (j = 0; j < 5; j++) {
			PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e904 * (f32)(90 + j * 72));
			PSMTXScale(scaleMtx, 1.0f, 1.0f, 1.0f);
			PSMTXConcat(rotMtx, scaleMtx, rotMtx);
			PSMTXTrans(mtx, 30.0f, 0.0f, 0.0f);
			PSMTXConcat(rotMtx, mtx, mtx);
			PSMTXRotRad(rotMtx, 'x', 0.0f);
			PSMTXConcat(mtx, rotMtx, workMtx);
			PSMTXConcat(viewMtx, outerMtx, mtx);
			PSMTXConcat(mtx, workMtx, mtx);
			GXLoadPosMtxImm(mtx, 0);
			GXSetCurrentMtx(0);
			GXBegin(0x90, 0, 0xc);
			tri2(0, 1, 2, 0, 1, 2, 3, 0);
			tri2(2, 3, 4, 0, 3, 4, 5, 0);
		}
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).

const GXColor dat_8041e900 = { 0xFF, 0xFF, 0xFF, 0x00 };
const f32 float_deg2rad_8041e904 = 0.017453292f;
const f32 float_6p2832_8041e908 = 6.2831855f;
const f32 float_360_8041e90c = 360.0f;

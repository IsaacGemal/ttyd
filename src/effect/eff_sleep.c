#include "eff/eff_sleep.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>

// .sdata2/.rodata constants, named to match the data objects retail uses.
extern const u32 dat_80422420;
extern const f32 float_deg2rad_80422424;
extern const f32 float_8_80422428;
extern const f32 float_6p2832_8042242c;
extern const f32 float_0p015625_80422430;
extern const f32 float_0p5_80422434;
extern const f32 float_0p125_80422440;
extern const f32 float_4_80422444;
#ifdef MUST_MATCH
// MUST_MATCH: retail reloads float_0 for every FIFO write; a plain const lets MWCC pool it into a
// register, so declare it volatile to force the same reloads (as in eff_mahorn.c).
extern const volatile f32 float_0_8042243c;
extern const f32 float_1_80422438;
#else
extern const f32 float_0_8042243c;
extern const f32 float_1_80422438;
#endif

//local prototypes
EffectEntry* effSleepEntry(s32 a1, s32 a2, f32 x, f32 y, f32 z, f32 scale, f32 rotation) {
	EffectEntry* effect;
	effSleepData* data;
	s32 smth;
	effSleepData* work;
	s32 i;
	f32 radians;
	f32 radius;

	effect = effEntry();
	effect->type = "Sleep";
	effect->count = 4;
	data = __memAlloc(HEAP_EFFECT, effect->count * sizeof(effSleepData));
	effect->userdata = data;
	effect->callback = effSleepMain;
	effect->flags |= 2;
	data->field_0x0 = a1;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;
	data->field_0x14 = 0;
	if (a2 <= 0) {
		data->field_0x18 = 1000;
	}
	else {
		data->field_0x18 = a2;
	}
	data->field_0x1C = 0;

	radians = (6.2831855f * rotation) / 360.0f;
	work = data + 1;
	for (i = 1, smth = 16; i < effect->count; i++, smth += 16, work++) {
#ifdef MUST_MATCH
		// The original computes 16*i between the cos call and the multiply; assigning radius
		// inside the expression makes the compiler schedule it that way.
		work->position.x = (radius = 16.0f * (f32)i) * (f32)cos(radians);
#else
		radius = 16.0f * (f32)i;
		work->position.x = radius * (f32)cos(radians);
#endif
		work->position.y = radius * (f32)sin(radians);
		work->position.z = 0.0f;
		work->scale = (0.2f * (f32)i) + 1.0f;
		work->field_0x14 = 0;
		work->field_0x20 = smth;
	}
	return effect;
}

void effSleepMain(EffectEntry* effect) {
	effSleepData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second local
	   before dispCalcZ; a single local folds the copy away (same idiom as effMahornMain,
	   effBombMain and effEnergyMain). */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 i;
	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x18 = 16;
	}

	if (data->field_0x18 < 1000) {
		data->field_0x18--;
	}

	if (data->field_0x18 < 32) {
		data->field_0x14 = data->field_0x18 * 8;
	}

	if (data->field_0x18 < 0) {
		effDelete(effect);
		return;
	}

	data->field_0x1C++;
	if (data->field_0x1C < 32) {
		data->field_0x14 = data->field_0x1C * 8 + 7;
	}

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->field_0x20 != 0) {
			data->field_0x20--;
		}
		else {
			data->field_0x14 += 8;
			if (data->field_0x14 > 255) {
				data->field_0x14 = 255;
			}
		}
	}

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, (DispCallback)effSleepDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, (DispCallback)effSleepDisp, effect);
#endif
}

// Sine of one 1/64-turn step; the multiply order and the u32 parameter are what retail's angle
// conversion uses.
static inline f32 effSleepSin(u32 a) {
	f32 rad = (f32)a * 6.2831855f;
	return float_0p5_80422434 * (f32)sin(rad * float_0p015625_80422430);
}

#ifdef MUST_MATCH
// MUST_MATCH: a GXColor built through an inline return lands in the scratch slot retail uses
// (same idiom as effCoinFukidashiColor).
static inline GXColor effSleepColor(u8 alpha) {
	GXColor color = *(GXColor*)&dat_80422420;
	color.a = alpha;
	return color;
}
#endif

// Materialise negEight*0.5 before the add; an inline helper adding its parameter second pins retail's
// commutative fadds operand order, which a bare expression does not.
static inline f32 effSleepVx(f32 nh, f32 s) {
	return nh + s;
}

// Remaining diffs are callee-saved allocation -- negEight f21 (retail f23), first sine s0 f23 (retail
// f21) and its fmuls/fadds, camId r25/headerAlpha r29/angle r25 (retail r26/r25/r29) -- plus four
// anonymous literal pools (6.2832, 1.0, 0.125, 4.0 load from @265-@268 instead of
// float_6p2832/float_1/float_0p125/float_4; naming them collapses MWCC's FPR scheduling to ~97%).
// NONMATCHING (99.75%): instruction stream matches; see REPORT.md.
void effSleepDisp(CameraId camId, EffectEntry* effect) {
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtxBase;
	Mtx mtxScale;
	Mtx mtxRot;
	Mtx mtxFinal;
	GXColor color;
	f32 negEight;
	f32 one;
	s32 headerAlpha;
	effSleepData* data = effect->userdata;
	s32 i;
	s32 j;
	s32 angle;

	cam = camGetPtr(camId);
	headerAlpha = data->field_0x14;
	PSMTXTrans(mtxBase, data->position.x, data->position.y, data->position.z);
	PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_80422424);
	PSMTXConcat(mtxBase, mtxRot, mtxBase);
	PSMTXConcat(mtxBase, mtxScale, mtxFinal);
	PSMTXConcat(cam->view, mtxFinal, mtxFinal);

	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3c, 0, 0x7d);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevOp(0, 0);
	effGetTexObj(0x8c, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);

	negEight = -float_8_80422428;
	one = 1.0f;

	data++;
	for (i = 1, angle = 50; i < effect->count; angle += 50, i++, data++) {
		if (data->field_0x20 != 0) {
			continue;
		}

#ifdef MUST_MATCH
		color = effSleepColor((data->field_0x14 * headerAlpha) / 255);
#else
		color = *(GXColor*)&dat_80422420;
		color.a = (data->field_0x14 * headerAlpha) / 255;
#endif
		GXSetChanMatColor(4, color);

		PSMTXTrans(mtxBase, data->position.x, data->position.y, data->position.z);
		PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtxBase, mtxScale, mtxBase);
		PSMTXConcat(mtxFinal, mtxBase, mtxBase);
		GXLoadPosMtxImm(mtxBase, 0);
		GXSetCurrentMtx(0);

		for (j = 0; j < 8; j++) {
			f32 s0 = effSleepSin(gp->retraceCount + angle + j * 10);
			f32 s1 = effSleepSin(gp->retraceCount + (angle + (j + 1) * 10));

			GXBegin(0x80, 0, 4);
			GXPosition3f32(effSleepVx(negEight * float_0p5_80422434, s1), one * (f32)(j + 1), float_0_8042243c);
			GXTexCoord2f32(float_0_8042243c, 0.125f * (f32)(7 - j));
			GXPosition3f32(4.0f + s1, one * (f32)(j + 1), float_0_8042243c);
#ifdef MUST_MATCH
			GXTexCoord2f32(*(volatile const f32*)&float_1_80422438, 0.125f * (f32)(7 - j));
#else
			GXTexCoord2f32(1.0f, 0.125f * (f32)(7 - j));
#endif
			GXPosition3f32(4.0f + s0, one * (f32)j, float_0_8042243c);
#ifdef MUST_MATCH
			GXTexCoord2f32(*(volatile const f32*)&float_1_80422438, 0.125f * (f32)(8 - j));
#else
			GXTexCoord2f32(1.0f, 0.125f * (f32)(8 - j));
#endif
			GXPosition3f32(effSleepVx(negEight * float_0p5_80422434, s0), one * (f32)j, float_0_8042243c);
			GXTexCoord2f32(float_0_8042243c, 0.125f * (f32)(8 - j));
		}
	}
}

// The .sdata2 constants the code above loads by name, defined after it.
const u32 dat_80422420 = 0xFFFFFF00;
const f32 float_deg2rad_80422424 = 0.017453292f;
const f32 float_8_80422428 = 8.0f;
const f32 float_6p2832_8042242c = 6.2831855f;
const f32 float_0p015625_80422430 = 0.015625f;
const f32 float_0p5_80422434 = 0.5f;
const f32 float_1_80422438 = 1.0f;
#ifdef MUST_MATCH
const volatile f32 float_0_8042243c = 0.0f;
#else
const f32 float_0_8042243c = 0.0f;
#endif

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

//local prototypes
EffectEntry* effSleepEntry(s32 a1, s32 duration, f32 x, f32 y, f32 z, f32 scale, f32 rotation) {
	EffectEntry* effect;
	effSleepData* data;
	s32 delay;
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
	if (duration <= 0) {
		data->field_0x18 = 1000;
	}
	else {
		data->field_0x18 = duration;
	}
	data->field_0x1C = 0;

	radians = (float_6p2832_8042242c * rotation) / 360.0f;
	work = data + 1;
	for (i = 1, delay = 16; i < effect->count; i++, delay += 16, work++) {
		// Assigning radius inside the expression schedules 16*i between the cos call and the multiply.
		work->position.x = (radius = 16.0f * (f32)i) * (f32)cos(radians);
		work->position.y = radius * (f32)sin(radians);
		work->position.z = 0.0f;
		work->scale = (0.2f * (f32)i) + 1.0f;
		work->field_0x14 = 0;
		work->field_0x20 = delay;
	}
	return effect;
}

void effSleepMain(EffectEntry* effect) {
	effSleepData* data = effect->userdata;
	// The Vec is built in `disp` and copied into a second local before dispCalcZ; a single local
	// folds the copy away (same idiom as effMahornMain, effBombMain and effEnergyMain).
	Vec position;
	Vec disp = { 0 };
	s32 i;
	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;

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

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, (DispCallback)effSleepDisp, effect);
}

// Sine of one 1/64-turn step; the multiply order and the u32 parameter are what retail's angle
// conversion uses.
static inline f32 effSleepSin(u32 angle) {
	f32 rad = float_6p2832_8042242c * (f32)angle;
	// The half-scale stays at the call site; that placement is what gives
	// effSleepDisp's negEight/half-product their f23/f22 FPRs.
	return (f32)sin(rad * float_0p015625_80422430);
}

// A GXColor built through an inline return lands in the scratch slot retail uses
// (same idiom as effCoinFukidashiColor).
static inline GXColor effSleepColor(u8 alpha) {
	GXColor color = *(GXColor*)&dat_80422420;
	color.a = alpha;
	return color;
}

// Materialise negEight*0.5 before the add; an inline helper adding its parameter second pins retail's
// commutative fadds operand order, which a bare expression does not.
static inline f32 effSleepLeftX(f32 negativeHalfWidth, f32 sine) {
	return negativeHalfWidth + sine;
}

// Declaring angle before data/i/j/headerAlpha and writing the second sine's angle as
// `gp->retraceCount + angle + (j + 1) * 10` fixes the GPR allocation (retail camId r26,
// headerAlpha r25, angle r29) and the integer add order.
// The half-scale placement in effSleepSin and the named `negativeHalfWidth` product
// fix the callee-saved FPR assignment: retail negEight f23, its half-product f22, the two sine
// results f21/f20. Only dat/deg2rad/8/6.2832/0.015625/0.5 are named; the rest (1.0, 0.0, 0.125,
// 4.0, "Sleep", 360, 16, 0.2) stay literal and the compiler pools them in exactly retail's order,
// which reproduces the whole .sdata2. Writing the 6.2832 multiply as `k * (f32)angle` (rather than
// `(f32)angle * k`) in effSleepSin is what pins the last two FPRs (mask f24, 6.2832 f25).
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
	f32 negativeHalfWidth;
	s32 angle;
	effSleepData* data = effect->userdata;
	s32 i;
	s32 j;
	s32 headerAlpha;

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

		color = effSleepColor((data->field_0x14 * headerAlpha) / 255);
		GXSetChanMatColor(4, color);

		PSMTXTrans(mtxBase, data->position.x, data->position.y, data->position.z);
		PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtxBase, mtxScale, mtxBase);
		PSMTXConcat(mtxFinal, mtxBase, mtxBase);
		GXLoadPosMtxImm(mtxBase, 0);
		GXSetCurrentMtx(0);

		for (j = 0; j < 8; j++) {
			f32 sine0 = float_0p5_80422434 * effSleepSin(gp->retraceCount + angle + j * 10);
			f32 sine1 = float_0p5_80422434 * effSleepSin(gp->retraceCount + angle + (j + 1) * 10);
			// One named half-product per iteration, assigned between the sine results
			// and GXBegin, is what makes retail's negEight=f23/half-product=f22.
			negativeHalfWidth = negEight * float_0p5_80422434;

			GXBegin(0x80, 0, 4);
			GXPosition3f32(effSleepLeftX(negativeHalfWidth, sine1), one * (f32)(j + 1), 0.0f);
			GXTexCoord2f32(0.0f, 0.125f * (f32)(7 - j));
			GXPosition3f32(4.0f + sine1, one * (f32)(j + 1), 0.0f);
			GXTexCoord2f32(1.0f, 0.125f * (f32)(7 - j));
			GXPosition3f32(4.0f + sine0, one * (f32)j, 0.0f);
			GXTexCoord2f32(1.0f, 0.125f * (f32)(8 - j));
			GXPosition3f32(effSleepLeftX(negativeHalfWidth, sine0), one * (f32)j, 0.0f);
			GXTexCoord2f32(0.0f, 0.125f * (f32)(8 - j));
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

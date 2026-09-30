#include "eff/n64/eff_magic_horao_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "eff/n64/eff_hokori_n64.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <math.h>
#include <stdlib.h>

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// .rodata
const char str_802FABA8[16] = "MagicHoraoN64";

// .sdata2 owned by this unit; the zero Vec for initializeVec and the int->float
// magic are left to the compiler's constant pool (@NNN), which objdiff folds back
// to the original vec3_802fab90 / double_to_int_802faba0 symbols.
extern const f32 float_0p02_8041f77c;
extern const f32 float_5_8041f784;
extern const f32 float_0p05_8041f788;

// The unit's int->float magic constant (2^52 + 2^31), kept so the unit owns the
// symbol in .rodata as the original does.
const f64 double_to_int_802faba0 = 4503601774854144.0;

EffectEntry* effMagicHoraoN64Entry(s32 type, s32 a1, s32 a2, s32 life, f32 x, f32 y, f32 z, f32 a4, f32 a5, f32 a6) {
	EffectEntry* effect;
	effMagicHoraoData* data;

	effect = effEntry();
	effect->type = str_802FABA8;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x48);
	effect->userdata = data;
	effect->callback = effMagicHoraoMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x28 = a6;
	data->field_0x14 = a4;
	data->field_0x1C = a1;
	data->field_0x18 = a5;
	data->field_0x20 = a2;
	data->alpha = float_1_8041f780;
	data->field_0x38 = rand() % 360;
	data->life = life;
	data->field_0x44 = 0;
	data->field_0x3C = a1;
	data->field_0x24 = a4;
	data->field_0x10 = float_0_8041f770;
	data->state = 0;
	return effect;
}

void effMagicHoraoMain(EffectEntry* effect) {
	effMagicHoraoData* data;
	Vec position;

	data = effect->userdata;
	position = initializeVec((Vec*)&data->x);
	data->life--;
	data->field_0x44++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	switch (data->state) {
	case 0:
		data->field_0x24 += data->field_0x28 * (data->field_0x18 - data->field_0x24);
		data->field_0x3C = (s32)(data->field_0x28 * (f32)(data->field_0x20 - data->field_0x3C) + (f32)data->field_0x3C);
		data->alpha -= float_0p02_8041f77c;
		if (data->alpha < 0.0f) {
			data->alpha = 0.0f;
			data->field_0x10 = 0.0f;
			data->field_0x30 = 10;
			data->state = 1;
		}
		break;
	case 1:
		data->field_0x30--;
		if (data->field_0x30 <= 0) {
			data->state = 2;
			data->field_0x30 = 10;
		}
		break;
	case 2:
		data->field_0x30--;
		if (data->field_0x30 <= 0) {
			data->state = 3;
		}
		break;
	case 3:
		data->field_0x10 -= float_1_8041f780;
		data->y += data->field_0x10;
		if (data->y < 0.0f) {
			data->y = 0.0f;
			effHokoriN64Entry(2, 30, float_5_8041f784 + data->x, data->y, data->z);
			effHokoriN64Entry(2, 30, data->x - float_5_8041f784, data->y, data->z);
			data->state = 4;
			data->field_0x30 = 20;
		}
		break;
	case 4:
		data->field_0x30--;
		if (data->field_0x30 <= 0) {
			data->state = 5;
		}
		break;
	case 5:
		if (data->life < 10) {
			data->field_0x3C = data->life * 25;
		}
		data->alpha -= float_0p05_8041f788;
		break;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMagicHoraoDisp, effect);
}

// .data vertex-layout descriptor, local to this file in the original.
static u32 size64x64_tex64x64_vtx[14] = {
    0xF3800000, 0x00000000, 0x00000000, 0x00FF0C80, 0x00000000, 0x08000000, 0x000000FF,
    0x0C801900, 0x00000800, 0x08000000, 0x00FFF380, 0x19000000, 0x00000800, 0x000000FF,
};

// 0x1C-byte generated quad vertices (same layout the sibling n64 effects use).
typedef struct MagicHoraoVtx {
	s16 x;         // 0x00
	s16 y;         // 0x02
	s16 z;         // 0x04
	s16 unk06;     // 0x06
	s16 unk08;     // 0x08
	s16 unk0A;     // 0x0A
	s16 unk0C;     // 0x0C
	s16 x2;        // 0x0E
	s16 y2;        // 0x10
	s16 z2;        // 0x12
	s16 unk14;     // 0x14
	s16 unk16;     // 0x16
	s16 unk18;     // 0x18
	s16 unk1A;     // 0x1A
} MagicHoraoVtx;

// .sdata2 / .sbss2 scratch colours the original builds its TEV colours from.
extern GXColor dat_8041f750;
extern GXColor unk_804231b8;
extern const f32 float_0p01_8041f754;
extern const f32 float_100_8041f758;
extern const f32 float_6p2832_8041f75c;
extern const f32 float_360_8041f760;
extern const f32 float_225_8041f764;
extern const f32 float_25_8041f768;
extern const f32 float_0p015625_8041f76c;
extern const f32 float_500_8041f774;
extern const f32 float_200_8041f778;

// MUST_MATCH: building colour2 in an inline that returns it keeps its by-value temporary in the
// original's stack slot (the plain field-by-field form below puts the local in the temporary's slot).
#ifdef MUST_MATCH
static inline GXColor makeHoraoColor2(u8 a) {
	GXColor c = unk_804231b8;
	c.a = a;
	return c;
}
#endif

// Register allocation is the only thing left. The local declaration order below is tuned so MWCC's
// GPR allocator reproduces the original's loop registers; moving a line changes which register each
// loop variable gets, so only reorder them together with a check. Remaining diffs: alpha is f29 where
// the target has f31 and the vertex loop's FPRs are rotated (target 2pi/magic/360/500/200 = f26-f30 and
// alpha f31; here alpha f29, 2pi/magic f30/f31, 360/500/200 f26/f27/f28); the first colour's local sits
// at 0x14 (target 0x10), so its argument copy is at 0x10 (target 0x14) and colour.a stores before the
// first fmadds; the vertex loop's i/off2/v are rotated (target i/off2/v = r26/r30/r24) and the draw
// loop keeps i/j in r23/r24 (target r24/r23); the int->float magic is pooled (@NNN) instead of the named
// double_to_int_802faba0 symbol because of the unit's extra compiler-emitted sqrt statics.
#pragma opt_lifetimes off // without it MWCC's lifetime analysis picks a worse register assignment here
// NONMATCHING (98.51%): FPR/GPR register-allocation and first-colour stack-slot differences above.
void effMagicHoraoDisp(CameraId cameraId, void* param) {
	effMagicHoraoData* data;
	Mtx mtx1;
	GXColor color2;
	MagicHoraoVtx* vp;
	GXTexObj texObj;
	f32 alpha;
	s32 j;
	s32 off2;
	s32 y;
	s32 off1;
	s32 counter;
	Mtx mtx2;
	GXColor color;
	f32 angle;
	MagicHoraoVtx* vtx;
	s32 i;
	s32 angleBase;
	EffectEntry* effect;
	s32 colorAlpha;
	CameraEntry* cam;
	s16 v;

	cam = camGetPtr(cameraId);
	effect = param;
	data = effect->userdata;
	alpha = data->alpha;
	angleBase = data->field_0x38;
	colorAlpha = data->field_0x3C;
	counter = data->field_0x44;

	PSMTXScale(mtx2, float_0p01_8041f754, float_0p01_8041f754, float_0p01_8041f754);
	PSMTXTrans(mtx1, float_100_8041f758 * data->x, float_100_8041f758 * data->y, float_100_8041f758 * data->z);
	PSMTXConcat(mtx2, mtx1, mtx1);
	PSMTXScale(mtx2, data->field_0x24, data->field_0x24, data->field_0x24);
	PSMTXConcat(mtx1, mtx2, mtx1);
	PSMTXConcat(cam->view, mtx1, mtx1);
	GXLoadPosMtxImm(mtx1, 0);
	GXSetCurrentMtx(0);

	angle = (float_6p2832_8041f75c * (f32)(counter * 30)) / float_360_8041f760;
	color = dat_8041f750;
	color.r = (s32)(float_25_8041f768 * (f32)sin(angle) + float_225_8041f764);
	color.a = colorAlpha;
	color.g = (s32)(float_25_8041f768 * (f32)sin(angle) + float_225_8041f764);
	GXSetTevColor(1, color);

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	PSMTXScale(mtx2, float_0p015625_8041f76c, float_0p015625_8041f76c, float_0_8041f770);
	GXLoadTexMtxImm(mtx2, 0x1E, 1);
	effSetVtxDescN64((effVtxArray*)size64x64_tex64x64_vtx);

	if (data->state == 1) {
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 0xFF);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
		GXSetTevAlphaIn(0, 7, 4, 1, 7);
		effGetTexObjN64(0x56, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);

#ifdef MUST_MATCH
		color2 = makeHoraoColor2(((0x1E - data->field_0x30) * 0xFF) / 0x1E);
#else
		color2 = unk_804231b8;
		color2.a = ((0x1E - data->field_0x30) * 0xFF) / 0x1E;
#endif
		GXSetTevColor(1, color2);

		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 0xFF);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
		GXSetTevAlphaIn(0, 7, 4, 1, 7);
		effGetTexObjN64(0x57, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	} else {
		if (data->state >= 1) {
			GXSetNumTevStages(1);
			GXSetTevOrder(0, 0, 0, 0xFF);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
			GXSetTevAlphaIn(0, 7, 4, 1, 7);
			effGetTexObjN64(0x57, &texObj);
			GXLoadTexObj(&texObj, 0);
		} else {
			GXSetNumTevStages(1);
			GXSetTevOrder(0, 0, 0, 0xFF);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
			GXSetTevAlphaIn(0, 7, 4, 1, 7);
			effGetTexObjN64(0x56, &texObj);
			GXLoadTexObj(&texObj, 0);
		}

		vtx = smartAlloc(0x1C0, 3)->address;
		off1 = counter * 10;
		off2 = off1;
		vp = vtx;
		y = 0;
		v = 0;
		for (i = 0; i < 16; i++) {
			s32 a;
			s32 b;

			a = (s32)(float_500_8041f774 * (f32)sin((float_6p2832_8041f75c * (f32)(angleBase + off1)) / float_360_8041f760) * alpha);
			b = (s32)(float_200_8041f778 * (f32)sin((float_6p2832_8041f75c * (f32)(angleBase + off2)) / float_360_8041f760) * alpha);
#ifdef MUST_MATCH
			vp->x = a - 0xC80;
			vp->y = b + y;
			vp->z = 0;
			vp->unk06 = 0;
			vp->unk08 = v;
			vp->x2 = a + 0xC80;
			vp->y2 = b + y;
			vp->z2 = 0;
			vp->unk14 = 0x800;
			vp->unk16 = v;
#else
			vp->x = a - 0xC80;
			vp->x2 = a + 0xC80;
			vp->y = b + y;
			vp->y2 = b + y;
			vp->z = 0;
			vp->z2 = 0;
			vp->unk06 = 0;
			vp->unk08 = v;
			vp->unk14 = 0x800;
			vp->unk16 = v;
#endif
			vp++;
			off1 += 60;
			off2 += 6;
			y += 400;
			v += 0x80;
		}
		DCFlushRange(vtx, 0x1C0);
		GXInvalidateVtxCache();
		effSetVtxDescN64((effVtxArray*)vtx);
		for (i = 0, j = 0; i < 15; i++, j += 2) {
			GXBegin(0x90, 0, 6);
			tri2(j, j + 2, j + 1, j, j + 1, j + 2, j + 3, j);
		}
	}
}
#pragma opt_lifetimes reset

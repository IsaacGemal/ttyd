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
#include <math_ext.h>
#include <stdlib.h>

// Local copy of system.c's inline helper so it inlines here as in the original.
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}


EffectEntry* effMagicHoraoN64Entry(s32 type, s32 a1, s32 a2, s32 life, f32 x, f32 y, f32 z, f32 a4, f32 a5, f32 a6) {
	EffectEntry* effect;
	effMagicHoraoData* data;

	effect = effEntry();
	effect->type = "MagicHoraoN64";
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
	data->alpha = 1.0f;
	data->field_0x38 = rand() % 360;
	data->life = life;
	data->field_0x44 = 0;
	data->field_0x3C = a1;
	data->field_0x24 = a4;
	data->field_0x10 = 0.0f;
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
		data->alpha -= 0.02f;
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
		data->field_0x10 -= 1.0f;
		data->y += data->field_0x10;
		if (data->y < 0.0f) {
			data->y = 0.0f;
			effHokoriN64Entry(2, 30, 5.0f + data->x, data->y, data->z);
			effHokoriN64Entry(2, 30, data->x - 5.0f, data->y, data->z);
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
		data->alpha -= 0.05f;
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
extern GXColor unk_804231b8;

// MUST_MATCH: building colour2 in an inline that returns it keeps its by-value temporary in the
// original's stack slot (the plain field-by-field form below puts the local in the temporary's slot).
#ifdef MUST_MATCH
static inline GXColor makeHoraoColor2(u8 a) {
	GXColor c = unk_804231b8;
	c.a = a;
	return c;
}

static inline GXColor makeHoraoColor(GXColor c, s32 counter, u8 a) {
	f32 angle = (6.2831855f * (f32)(counter * 30)) / 360.0f;
	c.r = (s32)(25.0f * (f32)sin(angle) + 225.0f);
	c.g = (s32)(25.0f * (f32)sin(angle) + 225.0f);
	c.a = a;
	return c;
}
#endif

// The FPR allocation (alpha in f31, the loop's 2pi/magic/360/500/200 in f26-f30) comes from the
// named `s` temp in the vertex loop, see the MUST_MATCH block there. The source order of the
// preheader assignments is also tuned: it decides the GPR schedule's load/increment interleave;
// only reorder together with a check. The vertex writes use the sibling n64 quad layout. The first
// colour is built by the makeHoraoColor inline above, whose base parameter (rather than the global
// read inside) is what puts its local at 0x10 and the by-value copy at 0x14.
// NONMATCHING (99.86%): everything matches except the zero-copy `v = y`, which this build folds to `li`
// where the original emits `mr` (a copy of the already-zero y register).
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
#ifndef MUST_MATCH
	f32 angle;
#endif
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

	PSMTXScale(mtx2, 0.01f, 0.01f, 0.01f);
	PSMTXTrans(mtx1, 100.0f * data->x, 100.0f * data->y, 100.0f * data->z);
	PSMTXConcat(mtx2, mtx1, mtx1);
	PSMTXScale(mtx2, data->field_0x24, data->field_0x24, data->field_0x24);
	PSMTXConcat(mtx1, mtx2, mtx1);
	PSMTXConcat(cam->view, mtx1, mtx1);
	GXLoadPosMtxImm(mtx1, 0);
	GXSetCurrentMtx(0);

#ifdef MUST_MATCH
	GXSetTevColor(1, makeHoraoColor(((GXColor){ 0x00, 0x00, 0xFF, 0x00 }), counter, colorAlpha));
#else
	angle = (6.2831855f * (f32)(counter * 30)) / 360.0f;
	color = ((GXColor){ 0x00, 0x00, 0xFF, 0x00 });
	color.r = (s32)(25.0f * (f32)sin(angle) + 225.0f);
	color.g = (s32)(25.0f * (f32)sin(angle) + 225.0f);
	color.a = colorAlpha;
	GXSetTevColor(1, color);
#endif

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	PSMTXScale(mtx2, 0.015625f, 0.015625f, 0.0f);
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
		y = 0;
		i = 0;
		vp = vtx;
		off2 = off1;
		v = y;
		for (; i < 16; i++) {
			s32 a;
			s32 b;

// MUST_MATCH: the named `s` temp (rather than reusing the sin expression inline) is what
// makes MWCC put alpha in f31 and the loop's constants in f26-f30, as the original does.
#ifdef MUST_MATCH
			f32 s;
			s = (f32)sin((6.2831855f * (f32)(angleBase + off1)) / 360.0f);
			a = (s32)(alpha * (500.0f * s));
			s = (f32)sin((6.2831855f * (f32)(angleBase + off2)) / 360.0f);
			b = (s32)(alpha * (200.0f * s));
#else
			a = (s32)(500.0f * (f32)sin((6.2831855f * (f32)(angleBase + off1)) / 360.0f) * alpha);
			b = (s32)(200.0f * (f32)sin((6.2831855f * (f32)(angleBase + off2)) / 360.0f) * alpha);
#endif
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
		for (i = 0; i < 15; i++) {
			j = i * 2;
			GXBegin(0x90, 0, 6);
			tri2(j, j + 2, j + 1, j, j + 1, j + 2, j + 3, j);
		}
	}
}

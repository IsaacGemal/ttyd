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


EffectEntry* effMagicHoraoN64Entry(s32 type, s32 colorAlpha, s32 endColorAlpha, s32 life, f32 x, f32 y, f32 z, f32 scale, f32 endScale, f32 scaleRate) {
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
	data->scaleRate = scaleRate;
	data->field_0x14 = scale;
	data->field_0x1C = colorAlpha;
	data->endScale = endScale;
	data->endColorAlpha = endColorAlpha;
	data->alpha = 1.0f;
	data->angleBase = rand() % 360;
	data->life = life;
	data->counter = 0;
	data->colorAlpha = colorAlpha;
	data->scale = scale;
	data->velY = 0.0f;
	data->state = 0;
	return effect;
}

void effMagicHoraoMain(EffectEntry* effect) {
	effMagicHoraoData* data;
	Vec position;

	data = effect->userdata;
	position = initializeVec((Vec*)&data->x);
	data->life--;
	data->counter++;
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	switch (data->state) {
	case 0:
		data->scale += data->scaleRate * (data->endScale - data->scale);
		data->colorAlpha = (s32)(data->scaleRate * (f32)(data->endColorAlpha - data->colorAlpha) + (f32)data->colorAlpha);
		data->alpha -= 0.02f;
		if (data->alpha < 0.0f) {
			data->alpha = 0.0f;
			data->velY = 0.0f;
			data->timer = 10;
			data->state = 1;
		}
		break;
	case 1:
		data->timer--;
		if (data->timer <= 0) {
			data->state = 2;
			data->timer = 10;
		}
		break;
	case 2:
		data->timer--;
		if (data->timer <= 0) {
			data->state = 3;
		}
		break;
	case 3:
		data->velY -= 1.0f;
		data->y += data->velY;
		if (data->y < 0.0f) {
			data->y = 0.0f;
			effHokoriN64Entry(2, 30, 5.0f + data->x, data->y, data->z);
			effHokoriN64Entry(2, 30, data->x - 5.0f, data->y, data->z);
			data->state = 4;
			data->timer = 20;
		}
		break;
	case 4:
		data->timer--;
		if (data->timer <= 0) {
			data->state = 5;
		}
		break;
	case 5:
		if (data->life < 10) {
			data->colorAlpha = data->life * 25;
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
	s16 x;        // 0x00
	s16 y;        // 0x02
	s16 z;        // 0x04
	s16 texU;     // 0x06
	s16 texV;     // 0x08
	u8 clr[4];    // 0x0A, RGBA8
	s16 x2;       // 0x0E
	s16 y2;       // 0x10
	s16 z2;       // 0x12
	s16 texU2;    // 0x14
	s16 texV2;    // 0x16
	u8 clr2[4];   // 0x18, RGBA8
} MagicHoraoVtx;

// .sbss2 4-byte gap after the unit's colour template.
#pragma section ".sdata" ".sbss2"
#pragma force_active on
__declspec(section ".sdata") u32 gap_10_804231BC_sbss2;
#pragma force_active reset
#pragma section ".sdata" ".sbss"

// Building the colour in an inline that returns it keeps the by-value temporaries in the
// original's stack slots (a plain field-by-field form puts the local in the temporary's slot).
static inline GXColor makeHoraoColor(GXColor c, s32 counter, u8 a) {
	f32 angle = (6.2831855f * (f32)(counter * 30)) / 360.0f;
	c.r = (s32)(25.0f * (f32)sin(angle) + 225.0f);
	c.g = (s32)(25.0f * (f32)sin(angle) + 225.0f);
	c.a = a;
	return c;
}

// The FPR allocation (alpha in f31, the loop's 2pi/magic/360/500/200 in f26-f30) comes from the
// named `s` temp in the vertex loop. The vertex loop is written
// per index (`&vtx[i]`, `i * 400`, `counter * 10 + i * k`) and the frontend strength-reduces it,
// which gives the original's preheader copies; the texture row `i * 2 * 0x40U` is unsigned, so the
// backend reduces it instead, and its zero start becomes a copy of the `i * 400` one. The first colour is built by
// the makeHoraoColor inline above, whose base parameter (rather than the global read inside) is
// what puts its local at 0x10 and the by-value copy at 0x14.
void effMagicHoraoDisp(CameraId cameraId, void* param) {
	effMagicHoraoData* data;
	Mtx mtx1;
	GXTexObj texObj;
	f32 alpha;
	s32 j;
	s32 counter;
	Mtx mtx2;
	MagicHoraoVtx* vtx;
	s32 i;
	s32 angleBase;
	EffectEntry* effect;
	s32 colorAlpha;
	CameraEntry* cam;

	cam = camGetPtr(cameraId);
	effect = param;
	data = effect->userdata;
	alpha = data->alpha;
	angleBase = data->angleBase;
	colorAlpha = data->colorAlpha;
	counter = data->counter;

	PSMTXScale(mtx2, 0.01f, 0.01f, 0.01f);
	PSMTXTrans(mtx1, 100.0f * data->x, 100.0f * data->y, 100.0f * data->z);
	PSMTXConcat(mtx2, mtx1, mtx1);
	PSMTXScale(mtx2, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx1, mtx2, mtx1);
	PSMTXConcat(cam->view, mtx1, mtx1);
	GXLoadPosMtxImm(mtx1, 0);
	GXSetCurrentMtx(0);

	GXSetTevColor(GX_TEVREG0, makeHoraoColor(((GXColor){ 0x00, 0x00, 0xFF, 0x00 }), counter, colorAlpha));

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(mtx2, 0.015625f, 0.015625f, 0.0f);
	GXLoadTexMtxImm(mtx2, GX_TEXMTX0, GX_MTX2x4);
	effSetVtxDescN64((effVtxArray*)size64x64_tex64x64_vtx);

	if (data->state == 1) {
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
		effGetTexObjN64(0x56, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXBegin(GX_TRIANGLES, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);

		GXSetTevColor(GX_TEVREG0, (GXColor){ 0x00, 0x00, 0x00, ((0x1E - data->timer) * 0xFF) / 0x1E });

		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
		effGetTexObjN64(0x57, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXBegin(GX_TRIANGLES, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	} else {
		if (data->state >= 1) {
			GXSetNumTevStages(1);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
			effGetTexObjN64(0x57, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
		} else {
			GXSetNumTevStages(1);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
			effGetTexObjN64(0x56, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
		}

		vtx = smartAlloc(0x1C0, 3)->address;
		for (i = 0; i < 16; i++) {
			MagicHoraoVtx* vp = &vtx[i];
			s32 xOffset;
			s32 yOffset;

			// The named `s` temp (rather than reusing the sin expression inline) is what
			// makes MWCC put alpha in f31 and the loop's constants in f26-f30, as the original does.
			f32 s;
			s = (f32)sin((6.2831855f * (f32)(angleBase + (counter * 10 + i * 60))) / 360.0f);
			xOffset = (s32)(alpha * (500.0f * s));
			s = (f32)sin((6.2831855f * (f32)(angleBase + (counter * 10 + i * 6))) / 360.0f);
			yOffset = (s32)(alpha * (200.0f * s));
			vp->x = xOffset - 0xC80;
			vp->y = yOffset + i * 400;
			vp->z = 0;
			vp->texU = 0;
			vp->texV = i * 2 * 0x40U;
			vp->x2 = xOffset + 0xC80;
			vp->y2 = yOffset + i * 400;
			vp->z2 = 0;
			vp->texU2 = 0x800;
			vp->texV2 = i * 2 * 0x40U;
		}
		DCFlushRange(vtx, 0x1C0);
		GXInvalidateVtxCache();
		effSetVtxDescN64((effVtxArray*)vtx);
		for (i = 0; i < 15; i++) {
			j = i * 2;
			GXBegin(GX_TRIANGLES, 0, 6);
			tri2(j, j + 2, j + 1, j, j + 1, j + 2, j + 3, j);
		}
	}
}

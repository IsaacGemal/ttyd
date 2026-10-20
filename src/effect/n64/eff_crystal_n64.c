#include "eff/n64/eff_crystal_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "math_ext.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2 scratch GXColor the original builds its TEV colour from.

// .data vertex arrays, referenced by effCrystalDisp.
u32 size108x108_tex32x32_vtx[0x38 / 4] = {
	0xFFCAFFCA, 0x00010000, 0x00000000, 0x00FF0036,
	0xFFCA0001, 0x04000000, 0x000000FF, 0x00360036,
	0x00010400, 0x04000000, 0x00FFFFCA, 0x00360001,
	0x00000400, 0x000000FF,
};

u32 size100x100_tex32x32_vtx[0x38 / 4] = {
	0xFFCEFFCE, 0xFFFF0000, 0x00000000, 0x00FF0032,
	0xFFCEFFFF, 0x04000000, 0x000000FF, 0x00320032,
	0xFFFF0400, 0x04000000, 0x00FFFFCE, 0x0032FFFF,
	0x00000400, 0x000000FF,
};

EffectEntry* effCrystalN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffCrystalData* data;

	effect = effEntry();
	effect->type = "CrystalN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffCrystalData));
	effect->userdata = data;
	effect->callback = effCrystalMain;
	effect->flags |= 2;

	data->type = type;
	data->timer = 0;
	data->camId = CAMERA_3D;
	if (time <= 0) {
		data->lifetime = 100;
	} else {
		data->lifetime = time;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0;

	switch (type) {
	case 0:
		data->variant = 0;
		break;
	case 1:
		data->variant = 2;
		break;
	case 2:
		data->variant = 4;
		break;
	case 3:
		data->variant = 6;
		break;
	case 4:
		data->variant = 7;
		break;
	case 5:
		data->variant = 8;
		break;
	case 6:
		data->variant = 9;
		break;
	default:
		data->variant = 10;
		data->scaleStep = -(scale - 0.1f) / (f32)time;
		break;
	}

	return effect;
}

void effCrystalMain(EffectEntry* effect) {
	EffCrystalData* data = effect->userdata;
	// Retail builds the Vec in a scratch slot and copies it into a second local
	// before dispCalcZ; a single local folds the copy away (same idiom as effGlassMain).
	Vec position;
	Vec disp = { 0 };
	s32 alpha;
	s32 fade;
	s32 timer;
	s32 life;
	s32 type;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 30;
	}

	if (data->lifetime < 100) {
		data->lifetime--;
	}
	data->timer++;

	life = data->lifetime;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	timer = data->timer;

	alpha = 255;
	type = data->variant;
	fade = 255;
	if (timer < 16) {
		alpha = timer * 16 - 1;
	}
	if (life < 16) {
		alpha = alpha * life * 16 / 256;
	}
	if (life < 16) {
		fade = life * 16;
	}

	switch (type) {
	case 0:
		data->tev0R = 70;
		data->tev0G = 180;
		data->tev0B = 120;
		{
			f32 t = (f32)sin(6.2831855f * (f32)(timer * 2) / 360.0f);
			data->alpha = 190 + (s32)(10.0f * t);
		}
		data->alpha = data->alpha * alpha / 255;
		break;
	case 1:
		data->tev0R = 70;
		data->tev0G = 255;
		data->tev0B = 230;
		data->alpha = 255;
		data->alpha = data->alpha * alpha / 255;
		break;
	case 2:
		data->tev0R = 170;
		data->tev0G = 40;
		data->tev0B = 110;
		{
			f32 t = (f32)sin(6.2831855f * (f32)(timer * 2) / 360.0f);
			data->alpha = 190 + (s32)(10.0f * t);
		}
		data->alpha = data->alpha * alpha / 255;
		break;
	case 3:
		data->tev0R = 220;
		data->tev0G = 40;
		data->tev0B = 170;
		data->alpha = 255;
		data->alpha = data->alpha * alpha / 255;
		break;
	case 4:
		data->tev0R = 235;
		data->tev0G = 235;
		data->tev0B = 200;
		{
			f32 t = (f32)sin(6.2831855f * (f32)(timer * 2) / 360.0f);
			data->alpha = 215 + (s32)(15.0f * t);
		}
		data->alpha = data->alpha * alpha / 255;
		break;
	case 5:
		data->tev0R = 235;
		data->tev0G = 235;
		data->tev0B = 200;
		data->alpha = 255;
		data->alpha = data->alpha * alpha / 255;
		break;
	case 6:
		data->tev0R = 70;
		data->tev0G = 180;
		data->tev0B = 120;
		data->alpha = 127;
		data->alpha = data->alpha * fade / 255;
		data->scale *= 1.1f;
		break;
	case 7:
		data->tev0R = 162;
		data->tev0G = 189;
		data->tev0B = 174;
		data->alpha = 160;
		data->alpha = data->alpha * fade / 255;
		data->scale *= 1.1f;
		break;
	case 8:
		data->tev0R = 170;
		data->tev0G = 40;
		data->tev0B = 110;
		data->alpha = 127;
		data->alpha = data->alpha * fade / 255;
		data->scale *= 1.1f;
		break;
	case 9:
		data->tev0R = 180;
		data->tev0G = 178;
		data->tev0B = 174;
		data->alpha = 160;
		data->alpha = data->alpha * fade / 255;
		data->scale *= 1.1f;
		break;
	default:
		data->tev0R = 50;
		data->tev0G = 50;
		data->tev0B = 255;
		data->alpha = 160;
		data->alpha = data->alpha * fade / 255;
		data->scale += data->scaleStep;
		break;
	}

	dispEntry(data->camId, dispCalcZ(&position), 2, effCrystalDisp, effect);
}

// Helper keeps the GXColor build in a separate local (r1+0x8 → arg r1+0xc), as the original does.
static inline void crystalSetColor(EffCrystalData* data, s32 alpha) {
	GXColor color = (GXColor){ 0, 0, 0, 0 };
	color.r = data->tev0R;
	color.g = data->tev0G;
	color.b = data->tev0B;
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
}

void effCrystalDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffCrystalData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx base;
	s32 timer;
	s32 alpha;
	s32 type;

	cam = camGetPtr(camId);
	data = effect->userdata;
	timer = data->timer;
	alpha = data->alpha;
	type = data->type;
	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(trans, scale, base);
	PSMTXConcat(cam->view, base, base);

	crystalSetColor(data, alpha);
	GXSetCullMode(GX_CULL_NONE);

	if (type < 3) {
		GXSetNumChans(0);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ONE, GX_CC_A0, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
		PSMTXScale(scale, 0.03125f, 0.0625f, 0.0f);
		GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
		effGetTexObjN64(0x6D, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		PSMTXRotRad(rot, 'z', 0.017453292f * (f32)timer);
		PSMTXConcat(base, rot, rot);
		GXLoadPosMtxImm(rot, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		effSetVtxDescN64((effVtxArray*)size108x108_tex32x32_vtx);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		PSMTXRotRad(rot, 'z', 0.017453292f * (f32)(-timer * 8));
		PSMTXConcat(base, rot, rot);
		GXLoadPosMtxImm(rot, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		effSetVtxDescN64((effVtxArray*)size100x100_tex32x32_vtx);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	} else {
		GXSetNumChans(0);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
		PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
		GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
		effGetTexObjN64(0x6E, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXLoadPosMtxImm(base, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		effSetVtxDescN64((effVtxArray*)size100x100_tex32x32_vtx);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}

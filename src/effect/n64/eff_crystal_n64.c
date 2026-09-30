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
GXColor unk_804230d0;

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

typedef struct EffCrystalData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	s32 lifetime;   // 0x10
	s32 timer;      // 0x14
	s32 field_0x18; // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	s32 field_0x24; // 0x24
	s32 field_0x28; // 0x28
	s32 field_0x2C; // 0x2C
	f32 scale;      // 0x30
	f32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
} EffCrystalData;

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
	data->field_0x38 = 4;
	if (time <= 0) {
		data->lifetime = 100;
	} else {
		data->lifetime = time;
	}
	data->field_0x2C = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 0;

	switch (type) {
	case 0:
		data->field_0x1C = 0;
		break;
	case 1:
		data->field_0x1C = 2;
		break;
	case 2:
		data->field_0x1C = 4;
		break;
	case 3:
		data->field_0x1C = 6;
		break;
	case 4:
		data->field_0x1C = 7;
		break;
	case 5:
		data->field_0x1C = 8;
		break;
	case 6:
		data->field_0x1C = 9;
		break;
	default:
		data->field_0x1C = 10;
		data->field_0x34 = -(scale - 0.1f) / (f32)time;
		break;
	}

	return effect;
}

void effCrystalMain(EffectEntry* effect) {
	EffCrystalData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second local
	   before dispCalcZ; a single local folds the copy away (same idiom as effGlassMain). */
	Vec position;
#endif
	Vec disp = { 0 };
	s32 alpha;
	s32 fade;
	s32 timer;
	s32 life;
	s32 type;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

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
	type = data->field_0x1C;
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
		data->field_0x20 = 70;
		data->field_0x24 = 180;
		data->field_0x28 = 120;
		data->field_0x2C = 190 + (s32)(10.0f * (f32)sin(6.2831855f * (f32)(timer * 2) / 360.0f));
		data->field_0x2C = data->field_0x2C * alpha / 255;
		break;
	case 1:
		data->field_0x20 = 70;
		data->field_0x24 = 255;
		data->field_0x28 = 230;
		data->field_0x2C = 255;
		data->field_0x2C = data->field_0x2C * alpha / 255;
		break;
	case 2:
		data->field_0x20 = 170;
		data->field_0x24 = 40;
		data->field_0x28 = 110;
		data->field_0x2C = 190 + (s32)(10.0f * (f32)sin(6.2831855f * (f32)(timer * 2) / 360.0f));
		data->field_0x2C = data->field_0x2C * alpha / 255;
		break;
	case 3:
		data->field_0x20 = 220;
		data->field_0x24 = 40;
		data->field_0x28 = 170;
		data->field_0x2C = 255;
		data->field_0x2C = data->field_0x2C * alpha / 255;
		break;
	case 4:
		data->field_0x20 = 235;
		data->field_0x24 = 235;
		data->field_0x28 = 200;
		data->field_0x2C = 215 + (s32)(15.0f * (f32)sin(6.2831855f * (f32)(timer * 2) / 360.0f));
		data->field_0x2C = data->field_0x2C * alpha / 255;
		break;
	case 5:
		data->field_0x20 = 235;
		data->field_0x24 = 235;
		data->field_0x28 = 200;
		data->field_0x2C = 255;
		data->field_0x2C = data->field_0x2C * alpha / 255;
		break;
	case 6:
		data->field_0x20 = 70;
		data->field_0x24 = 180;
		data->field_0x28 = 120;
		data->field_0x2C = 127;
		data->field_0x2C = data->field_0x2C * fade / 255;
		data->scale *= 1.1f;
		break;
	case 7:
		data->field_0x20 = 162;
		data->field_0x24 = 189;
		data->field_0x28 = 174;
		data->field_0x2C = 160;
		data->field_0x2C = data->field_0x2C * fade / 255;
		data->scale *= 1.1f;
		break;
	case 8:
		data->field_0x20 = 170;
		data->field_0x24 = 40;
		data->field_0x28 = 110;
		data->field_0x2C = 127;
		data->field_0x2C = data->field_0x2C * fade / 255;
		data->scale *= 1.1f;
		break;
	case 9:
		data->field_0x20 = 180;
		data->field_0x24 = 178;
		data->field_0x28 = 174;
		data->field_0x2C = 160;
		data->field_0x2C = data->field_0x2C * fade / 255;
		data->scale *= 1.1f;
		break;
	default:
		data->field_0x20 = 50;
		data->field_0x24 = 50;
		data->field_0x28 = 255;
		data->field_0x2C = 160;
		data->field_0x2C = data->field_0x2C * fade / 255;
		data->scale += data->field_0x34;
		break;
	}

#ifdef MUST_MATCH
	dispEntry(data->field_0x38, dispCalcZ(&position), 2, effCrystalDisp, effect);
#else
	dispEntry(data->field_0x38, dispCalcZ(&disp), 2, effCrystalDisp, effect);
#endif
}

// Helper keeps the GXColor build in a separate local (r1+0x8 → arg r1+0xc), as the original does.
static inline void crystalSetColor(EffCrystalData* data, s32 alpha) {
	GXColor color = unk_804230d0;
	color.r = data->field_0x20;
	color.g = data->field_0x24;
	color.b = data->field_0x28;
	color.a = alpha;
	GXSetTevColor(1, color);
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
	alpha = data->field_0x2C;
	type = data->type;
	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(trans, scale, base);
	PSMTXConcat(cam->view, base, base);

	crystalSetColor(data, alpha);
	GXSetCullMode(0);

	if (type < 3) {
		GXSetNumChans(0);
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 0xFF);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 8, 0xC, 3, 0xF);
		GXSetTevAlphaIn(0, 7, 1, 4, 7);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
		PSMTXScale(scale, 0.03125f, 0.0625f, 0.0f);
		GXLoadTexMtxImm(scale, 0x1E, 1);
		effGetTexObjN64(0x6D, &texObj);
		GXLoadTexObj(&texObj, 0);
		PSMTXRotRad(rot, 'z', 0.017453292f * (f32)timer);
		PSMTXConcat(base, rot, rot);
		GXLoadPosMtxImm(rot, 0);
		GXSetCurrentMtx(0);
		effSetVtxDescN64((effVtxArray*)size108x108_tex32x32_vtx);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		PSMTXRotRad(rot, 'z', 0.017453292f * (f32)(-timer * 8));
		PSMTXConcat(base, rot, rot);
		GXLoadPosMtxImm(rot, 0);
		GXSetCurrentMtx(0);
		effSetVtxDescN64((effVtxArray*)size100x100_tex32x32_vtx);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	} else {
		GXSetNumChans(0);
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 0xFF);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
		GXSetTevAlphaIn(0, 7, 4, 1, 7);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
		PSMTXScale(scale, 0.0625f, 0.0625f, 0.0f);
		GXLoadTexMtxImm(scale, 0x1E, 1);
		effGetTexObjN64(0x6E, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXLoadPosMtxImm(base, 0);
		GXSetCurrentMtx(0);
		effSetVtxDescN64((effVtxArray*)size100x100_tex32x32_vtx);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}

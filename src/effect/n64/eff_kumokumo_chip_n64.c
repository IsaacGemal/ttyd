#include "eff/n64/eff_kumokumo_chip_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "driver/camdrv.h"
#include "eff/eff_n64.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

// This unit's .sdata2 literals. Declared extern (like eff_kumokumo_n64.c): defining them
// here makes MWCC rematerialise the load as an anonymous pool entry (@NNN) instead of
// referencing the named symbol.
extern const f32 float_0p03125_8041f568;
extern const f32 float_0_8041f56c;

// Local vertex data (0x38 bytes); the original symbol is local to this unit's asm.
static u32 size32x32_tex32x32_vtx[0x38 / 4] = {
	0xFFF00000, 0x00000000, 0x00000000, 0x00FF0010,
	0x00000000, 0x04000000, 0x000000FF, 0x00100020,
	0x00000400, 0x04000000, 0x00FFFFF0, 0x00200000,
	0x00000400, 0x000000FF,
};

// .sbss2 colour scratch, global to this unit's asm
GXColor unk_80423198;
GXColor unk_8042319c;

const Vec vec3_802faa50 = { 0.0f, 0.0f, 0.0f };

void effKumokumoChipMain(EffectEntry* effect) {
	effKumokumoChipData* data;
	s32 lifetime;
	s32 type;
	s32 timer;
	Vec pos2;
	Vec pos;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;
	type = data->type;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	lifetime = data->lifetime;
	if (lifetime < 0) {
		effDelete(effect);
		return;
	}
	timer = data->timer;
	if (type == 0 || type == 2) {
		if (timer < 16) {
			data->a = (timer << 4) + 0xf;
		}
		else {
			data->a = 0xff;
		}
		if (lifetime < 16) {
			data->a = lifetime << 4;
		}
	}
	else {
		if (timer < 4) {
			data->a = (timer << 6) + 0x3f;
		}
		else {
			data->a = 0xff;
		}
		if (lifetime < 4) {
			data->a = lifetime << 6;
		}
	}
	data->x += data->xVel;
	data->y += data->yVel;
	data->z += data->zVel;

	dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effKumokumoChipDisp, effect);
}

// The patch-and-set of a TEV colour lives in this inline helper on purpose: it makes MWCC
// place the helper's GXColor local below its by-value argument temporary, which is the
// slot layout the original has.
static inline void chipSetColor(s32 id, s32 r, s32 g, s32 b, s32 a, GXColor* base) {
	GXColor color = *base;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	GXSetTevColor(id, color);
}

void effKumokumoChipDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam;
	effKumokumoChipData* data;
	Mtx mtx;
	Mtx scale;
	GXTexObj texObj;
	s32 alpha;

	cam = camGetPtr(camId);
	data = effect->userdata;
	alpha = data->a;

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);

	chipSetColor(1, data->r, data->g, data->b, alpha, &unk_80423198);
	chipSetColor(2, data->r2, data->g2, data->b2, data->a2, &unk_8042319c);
	effGetTexObjN64(0x6b, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scale, float_0p03125_8041f568, float_0p03125_8041f568, float_0_8041f56c);
	GXLoadTexMtxImm(scale, 0x1e, 1);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 4, 2, 8, 0xf);
	GXSetTevAlphaIn(0, 7, 1, 4, 7);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size32x32_tex32x32_vtx);
	GXBegin(0x90, 0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}

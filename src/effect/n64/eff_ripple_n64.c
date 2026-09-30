#include "eff/n64/eff_ripple_n64.h"
#include "eff/eff_n64.h"
#include "eff/eff_ripple.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

const Vec vec3_802fb520 = { 0.0f, 0.0f, 0.0f };

// .sdata2 constants owned by this unit (named after their original addresses).
const GXColor dat_8041fb98;
const f32 float_0p0625_8041fb9c;
const f32 float_0_8041fba0;
const f32 float_0p03125_8041fba4;
const f32 float_deg2rad_8041fba8;
const f32 float_1_8041fbac;
const f32 float_0p3_8041fbb0;
const f32 float_1p05_8041fbb4;
const f32 float_0p1_8041fbb8;
const f32 float_0p6_8041fbbc;
const f32 float_10_8041fbc0;
// .sbss2 TEV colour base (zero-initialised).
const GXColor unk_80423248;
// .data vertex array for effRippleDisp (0x38 bytes).
u32 size16x16_tex32x32_0_vtx[14] = {
	0xFFF80000, 0xFFF80000, 0x00000000, 0x00FF0008, 0x0000FFF8, 0x04000000, 0x000000FF,
	0x00080000, 0x00080400, 0x04000000, 0x00FFFFF8, 0x00000008, 0x00000400, 0x000000FF,
};

// dont_inline keeps effRippleN64Entry a real `bl` from effRippleMain as in retail.
#pragma dont_inline on

EffectEntry* effRippleN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	EffRippleData* data;

	effect = effEntry();
	effect->type = "RippleN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(EffRippleData));
	effect->userdata = data;
	effect->callback = effRippleMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x18 = 0;
	data->field_0x20 = 0;
	data->rx = 0.0f;
	data->rz = 0.0f;
	data->field_0x14 = 1.0f;
	data->camId = 4;
	switch (type) {
		case 1:
		case 10:
			data->field_0x10 = 0.6f;
			break;
		case 2:
		case 20:
			data->field_0x10 = 10.0f;
			break;
		case 3:
		case 30:
			data->field_0x10 = 10.0f;
			break;
		default:
			data->field_0x10 = 1.0f;
			break;
	}
	data->field_0x1C = 0x20;
	data->field_0x24 = rand() % 32;
	data->field_0x28 = y + scale;
	return effect;
}
#pragma dont_inline reset

// MWCC's callee-saved register colours differ: retail keeps `effect` in r28 and `timer` in r30, ours
// swaps them (effect r30, timer r28); that also moves the prologue's `mr r28, r3` and lets the
// scheduler hoist the vec loads. Every declaration/assignment order, `register`/`volatile`, variable
// splitting, opt_common_subs/opt_propagation and function order were tried.
// NONMATCHING (93.49%): only the effect/timer callee-saved colours (r28 vs r30) and the prologue schedule differ.
void effRippleMain(EffectEntry* effect) {
	EffRippleData* data = effect->userdata;
	Vec position2;
	Vec position;
	s32 type;
	s32 timer;

	position = vec3_802fb520;
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	timer = data->field_0x1C;
	type = data->type;
	data->field_0x1C--;
	data->field_0x20++;
	if (data->field_0x1C < 0) {
		effDelete(effect);
		return;
	}

	if (timer > 5) {
		data->field_0x18 = (s32)(0.3f * (f32)(120 - data->field_0x18) + (f32)data->field_0x18);
	}

	if ((type == 1 || type == 2 || type == 3) && timer == 15) {
#ifdef MUST_MATCH
		/* MUST_MATCH: the original loads the new effect's userdata into r3 before reading
		   data->rz, so rz lands in f1; the natural one-liner reads userdata first and reuses f0. */
		EffectEntry* e = effRippleN64Entry(type * 10, data->x, data->y, data->z, 0.0f);
		f32 rz = data->rz;
		EffRippleData* newData = e->userdata;
		f32 rx = data->rx;
		newData->rx = rx;
		newData->rz = rz;
#else
		EffRippleData* newData =
			effRippleN64Entry(type * 10, data->x, data->y, data->z, 0.0f)->userdata;
		newData->rx = data->rx;
		newData->rz = data->rz;
#endif
	}

	if (type == 2 || type == 20) {
		data->field_0x10 *= 1.05f;
	}
	if (type == 3 || type == 30) {
		data->field_0x10 *= 1.05f;
	}

	if (timer < 0x20) {
		data->field_0x18 = timer * 8;
	}

	data->field_0x24++;
	if (data->field_0x24 >= 0x20) {
		data->field_0x24 -= 0x20;
	}

	data->field_0x10 += 0.1f;
	dispEntry(data->camId, dispCalcZ(&position2), 2, effRippleDisp, effect);
}

// The original builds each branch's TEV colour in a helper-style local whose stack slot sits below
// its outgoing copy; writing the same code inline makes mwcc put the two slots the other way round.
static inline void effRippleSetColor(GXColor base, s32 alpha) {
	GXColor color = base;
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
}

void effRippleDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffRippleData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotXMtx;
	s32 type;
	s32 field24;

	cam = camGetPtr(camId);
	data = effect->userdata;
	type = data->type;

	if (type == 10 || type == 20 || (u32)(type - 1) <= 2 || type == 30) {
		effGetTexObj(0x5b, &texObj);
	} else {
		effGetTexObjN64(0x21, &texObj);
	}
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	effGetTexObjN64(0x22, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP1);

	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7d);
	PSMTXScale(scaleMtx, float_0p0625_8041fb9c, float_0p0625_8041fb9c, float_0_8041fba0);
	GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
	GXLoadTexMtxImm(scaleMtx, 0x21, 1);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xf, 0xf, 0xf, 2);
	GXSetTevAlphaIn(0, 7, 4, 1, 7);

	if (type == 1 || type == 10 || type == 3 || type == 30) {
		effRippleSetColor(unk_80423248, data->field_0x18);
	} else {
		effRippleSetColor(dat_8041fb98, data->field_0x18);
	}

	field24 = data->field_0x24;
	PSMTXScale(scaleMtx, float_0p03125_8041fba4, float_0p03125_8041fba4, float_0_8041fba0);
	PSMTXTrans(transMtx, float_0_8041fba0, (f32)field24, float_0_8041fba0);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, 0x21, 1);

	PSMTXTrans(transMtx, data->x, data->y, data->z);
	PSMTXRotRad(rotXMtx, 'x', float_deg2rad_8041fba8 * data->rx);
	PSMTXRotRad(scaleMtx, 'z', float_deg2rad_8041fba8 * data->rz);
	PSMTXConcat(rotXMtx, scaleMtx, rotXMtx);
	PSMTXScale(scaleMtx, data->field_0x14 * data->field_0x10, float_1_8041fbac,
	           data->field_0x14 * data->field_0x10);
	PSMTXConcat(transMtx, rotXMtx, transMtx);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	PSMTXConcat(cam->view, transMtx, transMtx);
	GXLoadPosMtxImm(transMtx, 0);
	GXSetCurrentMtx(0);
	GXSetCullMode(0);
	effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_0_vtx);
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
	tri2(0, 1, 2, 0, 0, 2, 3, 0);
}

void effRippleN64SetRxRz(EffectEntry* effect, f32 rx, f32 rz) {
	effRippleN64Data* data = effect->userdata;

	data->field_0x2C = rx;
	data->field_0x30 = rz;
}

const GXColor dat_8041fb98 = { 0x00, 0xFF, 0xFF, 0xFF };
const f32 float_0p0625_8041fb9c = 0.0625f;
const f32 float_0_8041fba0 = 0.0f;
const f32 float_0p03125_8041fba4 = 0.03125f;
const f32 float_deg2rad_8041fba8 = 0.017453292f;
const f32 float_1_8041fbac = 1.0f;
const f32 float_0p3_8041fbb0 = 0.3f;
const f32 float_1p05_8041fbb4 = 1.05f;
const f32 float_0p1_8041fbb8 = 0.1f;
const f32 float_0p6_8041fbbc = 0.6f;
const f32 float_10_8041fbc0 = 10.0f;

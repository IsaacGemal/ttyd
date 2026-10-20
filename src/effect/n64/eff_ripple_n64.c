#include "eff/n64/eff_ripple_n64.h"
#include "eff/eff_n64.h"
#include "eff/eff_ripple.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <stdlib.h>

// The unit's first .rodata object (retail +0x0): effRippleMain's zero position template.
__declspec(section ".rodata") Vec vec3_802fb520 = { 0.0f, 0.0f, 0.0f };

// .sdata2 constants owned by this unit (named after their original addresses).
const f32 float_0p0625_8041fb9c;
const f32 float_0_8041fba0;
// .sbss2 TEV colour base (zero-initialised). The .sdata/.sbss2 pragma pair puts the object in
// .sbss2; a plain tentative const definition would land in .sbss.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423248;
#pragma section ".sdata" ".sbss"
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
	data->alpha = 0;
	data->age = 0;
	data->rx = float_0_8041fba0;
	data->rz = float_0_8041fba0;
	data->field_0x14 = 1.0f;
	data->camId = 4;
	switch (type) {
		case 1:
		case 10:
			data->scale = 0.6f;
			break;
		case 2:
		case 20:
			data->scale = 10.0f;
			break;
		case 3:
		case 30:
			data->scale = 10.0f;
			break;
		default:
			data->scale = 1.0f;
			break;
	}
	data->timer = 0x20;
	data->scrollV = rand() % 32;
	data->field_0x28 = y + scale;
	return effect;
}
#pragma dont_inline reset


// Retail keeps `timer--` before `age++` and colours `effect`/`timer`/`type` r28/r30/r29.
// At -O4,p mwcc coalesces the two read-modify-writes the other way round and gives `effect` r30; at
// -O2 it emits the retail order and colours with the same 0x50 frame. `position` is built from the
// .rodata zero vector (`vec3_802fb520`, defined above); the direct field writes
// put the zero-vector load before the callee-save stores, as retail does.
#pragma push
#pragma optimization_level 2 // only this function needs the level-2 allocation; see above
void effRippleMain(EffectEntry* effect) {
	EffRippleData* data = effect->userdata;
	Vec dispPos;
	Vec position;
	s32 timer;
	s32 type;

	position = vec3_802fb520;
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	dispPos = position;

	timer = data->timer;
	type = data->type;
	data->timer--;
	data->age++;
	if (data->timer < 0) {
		effDelete(effect);
		return;
	}

	if (timer > 5) {
		data->alpha = (s32)(0.3f * (f32)(120 - data->alpha) + (f32)data->alpha);
	}

	if ((type == 1 || type == 2 || type == 3) && timer == 15) {
#ifdef MUST_MATCH
		/* MUST_MATCH: the original loads the new effect's userdata into r3 before reading
		   data->rz, so rz lands in f1; the natural one-liner reads userdata first and reuses f0. */
		EffectEntry* subEffect = effRippleN64Entry(type * 10, data->x, data->y, data->z, float_0_8041fba0);
		f32 rz = data->rz;
		EffRippleData* subData = subEffect->userdata;
		f32 rx = data->rx;
		subData->rx = rx;
		subData->rz = rz;
#else
		EffRippleData* subData =
			effRippleN64Entry(type * 10, data->x, data->y, data->z, float_0_8041fba0)->userdata;
		subData->rx = data->rx;
		subData->rz = data->rz;
#endif
	}

	if (type == 2 || type == 20) {
		data->scale *= 1.05f;
	}
	if (type == 3 || type == 30) {
		data->scale *= 1.05f;
	}

	if (timer < 0x20) {
		data->alpha = timer * 8;
	}

	data->scrollV++;
	if (data->scrollV >= 0x20) {
		data->scrollV -= 0x20;
	}

	data->scale += 0.1f;
	dispEntry(data->camId, dispCalcZ(&dispPos), 2, effRippleDisp, effect);
}
#pragma pop

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
	s32 texScroll;

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
		effRippleSetColor(unk_80423248, data->alpha);
	} else {
		effRippleSetColor((GXColor){ 0xFF, 0xFF, 0xFF, 0x00 }, data->alpha);
	}

	texScroll = data->scrollV;
	PSMTXScale(scaleMtx, 0.03125f, 0.03125f, float_0_8041fba0);
	PSMTXTrans(transMtx, float_0_8041fba0, (f32)texScroll, float_0_8041fba0);
	PSMTXConcat(scaleMtx, transMtx, transMtx);
	GXLoadTexMtxImm(transMtx, 0x21, 1);

	PSMTXTrans(transMtx, data->x, data->y, data->z);
	PSMTXRotRad(rotXMtx, 'x', 0.017453292f * data->rx);
	PSMTXRotRad(scaleMtx, 'z', 0.017453292f * data->rz);
	PSMTXConcat(rotXMtx, scaleMtx, rotXMtx);
	PSMTXScale(scaleMtx, data->field_0x14 * data->scale, 1.0f,
	           data->field_0x14 * data->scale);
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

const f32 float_0p0625_8041fb9c = 0.0625f;
const f32 float_0_8041fba0 = 0.0f;

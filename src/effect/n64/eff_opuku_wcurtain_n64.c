#include "eff/n64/eff_opuku_wcurtain_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/os/OSFastCast.h>
#include <stdlib.h>


// NONMATCHING (80.80%): the packed s16<->f32 vertex conversions are psq_l/psq_st through GQR5 in
// retail (the store saturates); they need inline asm, so here they lower to lha/xoris/lfd/fsubs loads
// and a __fsel clamp + fctiwz/sth store. The GQR2-5 blocks (OSInitFastCast, MUST_MATCH) match retail
// byte-for-byte, but the loop body still needs ~4 more instructions per component, keeps one fewer
// callee-saved FPR (f29-f31 vs f28-f31), uses an extra GPR (r25) and so a 0x120 frame instead of
// 0x110, which shifts every local slot and the GPRs the vertex-build and triangle loops use. Verified:
// every GC compiler here (1.0-3.0a5.2) lowers a scalar cast to the integer path and there is no
// fastcast pragma/flag/builtin; __vec2x32float__ emits qr0 psq_lx/psq_stx, not the W=1 qr5 form.

// Retail stores the transformed vertex through psq_st (GQR5, s16), which saturates to the s16 range.
static inline s16 clampS16(f32 value) {
	f32 hi = 32767.0f;
	f32 lo = -32768.0f;
	return (s16)__fsel(value - hi, hi, __fsel(value - lo, value, lo));
}

// .rodata
const char str_802FB2A4[] = "OpukuWcurtainN64";

// .sbss2 scratch color (same pattern as eff_opuku_jetwrain_n64.c's unk_804231d8).
GXColor unk_804231e0;

// .sdata2
extern const f32 float_0_8041f820;
extern const f32 float_neg1_8041f834;
extern const f32 float_0p01_8041f838;
extern const f32 float_0p5_8041f83c;

// Local copy of system.c's inline helper so it inlines here (as in eff_opuku_jetw_n64.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}
EffectEntry* effOpukuWcurtainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effOpukuWcurtainData* data;
	s32 i;

	entry = effEntry();
	entry->type = str_802FB2A4;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effOpukuWcurtainData));
	entry->userdata = data;
	entry->callback = effOpukuWcurtainMain;
	entry->flags |= 2;

	data->type = type;
	data->counter = 0;
	if (duration <= 0) {
		data->duration = 1000;
	} else {
		data->duration = duration;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x18 = 20;
	data->field_0x1C = 120;
	data->field_0x20 = 255;
	for (i = 0; i < 12; i++) {
		data->field_0x2C[i] = float_0_8041f820;
		data->field_0x5C[i] = (rand() % 11) * float_0p01_8041f838 + float_neg1_8041f834;
		data->field_0x5C[i] *= float_0p5_8041f83c;
	}

	return entry;
}
void effOpukuWcurtainMain(EffectEntry* effect) {
	effOpukuWcurtainData* data = effect->userdata;
	Vec position = initializeVec((Vec*)&data->x);
	s32 i;
	s32 counter;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->duration = 0x10;
	}
	if (data->duration < 1000) {
		data->duration--;
	}
	data->counter++;
	if (data->duration < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	if (data->duration < 0x10) {
		data->field_0x24 = data->duration << 4;
	}
	if (counter < 0x10) {
		data->field_0x24 = counter * 0x10 + 0xf;
	}
	for (i = 0; i < 12; i++) {
		data->field_0x2C[i] += data->field_0x5C[i];
		data->field_0x5C[i] += -0.05f;
		if (data->field_0x5C[i] > 5.0f) {
			data->field_0x5C[i] = 0.1f * (5.0f - data->field_0x5C[i]) + data->field_0x5C[i];
		}
	}
	dispEntry(4, dispCalcZ(&position), 2, effOpukuWcurtainDisp, effect);
}

// NONMATCHING (80.80%): the s16<->f32 psq_l/psq_st fastcast (GQR5) cannot be emitted from C; the
// __fsel clamp reproduces the store's saturation but the loop body, frame and registers still differ.
void effOpukuWcurtainDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	effOpukuWcurtainData* data = effect->userdata;
	OpukuWcurtainVtx* vtx;
	smartEntry* entry;
	GXTexObj texObj;
	Mtx transMtx;
	Mtx scaleMtx;
	GXColor color;
	Vec v;
	f32 scale;
	s32 i;
	s32 j;
	s32 texS;
	s32 x;
	s32 counter = data->counter;

	color = unk_804231e0;
	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = data->field_0x24;
	scale = 0.1f * data->scale;
	GXSetTevColor(GX_TEVREG0, color);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
	effGetTexObjN64(0x8F, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scaleMtx, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);

	entry = smartAlloc(0x2A0, HEAP_EFFECT);
	vtx = (OpukuWcurtainVtx*)entry->address;
	texS = counter << 5;
	x = -300;
	for (i = 0; i < 12; i++) {
		vtx[2 * i].x = x;
		vtx[2 * i].y = 600;
		vtx[2 * i].z = 0;
		vtx[2 * i].s = texS;
		vtx[2 * i].t = (s32)data->field_0x2C[i] << 5;
		vtx[2 * i].r = 0xFF;
		vtx[2 * i].g = 0xFF;
		vtx[2 * i].b = 0xFF;
		vtx[2 * i].a = 0xFF - i * 20;
		vtx[2 * i + 1].x = x;
		x += 50;
		vtx[2 * i + 1].y = (s32)(10.0f * (60.0f + data->field_0x2C[i]));
		vtx[2 * i + 1].z = 0;
		vtx[2 * i + 1].s = texS;
		texS += 0x80;
		vtx[2 * i + 1].t = 0x400;
		vtx[2 * i + 1].r = 0xFF;
		vtx[2 * i + 1].g = 0xFF;
		vtx[2 * i + 1].b = 0xFF;
		vtx[2 * i + 1].a = 0xFF - i * 20;
	}

	PSMTXTrans(transMtx, data->x, data->y, data->z);
	PSMTXScale(scaleMtx, scale, scale, scale);
	PSMTXConcat(transMtx, scaleMtx, transMtx);

	{
		OpukuWcurtainVtx* p = vtx;
		for (j = 0; j < i * 2; j++, p++) {
#ifdef MUST_MATCH
			OSInitFastCast(); /* MUST_MATCH: retail's GQR2-5 setup before the s16->f32 reads */
#endif
			v.x = p->x;
			v.y = p->y;
			v.z = p->z;
			PSMTXMultVec(transMtx, &v, &v);
#ifdef MUST_MATCH
			OSInitFastCast(); /* MUST_MATCH: retail's GQR2-5 setup before the f32->s16 writes */
#endif
			p->x = clampS16(v.x);
			p->y = clampS16(v.y);
			p->z = clampS16(v.z);
		}
	}

	DCFlushRange(vtx, 0x2A0);
	GXInvalidateVtxCache();
	effSetVtxDescN64((effVtxArray*)vtx);
	GXLoadPosMtxImm(cam->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	for (j = 0, x = 0; j < i - 1; j++, x += 2) {
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(x, x + 2, x + 1, x, x + 1, x + 2, x + 3, x);
	}
}

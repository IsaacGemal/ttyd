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

// .rodata

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a
// small-data section and place the object and the 4-byte section-tail gap by hand (the gap is declared
// first so it lands higher).
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_804231E4_sbss2[4];
__declspec(section ".sdata") GXColor unk_804231e0;
#pragma section ".sdata" ".sbss"

// .sdata2

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
	entry->type = "OpukuWcurtainN64";
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
		data->field_0x2C[i] = 0.0f;
		data->field_0x5C[i] = (rand() % 11) * 0.01f + -1.0f;
		data->field_0x5C[i] *= 0.5f;
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

// Building the colour inside an inline that returns it makes mwcc lay the local and the outgoing argument
// slot out the way the original does; building them inline in the caller puts them the wrong way round
// (same quirk as eff_akari_charge_n64.c).
#ifdef MUST_MATCH
static inline GXColor opukuWcurtainColor(s32 r, s32 g, s32 b, s32 a) {
	GXColor c = unk_804231e0;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}
#endif

// The packed s16<->f32 vertex conversions are psq_l/psq_st (GQR5) and match through the SDK
// OSs16tof32/OSf32tos16 fast-cast inlines; the colour local/outgoing copies now land in the original's
// slots through the opukuWcurtainColor inline above, and the transform loop, GX/TEV setup, matrix math and
// draw loop are instruction-for-instruction identical. What is left is one allocator 2-cycle: in the
// vertex-building loop texS and x are swapped. The original colours texS r4, x r5, fctiwz temp r6, 0xff
// r7, 0 r8, field_0x2C pointer r9, i*20 r10, alpha r11 and the walking vtx cursor r12; ours gives x r4 and
// texS r5, with everything else already matching. x is written as `10 * (i * 5 - 30)` and s as
// `(counter + i * 4) << 5` so the optimiser strength-reduces both to the same walking induction temps the
// original uses; only the order the two temps are created differs. The winning shape also puts
// `a = 0xFF - i * 20` just before the `vtx[0].a` store and uses a block-local `s32 x = j * 2;` in the
// triangle loop (that keeps the callee-saved r27/r30 assignment). Tried without moving the allocation:
// ~30 x spellings and ~30 s spellings (and their pairs), inline helpers for x/s/both, declaration-order
// sweeps (720 + 5040), `a` at every statement position, zero-valued chain terms, intermediate field
// temporaries, one named + one inline store, reusing the function-scope triangle x, every integer type,
// opt_*/schedule/peephole/pointer-analysis/global_optimizer/register-allocation pragmas (single and
// pairs) and every -O level; all still 8 diffs or worse. x's induction priority is either at the very
// top (r4) or below the loop constants (r8+), never between texS and them, so this is a two-value
// allocation tie (permuter work).
// NONMATCHING (99.82%): vertex-building loop's texS/x register 2-cycle only.
void effOpukuWcurtainDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	effOpukuWcurtainData* data = effect->userdata;
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
	s32 a;
	OpukuWcurtainVtx* vtx;
	OpukuWcurtainVtx* base;
	s32 counter = data->counter;

	scale = 0.1f * data->scale;
#ifdef MUST_MATCH
	GXSetTevColor(GX_TEVREG0, opukuWcurtainColor(data->field_0x18, data->field_0x1C, data->field_0x20, data->field_0x24));
#else
	color = unk_804231e0;
	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = data->field_0x24;
	GXSetTevColor(GX_TEVREG0, color);
#endif
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
	base = vtx;
	texS = counter << 5;
	for (i = 0, x = -300; i < 12; i++) {
		vtx[0].x = 10 * (i * 5 - 30);
		vtx[0].y = 600;
		vtx[0].z = 0;
		vtx[0].s = (counter + i * 4) << 5;
		vtx[0].t = (s32)data->field_0x2C[i] << 5;
		vtx[0].r = 0xFF;
		vtx[0].g = 0xFF;
		vtx[0].b = 0xFF;
		a = 0xFF - i * 20;
		vtx[0].a = a;
		vtx[1].x = 10 * (i * 5 - 30);
		vtx[1].y = (s32)(10.0f * (60.0f + data->field_0x2C[i]));
		vtx[1].z = 0;
		vtx[1].s = (counter + i * 4) << 5;
		vtx[1].t = 0x400;
		vtx[1].r = 0xFF;
		vtx[1].g = 0xFF;
		vtx[1].b = 0xFF;
		vtx[1].a = a;
		vtx += 2;
	}

	PSMTXTrans(transMtx, data->x, data->y, data->z);
	PSMTXScale(scaleMtx, scale, scale, scale);
	PSMTXConcat(transMtx, scaleMtx, transMtx);

	for (j = 0; j < i * 2; j++) {
		OSInitFastCast();
		OSs16tof32(&base[j].x, &v.x);
		OSs16tof32(&base[j].y, &v.y);
		OSs16tof32(&base[j].z, &v.z);
		PSMTXMultVec(transMtx, &v, &v);
		OSInitFastCast();
		OSf32tos16(&v.x, &base[j].x);
		OSf32tos16(&v.y, &base[j].y);
		OSf32tos16(&v.z, &base[j].z);
	}

	DCFlushRange(base, 0x2A0);
	GXInvalidateVtxCache();
	effSetVtxDescN64((effVtxArray*)base);
	GXLoadPosMtxImm(cam->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	for (j = 0; j < i - 1; j++) {
		s32 x = j * 2;
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(x, x + 2, x + 1, x, x + 1, x + 2, x + 3, x);
	}
}

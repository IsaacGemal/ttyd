#include "eff/n64/eff_horao_spell_n64.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>
#include <dolphin/os/OSFastCast.h>
#include "math_ext.h"
#include <string.h>

// .sdata
static u8 colr[20] = {
	0x78, 0x82, 0x8C, 0x96, 0xA0, 0xAA, 0xB4, 0xBE,
	0xC8, 0xC8, 0xBE, 0xB4, 0xAA, 0xA0, 0x96, 0x8C, 0x82, 0, 0, 0,
};
static u8 colg[20] = {
	0xAA, 0xA0, 0x96, 0x8C, 0x82, 0x78, 0x82, 0x8C,
	0x96, 0xA0, 0xAA, 0xB4, 0xBE, 0xC8, 0xC8, 0xBE, 0xB4, 0, 0, 0,
};
static u8 colb[20] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0,
};
// 4-byte zero word that ends the unit's .sdata: an all-zero object needs explicit_zero_data to be
// emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80413A4C_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

// .sdata2
// Retail reloads this scratch word every outer-loop iteration instead of hoisting it, which only happens
// if the object is writable; the byte content and .sdata2 slot are identical either way.
#ifdef MUST_MATCH
__declspec(section ".sdata2") u32 dat_8041f158 = 0xFFFFFF00;
#else
const u32 dat_8041f158 = 0xFFFFFF00;
#endif

// .sbss2 zero GXColor scratch the original copies before filling their fields; declared in reverse so
// the 4-byte gap lands after them.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor gap_10_8042315C_sbss2;
__declspec(section ".sdata") GXColor unk_80423158;
__declspec(section ".sdata") GXColor unk_80423154;
__declspec(section ".sdata") GXColor unk_80423150;
#pragma section ".sdata" ".sbss"

// .data: 32 vertices (16 pairs), values read byte-for-byte from the original `.4byte` block.
effHoraoSpellVtx horao_spell_vtx[32] = {
	{ { 0, -8, 0 }, { 0, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 8, 0 }, { 0, 1024 }, { 0, 0, 0, 0 } },
	{ { 0, -8, 0 }, { 512, 0 }, { 0, 0, 0, 50 } },
	{ { 0, 8, 0 }, { 512, 1024 }, { 0, 0, 0, 50 } },
	{ { 0, -8, 0 }, { 1024, 0 }, { 0, 0, 0, 100 } },
	{ { 0, 8, 0 }, { 1024, 1024 }, { 0, 0, 0, 100 } },
	{ { 0, -8, 0 }, { 1536, 0 }, { 0, 0, 0, 150 } },
	{ { 0, 8, 0 }, { 1536, 1024 }, { 0, 0, 0, 150 } },
	{ { 0, -8, 0 }, { 2048, 0 }, { 0, 0, 0, 200 } },
	{ { 0, 8, 0 }, { 2048, 1024 }, { 0, 0, 0, 200 } },
	{ { 0, -8, 0 }, { 2560, 0 }, { 0, 0, 0, 250 } },
	{ { 0, 8, 0 }, { 2560, 1024 }, { 0, 0, 0, 250 } },
	{ { 0, -8, 0 }, { 3072, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 3072, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 3584, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 3584, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 4096, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 4096, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 4608, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 4608, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 5120, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 5120, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 5632, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 5632, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 6144, 0 }, { 0, 0, 0, 255 } },
	{ { 0, 8, 0 }, { 6144, 1024 }, { 0, 0, 0, 255 } },
	{ { 0, -8, 0 }, { 6656, 0 }, { 0, 0, 0, 200 } },
	{ { 0, 8, 0 }, { 6656, 1024 }, { 0, 0, 0, 200 } },
	{ { 0, -8, 0 }, { 7168, 0 }, { 0, 0, 0, 100 } },
	{ { 0, 8, 0 }, { 7168, 1024 }, { 0, 0, 0, 100 } },
	{ { 0, -8, 0 }, { 7680, 0 }, { 0, 0, 0, 0 } },
	{ { 0, 8, 0 }, { 7680, 1024 }, { 0, 0, 0, 0 } },
};

// The .sbss2 scratch GXColors are defined above; the .data vertex table is defined just above.
EffectEntry* effHoraoSpellN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effHoraoSpellData* data;

	effect = effEntry();
	effect->type = "HoraoSpellN64\0\0";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x3C);
	effect->userdata = data;
	effect->callback = effHoraoSpellMain;
	effect->flags |= 2;
	data->type = type;
	data->age = 0;
	if (life <= 0) {
		data->life = 1000;
	} else {
		data->life = life;
	}
	data->color1A = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->color1R = 70;
	data->color1G = 180;
	data->color1B = 120;
	data->color2R = 20;
	data->color2G = 230;
	data->color2B = 50;
	data->color2A = 255;
	return effect;
}
void effHoraoSpellMain(EffectEntry* effect) {
	effHoraoSpellData* data;
	s32 timer;
	s32 counter;
	Vec pos2;
	Vec pos;

	data = effect->userdata;
	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}

	timer = data->life;
	if (timer < 1000) {
		data->life = timer - 1;
	}
	data->age++;
	timer = data->life;

	if (timer < 0) {
		effDelete(effect);
		return;
	}

	counter = data->age;
	if (timer < 16) {
		data->color1A = timer * 16;
	}
	if (counter < 16) {
		data->color1A = counter * 16 + 15;
	}
	dispEntry(4, dispCalcZ(&pos2), 2, effHoraoSpellDisp, effect);
}

// Copies one vertex table entry and transforms its position in place. Retail performs the s16<->f32
// conversions with the paired-single GQR5 path, i.e. the SDK's __OSs16tof32/__OSf32tos16 helpers
// bracketed by OSInitFastCast; plain casts would lower to lha/fctiwz.
static inline void effHoraoSpellSetVtx(effHoraoSpellVtx* vtx, effHoraoSpellVtx* src, Mtx mtx) {
	Vec v;

	memcpy(vtx, src, sizeof(effHoraoSpellVtx));
	OSInitFastCast();
	v.x = __OSs16tof32(&vtx->pos[0]);
	v.y = __OSs16tof32(&vtx->pos[1]);
	v.z = __OSs16tof32(&vtx->pos[2]);
	PSMTXMultVec(mtx, &v, &v);
	OSInitFastCast();
	vtx->pos[0] = __OSf32tos16(v.x);
	vtx->pos[1] = __OSf32tos16(v.y);
	vtx->pos[2] = __OSf32tos16(v.z);
}

// A GXColor built field-by-field and passed by value lands in the temporary's stack slot; the original
// builds it in an inline that takes the base colour as a parameter (as makeHoraoColor does in
// eff_magic_horao_n64), which moves the local above the by-value copy. The two early colours use
// makeHoraoSpellColor. The loop colour (built from the three colX tables and the alpha) must pass its
// base colour as the *last* parameter (makeHoraoSpellColor2p): that is what puts its by-value
// temporary in retail's 0x24 word and keeps colr/colg/colb/alpha in r7/r6/r5/r0. The
// makeHoraoSpellColor2 form, with the base read inside the inline, places the temporary in 0x20.
#ifdef MUST_MATCH
static inline GXColor makeHoraoSpellColor(GXColor c, s32 r, s32 g, s32 b, s32 a) {
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}

static inline GXColor makeHoraoSpellColor2(s32 colorIdx, u8 a) {
	GXColor c = unk_80423158;
	c.r = colr[colorIdx];
	c.g = colg[colorIdx];
	c.b = colb[colorIdx];
	c.a = a;
	return c;
}

static inline GXColor makeHoraoSpellColor2p(s32 colorIdx, u8 a, GXColor c) {
	c.r = colr[colorIdx];
	c.g = colg[colorIdx];
	c.b = colb[colorIdx];
	c.a = a;
	return c;
}
#endif

// A second copy of effHoraoSpellSetVtx whose three tail stores keep the base colour pointer in the
// register retail uses. The original helper derives one address base for all three stores; here
// pos[0] goes through the function parameter while pos[1]/pos[2] go through a `vtx->pos` pointer,
// which is what makes MWCC hold the second vertex pointer in r20 for the first store.
#ifdef MUST_MATCH
static inline void effHoraoSpellSetVtx2(effHoraoSpellVtx* vtx, effHoraoSpellVtx* src, Mtx mtx) {
	Vec v;
	s16* p;

	memcpy(vtx, src, sizeof(effHoraoSpellVtx));
	OSInitFastCast();
	v.x = __OSs16tof32(&vtx->pos[0]);
	v.y = __OSs16tof32(&vtx->pos[1]);
	v.z = __OSs16tof32(&vtx->pos[2]);
	PSMTXMultVec(mtx, &v, &v);
	OSInitFastCast();
	p = vtx->pos;
	p[0] = __OSf32tos16(v.x);
	vtx->pos[1] = __OSf32tos16(v.y);
	vtx->pos[2] = __OSf32tos16(v.z);
}
#else
#define effHoraoSpellSetVtx2 effHoraoSpellSetVtx
#endif

// The vertex s16<->f32 conversions use the SDK's psq_l/psq_st helpers (__OSs16tof32/__OSf32tos16
// with OSInitFastCast). This structure matches MWCC's allocation exactly: the five Mtx objects are
// declared in retail's slot order, `k = j * 2` lets strength reduction emit one shared byte offset
// (retail's r31) for vtx[] and horao_spell_vtx[] instead of running pointers, `(f32)(-counter * 2)`
// lets counter and the encoded negCounter bits share r15, recomputing `angle = -0x11A + i * 0x24`
// inside the outer loop makes the induction variable outrank the `counter * 8` CSE (angle r23,
// counter*8 r22), and makeHoraoSpellColor2p takes the base colour as the *last* parameter so the
// by-value GXColor temporary lands in retail's 0x24 word with colr/colg/colb/alpha in r7/r6/r5/r0.
void effHoraoSpellDisp(CameraId camId, void* param) {
	Mtx transMtx;
	s32 angle;
	s32 k;
	CameraEntry* cam = camGetPtr(camId);
	Mtx scaleMtx;
	Mtx rotMtx;
	s32 colorIdx;
	Mtx resultMtx;
	f32 scale;
	smartEntry* entry;
	s32 counter;
	GXTexObj texObj;
	EffectEntry* effect = param;
	GXColor color;
	effHoraoSpellData* data = effect->userdata;
	s32 alpha;
	f32 s;
	Mtx baseMtx;
	s32 i;
	s32 j;
	effHoraoSpellVtx* vtx;

	counter = data->age;
	alpha = data->color1A;
	PSMTXTrans(transMtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(transMtx, rotMtx, transMtx);
	scale = data->scale;
	PSMTXScale(scaleMtx, scale, scale, scale);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	PSMTXConcat(cam->view, transMtx, resultMtx);

#ifdef MUST_MATCH
	GXSetTevColor(GX_TEVREG0, makeHoraoSpellColor(unk_80423150, data->color1R, data->color1G, data->color1B, alpha));
	GXSetTevColor(GX_TEVREG1, makeHoraoSpellColor(unk_80423154, data->color2R, data->color2G, data->color2B, data->color2A));
#else
	color = unk_80423150;
	color.r = data->color1R;
	color.g = data->color1G;
	color.b = data->color1B;
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
	color = unk_80423154;
	color.r = data->color2R;
	color.g = data->color2G;
	color.b = data->color2B;
	color.a = data->color2A;
	GXSetTevColor(GX_TEVREG1, color);
#endif
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_RASA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	effGetTexObjN64(0x73, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scaleMtx, 0.0078125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetCullMode(GX_CULL_NONE);

	for (i = 0; i < 2; i++) {
		angle = -0x11A + i * 0x24;
		colorIdx = (counter + 4 * i) % 17;
		PSMTXRotRad(rotMtx, 'z', 0.017453292f * (f32)angle);
		PSMTXConcat(resultMtx, rotMtx, transMtx);
		GXLoadPosMtxImm(transMtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
		GXSetTevColor(GX_TEVREG0, makeHoraoSpellColor2p(colorIdx, data->color1A, unk_80423158));
#else
		color = unk_80423158;
		color.r = colr[colorIdx];
		color.g = colg[colorIdx];
		color.b = colb[colorIdx];
		color.a = data->color1A;
		GXSetTevColor(GX_TEVREG0, color);
#endif
		color = *(GXColor*)&dat_8041f158;
		GXSetTevColor(GX_TEVREG1, color);
		PSMTXScale(scaleMtx, 0.0078125f, 0.03125f, 0.0f);
		PSMTXTrans(transMtx, (f32)(-counter * 2), 0.0f, 0.0f);
		PSMTXConcat(scaleMtx, transMtx, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
		PSMTXRotRad(rotMtx, 'x', 0.34906584f);
		PSMTXTrans(transMtx, 6.0f, 0.0f, 0.0f);
		PSMTXConcat(rotMtx, transMtx, transMtx);
		s = (f32)sin(6.2831855f * (78.75f + (f32)(counter * 8)) / 360.0f);
		PSMTXRotRad(rotMtx, 'y', 0.017453292f * (-30.0f * s));
		PSMTXConcat(transMtx, rotMtx, baseMtx);
		entry = smartAlloc(0x1C0, 3);
		vtx = (effHoraoSpellVtx*)entry->address;
		for (j = 0; j < 16; j++) {
			k = j * 2;
			PSMTXTrans(transMtx, 8.0f, 0.0f, 0.0f);
			s = (f32)sin(6.2831855f * (f32)(counter * 8 + (j * 720) / 15) / 360.0f);
			PSMTXRotRad(rotMtx, 'y', 0.017453292f * (30.0f * s));
			PSMTXConcat(transMtx, rotMtx, transMtx);
			PSMTXRotRad(rotMtx, 'z', 0.10471976f);
			PSMTXConcat(transMtx, rotMtx, transMtx);
			scale = 0.01f * (f32)j + 1.0f;
			PSMTXScale(scaleMtx, scale, scale, scale);
			PSMTXConcat(transMtx, scaleMtx, transMtx);
			PSMTXConcat(baseMtx, transMtx, baseMtx);
			effHoraoSpellSetVtx(&vtx[k], &horao_spell_vtx[k], baseMtx);
			effHoraoSpellSetVtx2(&vtx[k + 1], &horao_spell_vtx[k + 1], baseMtx);
		}
		DCFlushRange(vtx, 0x1C0);
		GXInvalidateVtxCache();
		effSetVtxDescN64((effVtxArray*)vtx);
		for (j = 0; j < 15; j++) {
			k = j * 2;
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(k, k + 2, k + 1, k, k + 1, k + 2, k + 3, k);
		}
	}
}

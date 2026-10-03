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

extern GXColor unk_80423150;
extern GXColor unk_80423154;
extern GXColor unk_80423158;
extern effHoraoSpellVtx horao_spell_vtx[];

// .rodata
const Vec vec3_802fa7f8 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fa808 = 4503601774854144.0;
const char str_802FA810[] = "HoraoSpellN64";

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

// .sdata2
// Retail reloads this scratch word every outer-loop iteration instead of hoisting it, which only happens
// if the object is writable; the byte content and .sdata2 slot are identical either way.
#ifdef MUST_MATCH
__declspec(section ".sdata2") u32 dat_8041f158 = 0xFFFFFF00;
#else
const u32 dat_8041f158 = 0xFFFFFF00;
#endif

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

// .sbss2 scratch GXColors and the .data vertex table are declared in the unit header.
EffectEntry* effHoraoSpellN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effHoraoSpellData* data;

	effect = effEntry();
	effect->type = str_802FA810;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x3C);
	effect->userdata = data;
	effect->callback = effHoraoSpellMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (life <= 0) {
		data->field_0x10 = 1000;
	} else {
		data->field_0x10 = life;
	}
	data->field_0x24 = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x38 = scale;
	data->field_0x18 = 70;
	data->field_0x1C = 180;
	data->field_0x20 = 120;
	data->field_0x28 = 20;
	data->field_0x2C = 230;
	data->field_0x30 = 50;
	data->field_0x34 = 255;
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
		data->field_0x10 = 16;
	}

	timer = data->field_0x10;
	if (timer < 1000) {
		data->field_0x10 = timer - 1;
	}
	data->field_0x14++;
	timer = data->field_0x10;

	if (timer < 0) {
		effDelete(effect);
		return;
	}

	counter = data->field_0x14;
	if (timer < 16) {
		data->field_0x24 = timer * 16;
	}
	if (counter < 16) {
		data->field_0x24 = counter * 16 + 15;
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
// eff_magic_horao_n64), which moves the local above the by-value copy. The loop's colour (built from
// the three colX tables and the alpha) gets the same treatment through makeHoraoSpellColor2, which
// moved its by-value temporary from 0x38 to the original's slot.
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
#endif

// The vertex s16<->f32 conversions use the SDK's psq_l/psq_st helpers
// (__OSs16tof32/__OSf32tos16 with OSInitFastCast); the loop structure, the GX/TEV/matrix calls, the
// frame (0x2b0), every local's stack slot except the by-value GXColor temporaries, and the fp
// constant registers (f26=2pi, f29=360, f30=deg2rad, f31=30, f17=0.01, f18=1.0, f28=-30, f27=magic)
// all match retail. Declaring the five Mtx objects in retail's slot order (transMtx, scaleMtx,
// rotMtx, resultMtx, baseMtx across the high-to-low slots), the largely reverse-engineered order of
// the remaining locals, an explicit `k = 2*j` induction variable for both vertex loops, and the
// makeHoraoSpellColor2 inline (which put the loop GXColor temporary back in retail's 0x24 slot) are
// what recovered the low-scratch layout, the loop shapes and the early colour temporaries. The
// -2*counter value must be computed into a local `s32 negCounter` declared *before* `counter` and
// assigned right after `alpha = data->field_0x24;`; that placement is what makes MWCC keep the
// callee-saved set starting at r15 as retail does. Declaring or assigning it anywhere else reorders
// the preheader and drops the score to 93.3% (109 diffs), and an explicit counter8 local does not
// help. Even with it, retail computes the encoded bits in the preheader from `counter` and reuses r15,
// while ours computes `-(counter*2)` once at the top into r15 and keeps `counter` in r17. Folding the
// outer loop's initialiser into one `for (i = 0, angle = -0x11A; ...)` (rather than a separate
// `angle = -0x11A;` line) puts `li r18,0` before `li r23,-0x11a` as retail does and recovers two more
// instructions (96.29 -> 96.31%).
// What still differs: the integer register allocation (counter r17 vs retail r15, alpha r18 vs r17,
// counter8 r25 vs r22, the 0x4330 magic r30 vs r27, and vtx/src running pointers); the preheader
// schedule; the int->float magic (retail folds `lfd f27, double_to_int_802fa808@l(r4)` from the named
// .rodata global, our anonymous @-pool copy needs `addi` + `lfd 0x0`, a project-wide MWCC limitation);
// and vtx[k]/src[k] addressing, where we keep running pointers (r27/r28) instead of retail's single
// offset register (r31).
// NONMATCHING (96.31%): integer register allocation (counter r17 vs r15, counter8 r25 vs r22, ...),
// the preheader schedule, the anonymous i2f-magic address, and the vtx/src induction variables.
void effHoraoSpellDisp(CameraId camId, void* param) {
	Mtx transMtx;
	s32 negCounter;
	s32 counter;
	s32 angle;
	s32 colorIdx;
	Mtx scaleMtx;
	Mtx rotMtx;
	EffectEntry* effect = param;
	Mtx resultMtx;
	f32 scale;
	effHoraoSpellVtx* vtx;
	smartEntry* entry;
	GXTexObj texObj;
	s32 j;
	GXColor color;
	s32 alpha;
	s32 k;
	f32 s;
	Mtx baseMtx;
	CameraEntry* cam = camGetPtr(camId);
	effHoraoSpellData* data = effect->userdata;
	s32 i;

	counter = data->field_0x14;
	alpha = data->field_0x24;
	negCounter = counter * -2;
	PSMTXTrans(transMtx, data->x, data->y, data->z);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(transMtx, rotMtx, transMtx);
	scale = data->field_0x38;
	PSMTXScale(scaleMtx, scale, scale, scale);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	PSMTXConcat(cam->view, transMtx, resultMtx);

#ifdef MUST_MATCH
	GXSetTevColor(GX_TEVREG0, makeHoraoSpellColor(unk_80423150, data->field_0x18, data->field_0x1C, data->field_0x20, alpha));
	GXSetTevColor(GX_TEVREG1, makeHoraoSpellColor(unk_80423154, data->field_0x28, data->field_0x2C, data->field_0x30, data->field_0x34));
#else
	color = unk_80423150;
	color.r = data->field_0x18;
	color.g = data->field_0x1C;
	color.b = data->field_0x20;
	color.a = alpha;
	GXSetTevColor(GX_TEVREG0, color);
	color = unk_80423154;
	color.r = data->field_0x28;
	color.g = data->field_0x2C;
	color.b = data->field_0x30;
	color.a = data->field_0x34;
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

	for (i = 0, angle = -0x11A; i < 2; i++, angle += 0x24) {
		colorIdx = (counter + 4 * i) % 17;
		PSMTXRotRad(rotMtx, 'z', 0.017453292f * (f32)angle);
		PSMTXConcat(resultMtx, rotMtx, transMtx);
		GXLoadPosMtxImm(transMtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
		GXSetTevColor(GX_TEVREG0, makeHoraoSpellColor2(colorIdx, data->field_0x24));
#else
		color = unk_80423158;
		color.r = colr[colorIdx];
		color.g = colg[colorIdx];
		color.b = colb[colorIdx];
		color.a = data->field_0x24;
		GXSetTevColor(GX_TEVREG0, color);
#endif
		color = *(GXColor*)&dat_8041f158;
		GXSetTevColor(GX_TEVREG1, color);
		PSMTXScale(scaleMtx, 0.0078125f, 0.03125f, 0.0f);
		PSMTXTrans(transMtx, (f32)negCounter, 0.0f, 0.0f);
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
		for (j = 0, k = 0; j < 16; j++, k += 2) {
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
			effHoraoSpellSetVtx(&vtx[k + 1], &horao_spell_vtx[k + 1], baseMtx);
		}
		DCFlushRange(vtx, 0x1C0);
		GXInvalidateVtxCache();
		effSetVtxDescN64((effVtxArray*)vtx);
		for (j = 0, k = 0; j < 15; j++, k += 2) {
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(k, k + 2, k + 1, k, k + 1, k + 2, k + 3, k);
		}
	}
}

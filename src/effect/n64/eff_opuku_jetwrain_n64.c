#include "eff/n64/eff_opuku_jetwrain_n64.h"
#include "eff/eff_n64.h"
#include "math_ext.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>

// .sdata2 float constants (named after their original addresses).
static const f32 float_0_8041f7d8 = 0.0f;
static const f32 float_480_8041f7dc = 480.0f;
static const f32 float_608_8041f7e0 = 608.0f;
static const f32 float_1_8041f7e4 = 1.0f;
static const f32 float_32768_8041f7e8 = 32768.0f;
static const f32 float_0p0039062_8041f7ec = 0.00390625f;

// .data vertex array, modified in place by effOpukuJetwrainDisp.
u32 opuku_jetwrain_vtx[56] = {
	0xFF60FF88, 0xFEC00000, 0x00000000, 0x00FFFF70, 0xFF88FEC0, 0x02000000, 0x000000FF,
	0xFF60FF98, 0xFEC00000, 0x02000000, 0x00FFFF70, 0xFF98FEC0, 0x02000200, 0x000000FF,
	0xFF60FF88, 0xFEC00000, 0x00000000, 0x00FFFF70, 0xFF88FEC0, 0x02800000, 0x000000FF,
	0xFF60FF98, 0xFEC00000, 0x02000000, 0x00FFFF70, 0xFF98FEC0, 0x02800200, 0x000000FF,
	0xFF60FF88, 0xFEC00000, 0x00000000, 0x00FFFF70, 0xFF88FEC0, 0x02000000, 0x000000FF,
	0xFF60FF98, 0xFEC00000, 0x02800000, 0x00FFFF70, 0xFF98FEC0, 0x02000280, 0x000000FF,
	0xFF60FF88, 0xFEC00000, 0x00000000, 0x00FFFF70, 0xFF88FEC0, 0x02800000, 0x000000FF,
	0xFF60FF98, 0xFEC00000, 0x02800000, 0x00FFFF70, 0xFF98FEC0, 0x02800280, 0x000000FF,
};

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a
// small-data section and place it by hand.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804231d8;

EffectEntry* effOpukuJetwrainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effOpukuJetwrainData* data;
	s32 i;
	s32 j;

	effect = effEntry();
	effect->type = "OpukuJetwrainN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x308);
	effect->userdata = data;
	effect->callback = effOpukuJetwrainMain;
	effect->flags |= 2;
	data->type = type;
	data->counter = 0;
	if (duration <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = duration;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->color1R = 10;
	data->color1G = 0x6e;
	data->color1B = 0xff;
	data->color2R = 0xbe;
	data->color2G = 0xdc;
	data->color2B = 0xff;
	for (i = 0; i < 19; i++) {
		for (j = 0; 13 > j; j++) {
			data->height[i][j] = 0;
			data->velocity[i][j] = 0;
		}
	}
	return effect;
}

static inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

#ifdef MUST_MATCH
// Build the tev colour in an inline so mwcc keeps the temporary at 0x8(r1) and copies it to the
// outgoing argument slot at 0xc(r1); with the channels set on a plain local the two slots swap.
static inline GXColor makeTevColor(u8 r, u8 g, u8 b, u8 a) {
	GXColor c = unk_804231d8;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}
#endif

// Matching this function was allocator work only. Three source details move mwcc's coloring onto the
// original's registers: `counter` is declared after `j` (so the allocator gives j r27 and counter r28,
// the original's pair, and stmw/lmw drop from r24..r31 to r25..r31); the second loop names the shared
// halfCorners = 0.5*corners (f7) while leaving `- 0.7*fourCenter` inline so mwcc emits the original's
// fnmsubs; and the third loop compares `12 > i` (see the MUST_MATCH note at the loop).
void effOpukuJetwrainMain(EffectEntry* effect) {
	s32 lifetime;
	effOpukuJetwrainData* data = effect->userdata;
	Vec position = initializeVec((Vec*)&data->x);
	s32 alpha;
	f32 scale;
	s32 i;
	s32 j;
	s32 counter;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x20;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->counter++;
	lifetime = data->lifetime;
	if (lifetime < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	alpha = 0xff;
	if (lifetime < 0x20) {
		alpha = lifetime * 8;
	}
	if (counter < 0x10) {
		alpha = counter * 16 + 15;
	}
	data->alpha = alpha;
	scale = (f32)alpha / 255.0f;

	// Seed the height field with a travelling sine wave.
	for (j = 1; j < 18; j++) {
		{
			f32 s = (f32)sin(6.2831855f * (f32)((counter - j) * -20) / 360.0f);
			data->height[j][6] = (s8)(f32)(scale * (-64.0f * s));
		}
	}

	// Relax each height into the velocity field (weighted neighbour sum minus the centre), damped by 0.98.
	for (j = 1; j < 18; j++) {
		for (i = 1; i < 12; i++) {
			f32 center = (f32)(s8)data->height[j][i];
			f32 fourCenter = 4.0f * center;
			f32 corners = (f32)(s8)data->height[j + 1][i + 1] + (f32)(s8)data->height[j - 1][i + 1] + (f32)(s8)data->height[j + 1][i - 1] + (f32)(s8)data->height[j - 1][i - 1];
			f32 edges = (f32)(s8)data->height[j + 1][i] + (f32)(s8)data->height[j - 1][i] + (f32)(s8)data->height[j][i + 1] + (f32)(s8)data->height[j][i - 1];
			f32 v = (f32)data->velocity[j][i];
			f32 halfCorners = 0.5f * corners;
			f32 t = halfCorners + (edges - fourCenter) - 0.7f * fourCenter;
			data->velocity[j][i] = (s16)(v + t);
			data->velocity[j][i] *= 0.98f;
			if (lifetime < 0x20) {
				data->height[j][i] = (s8)((f32)(s8)data->height[j][i] * scale);
			}
		}
	}

	// Integrate the velocity field back into the heights.
	for (j = 1; j < 18; j++) {
#ifdef MUST_MATCH
		// `12 > i` (not `i < 12`): the reversed comparison makes mwcc hoist the loop's constants out
		// of the j loop and drop the entry guard, matching the original's code; see the report.
		for (i = 1; 12 > i; i++) {
#else
		for (i = 1; i < 12; i++) {
#endif
			data->height[j][i] = (s8)((f32)(s8)data->height[j][i] + 0.02f * (f32)data->velocity[j][i]);
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effOpukuJetwrainDisp, effect);
}

// Match-only: mwcc's allocator gives xBase r3 and xEnd r4 only when the field offsets are
// computed as the target does. The natural form (fieldBase/fieldEnd assigned after the loop
// update, under #else) makes mwcc swap them into r0/r3. Under MUST_MATCH, fieldBase/fieldEnd
// are assigned in the inner-loop body (hoisted) and fieldEnd is built inline as
// (iPos + xEnd + 0x10) << 2, which keeps xEnd a pure source and yields the target's
// addi r0,r4,0x10 / add r3,r24,r3 / add r0,r24,r0 sequence. All callee-saved registers
// (vtx=r31, magic 0x4330=r29, xoris=r30, iPos=r24, fieldBase=r26, fieldEnd=r25) match.
void effOpukuJetwrainDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	s32 alpha;
	void* tex;
	effOpukuJetwrainVtx* vtx;
	s32 fieldEnd;
	s32 iPos;
	s32 fieldBase;
	s32 xBase;
	s32 jPos;
	s32 x;
	effOpukuJetwrainData* data;
	s32 j;
	s32 i;
	s32 outerEdge;
	s32 xEnd;
#ifdef MUST_MATCH
	s32 x2;
	s32 innerEdge;
#else
	s32 innerEdge;
	s32 x2;
#endif
	s32 base;
	smartEntry* entry;
	Mtx transMtx;
	Mtx texMtx;
	Mtx44 orthoMtx;
	Mtx lookAtMtx;
	GXTexObj texObj;
	f32 proj[7];
	Vec up;
	Vec target;
	Vec eye;
	GXColor color;
	camGetPtr(camId);
	data = effect->userdata;
	alpha = data->alpha;
	GXGetProjectionv(proj);
	C_MTXOrtho(orthoMtx, 0.0f, 480.f, 0.0f, 608.0f, 1.0f, 32768.0f);
	GXSetProjection(orthoMtx, GX_ORTHOGRAPHIC);
	GXSetZMode(0, GX_ALWAYS, 0);
	entry = smartAlloc(GXGetTexBufferSize(304, 240, GX_TF_RGB565, 0, 0), HEAP_EFFECT);
	tex = entry->address;
	GXSetTexCopySrc(0, 0, 608, 480);
	GXSetTexCopyDst(304, 240, GX_TF_RGB565, GX_TRUE);
	GXCopyTex(tex, 0);
	GXPixModeSync();
	GXInitTexObj(&texObj, tex, 304, 240, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, 0);
	GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(0, GX_CC_ZERO, GX_CC_C0, GX_CC_A0, GX_CC_TEXC);
	GXSetTevAlphaIn(0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
#ifdef MUST_MATCH
	color = makeTevColor(data->color1R, data->color1G, data->color1B, alpha >> 1);
#else
	color = (GXColor){ 0, 0, 0, 0 };
	color.r = data->color1R;
	color.g = data->color1G;
	color.b = data->color1B;
	color.a = alpha >> 1;
#endif
	GXSetTevColor(GX_TEVREG0, color);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)opuku_jetwrain_vtx);
	up = (Vec){ 0.0f, 1.0f, 0.0f };
	target = (Vec){ 304.0f, 240.0f, 0.0f };
	eye = (Vec){ 304.0f, 240.0f, 100.0f };
	C_MTXLookAt(lookAtMtx, &eye, &up, &target);
	GXLoadPosMtxImm(lookAtMtx, 0);
	GXSetCurrentMtx(0);
	for (i = 0; i < 12; i++) {
		outerEdge = 0;
		if (i == 0) {
			xBase = -4;
			outerEdge = 1;
		} else {
			xBase = 0;
		}
		if (i == 11) {
			xEnd = 4;
			outerEdge = 1;
		} else {
			xEnd = 0;
		}
		iPos = 0x18 + i * 0x10;
#ifdef MUST_MATCH
		xBase = iPos + xBase;
#else
		xBase += iPos;
		xEnd += iPos + 0x10;
#endif
#ifndef MUST_MATCH
		fieldBase = xBase << 2;
		fieldEnd = xEnd << 2;
#endif
		for (j = 0; j < 18; j++) {
#ifdef MUST_MATCH
			fieldBase = xBase << 2;
			fieldEnd = (iPos + xEnd + 0x10) << 2;
#endif
			innerEdge = 0;
			if (j == 0) {
				x = -4;
				innerEdge = 1;
			} else {
				x = 0;
			}
			if (j == 17) {
				x2 = 4;
				innerEdge = 1;
			} else {
				x2 = 0;
			}
			jPos = 0x10 + j * 0x10;

			PSMTXScale(texMtx, 0.00390625f, 0.00390625f, 1.0f);
#ifdef MUST_MATCH
			x = jPos + x;
#else
			x += jPos;
#endif
			PSMTXTrans(transMtx, (f32)x, (f32)xBase, 0.0f);
			PSMTXConcat(texMtx, transMtx, texMtx);
			GXLoadTexMtxImm(texMtx, GX_TEXMTX0, GX_MTX2x4);

			if (innerEdge && outerEdge) {
				base = 12;
			} else if (innerEdge) {
				base = 4;
			} else if (outerEdge) {
				base = 8;
			} else {
				base = 0;
			}

			vtx = (effOpukuJetwrainVtx*)opuku_jetwrain_vtx;
			vtx[base + 0].x = x << 2;
			vtx[base + 0].y = (s8)data->height[j][i] + fieldBase;
			vtx[base + 1].x = (jPos + x2 + 0x10) << 2;
			vtx[base + 1].y = (s8)data->height[j + 1][i] + fieldBase;
			vtx[base + 2].x = x << 2;
			vtx[base + 2].y = (s8)data->height[j][i + 1] + fieldEnd;
			vtx[base + 3].x = (jPos + x2 + 0x10) << 2;
			vtx[base + 3].y = (s8)data->height[j + 1][i + 1] + fieldEnd;
			DCFlushRange(opuku_jetwrain_vtx, 0xE0);
			GXInvalidateVtxCache();
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(base, base + 3, base + 1, 0, base, base + 2, base + 3, 0);
			sysWaitDrawSync();
		}
	}
	GXSetProjectionv(proj);
}

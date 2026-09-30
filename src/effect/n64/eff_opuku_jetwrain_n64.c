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

// .rodata
const Vec vec3_802fb248 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fb254 = { 0.0f, 1.0f, 0.0f };
const Vec vec3_802fb260 = { 304.0f, 240.0f, 0.0f };
const Vec vec3_802fb26c = { 304.0f, 240.0f, 100.0f };
const f64 double_to_int_802fb278 = 4503601774854144.0;
const char str_802FB280[20] = "OpukuJetwrainN64";

// .sdata2 float constants (named after their original addresses).
const f32 float_0_8041f7d8 = 0.0f;
const f32 float_480_8041f7dc = 480.0f;
const f32 float_608_8041f7e0 = 608.0f;
const f32 float_1_8041f7e4 = 1.0f;
const f32 float_32768_8041f7e8 = 32768.0f;
const f32 float_0p0039062_8041f7ec = 0.00390625f;
const f32 float_255_8041f7f0 = 255.0f;
const f32 float_6p2832_8041f7f4 = 6.2831855f;
const f32 float_360_8041f7f8 = 360.0f;
const f32 float_neg64_8041f7fc = -64.0f;
const f32 float_4_8041f800 = 4.0f;
const f32 float_0p5_8041f804 = 0.5f;
const f32 float_0p7_8041f808 = 0.7f;
const f32 float_0p98_8041f80c = 0.98f;
const f32 float_0p02_8041f810 = 0.02f;

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

// .sbss2
GXColor unk_804231d8;

EffectEntry* effOpukuJetwrainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effOpukuJetwrainData* data;
	s32 i;
	s32 j;

	effect = effEntry();
	effect->type = str_802FB280;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x308);
	effect->userdata = data;
	effect->callback = effOpukuJetwrainMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (duration <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = duration;
	}
	data->field_0x1F = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x1C = 10;
	data->field_0x1D = 0x6e;
	data->field_0x1E = 0xff;
	data->field_0x20 = 0xbe;
	data->field_0x21 = 0xdc;
	data->field_0x22 = 0xff;
	for (i = 0; i < 19; i++) {
		for (j = 0; 13 > j; j++) {
			data->field_0x23[i][j] = 0;
			data->field_0x11A[i][j] = 0;
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

// The first loop's callee-saved allocation is shifted one register down: mwcc puts counter in r26
// (original r28) and the 0x4330 double-magic temp in r24 (original r25), so stmw/lmw saves r24..r31
// instead of r25..r31; effect/data/lifetime are correct. That also renames every scratch register in
// all three loops. In the second loop the byte loads are scheduled in a different order and the
// constants land in other FPRs (mine f1/f6/f0/f5/f4, original f3/f1/f2/f0/f6).
// NONMATCHING (92.87%): codegen-only register/scheduling differences; arithmetic and structure match.
void effOpukuJetwrainMain(EffectEntry* effect) {
	s32 lifetime;
	effOpukuJetwrainData* data = effect->userdata;
	Vec position = initializeVec((Vec*)&data->x);
	s32 counter;
	s32 alpha;
	f32 scale;
	s32 i;
	s32 j;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x20;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->field_0x14++;
	lifetime = data->lifetime;
	if (lifetime < 0) {
		effDelete(effect);
		return;
	}
	counter = data->field_0x14;
	alpha = 0xff;
	if (lifetime < 0x20) {
		alpha = lifetime * 8;
	}
	if (counter < 0x10) {
		alpha = counter * 16 + 15;
	}
	data->field_0x1F = alpha;
	scale = (f32)alpha / float_255_8041f7f0;

	for (i = 1; i < 18; i++) {
		data->field_0x23[i][6] = (s8)(f32)(scale * (float_neg64_8041f7fc * (f32)sin(float_6p2832_8041f7f4 * (f32)((counter - i) * -20) / float_360_8041f7f8)));
	}

	for (j = 1; j < 18; j++) {
		for (i = 1; i < 12; i++) {
			f32 center = (f32)(s8)data->field_0x23[j][i];
			f32 corners = (f32)(s8)data->field_0x23[j - 1][i - 1] + (f32)(s8)data->field_0x23[j - 1][i + 1] + (f32)(s8)data->field_0x23[j + 1][i - 1] + (f32)(s8)data->field_0x23[j + 1][i + 1];
			f32 edges = (f32)(s8)data->field_0x23[j - 1][i] + (f32)(s8)data->field_0x23[j][i - 1] + (f32)(s8)data->field_0x23[j][i + 1] + (f32)(s8)data->field_0x23[j + 1][i];
			f32 v = (f32)data->field_0x11A[j][i];
			f32 t = float_0p5_8041f804 * corners;
			t = t + (edges - float_4_8041f800 * center);
			t = t - float_0p7_8041f808 * (float_4_8041f800 * center);
			data->field_0x11A[j][i] = (s16)(v + t);
			data->field_0x11A[j][i] *= float_0p98_8041f80c;
			if (lifetime < 0x20) {
				data->field_0x23[j][i] = (s8)((f32)(s8)data->field_0x23[j][i] * scale);
			}
		}
	}

	for (j = 1; j < 18; j++) {
		for (i = 1; i < 12; i++) {
			data->field_0x23[j][i] = (s8)((f32)(s8)data->field_0x23[j][i] + float_0p02_8041f810 * (f32)data->field_0x11A[j][i]);
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effOpukuJetwrainDisp, effect);
}

// The instruction sequence and the .sdata2/.rodata pool now match the target; what is left is the
// compiler's register choice for the outer loop and the int->float magic. Original: vtx=r31,
// magic 0x4330=r29, xoris temp=r30, iPos=r24, fieldBase=r26, fieldEnd=r25, xBase=r3, xEnd=r4.
// Ours: vtx=r29, magic=r30, xoris temp=r31, iPos=r26, fieldBase=r24, fieldEnd=r25, xBase=r0,
// xEnd=r3 (the {r29,r30,r31} triple and the {r24,r26} pair are permuted; r25, i, j, jPos, data,
// the field pointer and the conversion operands all match). Declaration-order, arithmetic-order,
// pragma and type sweeps did not change the permutation.
// NONMATCHING (99.51%): register-allocation-only differences (see above).
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
	s32 innerEdge;
	s32 x2;
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
	alpha = data->field_0x1F;
	GXGetProjectionv(proj);
	C_MTXOrtho(orthoMtx, float_0_8041f7d8, float_480_8041f7dc, float_0_8041f7d8, float_608_8041f7e0, float_1_8041f7e4, float_32768_8041f7e8);
	GXSetProjection(orthoMtx, GX_ORTHOGRAPHIC);
	GXSetZMode(0, GX_ALWAYS, 0);
	entry = smartAlloc(GXGetTexBufferSize(304, 240, GX_TF_RGB565, 0, 0), HEAP_EFFECT);
	tex = entry->address;
	GXSetTexCopySrc(0, 0, 608, 480);
	GXSetTexCopyDst(304, 240, GX_TF_RGB565, GX_TRUE);
	GXCopyTex(tex, 0);
	GXPixModeSync();
	GXInitTexObj(&texObj, tex, 304, 240, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, 0);
	GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, float_0_8041f7d8, float_0_8041f7d8, float_0_8041f7d8, 0, 0, GX_ANISO_1);
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
	color = makeTevColor(data->field_0x1C, data->field_0x1D, data->field_0x1E, alpha >> 1);
#else
	color = unk_804231d8;
	color.r = data->field_0x1C;
	color.g = data->field_0x1D;
	color.b = data->field_0x1E;
	color.a = alpha >> 1;
#endif
	GXSetTevColor(GX_TEVREG0, color);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)opuku_jetwrain_vtx);
	up = vec3_802fb254;
	target = vec3_802fb260;
	eye = vec3_802fb26c;
	C_MTXLookAt(lookAtMtx, &eye, &up, &target);
	GXLoadPosMtxImm(lookAtMtx, 0);
	GXSetCurrentMtx(0);
	vtx = (effOpukuJetwrainVtx*)opuku_jetwrain_vtx;

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
		xEnd += 0x10;
		xBase += iPos;
		xEnd += iPos;
#else
		xBase += iPos;
		xEnd += iPos + 0x10;
#endif
		fieldBase = xBase << 2;
		fieldEnd = xEnd << 2;

		for (j = 0; j < 18; j++) {
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

			PSMTXScale(texMtx, float_0p0039062_8041f7ec, float_0p0039062_8041f7ec, float_1_8041f7e4);
			x += jPos;
			PSMTXTrans(transMtx, (f32)x, (f32)xBase, float_0_8041f7d8);
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

			vtx[base + 0].x = x << 2;
			vtx[base + 0].y = (s8)data->field_0x23[j][i] + fieldBase;
			vtx[base + 1].x = (jPos + x2 + 0x10) << 2;
			vtx[base + 1].y = (s8)data->field_0x23[j + 1][i] + fieldBase;
			vtx[base + 2].x = x << 2;
			vtx[base + 2].y = (s8)data->field_0x23[j][i + 1] + fieldEnd;
			vtx[base + 3].x = (jPos + x2 + 0x10) << 2;
			vtx[base + 3].y = (s8)data->field_0x23[j + 1][i + 1] + fieldEnd;
			DCFlushRange(opuku_jetwrain_vtx, 0xE0);
			GXInvalidateVtxCache();
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(base, base + 3, base + 1, 0, base, base + 2, base + 3, 0);
			sysWaitDrawSync();
		}
	}
	GXSetProjectionv(proj);
}
